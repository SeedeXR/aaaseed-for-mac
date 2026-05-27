// tests/unit/input_view_test.mm
//
// Phase 4 input-plumbing unit test. Synthesizes NSEvent keyDown / keyUp
// events and verifies AAASeedInputView records them in its
// lastKeyCode + pressedKeys + keyEventCount surface.
//
// Why an Objective-C++ unit test : NSEvent is an AppKit type and the
// view's keyDown:/keyUp: handlers are ObjC methods. C++ GoogleTest
// alone can't synthesize / dispatch the events. The .mm extension lets
// gtest's harness coexist with AppKit calls.
//
// What this test DOES NOT do : run a full NSApplication event loop.
// We invoke keyDown:/keyUp: directly on the view -- that's the
// dispatch the responder chain would do, just without the loop.
// Sufficient to verify the data-path semantics. The full event-loop
// dispatch is exercised by the existing `aaaseed_app_smoke` integration
// test (the .app boots, draws frames, exits).
//
// CTest label : unit;input.

#import <Cocoa/Cocoa.h>
#import <gtest/gtest.h>

#import "src/ui/macos/AAASeedInputView.h"
#include "src/ui/macos/aaa_event_bridge.h"
#include "src/ui/macos/aaa_event_bridge_gesture.h"

namespace
{
    //	Build a synthetic NSEvent representing a keyDown / keyUp at the
    //	given key code. Uses the canonical NSEvent factory.
    NSEvent* make_key_event( NSEventType type, unsigned short code )
    {
        NSString* chars = @"";
        return [NSEvent keyEventWithType:type
                                location:NSZeroPoint
                           modifierFlags:0
                               timestamp:0
                            windowNumber:0
                                 context:nil
                              characters:chars
             charactersIgnoringModifiers:chars
                               isARepeat:NO
                                 keyCode:code];
    }
}

TEST( InputView, AcceptsFirstResponderReturnsYes )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        EXPECT_TRUE( [view acceptsFirstResponder] );
        EXPECT_TRUE( [view becomeFirstResponder] );
        [view release];
    }
}

TEST( InputView, KeyDownRecordsCodeAndIncrementsCount )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        EXPECT_EQ( view.lastKeyCode,     -1 );
        EXPECT_EQ( view.keyEventCount,    0 );
        EXPECT_EQ( view.pressedKeys.count, 0u );

        NSEvent* down_a = make_key_event( NSEventTypeKeyDown, 0 /* 'a' */ );
        [view keyDown:down_a];

        EXPECT_EQ( view.lastKeyCode,     0 );
        EXPECT_EQ( view.keyEventCount,   1 );
        EXPECT_EQ( view.pressedKeys.count, 1u );
        EXPECT_TRUE( [view.pressedKeys containsObject:@(0u)] );

        [view release];
    }
}

TEST( InputView, KeyUpRemovesFromPressedSet )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        [view keyDown:make_key_event( NSEventTypeKeyDown, 13 /* 'w' */ )];
        [view keyDown:make_key_event( NSEventTypeKeyDown, 1  /* 's' */ )];
        EXPECT_EQ( view.pressedKeys.count, 2u );

        [view keyUp:make_key_event( NSEventTypeKeyUp, 13 )];
        EXPECT_EQ( view.pressedKeys.count, 1u );
        EXPECT_FALSE( [view.pressedKeys containsObject:@(13u)] );
        EXPECT_TRUE(  [view.pressedKeys containsObject:@(1u)]  );

        EXPECT_EQ( view.keyEventCount, 3 );

        [view release];
    }
}

TEST( InputView, MultipleSameKeyDownDoesNotDoubleCount )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        [view keyDown:make_key_event( NSEventTypeKeyDown, 42 )];
        [view keyDown:make_key_event( NSEventTypeKeyDown, 42 )];
        [view keyDown:make_key_event( NSEventTypeKeyDown, 42 )];

        //	Set-based storage : same key code only counted once in
        //	pressedKeys. keyEventCount still tracks every event.
        EXPECT_EQ( view.pressedKeys.count, 1u );
        EXPECT_EQ( view.keyEventCount,     3  );

        [view release];
    }
}

//	--- Mouse coverage (continuation 37) -----------------------------

namespace
{
    NSEvent* make_mouse_event( NSEventType type, NSPoint loc, NSInteger button )
    {
        return [NSEvent mouseEventWithType:type
                                  location:loc
                             modifierFlags:0
                                 timestamp:0
                              windowNumber:0
                                   context:nil
                               eventNumber:0
                                clickCount:1
                                  pressure:1.0];
        (void) button;
    }
}

