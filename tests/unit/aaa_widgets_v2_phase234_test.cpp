//  tests/unit/aaa_widgets_v2_phase234_test.cpp
//
//  c148-A v2 Phases 2-4 : unit tests for the 5 new widget primitives :
//    Phase 2 : HSV color picker.
//    Phase 3 : modal dialog + single-line text input.
//    Phase 4 : hot-reload button + collapsing panel.
//
//  Hermetic doctrine : plain .cpp (header is pure C++ ; no ObjC types
//  leak). Uses a live MetalBackend so end_frame exercises the real GPU
//  path (matches the c147-A aaa_widgets_mac_test.cpp pattern).

#include "src/gol/metal/metal_backend.h"
#include "src/ui/widgets/aaa_widgets_mac.h"

#include <gtest/gtest.h>

#include <cmath>
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

//  Helper : drive begin_frame with default-quiet mouse state.
void quiet_frame( aaa::ui::widgets::WidgetSystem& ws,
                  double mx = -100.0, double my = -100.0,
                  bool pressed = false, bool released = false )
{
    ws.begin_frame( 512, 384, mx, my, pressed, released );
}

}   //  anonymous


// =======================================================================
// HSV color picker (4 tests)
// =======================================================================

//  HSV-1 : the picker returns its initial RGBA when no interaction has
//  occurred. The Smith-1978 RGB->HSV round-trip + HSV->RGB at the same
//  HSV produces values within float epsilon of the input.
TEST( WidgetsV2Hsv, HsvPickerReturnsInitialValueWithoutClick )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 200.0f );
        aaa::ui::widgets::Color4f const seed{ 0.6f, 0.3f, 0.8f, 0.75f };
        aaa::ui::widgets::Color4f const out =
            ws.hsv_color_picker( "tint", seed );
        ws.end_panel();
        ws.end_frame();

        //  Tolerance is loose because RGB->HSV->RGB has a small error
        //  at off-axis hues.
        EXPECT_NEAR( out.r, seed.r, 1e-3f );
        EXPECT_NEAR( out.g, seed.g, 1e-3f );
        EXPECT_NEAR( out.b, seed.b, 1e-3f );
        EXPECT_NEAR( out.a, seed.a, 1e-6f );
        EXPECT_FALSE( ws.last_frame_had_interaction() );
    }
    delete backend;
}


//  HSV-2 : dragging inside the SV square advances saturation/value.
//  The SV square top-left corner is at the panel's widget-cursor x,y ;
//  pressing at (sq_x + 64, sq_y + 32) targets sat ~ 0.5, val ~ 0.75.
//  The returned color should differ from the seed (which had sat=1,
//  val=1 implicit if seed is pure-hue red).
TEST( WidgetsV2Hsv, HsvSaturationDragAdjustsSat )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        float const panel_x = 10.0f, panel_y = 10.0f, panel_w = 200.0f;
        //  First widget row inside the panel starts at y = 10+22+6 = 38.
        //  SV square is 128x128 ; click inside at (panel_x + pad + 64, 38 + 32).
        double const click_x = panel_x + 8.0f + 32.0;
        double const click_y = 38.0 + 16.0;

        //  Frame 0 : initial state, no input.
        ws.begin_frame( 512, 384, -100.0, -100.0, false, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 300.0f );
        aaa::ui::widgets::Color4f const seed{ 1.0f, 0.0f, 0.0f, 1.0f };   //  red
        aaa::ui::widgets::Color4f c0 = ws.hsv_color_picker( "tint", seed );
        ws.end_panel();
        ws.end_frame();

        //  Frame 1 : press inside the SV square at low saturation.
        ws.begin_frame( 512, 384, click_x, click_y, true, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 300.0f );
        aaa::ui::widgets::Color4f c1 = ws.hsv_color_picker( "tint", seed );
        ws.end_panel();
        ws.end_frame();

        //  c1 should have less saturation than c0 (closer to white) -- so
        //  the green + blue channels jump up from c0's near-zero. We
        //  verify SOMETHING changed.
        bool const same = ( c0.r == c1.r ) && ( c0.g == c1.g ) && ( c0.b == c1.b );
        EXPECT_FALSE( same )
            << "HSV picker SV drag did not change output color. "
            << "c0=" << c0.r << "," << c0.g << "," << c0.b
            << " c1=" << c1.r << "," << c1.g << "," << c1.b;
        EXPECT_TRUE( ws.last_frame_had_interaction() );
    }
    delete backend;
}


