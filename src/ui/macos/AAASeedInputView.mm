// AAASeedInputView.mm — Phase 4 input plumbing MVP.
//
// MTKView subclass that captures keyboard events on Mac. Continuation
// 36. See header for design notes.
//
// What this file IS :
//   - acceptsFirstResponder = YES so the view participates in the
//     keyboard responder chain.
//   - keyDown: / keyUp: that record the latest keycode and maintain a
//     pressed-key set.
//   - NSLog output on each key event so the .app smoke test surfaces
//     real input dispatch (visible in `log stream --process aaaseed_app`).
//
// What this file is NOT (yet) :
//   - A c_event_keyboard adapter -- engine layer not Mac-ported yet.
//   - Modifier-key (cmd/option/shift/ctrl) handling beyond a flag count.
//
// c133 / Phase 4 view-side wiring : every NSResponder override now also
// pushes its NSEvent into a per-view-owned bridge BEFORE the existing
// local-buffer side-effects :
//   - keyboard + mouse : `EventBridge::push(EngineEvent)` (c119-B)
//   - scrollWheel / magnify / rotate / swipe : `GestureBridge::
//     handle_ns_event(event)` (c132-B real-path ingest)
// New overrides : `mouseMoved:`, `magnifyWithEvent:`, `rotateWithEvent:`,
// `swipeWithEvent:`. `mouseMoved:` requires `acceptsMouseMovedEvents:YES`
// on the host window + an NSTrackingArea covering view bounds ; set in
// `viewDidMoveToWindow` + `updateTrackingAreas`.
//
// Manual reference counting on metal-cpp boundary applies here too
// (the umbrella ObjC++ TU is compiled with -fno-objc-arc per
// src/ui/macos/CMakeLists.txt).

#import "AAASeedInputView.h"

#include "aaa_event_bridge.h"
#include "aaa_event_bridge_gesture.h"

//	c144 : MEU runner forwarding. The InputView reaches the MTKView
//	delegate to find a Runner pointer ; if present, each NSResponder
//	override ALSO calls the matching on_*_event entry point so a Lua
//	script polling aaa.key_down / aaa.mouse_xy sees the same state the
//	bridge surface sees. Additive only -- the c133-A EventBridge path
//	is untouched.
//
//	IMPORTANT (c133 link-decoupling) : we do NOT `#import "AAASeedMTKView.h"`
//	here -- the existing c133 input_view unit test compiles this .mm
//	WITHOUT linking AAASeedMTKView.mm, so a static reference to the
//	`AAASeedMTKViewDelegate` class would break that build. Instead we
//	probe the delegate via `respondsToSelector:` at runtime ; the
//	runner pointer comes through a `-meuRunner` selector dispatch.
#include "src/meu/aaa_meu_runner_mac.h"
#include "src/ui/widgets/aaa_widgets_mac.h"

#include <memory>

@implementation AAASeedInputView
{
    NSMutableSet< NSNumber* >* _pressedKeysMutable;
    NSInteger                  _keyEventCount;
    NSMutableSet< NSNumber* >* _mouseButtonsMutable;
    NSPoint                    _lastMouseLocation;
    NSInteger                  _mouseEventCount;

    //	c133 view-side wiring : per-view bridge ownership. unique_ptr
    //	guarantees destruction on -dealloc ; no global state.
    std::unique_ptr< aaa::input::EventBridge   > _eventBridge;
    std::unique_ptr< aaa::input::GestureBridge > _gestureBridge;

    //	Tracking area for mouseMoved: dispatch. Recreated in
    //	updateTrackingAreas every time the view's bounds change.
    NSTrackingArea*            _trackingArea;

    //	c150-A v4 : NSTextInputClient state shadow. _currentMarkedText
    //	mirrors the WidgetSystem's composition buffer so the protocol
    //	query methods (markedRange, hasMarkedText) return the same shape
    //	without round-tripping through the C++ layer on every call.
    //	Retained ; released in dealloc.
    NSString*                  _currentMarkedText;
    NSRange                    _markedSelectedRange;
}

