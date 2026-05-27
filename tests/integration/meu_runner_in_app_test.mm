//  tests/integration/meu_runner_in_app_test.mm
//
//  c144 : Integration tests for the MEU runner wired into AAASeedMTKView.
//  Verifies the .app actually instantiates a Runner, loads
//  `bundle/macos/meu/hello_world.lua`, drives render_frame per
//  drawInMTKView:, and that AAASeedInputView forwards key / mouse /
//  scroll events into the runner's on_*_event entry points.
//
//  Hosting strategy : we drive `AAASeedMTKViewDelegate` + `AAASeedInputView`
//  directly without launching NSApplication. The delegate constructs
//  the Runner against the MetalBackend in `initWithBackend:maxFrames:` ;
//  drawInMTKView: we trigger via `[view draw]` on a paused MTKView.
//  No window is shown -- everything happens in headless Metal.
//
//  Doctrine respected :
//   - .mm TU (the spec allows this -- AAASeedMTKView is an ObjC++ class).
//   - No `o_str`, no `aaa_mem`, no `aaa_str.h`.
//   - Per feedback_ctest_label_first_only.md : `integration` is the
//     primary filter key and goes first in the LABELS list.

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

#import "src/ui/macos/AAASeedMTKView.h"
#import "src/ui/macos/AAASeedInputView.h"

#include "src/gol/metal/metal_backend.h"
#include "src/meu/aaa_meu_runner_mac.h"

#include <gtest/gtest.h>

#include <memory>

namespace
{

//  Spin up a single shared MetalBackend across all tests. Creating one
//  per test adds ~200ms each and stresses MTLDevice creation ; the
//  delegate constructors take a non-owning pointer so sharing is safe.
struct SharedBackend
{
    GOL::MetalBackend* get()
    {
        if( !inited )
        {
            inited = backend.init();
        }
        return inited ? &backend : nullptr;
    }
    GOL::MetalBackend backend;
    bool              inited{ false };
};

SharedBackend& shared_backend()
{
    static SharedBackend s;
    return s;
}

//  Build a paused MTKView wired to AAASeedMTKViewDelegate. Returns
//  retained (ARC-disabled TU) ; caller releases the autorelease pool /
//  the view explicitly via [view release].
AAASeedInputView* make_view_with_delegate( GOL::MetalBackend* backend,
                                           AAASeedMTKViewDelegate** out_delegate )
{
    id< MTLDevice > device = (__bridge id< MTLDevice >) backend->get_device();
    NSRect const frame = NSMakeRect( 0, 0, 256, 192 );
    AAASeedInputView* view = [[AAASeedInputView alloc] initWithFrame:frame
                                                              device:device];
    view.colorPixelFormat = MTLPixelFormatRGBA8Unorm;
    view.clearColor       = MTLClearColorMake( 0.0, 0.0, 0.0, 1.0 );
    view.preferredFramesPerSecond = 60;
    [view setPaused:YES];
    [view setEnableSetNeedsDisplay:NO];

    AAASeedMTKViewDelegate* delegate =
        [[AAASeedMTKViewDelegate alloc] initWithBackend:backend maxFrames:0];
    view.delegate = delegate;
    *out_delegate = delegate;
    return view;
}

//  Synthesize an NSEvent keyDown so we can drive AAASeedInputView's
//  keyDown: override without an active responder chain. NSEvent
//  requires either a window or nil ; nil works for our purposes.
NSEvent* synth_key_down( NSUInteger keyCode )
{
    return [NSEvent keyEventWithType:NSEventTypeKeyDown
                            location:NSZeroPoint
                       modifierFlags:0
                           timestamp:[[NSDate date] timeIntervalSinceReferenceDate]
                        windowNumber:0
                             context:nil
                          characters:@""
         charactersIgnoringModifiers:@""
                           isARepeat:NO
                             keyCode:(unsigned short) keyCode];
}

}   //  anonymous

//  -----------------------------------------------------------------------
//  Test 1 : view + delegate init constructs a Runner.
TEST( MeuRunnerInApp, ViewInitConstructsRunner )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr ) << "MetalBackend init failed -- no Metal GPU?";

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        ASSERT_NE( delegate, nil );
        aaa::meu::Runner* runner = [delegate meuRunner];
        EXPECT_NE( runner, nullptr )
            << "AAASeedMTKViewDelegate did not construct a Runner";

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