//  HSV-3 : dragging in the hue bar advances the hue. The hue bar lives
//  immediately RIGHT of the SV square at x = sq_x + 128 + 4. Press at
//  hue_x + 8 (center of bar), y = sq_y + 96 (3/4 down the bar).
TEST( WidgetsV2Hsv, HsvHueDragAdjustsHue )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        float const panel_x = 10.0f, panel_y = 10.0f, panel_w = 240.0f;
        double const sq_x = panel_x + 8.0f;
        double const sq_y = 38.0;
        double const hue_x = sq_x + 128.0 + 4.0 + 8.0;
        double const hue_y = sq_y + 96.0;

        //  Frame 0 : initial.
        ws.begin_frame( 512, 384, -100.0, -100.0, false, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 300.0f );
        aaa::ui::widgets::Color4f const seed{ 1.0f, 0.0f, 0.0f, 1.0f };
        aaa::ui::widgets::Color4f c0 = ws.hsv_color_picker( "tint", seed );
        ws.end_panel();
        ws.end_frame();

        //  Frame 1 : press in the hue bar.
        ws.begin_frame( 512, 384, hue_x, hue_y, true, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 300.0f );
        aaa::ui::widgets::Color4f c1 = ws.hsv_color_picker( "tint", seed );
        ws.end_panel();
        ws.end_frame();

        bool const same = ( c0.r == c1.r ) && ( c0.g == c1.g ) && ( c0.b == c1.b );
        EXPECT_FALSE( same )
            << "HSV hue-bar drag did not change output color.";
        EXPECT_TRUE( ws.last_frame_had_interaction() );
    }
    delete backend;
}


//  HSV-4 : HSV->RGB round-trips at cardinal hue positions. We can't
//  reach the conversion helpers directly (anonymous-namespace internals)
//  so we verify via the public API : feed in a pure cardinal RGB, get
//  it back unchanged. Red, green, blue, yellow, cyan, magenta, white,
//  black all hit hue/sat boundaries that exercise distinct branches of
//  the Smith 1978 algorithm.
TEST( WidgetsV2Hsv, HsvToRgbRoundTripsAtCardinalPoints )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        struct Probe { aaa::ui::widgets::Color4f c; char const* name; };
        Probe const probes[] = {
            { { 1.0f, 0.0f, 0.0f, 1.0f }, "red"     },
            { { 0.0f, 1.0f, 0.0f, 1.0f }, "green"   },
            { { 0.0f, 0.0f, 1.0f, 1.0f }, "blue"    },
            { { 1.0f, 1.0f, 0.0f, 1.0f }, "yellow"  },
            { { 0.0f, 1.0f, 1.0f, 1.0f }, "cyan"    },
            { { 1.0f, 0.0f, 1.0f, 1.0f }, "magenta" },
            { { 1.0f, 1.0f, 1.0f, 1.0f }, "white"   },
            { { 0.0f, 0.0f, 0.0f, 1.0f }, "black"   },
        };

        for( auto const& p : probes )
        {
            quiet_frame( ws );
            ws.begin_panel( "P", 10.0f, 10.0f, 240.0f, 300.0f );
            //  Use a distinct label per probe so each gets fresh state.
            std::string label = std::string( "tint_" ) + p.name;
            aaa::ui::widgets::Color4f const out =
                ws.hsv_color_picker( label, p.c );
            ws.end_panel();
            ws.end_frame();
            EXPECT_NEAR( out.r, p.c.r, 1e-3f ) << "round-trip failed for " << p.name;
            EXPECT_NEAR( out.g, p.c.g, 1e-3f ) << "round-trip failed for " << p.name;
            EXPECT_NEAR( out.b, p.c.b, 1e-3f ) << "round-trip failed for " << p.name;
        }
    }
    delete backend;
}


// =======================================================================
// Modal dialog (4 tests)
// =======================================================================