- (instancetype)initWithFrame:(CGRect)frameRect device:(id< MTLDevice >)device
{
    self = [super initWithFrame:frameRect device:device];
    if( self )
    {
        _pressedKeysMutable  = [[NSMutableSet alloc] init];
        _keyEventCount       = 0;
        self.lastKeyCode     = -1;
        _mouseButtonsMutable = [[NSMutableSet alloc] init];
        _lastMouseLocation   = NSZeroPoint;
        _mouseEventCount     = 0;
        self.lastScrollDelta = NSZeroPoint;

        //	c133 : eager-allocate per-view bridges. Cheap (each is just a
        //	std::vector + a few scalars) ; alloc-on-first-use would
        //	complicate the accessor contract.
        _eventBridge   = std::make_unique< aaa::input::EventBridge   >();
        _gestureBridge = std::make_unique< aaa::input::GestureBridge >();

        _trackingArea = nil;

        //	c150-A v4 : NSTextInputClient state. Start with no marked
        //	composition + a zero-length selected range.
        _currentMarkedText   = [@"" retain];
        _markedSelectedRange = NSMakeRange( NSNotFound, 0 );

        //	c149-A v3 Feature 1 : register for file-URL drag types. The
        //	view accepts dropped .lua files ; the dragging-destination
        //	methods below filter by extension + route to the MEU
        //	runner's drop_file. Pasteboard registration is required
        //	BEFORE the drag enters the view ; no NSWindow plumbing
        //	needed since the view inherits NSResponder and is the
        //	first hit-test target for AppKit's drag tracking.
        [self registerForDraggedTypes:@[ NSPasteboardTypeFileURL ]];
    }
    return self;
}

//	c149-A v3 Feature 1 : NSDraggingDestination protocol implementation.
//	On the first entry of a drag operation we inspect the pasteboard ;
//	if there's at least one .lua file URL we accept the drop and toggle
//	the widget system's drop-target highlight so the user sees the
//	accept. draggingExited / draggingEnded reset the highlight ; the
//	performDragOperation: callback routes into the MEU runner.
- (NSDragOperation)draggingEntered:(id< NSDraggingInfo >)sender
{
    NSPasteboard* pb = [sender draggingPasteboard];
    NSArray* urls = [pb readObjectsForClasses:@[ [NSURL class] ]
                                       options:nil];
    BOOL has_lua = NO;
    for( NSURL* u in urls )
    {
        if( u != nil && [u isFileURL] )
        {
            NSString* ext = [[u pathExtension] lowercaseString];
            if( [ext isEqualToString:@"lua"] )
            {
                has_lua = YES;
                break;
            }
        }
    }
    if( has_lua )
    {
        aaa::ui::widgets::WidgetSystem* ws = [self aaa_widget_system];
        if( ws ) ws->set_drop_target_highlight( true );
        return NSDragOperationCopy;
    }
    return NSDragOperationNone;
}

- (void)draggingExited:(id< NSDraggingInfo >)sender
{
    (void) sender;
    aaa::ui::widgets::WidgetSystem* ws = [self aaa_widget_system];
    if( ws ) ws->set_drop_target_highlight( false );
}

- (BOOL)prepareForDragOperation:(id< NSDraggingInfo >)sender
{
    (void) sender;
    return YES;
}

