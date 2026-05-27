//  tests/integration/widgets_in_app_test.mm
//
//  c147-A : Integration tests for the immediate-mode widget system
//  wired into AAASeedMTKView. Verifies :
//    1. The view constructs a WidgetSystem in init.
//    2. A Lua script calling `aaa.ui.slider(...)` lands a single widget
//       in the system's per-frame queue (count == 1).
//    3. Synthesizing a mouse-down inside the slider's rect flips
//       last_frame_had_interaction true on the next draw.
//
//  Hosting strategy : same as c144 meu_runner_in_app_test.mm and c146
//  meu_hud_in_app_test.mm -- drive AAASeedMTKView + AAASeedInputView
//  directly via the headless MTKView path. No NSApplication needed.
//
//  Doctrine respected :
//    - .mm TU (AAASeedMTKView is ObjC++).
//    - No `o_str`, no `aaa_mem`, no `aaa_str.h`.
//    - Per feedback_ctest_label_first_only.md : `integration` is the
//      primary label and goes FIRST in the LABELS list.

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

#import "src/ui/macos/AAASeedMTKView.h"
#import "src/ui/macos/AAASeedInputView.h"

#include "src/gol/metal/metal_backend.h"
#include "src/meu/aaa_meu_runner_mac.h"
#include "src/ui/widgets/aaa_widgets_mac.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace
{

struct SharedBackend
{
    GOL::MetalBackend* get()
    {
        if( !inited ) inited = backend.init();
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

//  Write a tiny Lua script into a temp file that calls aaa.ui.slider on
//  every on_frame. Used by both tests so the test-time runner has a
//  widget-emitting script attached BEFORE [view draw] fires.
std::string write_widget_script( std::string const& stem )
{
    auto const tmp =
        std::filesystem::temp_directory_path() /
        ( "aaaseed_widgets_in_app_test_" + stem + ".lua" );
    std::ofstream out( tmp );
    out << "function aaa.on_frame(w, h, f)\n";
    out << "  aaa.ui.begin_panel('test', 20, 20, 180, 120)\n";
    out << "  aaa.ui.slider('foo', 0.5, 0.0, 1.0)\n";
    out << "  aaa.ui.end_panel()\n";
    out << "end\n";
    out.close();
    return tmp.string();
}

}   //  anonymous


//  -----------------------------------------------------------------------
//  Test 1 : a script calling aaa.ui.slider() produces a widget_count of
//  1 per frame. Drives 3 frames so the steady-state count is stable.
TEST( WidgetsInApp, WidgetsRenderInDrawInMTKView )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr ) << "MetalBackend init failed";

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        aaa::meu::Runner* runner = [delegate meuRunner];
        ASSERT_NE( runner, nullptr );
        aaa::ui::widgets::WidgetSystem* ws = [delegate widgetSystem];
        ASSERT_NE( ws, nullptr );

        std::string const path = write_widget_script( "render" );
        ASSERT_TRUE( runner->load_script( path ) )
            << "load_script failed for " << path;
        //  Reinstall widget pointer after load_script (load_script
        //  reopens the lua_State and reinstalls bindings ; the runner
        //  preserves the widget pointer across the reset).
        runner->set_widget_system( ws );

        //  Drive 3 frames. Each draw fires the script's on_frame which
        //  calls aaa.ui.slider() -> widget_count = 1.
        for( int i = 0; i < 3; ++i )
        {
            [view draw];
        }

        EXPECT_EQ( ws->last_frame_widget_count(), 1 )
            << "expected exactly one widget rendered per frame, got "
            << ws->last_frame_widget_count();

        std::error_code ec;
        std::filesystem::remove( path, ec );

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}


//  -----------------------------------------------------------------------
//  Test 2 : synthetic mouse click + drag inside the slider rect flips
//  had_interaction true on the next frame. The slider rect is the first
//  widget row inside the panel at (20, 20, 180, 120) -- in widget local
//  pixel coords that's roughly y = 20 + 22 + 6 = 48, x = [28, 192].
TEST( WidgetsInApp, MouseClickReachesWidgets )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        aaa::meu::Runner* runner = [delegate meuRunner];
        ASSERT_NE( runner, nullptr );
        aaa::ui::widgets::WidgetSystem* ws = [delegate widgetSystem];
        ASSERT_NE( ws, nullptr );

        std::string const path = write_widget_script( "click" );
        ASSERT_TRUE( runner->load_script( path ) );
        runner->set_widget_system( ws );

        //  Frame 0 : queue a press-edge with mouse INSIDE the slider's
        //  rect. AAASeedMTKView reads lastMouseLocation from the input
        //  view + applies top-left flip ; the synthesized location uses
        //  NSView bottom-left, so the y we set translates to
        //  drawable_h - y = 192 - y on the top-left side. Slider lives
        //  at top-left y ~ 48, so we use NSView y = 192 - 48 = 144.
        //  Click x = 60 (inside the panel's 20..200 range).
        NSEvent* down = [NSEvent mouseEventWithType:NSEventTypeLeftMouseDown
                                           location:NSMakePoint( 60.0, 144.0 )
                                      modifierFlags:0
                                          timestamp:[[NSDate date] timeIntervalSinceReferenceDate]
                                       windowNumber:0
                                            context:nil
                                        eventNumber:0
                                         clickCount:1
                                           pressure:1.0];
        ASSERT_NE( down, nil );
        [view mouseDown:down];

        //  Now drive a frame so the widget system sees the press-edge.
        [view draw];
        EXPECT_TRUE( ws->last_frame_had_interaction() )
            << "mouse-down inside the slider rect did not register as "
            << "interaction. Widget system mouse routing is broken.";

        std::error_code ec;
        std::filesystem::remove( path, ec );

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}