//  MOD-1 : a freshly-constructed WidgetSystem has no open modal. The
//  begin_modal call returns open=false until show_modal is called.
TEST( WidgetsV2Modal, ModalNotOpenByDefault )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        auto const r = ws.begin_modal( "test", 200.0f, 120.0f );
        ws.end_modal();
        ws.end_frame();
        EXPECT_FALSE( r.open );
        EXPECT_FALSE( ws.is_modal_open() );
    }
    delete backend;
}


//  MOD-2 : after show_modal("test"), begin_modal("test", ...) on the
//  NEXT frame returns open=true.
TEST( WidgetsV2Modal, ShowModalSetsOpenTrue )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        ws.show_modal( "test" );
        EXPECT_TRUE( ws.is_modal_open() );

        quiet_frame( ws );
        auto const r = ws.begin_modal( "test", 200.0f, 120.0f );
        ws.end_modal();
        ws.end_frame();
        EXPECT_TRUE( r.open );
    }
    delete backend;
}


//  MOD-3 : clicking the cancel button closes the modal.
//  Layout : the modal is centered. Width 200, height 120 on a 512x384
//  viewport ; modal x = (512-200)/2 = 156, y = (384-120)/2 = 132.
//  Cancel button : at (ok_x - kModalButtonW - 6 = 156+200-70-8 - 70 - 6
//  = 212, y = 132 + 120 - 24 - 6 = 222).
TEST( WidgetsV2Modal, ModalCancelClosesIt )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        ws.show_modal( "test" );
        EXPECT_TRUE( ws.is_modal_open() );

        //  Mouse over Cancel button : cancel_x ~ 212, cancel_y ~ 222.
        //  Center hit point.
        double const cx = 212.0 + 35.0;
        double const cy = 222.0 + 12.0;

        //  Frame 0 : press inside cancel.
        ws.begin_frame( 512, 384, cx, cy, true, false );
        ws.begin_modal( "test", 200.0f, 120.0f );
        ws.end_modal();
        ws.end_frame();
        EXPECT_TRUE( ws.is_modal_open() );

        //  Frame 1 : release inside cancel -- fires the click + closes.
        ws.begin_frame( 512, 384, cx, cy, false, true );
        auto const r = ws.begin_modal( "test", 200.0f, 120.0f );
        ws.end_modal();
        ws.end_frame();
        EXPECT_TRUE( r.cancel_clicked );
        EXPECT_FALSE( ws.is_modal_open() );
    }
    delete backend;
}


//  MOD-4 : clicking OK closes the modal + flips ok_clicked.
TEST( WidgetsV2Modal, ModalOkClosesIt )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        ws.show_modal( "test" );

        //  OK button x = (modal_x + modal_w - kModalButtonW - 8) =
        //  156 + 200 - 70 - 8 = 278. Center hit point at (278+35, 222+12).
        double const ox = 278.0 + 35.0;
        double const oy = 222.0 + 12.0;

        //  Frame 0 : press.
        ws.begin_frame( 512, 384, ox, oy, true, false );
        ws.begin_modal( "test", 200.0f, 120.0f );
        ws.end_modal();
        ws.end_frame();

        //  Frame 1 : release.
        ws.begin_frame( 512, 384, ox, oy, false, true );
        auto const r = ws.begin_modal( "test", 200.0f, 120.0f );
        ws.end_modal();
        ws.end_frame();
        EXPECT_TRUE( r.ok_clicked );
        EXPECT_FALSE( ws.is_modal_open() );
    }
    delete backend;
}


// =======================================================================
// Text input (4 tests)
// =======================================================================

//  TXT-1 : text_input returns the caller's current_value when NOT
//  focused. Pre-edit value passes through unchanged.
TEST( WidgetsV2Text, TextInputReturnsCurrentValueIfNotFocused )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        ws.begin_panel( "P", 10.0f, 10.0f, 240.0f, 120.0f );
        std::string const out = ws.text_input( "name", "alex", 64 );
        ws.end_panel();
        ws.end_frame();
        EXPECT_EQ( out, "alex" );
        EXPECT_EQ( ws.focused_text_input_id(), 0u );
    }
    delete backend;
}