- (BOOL)performDragOperation:(id< NSDraggingInfo >)sender
{
    aaa::ui::widgets::WidgetSystem* ws = [self aaa_widget_system];
    if( ws ) ws->set_drop_target_highlight( false );

    NSPasteboard* pb = [sender draggingPasteboard];
    NSArray* urls = [pb readObjectsForClasses:@[ [NSURL class] ]
                                       options:nil];
    aaa::meu::Runner* runner = [self aaa_meu_runner];
    if( runner == nullptr ) return NO;

    BOOL any_ok = NO;
    for( NSURL* u in urls )
    {
        if( u == nil || ![u isFileURL] ) continue;
        NSString* path_ns = [u path];
        if( path_ns == nil ) continue;
        NSString* ext = [[u pathExtension] lowercaseString];
        if( ![ext isEqualToString:@"lua"] ) continue;

        //	Optional quarantine guard (c149-A spec) : log a warning when
        //	the dropped file has the com.apple.quarantine xattr set.
        //	Don't BLOCK the drop -- the user can rinse manually if Gatekeeper
        //	complains downstream. NSURL's resourceValuesForKeys + the
        //	NSURLQuarantinePropertiesKey is the public path to this.
        NSError* qerr = nil;
        id qprops = nil;
        [u getResourceValue:&qprops
                     forKey:NSURLQuarantinePropertiesKey
                      error:&qerr];
        if( qprops != nil && qprops != [NSNull null] )
        {
            NSLog( @"AAASeed.drag-drop: warning, dropped .lua has quarantine attrs : %@", path_ns );
        }

        std::string const path( [path_ns UTF8String] );
        if( runner->drop_file( path ) )
        {
            any_ok = YES;
            NSLog( @"AAASeed.drag-drop: loaded %@", path_ns );
            break;   //  Runner holds ONE script per c142-B ; first wins.
        }
    }
    return any_ok;
}

