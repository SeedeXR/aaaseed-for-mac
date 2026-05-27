//  tests/unit/aaa_widgets_mac_test.cpp
//
//  c147-A : unit tests for the Mac-native widget UI. Verifies the
//  immediate-mode API holds together end-to-end : ctor / begin / end /
//  panel / slider clamp / slider drag / button click+release / color
//  cycle / count / empty label.
//
//  Hermetic doctrine : plain .cpp (header is pure C++, no ObjC types
//  leak into the test TU). Uses a live MetalBackend so end_frame
//  exercises the real GPU path (matches the meu_runner_test pattern).

#include "src/gol/metal/metal_backend.h"
#include "src/ui/widgets/aaa_widgets_mac.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>

namespace
{

GOL::MetalBackend* make_backend()
{
    auto* b = new GOL::MetalBackend();
    if( !b->init() )
    {
        delete b;
        return nullptr;
    }
    return b;
}

//  Helper : drive begin_frame with default-quiet mouse state. width/
//  height match the integration test's MTKView at 256x192 so panel
//  default placement lands inside the viewport.
void quiet_frame( aaa::ui::widgets::WidgetSystem& ws,
                  double mx = 0.0, double my = 0.0,
                  bool pressed = false, bool released = false )
{
    ws.begin_frame( 256, 192, mx, my, pressed, released );
}

}   //  anonymous


//  -----------------------------------------------------------------------
//  Test 1 : ctor / dtor smoke. Construct the system with a live backend,
//  destroy it -- no crash. Documents the zero-frame baseline.
TEST( Widgets, CtorDtorSmoke )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        EXPECT_EQ( ws.last_frame_widget_count(), 0 );
        EXPECT_FALSE( ws.last_frame_had_interaction() );
    }
    delete backend;
}


//  Test 2 : begin / end frame without any widgets is safe.
TEST( Widgets, BeginEndFrameBalanced )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        ws.end_frame();
        EXPECT_EQ( ws.last_frame_widget_count(), 0 );
        EXPECT_FALSE( ws.last_frame_had_interaction() );
    }
    delete backend;
}


//  Test 3 : begin_panel + end_panel work. The panel emits chrome (header
//  bar) but no widgets are placed -- widget_count stays at zero (panel
//  isn't a widget per the spec).
TEST( Widgets, BeginEndPanelBalanced )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        aaa::ui::widgets::PanelHandle ph =
            ws.begin_panel( "Test", 10.0f, 10.0f, 180.0f, 120.0f );
        EXPECT_TRUE( ph.valid );
        EXPECT_NE( ph.id, 0u );
        ws.end_panel();
        ws.end_frame();
        //  Panel chrome ≠ widgets ; spec defers label rendering to host.
        EXPECT_EQ( ws.last_frame_widget_count(), 0 );
    }
    delete backend;
}


//  Test 4 : slider clamps to range. value=100, min=0, max=10 -> 10.
TEST( Widgets, SliderClampsToRange )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        float const out = ws.slider( "vol", 100.0f, 0.0f, 10.0f );
        ws.end_panel();
        ws.end_frame();
        EXPECT_FLOAT_EQ( out, 10.0f );
    }
    delete backend;
}


//  Test 5 : slider returns its value unchanged when there's no drag.
//  begin_frame with mouse_x/y far away from the slider's rect ; the
//  returned value matches the input exactly.
TEST( Widgets, SliderReturnsValueWithoutDrag )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        //  Mouse at (-100, -100) is outside any panel -> no hover, no
        //  drag, no interaction.
        ws.begin_frame( 256, 192, -100.0, -100.0, false, false );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        float const out = ws.slider( "vol", 0.42f, 0.0f, 1.0f );
        ws.end_panel();
        ws.end_frame();
        EXPECT_FLOAT_EQ( out, 0.42f );
        EXPECT_FALSE( ws.last_frame_had_interaction() );
    }
    delete backend;
}


//  Test 6 : slider drag advances value. Three-step interaction :
//   (1) frame 0 : press inside slider's rect -- arms the drag.
//   (2) frame 1 : while held, mouse moved right -- value advanced.
//   (3) frame 2 : mouse released -- drag ends.
//  Slider rect is determined by the panel layout : first widget after
//  the panel header, so rect ~ (panel_x + pad, panel_y + header_h +
//  spacing, panel_w - 2*pad, widget_h).
TEST( Widgets, SliderDragsOnHorizontalMouseMove )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        //  Place a panel at (10, 10) with width 200. First widget row
        //  starts ~y = 10 + 22 + 6 = 38. Mouse at (60, 50) hits the
        //  slider's center. We seed the drag with a press-edge here.
        float const panel_x = 10.0f;
        float const panel_y = 10.0f;
        float const panel_w = 200.0f;
        double const mouse_inside_x = 60.0;
        double const mouse_inside_y = 50.0;

        //  Frame 0 : press inside the slider.
        ws.begin_frame( 256, 192, mouse_inside_x, mouse_inside_y,
                        /*pressed*/ true, /*released*/ false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        float v0 = ws.slider( "drag_me", 0.50f, 0.0f, 1.0f );
        ws.end_panel();
        ws.end_frame();
        (void) v0;

        //  Frame 1 : hold + move right.
        ws.begin_frame( 256, 192, mouse_inside_x + 40.0, mouse_inside_y,
                        /*pressed*/ false, /*released*/ false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        float v1 = ws.slider( "drag_me", 0.50f, 0.0f, 1.0f );
        ws.end_panel();
        ws.end_frame();

        EXPECT_GT( v1, 0.50f )
            << "slider value did not advance under rightward drag ; "
            << "got v1=" << v1;
        EXPECT_TRUE( ws.last_frame_had_interaction() );
        EXPECT_GT( ws.slider_drag_delta_pixels( "drag_me" ), 30.0f );

        //  Frame 2 : release -- drag should end ; on a later quiet
        //  frame with no mouse motion the value stays put.
        ws.begin_frame( 256, 192, mouse_inside_x + 40.0, mouse_inside_y,
                        false, /*released*/ true );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        float v2 = ws.slider( "drag_me", v1, 0.0f, 1.0f );
        ws.end_panel();
        ws.end_frame();
        EXPECT_FLOAT_EQ( v2, v1 );
    }
    delete backend;
}