//  TXT-2 : click to focus, send a keystroke, the buffer mirrors it.
//  Slider+input both at the first widget row inside the panel.
TEST( WidgetsV2Text, TextInputInsertsCharOnFocusedKey )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        float const panel_x = 10.0f, panel_y = 10.0f, panel_w = 200.0f;
        //  First widget row at y ~ 38. Click x = 60 (inside row).
        double const cx = 60.0;
        double const cy = 50.0;

        //  Frame 0 : click to focus.
        ws.begin_frame( 512, 384, cx, cy, true, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        ws.text_input( "name", "", 64 );
        ws.end_panel();
        ws.end_frame();
        EXPECT_NE( ws.focused_text_input_id(), 0u )
            << "click did not focus the text input";

        //  Send 'a' = 0x61. Then drive another frame ; the returned
        //  string includes the inserted char.
        ws.on_text_input( 0x61 );

        ws.begin_frame( 512, 384, cx, cy, false, false );
        ws.begin_panel( "P", panel_x, panel_y, panel_w, 120.0f );
        std::string const out = ws.text_input( "name", "", 64 );
        ws.end_panel();
        ws.end_frame();
        EXPECT_EQ( out, "a" );
    }
    delete backend;
}


//  TXT-3 : backspace removes the trailing character.
TEST( WidgetsV2Text, TextInputBackspaceRemovesLastChar )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        double const cx = 60.0, cy = 50.0;

        //  Frame 0 : focus.
        ws.begin_frame( 512, 384, cx, cy, true, false );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        ws.text_input( "name", "", 64 );
        ws.end_panel();
        ws.end_frame();
        ASSERT_NE( ws.focused_text_input_id(), 0u );

        //  Type 'a','b','c'.
        ws.on_text_input( 'a' );
        ws.on_text_input( 'b' );
        ws.on_text_input( 'c' );

        //  Frame 1 : read buffer == "abc".
        ws.begin_frame( 512, 384, cx, cy, false, false );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        std::string const s1 = ws.text_input( "name", "", 64 );
        ws.end_panel();
        ws.end_frame();
        EXPECT_EQ( s1, "abc" );

        //  Backspace + read again.
        ws.on_text_input( 0x08 );
        ws.begin_frame( 512, 384, cx, cy, false, false );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        std::string const s2 = ws.text_input( "name", "", 64 );
        ws.end_panel();
        ws.end_frame();
        EXPECT_EQ( s2, "ab" );
    }
    delete backend;
}


//  TXT-4 : the max_length argument clamps the buffer. Repeated chars
//  past max_length are dropped (or truncated on the next frame).
TEST( WidgetsV2Text, TextInputRespectsMaxLength )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        double const cx = 60.0, cy = 50.0;
        std::size_t const max_len = 3;

        //  Frame 0 : focus.
        ws.begin_frame( 512, 384, cx, cy, true, false );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        ws.text_input( "name", "", max_len );
        ws.end_panel();
        ws.end_frame();
        ASSERT_NE( ws.focused_text_input_id(), 0u );

        //  Send 6 chars.
        for( char c : std::string( "abcdef" ) )
        {
            ws.on_text_input( (std::uint32_t) c );
        }

        //  Read back ; the next text_input call should clamp the buffer
        //  to max_len.
        ws.begin_frame( 512, 384, cx, cy, false, false );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        std::string const out = ws.text_input( "name", "", max_len );
        ws.end_panel();
        ws.end_frame();
        EXPECT_LE( out.size(), max_len )
            << "text input did not clamp to max_length=" << max_len
            << ", got '" << out << "'";
    }
    delete backend;
}


// =======================================================================
// Hot-reload button (1 test)
// =======================================================================