TEST( InputView, LeftMouseDownAndUpRecordsLocationAndSet )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        EXPECT_EQ( view.mouseEventCount,         0  );
        EXPECT_EQ( view.mouseButtonsPressed.count, 0u );

        NSPoint at = NSMakePoint( 12.5, 34.0 );
        [view mouseDown:make_mouse_event( NSEventTypeLeftMouseDown, at, 0 )];

        EXPECT_EQ( view.lastMouseLocation.x,  12.5 );
        EXPECT_EQ( view.lastMouseLocation.y,  34.0 );
        EXPECT_EQ( view.mouseEventCount,      1    );
        EXPECT_EQ( view.mouseButtonsPressed.count, 1u );

        [view mouseUp:make_mouse_event( NSEventTypeLeftMouseUp, at, 0 )];
        EXPECT_EQ( view.mouseButtonsPressed.count, 0u );
        EXPECT_EQ( view.mouseEventCount,           2  );

        [view release];
    }
}

TEST( InputView, RightAndOtherMouseTracked )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        NSPoint at = NSMakePoint( 1, 2 );
        [view rightMouseDown:make_mouse_event( NSEventTypeRightMouseDown, at, 1 )];
        [view otherMouseDown:make_mouse_event( NSEventTypeOtherMouseDown, at, 2 )];
        EXPECT_EQ( view.mouseButtonsPressed.count, 2u );

        [view rightMouseUp:make_mouse_event( NSEventTypeRightMouseUp, at, 1 )];
        EXPECT_EQ( view.mouseButtonsPressed.count, 1u );

        [view otherMouseUp:make_mouse_event( NSEventTypeOtherMouseUp, at, 2 )];
        EXPECT_EQ( view.mouseButtonsPressed.count, 0u );

        [view release];
    }
}

TEST( InputView, MouseDraggedUpdatesLocationOnly )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        [view mouseDown:make_mouse_event( NSEventTypeLeftMouseDown,
                                          NSMakePoint( 10, 20 ), 0 )];
        EXPECT_EQ( view.mouseButtonsPressed.count, 1u );

        [view mouseDragged:make_mouse_event( NSEventTypeLeftMouseDragged,
                                             NSMakePoint( 30, 40 ), 0 )];
        EXPECT_EQ( view.lastMouseLocation.x, 30.0 );
        EXPECT_EQ( view.lastMouseLocation.y, 40.0 );
        //	Dragging does NOT add/remove buttons from the pressed set ;
        //	the mouseDown that started the drag is still in there.
        EXPECT_EQ( view.mouseButtonsPressed.count, 1u );
        EXPECT_EQ( view.mouseEventCount,           2  );

        [view release];
    }
}

//	NOTE : a `scrollWheel:` synthesizable-NSEvent test was attempted in
//	continuation 37 but NSEvent's `+mouseEventWithType:` factory throws
//	an exception when fed `NSEventTypeScrollWheel`. Real scroll events
//	come from CGEvent / IOHID and synthesizing them in a unit test is
//	awkward without a running event loop. The scrollWheel: handler
//	follows the same shape as the mouse handlers (counter increment,
//	delta stash), so the pattern is covered by the mouse tests. Real
//	scroll dispatch is exercised at runtime in the .app bundle.

//	--- c133 : view-side bridge wiring coverage -----------------------
//
//	AAASeedInputView now owns a per-view EventBridge (c119-B) +
//	GestureBridge (c132-B). Every NSResponder override pushes its
//	NSEvent into the matching bridge BEFORE updating local buffers.
//	These tests verify the forward path AND that c36-c37's local-buffer
//	behavior is still intact (regression guard).
//
//	NSEvent synthesis caveat (same as the older block above) : AppKit's
//	`+mouseEventWithType:` THROWS for NSEventTypeScrollWheel /
//	NSEventTypeMagnify / NSEventTypeRotate / NSEventTypeSwipe. So for
//	those four families the forwarding tests :
//	  - call the view's gesture override with a `nil` event (must not
//	    crash ; the bridge's handle_ns_event guards nil),
//	  - then `post_*` directly into the per-view bridge via the
//	    `gestureBridge` accessor and drain via the same accessor to
//	    prove the per-view ownership wire is intact (the view's bridge
//	    pointer IS the one consumers reach through the accessor).