- (void)dealloc
{
    [_pressedKeysMutable  release];
    [_mouseButtonsMutable release];
    [_currentMarkedText   release];
    if( _trackingArea != nil )
    {
        [self removeTrackingArea:_trackingArea];
        [_trackingArea release];
    }
    //	unique_ptr destructors run via the ObjC++ object-destruction
    //	mechanism ; no explicit reset needed.
    [super dealloc];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

//	c144 : MEU runner lookup helper. The MTKView's delegate is the
//	AAASeedMTKViewDelegate that owns the Runner. We probe via
//	respondsToSelector: rather than isKindOfClass: so this .mm stays
//	link-time independent of AAASeedMTKView.mm (the c133 unit test
//	compiles the InputView WITHOUT the MTKView delegate). Returns
//	nullptr when the delegate doesn't expose `-meuRunner` OR when
//	the runner failed to construct -- callers must null-check.
- (aaa::meu::Runner*)aaa_meu_runner
{
    id< MTKViewDelegate > d = self.delegate;
    if( d && [d respondsToSelector:@selector( meuRunner )] )
    {
        //	NSInvocation gives us back the raw pointer without the
        //	compiler needing to know AAASeedMTKViewDelegate's class
        //	symbol. methodSignatureForSelector + invoke + getReturnValue
        //	matches the doctrine in feedback_bridge_api_standardization.md
        //	(void* across hermetic-sublib boundaries).
        NSMethodSignature* sig =
            [(NSObject*) d methodSignatureForSelector:@selector( meuRunner )];
        if( sig != nil )
        {
            NSInvocation* inv = [NSInvocation invocationWithMethodSignature:sig];
            [inv setSelector:@selector( meuRunner )];
            [inv setTarget:d];
            [inv invoke];
            aaa::meu::Runner* runner = nullptr;
            [inv getReturnValue:&runner];
            return runner;
        }
    }
    return nullptr;
}

//	c148-A : widget system lookup helper. Same dispatch pattern as
//	aaa_meu_runner above ; the delegate exposes a `widgetSystem`
//	selector that returns the raw WidgetSystem*. Returns nullptr when
//	the delegate is missing OR the widget system hasn't been
//	constructed yet (e.g. backend init failure path).
- (aaa::ui::widgets::WidgetSystem*)aaa_widget_system
{
    id< MTKViewDelegate > d = self.delegate;
    if( d && [d respondsToSelector:@selector( widgetSystem )] )
    {
        NSMethodSignature* sig =
            [(NSObject*) d methodSignatureForSelector:@selector( widgetSystem )];
        if( sig != nil )
        {
            NSInvocation* inv = [NSInvocation invocationWithMethodSignature:sig];
            [inv setSelector:@selector( widgetSystem )];
            [inv setTarget:d];
            [inv invoke];
            aaa::ui::widgets::WidgetSystem* ws = nullptr;
            [inv getReturnValue:&ws];
            return ws;
        }
    }
    return nullptr;
}

//	Allow the window to immediately make us the first responder when the
//	user clicks anywhere in the view -- otherwise the first key press
//	would be eaten by the window's title bar.
- (BOOL)becomeFirstResponder
{
    return YES;
}

//	c133 : when the view lands in a window, opt that window into
//	mouse-moved dispatch. Without this AppKit drops `mouseMoved:` events
//	on the floor (the default is NO because of the per-frame overhead
//	on busy event loops).
- (void)viewDidMoveToWindow
{
    [super viewDidMoveToWindow];
    NSWindow* const w = [self window];
    if( w != nil )
    {
        [w setAcceptsMouseMovedEvents:YES];
    }
}

//	c133 : (re)install an NSTrackingArea covering the entire view bounds
//	on every layout pass. AppKit only delivers `mouseMoved:` when a
//	matching tracking area is active. The `inVisibleRect` option keeps
//	the area auto-sized to whatever portion of the view is exposed.
- (void)updateTrackingAreas
{
    [super updateTrackingAreas];
    if( _trackingArea != nil )
    {
        [self removeTrackingArea:_trackingArea];
        [_trackingArea release];
        _trackingArea = nil;
    }
    NSTrackingAreaOptions const opts =
        NSTrackingMouseMoved         |
        NSTrackingActiveInKeyWindow  |
        NSTrackingInVisibleRect;
    _trackingArea = [[NSTrackingArea alloc] initWithRect:NSZeroRect
                                                 options:opts
                                                   owner:self
                                                userInfo:nil];
    [self addTrackingArea:_trackingArea];
}

- (void)keyDown:(NSEvent*)event
{
    NSUInteger const code = [event keyCode];

    //	c133 : ingest into EventBridge BEFORE updating local buffers, so a
    //	consumer that drains immediately after dispatch sees the event
    //	even if the local-buffer surface is in flux.
    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind     = aaa::input::EngineEvent::Kind::KEY_DOWN;
        ev.key_code = (std::int32_t) code;
        _eventBridge->push( ev );
    }

    //	c144 : also forward to the MEU runner so Lua scripts see the
    //	keypress on the SAME frame the bridge surface sees it.
    if( aaa::meu::Runner* runner = [self aaa_meu_runner] )
    {
        runner->on_key_event( (int) code, true );
    }

    [_pressedKeysMutable addObject:@(code)];
    self.lastKeyCode = (NSInteger) code;
    _keyEventCount++;

    //	c148-A : when a widget text input is focused, route this event
    //	through the standard NSResponder text-input cascade so AppKit
    //	calls -insertText:replacementRange:, -deleteBackward:,
    //	-insertNewline:, or -cancelOperation: as appropriate. The
    //	overrides below forward the resulting codepoint into the widget
    //	system. When NO text input is focused, we skip the cascade so
    //	non-text scripts still see the raw key events without printable
    //	chars being eaten.
    //
    //	c150-A v4 : expand the gate to cover text_area focus + any
    //	in-progress IME composition. Without this, the NSTextInputContext
    //	chain cannot deliver setMarkedText: callbacks since AppKit only
    //	routes via interpretKeyEvents:. The downstream selectors are
    //	gated by the WidgetSystem's focused-id check so they no-op
    //	cleanly when nothing is focused.
    aaa::ui::widgets::WidgetSystem* const ws = [self aaa_widget_system];
    bool const text_input_focused = ws != nullptr && ws->focused_text_input_id() != 0;
    bool const ime_active         = ws != nullptr && ws->has_marked_text();
    bool const text_area_focused  = ws != nullptr && ws->focused_text_area_id() != 0;
    if( text_input_focused || text_area_focused || ime_active )
    {
        [self interpretKeyEvents:@[ event ]];
    }

    //	Visible in `log stream --process aaaseed_app`. Useful for
    //	smoke-testing the chain without a debugger attached. When the
    //	c_event_keyboard adapter lands, this NSLog goes away.
    NSLog( @"AAASeed.input: keyDown code=%lu chars=%@",
           (unsigned long) code,
           [event charactersIgnoringModifiers] );
}

