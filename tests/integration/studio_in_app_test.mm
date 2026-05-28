//  tests/integration/studio_in_app_test.mm
//
//  c148 : Integration tests for the AAASeed ImGui Studio authoring surface
//  wired into AAASeedMTKView.
//
//  Strategy : We spin up a real GOL::MetalBackend and an AAASeedMTKViewDelegate
//  (which internally constructs the Studio), then drive it via the accessor
//  `[delegate studio]`. We do NOT launch NSApplication and do NOT open a
//  window — the Studio is exercised headlessly via its pure-C++ data model
//  API, verifying that wiring succeeded and the Studio survives multiple
//  simulated frames via [view draw].
//
//  Doctrine:
//   - .mm TU (AAASeedMTKView is ObjC++; we use it directly).
//   - -fno-objc-arc (matches src/ui/macos/CMakeLists.txt).
//   - No o_str / aaa_mem / aaa_str / aaaseed_code_utils.
//   - Labels: "integration;studio".

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

#import "src/ui/macos/AAASeedMTKView.h"
#import "src/ui/macos/AAASeedInputView.h"

#include "src/gol/metal/metal_backend.h"
#include "src/ui/studio/aaa_studio.h"

#include <gtest/gtest.h>
#include <memory>

namespace
{

// ---------------------------------------------------------------------------
// Shared real MetalBackend (expensive to create; reuse across tests)
// ---------------------------------------------------------------------------
struct SharedBackend
{
    GOL::MetalBackend* get()
    {
        if( !inited )
            inited = backend.init();
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

// ---------------------------------------------------------------------------
// Build a paused MTKView with the app delegate wired up
// ---------------------------------------------------------------------------
AAASeedInputView* make_view( GOL::MetalBackend* backend,
                              AAASeedMTKViewDelegate** out_delegate )
{
    id< MTLDevice > device = (__bridge id< MTLDevice >) backend->get_device();
    NSRect const frame     = NSMakeRect( 0, 0, 320, 240 );

    AAASeedInputView* view = [[AAASeedInputView alloc] initWithFrame:frame
                                                              device:device];
    view.colorPixelFormat          = MTLPixelFormatRGBA8Unorm;
    view.clearColor                = MTLClearColorMake( 0.0, 0.0, 0.0, 1.0 );
    view.preferredFramesPerSecond  = 60;
    [view setPaused:YES];
    [view setEnableSetNeedsDisplay:NO];

    AAASeedMTKViewDelegate* delegate =
        [[AAASeedMTKViewDelegate alloc] initWithBackend:backend maxFrames:0];
    view.delegate = delegate;
    *out_delegate = delegate;
    return view;
}

} // anonymous

// ---------------------------------------------------------------------------
// Test 1 : Studio accessor returns non-null after a draw tick
// ---------------------------------------------------------------------------
TEST( StudioInApp, StudioAccessorNonNullAfterDraw )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr ) << "MetalBackend init failed — no Metal GPU?";

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView*       view     = make_view( backend, &delegate );
        ASSERT_NE( delegate, nil );

        // Drive one render tick so the lazy Studio init inside
        // drawInMTKView: fires.
        [view draw];

        aaa::ui::studio::Studio* studio = [delegate studio];
        EXPECT_NE( studio, nullptr )
            << "Studio was not created after first drawInMTKView: tick";

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

// ---------------------------------------------------------------------------
// Test 2 : Studio data model is functional after in-app init
// ---------------------------------------------------------------------------
TEST( StudioInApp, StudioDataModelUsableAfterInit )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView*       view     = make_view( backend, &delegate );

        [view draw];

        aaa::ui::studio::Studio* studio = [delegate studio];
        ASSERT_NE( studio, nullptr );

        // Node graph round-trip inside a real in-app studio.
        std::uint32_t id = studio->add_node( "InAppNode", 10.0f, 20.0f );
        EXPECT_NE( id, 0u );
        ASSERT_NE( studio->find_node( id ), nullptr );
        EXPECT_EQ( studio->find_node( id )->label, "InAppNode" );

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

// ---------------------------------------------------------------------------
// Test 3 : Console log survives multiple draw ticks (no crash, no leak)
// ---------------------------------------------------------------------------
TEST( StudioInApp, ConsoleLogSurvivesMultipleFrames )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView*       view     = make_view( backend, &delegate );

        [view draw];

        aaa::ui::studio::Studio* studio = [delegate studio];
        ASSERT_NE( studio, nullptr );

        for( int i = 0; i < 30; ++i )
        {
            studio->log( aaa::ui::studio::ConsoleEntry::INFO,
                         "frame " + std::to_string( i ) );
            //  c151-A : new_frame MUST be paired with render() to balance
            //  ImGui's frame lifecycle. Passing a null encoder makes
            //  render() emit no GPU work but still calls EndFrame().
            studio->new_frame( 320u, 240u, 1.0 / 60.0 );
            studio->render( nullptr );
        }

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

// ---------------------------------------------------------------------------
// Test 4 : Camera state survives round-trip through an in-app studio
// ---------------------------------------------------------------------------
TEST( StudioInApp, CameraStateRoundTripInApp )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView*       view     = make_view( backend, &delegate );
        [view draw];

        aaa::ui::studio::Studio* studio = [delegate studio];
        ASSERT_NE( studio, nullptr );

        aaa::ui::studio::CameraState cam;
        cam.pos[0] = 5.0f; cam.pos[1] = 3.0f; cam.pos[2] = 7.0f;
        cam.fov_deg = 45.0f;
        studio->set_camera( cam );

        EXPECT_FLOAT_EQ( studio->camera().pos[0],  5.0f  );
        EXPECT_FLOAT_EQ( studio->camera().fov_deg, 45.0f );

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}