TEST( InputView, KeyEventForwardsToEventBridge )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );
        ASSERT_NE( [view eventBridge], nullptr );

        //	Bridge starts empty.
        EXPECT_EQ( [view eventBridge]->pending(), 0u );

        [view keyDown:make_key_event( NSEventTypeKeyDown, 99 )];
        [view keyUp  :make_key_event( NSEventTypeKeyUp,   99 )];

        //	2 events queued : KEY_DOWN then KEY_UP.
        EXPECT_EQ( [view eventBridge]->pending(), 2u );

        aaa::input::EngineEvent ev;
        ASSERT_TRUE( [view eventBridge]->next_event( ev ) );
        EXPECT_EQ( ev.kind,     aaa::input::EngineEvent::Kind::KEY_DOWN );
        EXPECT_EQ( ev.key_code, 99 );

        ASSERT_TRUE( [view eventBridge]->next_event( ev ) );
        EXPECT_EQ( ev.kind,     aaa::input::EngineEvent::Kind::KEY_UP );
        EXPECT_EQ( ev.key_code, 99 );

        EXPECT_FALSE( [view eventBridge]->next_event( ev ) );

        [view release];
    }
}

TEST( InputView, MouseEventForwardsToEventBridge )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );
        ASSERT_NE( [view eventBridge], nullptr );

        NSPoint at = NSMakePoint( 7.5, 11.25 );
        [view mouseDown:make_mouse_event( NSEventTypeLeftMouseDown,  at, 0 )];
        [view mouseUp  :make_mouse_event( NSEventTypeLeftMouseUp,    at, 0 )];
        [view rightMouseDown:make_mouse_event( NSEventTypeRightMouseDown, at, 1 )];

        EXPECT_EQ( [view eventBridge]->pending(), 3u );

        aaa::input::EngineEvent ev;
        ASSERT_TRUE( [view eventBridge]->next_event( ev ) );
        EXPECT_EQ( ev.kind,   aaa::input::EngineEvent::Kind::MOUSE_DOWN );
        EXPECT_EQ( ev.button, 0 );
        EXPECT_DOUBLE_EQ( ev.x,  7.5  );
        EXPECT_DOUBLE_EQ( ev.y, 11.25 );

        ASSERT_TRUE( [view eventBridge]->next_event( ev ) );
        EXPECT_EQ( ev.kind,   aaa::input::EngineEvent::Kind::MOUSE_UP );
        EXPECT_EQ( ev.button, 0 );

        ASSERT_TRUE( [view eventBridge]->next_event( ev ) );
        EXPECT_EQ( ev.kind,   aaa::input::EngineEvent::Kind::MOUSE_DOWN );
        EXPECT_EQ( ev.button, 1 );

        [view release];
    }
}

TEST( InputView, ScrollEventForwardsToGestureBridge )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );
        ASSERT_NE( [view gestureBridge], nullptr );

        //	Forward path : nil-tolerant (AppKit can't manufacture real
        //	scroll NSEvents in a hermetic unit test). Typed-nil cast
        //	silences the clang -Wnonnull warning on the NSEvent param.
        //	Runtime-aliased nil (volatile) so clang's -Wnonnull static
        //	analyzer can't fold the value back to a literal nil before
        //	the message send. The view's gesture overrides + the bridge
        //	guard nil at runtime ; verifying that contract is the point.
        NSEvent* volatile nil_event_v = nil;
        NSEvent* const nil_event = nil_event_v;
        [view scrollWheel:nil_event];
        EXPECT_EQ( [view gestureBridge]->pending_scroll(), 0u );

        //	Per-view ownership wire : posting through the accessor lands
        //	in the same bridge the view forwards into.
        [view gestureBridge]->post_scroll_event( 2.5, -1.0, /*momentum=*/false, /*ts=*/0 );
        EXPECT_EQ( [view gestureBridge]->pending_scroll(), 1u );

        std::vector< aaa::input::ScrollEvent > events =
            [view gestureBridge]->drain_scroll_events();
        ASSERT_EQ( events.size(), 1u );
        EXPECT_DOUBLE_EQ( events[ 0 ].dx,  2.5 );
        EXPECT_DOUBLE_EQ( events[ 0 ].dy, -1.0 );

        [view release];
    }
}

TEST( InputView, MagnifyEventForwardsToGestureBridge )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );
        ASSERT_NE( [view gestureBridge], nullptr );

        //	nil-tolerant forward (real NSEventTypeMagnify is not
        //	synthesizable via AppKit's public factories).
        //	Runtime-aliased nil (volatile) so clang's -Wnonnull static
        //	analyzer can't fold the value back to a literal nil before
        //	the message send. The view's gesture overrides + the bridge
        //	guard nil at runtime ; verifying that contract is the point.
        NSEvent* volatile nil_event_v = nil;
        NSEvent* const nil_event = nil_event_v;
        [view magnifyWithEvent:nil_event];
        EXPECT_EQ( [view gestureBridge]->pending_magnify(), 0u );

        //	Per-view-ownership round-trip.
        [view gestureBridge]->post_magnify_event( 0.20 );
        [view gestureBridge]->post_magnify_event( 0.05 );
        EXPECT_EQ( [view gestureBridge]->pending_magnify(), 2u );

        std::vector< aaa::input::MagnifyEvent > events =
            [view gestureBridge]->drain_magnify_events();
        ASSERT_EQ( events.size(), 2u );
        EXPECT_DOUBLE_EQ( events[ 0 ].delta,      0.20 );
        EXPECT_DOUBLE_EQ( events[ 0 ].cumulative, 0.20 );
        EXPECT_DOUBLE_EQ( events[ 1 ].delta,      0.05 );
        EXPECT_DOUBLE_EQ( events[ 1 ].cumulative, 0.25 );

        [view release];
    }
}