//	c148-A : NSResponder text-input cascade. interpretKeyEvents: routes
//	keystrokes that look like text into these four selectors. Each one
//	forwards the matching codepoint into the WidgetSystem so the
//	currently-focused text input updates. We do NOT call super, which
//	would beep or surface the event as a no-op key command.

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange
{
    (void) replacementRange;
    aaa::ui::widgets::WidgetSystem* const ws = [self aaa_widget_system];
    if( ws == nullptr ) return;
    NSString* s = nil;
    if( [string isKindOfClass:[NSAttributedString class]] )
    {
        s = [(NSAttributedString*) string string];
    }
    else if( [string isKindOfClass:[NSString class]] )
    {
        s = (NSString*) string;
    }
    if( s == nil ) return;
    NSUInteger const n = [s length];
    for( NSUInteger i = 0; i < n; ++i )
    {
        unichar const c = [s characterAtIndex:i];
        //	Restrict to printable ASCII per the spec (no IME yet).
        if( c >= 0x20 && c <= 0x7E )
        {
            ws->on_text_input( (std::uint32_t) c );
        }
    }
}

//	Older NSResponder protocol entry point ; some AppKit code paths
//	still hit this. Forward to the (string, range) variant.
- (void)insertText:(id)string
{
    [self insertText:string replacementRange:NSMakeRange( NSNotFound, 0 )];
}

- (void)deleteBackward:(id)sender
{
    (void) sender;
    aaa::ui::widgets::WidgetSystem* const ws = [self aaa_widget_system];
    if( ws == nullptr ) return;
    ws->on_text_input( 0x08 );   //  backspace
}

- (void)insertNewline:(id)sender
{
    (void) sender;
    aaa::ui::widgets::WidgetSystem* const ws = [self aaa_widget_system];
    if( ws == nullptr ) return;
    ws->on_text_input( 0x0A );   //  newline / enter
}

- (void)cancelOperation:(id)sender
{
    (void) sender;
    aaa::ui::widgets::WidgetSystem* const ws = [self aaa_widget_system];
    if( ws == nullptr ) return;
    ws->on_text_input( 0x1B );   //  escape
}

- (void)keyUp:(NSEvent*)event
{
    NSUInteger const code = [event keyCode];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind     = aaa::input::EngineEvent::Kind::KEY_UP;
        ev.key_code = (std::int32_t) code;
        _eventBridge->push( ev );
    }

    //	c144 : MEU runner forward (release edge).
    if( aaa::meu::Runner* runner = [self aaa_meu_runner] )
    {
        runner->on_key_event( (int) code, false );
    }

    [_pressedKeysMutable removeObject:@(code)];
    _keyEventCount++;

    NSLog( @"AAASeed.input: keyUp code=%lu", (unsigned long) code );
}

- (NSSet< NSNumber* >*)pressedKeys
{
    return [[_pressedKeysMutable copy] autorelease];
}

- (NSInteger)keyEventCount
{
    return _keyEventCount;
}

//	--- Mouse + scroll (continuation 37) -------------------------------
//
//	Mouse events follow the same shape as keyboard : record state, log
//	to syslog for debugger-less observability, ingest into EventBridge
//	for the engine-side drain.
//
//	c133 : `mouseMoved:` IS now handled here (paired with the
//	updateTrackingAreas + viewDidMoveToWindow setup above).

//	Map AppKit button type to EngineEvent button index (0 / 1 / 2).
static std::int32_t aaa_button_for_event_type( NSEventType t )
{
    if( t == NSEventTypeLeftMouseDown  || t == NSEventTypeLeftMouseUp  ) { return 0; }
    if( t == NSEventTypeRightMouseDown || t == NSEventTypeRightMouseUp ) { return 1; }
    return 2;
}

