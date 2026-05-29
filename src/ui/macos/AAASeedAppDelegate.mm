// AAASeedAppDelegate.mm — top-level Cocoa app delegate. Creates the
// NSWindow + MTKView + AAASeedMTKViewDelegate trio and owns the C++
// GOL::MetalBackend instance for the app's lifetime.

#import "AAASeedAppDelegate.h"
#import "AAASeedMTKView.h"
#import "AAASeedInputView.h"

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "src/gol/metal/metal_backend.h"
// c152-D : full Runner header for the projectPath load path.
#include "src/meu/aaa_meu_runner_mac.h"

#include <memory>
#include <string>

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

    //	2. Create the window. c151-C : sized for the Studio authoring
    //	surface (1440x900) ; resizable + fullscreen-capable so the
    //	green traffic-light enters native fullscreen and the corner
    //	drag-handle resizes. Without NSWindowStyleMaskResizable the
    //	zoom button was a no-op and Cmd+Ctrl+F did nothing.
    NSRect const frame = NSMakeRect( 100, 100, 1440, 900 );
    NSWindowStyleMask const style =
        NSWindowStyleMaskTitled         |
        NSWindowStyleMaskClosable       |
        NSWindowStyleMaskMiniaturizable |
        NSWindowStyleMaskResizable;

    _window = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:style
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    [_window setTitle:@"AAASeed"];
    //	c151-C : opt-in to native fullscreen so the green traffic-light
    //	enters Mission Control fullscreen instead of just zoom-to-fit.
    //	Cmd+Ctrl+F (the system "Enter Full Screen" shortcut) also works.
    [_window setCollectionBehavior:
        ( [_window collectionBehavior] |
          NSWindowCollectionBehaviorFullScreenPrimary )];
    //	c151-C : minimum size so panel layout doesn't break.
    [_window setMinSize:NSMakeSize( 800, 600 )];

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

    // c152-D : if launched with `--project <path>`, replace the
    // bundled hello_world.lua that initWithBackend just loaded with
    // the user-supplied .aaaproj.lua. The Studio's Play button
    // spawns us this way.
    if( self.projectPath && [self.projectPath length] > 0 )
    {
        auto* runner = [_mtkDelegate meuRunner];
        if( runner )
        {
            std::string const path =
                std::string( [self.projectPath fileSystemRepresentation] );
            if( runner->load_script( path ) )
            {
                NSLog( @"AAASeed: runtime loaded project '%s'",
                       path.c_str() );
                runner->enable_file_watch();
            }
            else
            {
                NSLog( @"AAASeed: project load_script('%s') FAILED ; "
                       @"falling back to bundled hello_world.lua.",
                       path.c_str() );
            }
        }
    }

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