TEST( InputView, RotateEventForwardsToGestureBridge )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );
        ASSERT_NE( [view gestureBridge], nullptr );

        //	Runtime-aliased nil (volatile) so clang's -Wnonnull static
        //	analyzer can't fold the value back to a literal nil before
        //	the message send. The view's gesture overrides + the bridge
        //	guard nil at runtime ; verifying that contract is the point.
        NSEvent* volatile nil_event_v = nil;
        NSEvent* const nil_event = nil_event_v;
        [view rotateWithEvent:nil_event];
        EXPECT_EQ( [view gestureBridge]->pending_rotate(), 0u );

        //	Verify the deg->rad conversion utility is still wired and
        //	the per-view bridge round-trips it. 90 deg = pi/2.
        double const rad_90 =
            aaa::input::GestureBridge::degrees_to_radians( 90.0 );
        [view gestureBridge]->post_rotate_event( rad_90 );

        std::vector< aaa::input::RotateEvent > events =
            [view gestureBridge]->drain_rotate_events();
        ASSERT_EQ( events.size(), 1u );
        EXPECT_NEAR( events[ 0 ].delta_rad,      1.5707963267948966, 1.0e-12 );
        EXPECT_NEAR( events[ 0 ].cumulative_rad, 1.5707963267948966, 1.0e-12 );

        //	Swipe override coverage in the same TU : nil-tolerant + no
        //	queue change since handle_ns_event(nil) returns false. Keeps
        //	the new override compiled-against + reachable.
        [view swipeWithEvent:nil_event];
        EXPECT_EQ( [view gestureBridge]->pending_swipe(), 0u );

        [view release];
    }
}

TEST( InputView, LocalBuffersStillPopulated )
{
    //	Regression guard for c36-c37 : the existing NSMutableSet /
    //	NSPoint / counter side-buffers MUST continue to fill exactly as
    //	before, even with the new bridge-forward wiring on top.
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        //	Keyboard side-buffer.
        [view keyDown:make_key_event( NSEventTypeKeyDown, 7 )];
        EXPECT_EQ( view.lastKeyCode,       7 );
        EXPECT_EQ( view.keyEventCount,     1 );
        EXPECT_EQ( view.pressedKeys.count, 1u );
        EXPECT_TRUE( [view.pressedKeys containsObject:@(7u)] );

        [view keyUp:make_key_event( NSEventTypeKeyUp, 7 )];
        EXPECT_EQ( view.pressedKeys.count, 0u );
        EXPECT_EQ( view.keyEventCount,     2 );

        //	Mouse side-buffer.
        NSPoint at = NSMakePoint( 50, 60 );
        [view mouseDown:make_mouse_event( NSEventTypeLeftMouseDown, at, 0 )];
        EXPECT_EQ( view.lastMouseLocation.x, 50.0 );
        EXPECT_EQ( view.lastMouseLocation.y, 60.0 );
        EXPECT_EQ( view.mouseButtonsPressed.count, 1u );
        EXPECT_EQ( view.mouseEventCount,           1 );

        [view mouseUp:make_mouse_event( NSEventTypeLeftMouseUp, at, 0 )];
        EXPECT_EQ( view.mouseButtonsPressed.count, 0u );
        EXPECT_EQ( view.mouseEventCount,           2 );

        //	mouseMoved: side-buffer (NEW c133 override) -- updates the
        //	same lastMouseLocation + counter without touching the
        //	pressed-buttons set.
        NSPoint at2 = NSMakePoint( 12, 34 );
        [view mouseMoved:make_mouse_event( NSEventTypeMouseMoved, at2, 0 )];
        EXPECT_EQ( view.lastMouseLocation.x, 12.0 );
        EXPECT_EQ( view.lastMouseLocation.y, 34.0 );
        EXPECT_EQ( view.mouseButtonsPressed.count, 0u );
        EXPECT_EQ( view.mouseEventCount,           3 );

        [view release];
    }
}
