// AAASeedAppDelegate.mm — top-level Cocoa app delegate. Creates the
// NSWindow + MTKView + AAASeedMTKViewDelegate trio and owns the C++
// GOL::MetalBackend instance for the app's lifetime.

#import "AAASeedAppDelegate.h"
#import "AAASeedMTKView.h"
#import "AAASeedInputView.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "src/gol/metal/metal_backend.h"

#include <memory>

@implementation AAASeedAppDelegate
{
    NSWindow*                    _window;
    MTKView*                     _mtkView;
    AAASeedMTKViewDelegate*      _mtkDelegate;
    std::unique_ptr< GOL::MetalBackend > _backend;
    NSTimer*                     _quitWatchdog;
    NSTimer*                     _drawPump;
    NSInteger                    _frameCountObserved;
}

@synthesize maxFrames = _maxFrames;

- (instancetype)init
{
    self = [super init];
    if( self )
    {
        _maxFrames = 0;
    }
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    (void) notification;

    //	1. Initialize the C++ Metal backend before the view tries to render.
    _backend = std::make_unique< GOL::MetalBackend >();
    if( !_backend->init() )
    {
        NSLog( @"AAASeed: MetalBackend::init() failed (no Metal-capable GPU?)" );
        [NSApp terminate:nil];
        return;
    }

    //	2. Create the window. 512x384 is small enough to be ignored, big
    //	enough to show "yes, there's a window with a red rectangle in it".
    NSRect const frame = NSMakeRect( 200, 200, 512, 384 );
    NSWindowStyleMask const style =
        NSWindowStyleMaskTitled    |
        NSWindowStyleMaskClosable  |
        NSWindowStyleMaskMiniaturizable;

    _window = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:style
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    [_window setTitle:@"AAASeed (Metal)"];

    //	3. Create the MTKView. Hand it the same MTLDevice the backend
    //	already created — no double-allocation.
    //	Continuation 36 : use AAASeedInputView (MTKView subclass) so the
    //	view accepts keyboard focus and captures keyDown/keyUp.
    id< MTLDevice > device = (__bridge id< MTLDevice >) _backend->get_device();
    _mtkView = [[AAASeedInputView alloc] initWithFrame:frame device:device];
    _mtkView.colorPixelFormat = MTLPixelFormatRGBA8Unorm;
    _mtkView.clearColor       = MTLClearColorMake( 1.0, 0.0, 0.0, 1.0 );
    _mtkView.preferredFramesPerSecond = 60;

    _mtkDelegate = [[AAASeedMTKViewDelegate alloc] initWithBackend:_backend.get()
                                                          maxFrames:_maxFrames];
    _mtkView.delegate = _mtkDelegate;

    [_window setContentView:_mtkView];
    [_window center];
    [_window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    //	Promote the input view to first responder so key events route to
    //	it without the user clicking the view first. (Continuation 36)
    [_window makeFirstResponder:_mtkView];

    //	4. If maxFrames is set, install a polling timer that watches the
    //	delegate's shouldTerminate flag (it flips when the view has drawn
    //	N frames) PLUS a hard wall-clock fallback that fires after N
    //	seconds regardless. Terminal-launched binaries without an .app
    //	bundle sometimes never receive draw callbacks (focus / activation
    //	quirks) ; the wall-clock fallback guarantees the binary always
    //	exits — important for CI smoke tests.
    if( _maxFrames > 0 )
    {
        //	Manual frame pump (continuation 56). When the binary is
        //	launched from a non-interactive shell (popen / pipe), the
        //	MTKView's internal CVDisplayLink does NOT fire reliably --
        //	there's no display attached and the runloop may never get
        //	a refresh-tick. We drive frames manually at 60 Hz so the
        //	per-frame perf test gets steady-state measurements without
        //	depending on the display server.
        [_mtkView setPaused:YES];
        [_mtkView setEnableSetNeedsDisplay:NO];
        _drawPump = [NSTimer scheduledTimerWithTimeInterval:( 1.0 / 60.0 )
                                                      target:self
                                                    selector:@selector(pumpDraw:)
                                                    userInfo:nil
                                                     repeats:YES];

        _quitWatchdog = [NSTimer scheduledTimerWithTimeInterval:0.05
                                                          target:self
                                                        selector:@selector(checkQuit:)
                                                        userInfo:nil
                                                         repeats:YES];
        //	Wall-clock fallback : at most maxFrames seconds. Generous —
        //	at 60 fps a single frame is ~17 ms so this is well above the
        //	expected exit time and only fires if drawInMTKView never
        //	runs at all.
        NSTimeInterval const wallSeconds = (NSTimeInterval) _maxFrames;
        dispatch_after( dispatch_time( DISPATCH_TIME_NOW, (int64_t)( wallSeconds * NSEC_PER_SEC ) ),
                        dispatch_get_main_queue(),
                        ^{
                            NSLog( @"AAASeed: wall-clock timeout (%g s) reached, "
                                   @"terminating. drawInMTKView fired %ld times.",
                                   wallSeconds, (long) self->_frameCountObserved );
                            [NSApp terminate:nil];
                        } );
    }
}


- (void)pumpDraw:(NSTimer*)timer
{
    (void) timer;
    //	Force the MTKView delegate's drawInMTKView to fire on the main
    //	runloop. The MTKView has setPaused:YES so its internal display
    //	link is silent ; -draw is the explicit one-frame request.
    [_mtkView draw];
    ++_frameCountObserved;
}

- (void)checkQuit:(NSTimer*)timer
{
    (void) timer;
    if( _mtkDelegate.shouldTerminate )
    {
        [_drawPump invalidate];
        _drawPump = nil;
        [_quitWatchdog invalidate];
        _quitWatchdog = nil;
        [NSApp terminate:nil];
    }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    (void) sender;
    return YES;
}

- (void)applicationWillTerminate:(NSNotification*)notification
{
    (void) notification;
    _mtkView.delegate = nil;
    _mtkDelegate      = nil;
    _backend.reset();
}

@end