//  HR-1 : the button works without a callback set (no-op + still
//  reports the click). Then set a callback ; the button fires it on
//  click-release.
TEST( WidgetsV2HotReload, HotReloadButtonRequiresRunnerSet )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        //  Phase 1 : without a callback, the button still tracks clicks
        //  but reload is a silent no-op.
        double const cx = 60.0, cy = 50.0;

        //  Frame 0 : press.
        ws.begin_frame( 512, 384, cx, cy, true, false );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        bool const r0 = ws.hot_reload_button( "Reload" );
        ws.end_panel();
        ws.end_frame();
        EXPECT_FALSE( r0 );

        //  Frame 1 : release -> click.
        ws.begin_frame( 512, 384, cx, cy, false, true );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        bool const r1 = ws.hot_reload_button( "Reload" );
        ws.end_panel();
        ws.end_frame();
        EXPECT_TRUE( r1 );

        //  Phase 2 : install a callback ; on next click it fires.
        int call_count = 0;
        ws.set_reload_callback( [ &call_count ]{ ++call_count; } );

        //  Frame 2 : press.
        ws.begin_frame( 512, 384, cx, cy, true, false );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        ws.hot_reload_button( "Reload" );
        ws.end_panel();
        ws.end_frame();
        EXPECT_EQ( call_count, 0 );

        //  Frame 3 : release.
        ws.begin_frame( 512, 384, cx, cy, false, true );
        ws.begin_panel( "P", 10.0f, 10.0f, 200.0f, 120.0f );
        bool const r3 = ws.hot_reload_button( "Reload" );
        ws.end_panel();
        ws.end_frame();
        EXPECT_TRUE( r3 );
        EXPECT_EQ( call_count, 1 )
            << "reload callback was not invoked on click-release";
    }
    delete backend;
}


// =======================================================================
// Collapsing panel (2 tests)
// =======================================================================

//  CP-1 : default state is EXPANDED. begin_collapsing_panel returns
//  true on the first frame the panel is seen.
TEST( WidgetsV2Collapsing, CollapsingPanelStartsExpanded )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        quiet_frame( ws );
        bool const expanded = ws.begin_collapsing_panel(
            "Section", 10.0f, 10.0f, 200.0f, 120.0f );
        ws.end_collapsing_panel();
        ws.end_frame();
        EXPECT_TRUE( expanded );
        EXPECT_TRUE( ws.is_panel_expanded( "Section" ) );
    }
    delete backend;
}


//  CP-2 : clicking the chevron toggles expanded. Chevron rect = (x, y,
//  kChevronSide, kWidgetHeight) = (10, 10, 22, 22). Click at (12, 12)
//  hits it. Press+release on frames 0+1 ; frame 2's begin_collapsing_
//  panel reports collapsed.
TEST( WidgetsV2Collapsing, CollapsingPanelToggleOnChevronClick )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        float const x = 10.0f, y = 10.0f, w = 200.0f, h = 120.0f;
        double const cx = 12.0, cy = 12.0;

        //  Frame 0 : press inside chevron.
        ws.begin_frame( 512, 384, cx, cy, true, false );
        bool e0 = ws.begin_collapsing_panel( "S", x, y, w, h );
        ws.end_collapsing_panel();
        ws.end_frame();
        EXPECT_TRUE( e0 ) << "still expanded on press frame (toggle on release)";

        //  Frame 1 : release inside chevron -> toggle fires.
        ws.begin_frame( 512, 384, cx, cy, false, true );
        bool e1 = ws.begin_collapsing_panel( "S", x, y, w, h );
        ws.end_collapsing_panel();
        ws.end_frame();
        EXPECT_FALSE( e1 ) << "release did not toggle collapsed";
        EXPECT_FALSE( ws.is_panel_expanded( "S" ) );

        //  Frame 2 : quiet -- stays collapsed.
        quiet_frame( ws );
        bool e2 = ws.begin_collapsing_panel( "S", x, y, w, h );
        ws.end_collapsing_panel();
        ws.end_frame();
        EXPECT_FALSE( e2 );

        //  Frames 3+4 : click chevron again -> back to expanded.
        ws.begin_frame( 512, 384, cx, cy, true, false );
        ws.begin_collapsing_panel( "S", x, y, w, h );
        ws.end_collapsing_panel();
        ws.end_frame();

        ws.begin_frame( 512, 384, cx, cy, false, true );
        bool e4 = ws.begin_collapsing_panel( "S", x, y, w, h );
        ws.end_collapsing_panel();
        ws.end_frame();
        EXPECT_TRUE( e4 ) << "second click did not re-expand";
    }
    delete backend;
}