- (void)mouseDown:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind   = aaa::input::EngineEvent::Kind::MOUSE_DOWN;
        ev.button = aaa_button_for_event_type( [event type] );
        ev.x      = _lastMouseLocation.x;
        ev.y      = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    //	c144 : MEU runner forward.
    if( aaa::meu::Runner* runner = [self aaa_meu_runner] )
    {
        runner->on_mouse_event( _lastMouseLocation.x, _lastMouseLocation.y,
                                aaa_button_for_event_type( [event type] ), true );
    }

    //	c147-A : mouse press EDGE forward to the MTKView delegate. The
    //	widget system consumes these edge flags once per frame to detect
    //	"click started this frame". Dispatched via NSObject's generic
    //	performSelector dispatch so this .mm stays link-time independent
    //	of AAASeedMTKView.mm (mirrors the aaa_meu_runner lookup pattern).
    {
        id< MTKViewDelegate > d = self.delegate;
        if( d != nil && [d respondsToSelector:@selector( queueMousePressed )] )
        {
            [(NSObject*) d performSelector:@selector( queueMousePressed )];
        }
    }

    [_mouseButtonsMutable addObject:@( NSEventTypeLeftMouseDown )];
    _mouseEventCount++;
    NSLog( @"AAASeed.input: mouseDown left at (%.1f, %.1f)",
           _lastMouseLocation.x, _lastMouseLocation.y );
}

- (void)mouseUp:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind   = aaa::input::EngineEvent::Kind::MOUSE_UP;
        ev.button = aaa_button_for_event_type( [event type] );
        ev.x      = _lastMouseLocation.x;
        ev.y      = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    //	c144 : MEU runner forward.
    if( aaa::meu::Runner* runner = [self aaa_meu_runner] )
    {
        runner->on_mouse_event( _lastMouseLocation.x, _lastMouseLocation.y,
                                aaa_button_for_event_type( [event type] ), false );
    }

    //	c147-A : mouse release EDGE forward to the MTKView delegate. See
    //	mouseDown: above for the dispatch pattern. The widget system
    //	consumes the release-edge to fire button click + release any
    //	in-progress slider drag.
    {
        id< MTKViewDelegate > d = self.delegate;
        if( d != nil && [d respondsToSelector:@selector( queueMouseReleased )] )
        {
            [(NSObject*) d performSelector:@selector( queueMouseReleased )];
        }
    }

    [_mouseButtonsMutable removeObject:@( NSEventTypeLeftMouseDown )];
    _mouseEventCount++;
    NSLog( @"AAASeed.input: mouseUp left at (%.1f, %.1f)",
           _lastMouseLocation.x, _lastMouseLocation.y );
}

- (void)rightMouseDown:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind   = aaa::input::EngineEvent::Kind::MOUSE_DOWN;
        ev.button = aaa_button_for_event_type( [event type] );
        ev.x      = _lastMouseLocation.x;
        ev.y      = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    [_mouseButtonsMutable addObject:@( NSEventTypeRightMouseDown )];
    _mouseEventCount++;
    NSLog( @"AAASeed.input: mouseDown right at (%.1f, %.1f)",
           _lastMouseLocation.x, _lastMouseLocation.y );
}

- (void)rightMouseUp:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind   = aaa::input::EngineEvent::Kind::MOUSE_UP;
        ev.button = aaa_button_for_event_type( [event type] );
        ev.x      = _lastMouseLocation.x;
        ev.y      = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    [_mouseButtonsMutable removeObject:@( NSEventTypeRightMouseDown )];
    _mouseEventCount++;
    NSLog( @"AAASeed.input: mouseUp right at (%.1f, %.1f)",
           _lastMouseLocation.x, _lastMouseLocation.y );
}

- (void)otherMouseDown:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind   = aaa::input::EngineEvent::Kind::MOUSE_DOWN;
        ev.button = aaa_button_for_event_type( [event type] );
        ev.x      = _lastMouseLocation.x;
        ev.y      = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    [_mouseButtonsMutable addObject:@( NSEventTypeOtherMouseDown )];
    _mouseEventCount++;
}