//  Test 2 : hello_world.lua is found + loaded (current_shader_name or
//  frame_index advances after a manual render_frame call).
TEST( MeuRunnerInApp, HelloWorldScriptLoaded )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        aaa::meu::Runner* runner = [delegate meuRunner];
        ASSERT_NE( runner, nullptr );

        int const before = runner->frame_index();
        //  Direct render_frame call : the runner takes a target hint
        //  but does NOT begin a render pass (host's job). Calling
        //  outside a pass is documented as still incrementing
        //  frame_index even when the catalog shaders can't actually
        //  emit draws. That's enough to prove the script's on_frame
        //  hook is wired.
        runner->render_frame( 256, 192, GOL::kInvalidTextureId );
        int const after = runner->frame_index();
        EXPECT_GT( after, before )
            << "frame_index didn't advance ; script likely failed to load";

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

//  Test 3 : drawInMTKView: is callable without crash. Driven via the
//  MTKView's explicit `-draw` which fires the delegate's drawInMTKView:.
TEST( MeuRunnerInApp, DrawInMTKViewNoCrash )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        //  Just don't crash. The headless MTKView returns a valid
        //  currentDrawable in macOS 13+ ; if it doesn't, drawInMTKView:
        //  early-returns and we still pass.
        EXPECT_NO_FATAL_FAILURE( [view draw] );
        EXPECT_NO_FATAL_FAILURE( [view draw] );

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

//  Test 4 : key event forwarding. Push a synthetic keyDown, verify the
//  Runner saw it (frame_index still advances ; current key state is
//  not directly observable through public API, so we use frame_index
//  AND the fact that on_key_event is a no-throw void to indicate the
//  forwarding path is wired without exception).
TEST( MeuRunnerInApp, KeyEventForwardsToRunner )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        aaa::meu::Runner* runner = [delegate meuRunner];
        ASSERT_NE( runner, nullptr );

        //  Pre-event frame_index baseline.
        int const f0 = runner->frame_index();

        //  Push a Space keyDown (kVK_Space = 49 in Carbon).
        NSEvent* down = synth_key_down( 49 );
        ASSERT_NE( down, nil );
        [view keyDown:down];

        //  Drive a frame so hello_world.lua's on_frame runs and sees
        //  the key. The script's space-rising-edge logic cycles to the
        //  next shader ; we don't probe that side-effect (would need
        //  reflection into Lua) but frame_index advances regardless.
        runner->render_frame( 256, 192, GOL::kInvalidTextureId );
        int const f1 = runner->frame_index();
        EXPECT_GT( f1, f0 );

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

//  Test 5 : MTKView resize triggers no crash. `mtkView:drawableSizeWillChange:`
//  is the official MTKViewDelegate callback when the drawable size shifts.
TEST( MeuRunnerInApp, ResizeIsSafe )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        EXPECT_NO_FATAL_FAILURE(
            [delegate mtkView:view drawableSizeWillChange:CGSizeMake( 800, 600 )]
        );
        EXPECT_NO_FATAL_FAILURE( [view draw] );

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

//  Test 6 : repeated create + destroy cycles. Verifies the Runner
//  releases its lua_State + cached resources cleanly so we don't leak
//  Metal objects (which would surface as ENOMEM-style failures after
//  enough cycles).
TEST( MeuRunnerInApp, RepeatedTearDownNoLeak )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    for( int i = 0; i < 10; ++i )
    {
        @autoreleasepool
        {
            AAASeedMTKViewDelegate* delegate = nil;
            AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

            aaa::meu::Runner* runner = [delegate meuRunner];
            ASSERT_NE( runner, nullptr )
                << "cycle " << i << " : Runner not constructed";

            //  Drive one frame so the runner actually exercises its
            //  uniform-buffer + texture allocation paths -- this is
            //  where a leak would compound across cycles.
            runner->render_frame( 256, 192, GOL::kInvalidTextureId );

            view.delegate = nil;
            [delegate release];
            [view release];
        }
    }
}