//  Test 7 : button click returns true ONLY on the frame the release
//  fires inside the rect. Subsequent frames return false until the
//  next press+release cycle.
TEST( Widgets, ButtonClickReturnsTrueOnce )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        float const panel_x = 10.0f, panel_y = 10.0f, panel_w = 200.0f;
        //  Button position : first widget row after header. Mouse hit
        //  point ~ (60, 50) lands inside the row.
        double const mx = 60.0, my = 50.0;

        //  Frame 0 : press inside the button.
        ws.begin_frame( 256, 192, mx, my, true, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        bool r0 = ws.button( "go" );
        ws.end_panel();
        ws.end_frame();
        EXPECT_FALSE( r0 ) << "button should not click on press-only frame";
        EXPECT_TRUE( ws.is_button_armed( "go" ) );

        //  Frame 1 : release inside the button -- click fires.
        ws.begin_frame( 256, 192, mx, my, false, true );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        bool r1 = ws.button( "go" );
        ws.end_panel();
        ws.end_frame();
        EXPECT_TRUE( r1 ) << "button did not fire on release-inside";

        //  Frame 2 : quiet frame -- click should NOT re-fire.
        ws.begin_frame( 256, 192, mx, my, false, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        bool r2 = ws.button( "go" );
        ws.end_panel();
        ws.end_frame();
        EXPECT_FALSE( r2 ) << "button re-fired on a quiet frame";
    }
    delete backend;
}


//  Test 8 : color well cycles through the preset list. Two distinct
//  clicks should land on two different preset colors ; N clicks land
//  on the Nth preset (mod count).
TEST( Widgets, ColorWellCyclesThroughPresets )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        //  Position : place panel at (10, 10) width 200. Color-well
        //  swatch lives on the RIGHT edge of the widget row, occupying
        //  the last 22 px (kColorWellSide). Row x range = [10+8, 10+200-8]
        //  = [18, 202]. Swatch range = [202-22, 202] = [180, 202].
        //  Click x ~ 190 hits the swatch.
        float const panel_x = 10.0f, panel_y = 10.0f, panel_w = 200.0f;
        double const click_x = 190.0, click_y = 50.0;

        aaa::ui::widgets::Color4f start{ 0.0f, 0.0f, 0.0f, 1.0f };

        aaa::ui::widgets::Color4f c0, c1, c2;

        //  Frame 0 : initial state ; no click yet.
        ws.begin_frame( 256, 192, click_x, click_y, false, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        c0 = ws.color_well( "fg", start );
        ws.end_panel();
        ws.end_frame();

        //  Frame 1 : click once -- preset advances by 1.
        ws.begin_frame( 256, 192, click_x, click_y, true, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        c1 = ws.color_well( "fg", start );
        ws.end_panel();
        ws.end_frame();

        //  Frame 2 : second press-edge -- preset advances again.
        ws.begin_frame( 256, 192, click_x, click_y, true, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        c2 = ws.color_well( "fg", start );
        ws.end_panel();
        ws.end_frame();

        EXPECT_FALSE( ( c0.r == c1.r ) && ( c0.g == c1.g )
                  && ( c0.b == c1.b ) )
            << "color well did not advance on first click.";
        EXPECT_FALSE( ( c1.r == c2.r ) && ( c1.g == c2.g )
                  && ( c1.b == c2.b ) )
            << "color well did not advance on second click.";
    }
    delete backend;
}


//  Test 9 : last_frame_widget_count tracks 3 sliders + 2 buttons = 5.
TEST( Widgets, LastFrameWidgetCountAccurate )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 200.0f );
        ws.slider( "a", 0.1f, 0.0f, 1.0f );
        ws.slider( "b", 0.2f, 0.0f, 1.0f );
        ws.slider( "c", 0.3f, 0.0f, 1.0f );
        (void) ws.button( "x" );
        (void) ws.button( "y" );
        ws.end_panel();
        ws.end_frame();
        EXPECT_EQ( ws.last_frame_widget_count(), 5 );
    }
    delete backend;
}


//  Test 10 : empty-label widgets still hash + track without crash.
TEST( Widgets, NoCrashOnEmptyLabel )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        ws.begin_panel( "", 10.0f, 10.0f, 200.0f, 120.0f );
        float v = 0.0f;
        EXPECT_NO_FATAL_FAILURE(
            v = ws.slider( "", 0.5f, 0.0f, 1.0f )
        );
        EXPECT_NO_FATAL_FAILURE( (void) ws.button( "" ) );
        EXPECT_NO_FATAL_FAILURE(
            (void) ws.color_well( "",
                aaa::ui::widgets::Color4f{ 1.0f, 0.0f, 0.0f, 1.0f } )
        );
        ws.end_panel();
        ws.end_frame();
        EXPECT_FLOAT_EQ( v, 0.5f );
        EXPECT_EQ( ws.last_frame_widget_count(), 3 );
    }
    delete backend;
}