- (void)otherMouseUp:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind   = aaa::input::EngineEvent::Kind::MOUSE_UP;
        ev.button = aaa_button_for_event_type( [event type] );
        ev.x      = _lastMouseLocation.x;
        ev.y      = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    [_mouseButtonsMutable removeObject:@( NSEventTypeOtherMouseDown )];
    _mouseEventCount++;
}

- (void)mouseDragged:(NSEvent*)event
{
    //	`mouseDragged:` fires while a button is held -- engine code that
    //	wants drag deltas reads lastMouseLocation across frames.
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind = aaa::input::EngineEvent::Kind::MOUSE_MOVE;
        ev.x    = _lastMouseLocation.x;
        ev.y    = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    _mouseEventCount++;
}

- (void)rightMouseDragged:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind = aaa::input::EngineEvent::Kind::MOUSE_MOVE;
        ev.x    = _lastMouseLocation.x;
        ev.y    = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    _mouseEventCount++;
}

- (void)otherMouseDragged:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind = aaa::input::EngineEvent::Kind::MOUSE_MOVE;
        ev.x    = _lastMouseLocation.x;
        ev.y    = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    _mouseEventCount++;
}

//	c133 : pointer-without-button-held motion. Tracking-area + window
//	flag in viewDidMoveToWindow / updateTrackingAreas.
- (void)mouseMoved:(NSEvent*)event
{
    _lastMouseLocation = [event locationInWindow];

    if( _eventBridge )
    {
        aaa::input::EngineEvent ev;
        ev.kind = aaa::input::EngineEvent::Kind::MOUSE_MOVE;
        ev.x    = _lastMouseLocation.x;
        ev.y    = _lastMouseLocation.y;
        _eventBridge->push( ev );
    }

    _mouseEventCount++;
}

- (void)scrollWheel:(NSEvent*)event
{
    //	c133 : forward into GestureBridge BEFORE buffer push so a consumer
    //	draining the gesture queue immediately after dispatch sees it.
    if( _gestureBridge )
    {
        _gestureBridge->handle_ns_event( event );
    }

    //	c144 : MEU runner forward (additive).
    if( aaa::meu::Runner* runner = [self aaa_meu_runner] )
    {
        runner->on_scroll_event( [event scrollingDeltaX], [event scrollingDeltaY] );
    }

    self.lastScrollDelta = NSMakePoint( [event scrollingDeltaX], [event scrollingDeltaY] );
    _mouseEventCount++;
    NSLog( @"AAASeed.input: scrollWheel delta=(%.2f, %.2f)",
           self.lastScrollDelta.x, self.lastScrollDelta.y );
}

//	c133 : trackpad pinch ; GestureBridge picks the magnify queue based on
//	[event type].
- (void)magnifyWithEvent:(NSEvent*)event
{
    if( _gestureBridge )
    {
        _gestureBridge->handle_ns_event( event );
    }
    _mouseEventCount++;
}

//	c133 : trackpad two-finger rotate (NSEvent.rotation in DEGREES ; the
//	bridge converts to radians at ingest).
- (void)rotateWithEvent:(NSEvent*)event
{
    if( _gestureBridge )
    {
        _gestureBridge->handle_ns_event( event );
    }
    _mouseEventCount++;
}

//	c133 : three-finger swipe. AppKit reports a content-direction delta ;
//	the bridge inverts to finger-intent.
- (void)swipeWithEvent:(NSEvent*)event
{
    if( _gestureBridge )
    {
        _gestureBridge->handle_ns_event( event );
    }
    _mouseEventCount++;
}

- (NSSet< NSNumber* >*)mouseButtonsPressed
{
    return [[_mouseButtonsMutable copy] autorelease];
}

- (NSPoint)lastMouseLocation
{
    return _lastMouseLocation;
}

- (NSInteger)mouseEventCount
{
    return _mouseEventCount;
}

//	c133 view-side wiring : per-view bridge accessors.
- (aaa::input::EventBridge*)eventBridge
{
    return _eventBridge.get();
}

- (aaa::input::GestureBridge*)gestureBridge
{
    return _gestureBridge.get();
}

@end
