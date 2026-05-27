// tests/unit/aaa_widgets_v3_test.cpp
//
// c149-A v3 unit tests : drag-drop, file dialog, preset save/load, and
// nested collapsing-panel coverage. Hermetic .cpp ; uses a live
// MetalBackend so end_frame exercises the real GPU path (mirrors the
// c147-A / c148-A widget test pattern).

#include "src/gol/metal/metal_backend.h"
#include "src/meu/aaa_meu_runner_mac.h"
#include "src/ui/macos/aaa_file_dialog.h"
#include "src/ui/widgets/aaa_widgets_mac.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
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

std::string write_temp_script( std::string const& stem,
                               std::string const& body )
{
    auto const tmp =
        std::filesystem::temp_directory_path() /
        ( "aaaseed_v3_test_" + stem + ".lua" );
    std::ofstream out( tmp );
    out << body;
    out.close();
    return tmp.string();
}

}   //  anonymous

// =======================================================================
// Drag-drop (3 tests)
// =======================================================================

TEST( WidgetsV3DragDrop, DropLuaFilePathSetsScriptPath )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::meu::Runner runner( backend );

        std::string const lua = write_temp_script( "drop_lua",
            "function aaa.on_frame(w,h,f) end\n" );

        EXPECT_TRUE( runner.drop_file( lua ) );
        //  Frame index is 0 immediately after load_script ; the script
        //  loaded means render_frame would dispatch on_frame.
        EXPECT_EQ( runner.frame_index(), 0 );

        std::error_code ec;
        std::filesystem::remove( lua, ec );
    }
    delete backend;
}

TEST( WidgetsV3DragDrop, DropNonLuaFileIsIgnored )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::meu::Runner runner( backend );
        //  Make a real file with .txt extension so the path exists ;
        //  drop_file should still reject it on extension grounds.
        auto const tmp =
            std::filesystem::temp_directory_path() / "aaaseed_v3_test_drop.txt";
        std::ofstream{ tmp } << "not a script";
        EXPECT_FALSE( runner.drop_file( tmp.string() ) );
        std::error_code ec;
        std::filesystem::remove( tmp, ec );
    }
    delete backend;
}

TEST( WidgetsV3DragDrop, DropEmptyPathDoesNothing )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::meu::Runner runner( backend );
        EXPECT_FALSE( runner.drop_file( std::string() ) );
    }
    delete backend;
}

// =======================================================================
// File dialog (2 tests : confirm guard under no-event-loop)
// =======================================================================

TEST( WidgetsV3FileDialog, OpenFileDialogReturnsNilWithoutEventLoop )
{
    std::vector< aaa::ui::FileDialogFilter > filters;
    filters.push_back( { "Lua scripts", { "lua" } } );
    auto const r = aaa::ui::open_file_dialog( "Test open", filters );
    EXPECT_FALSE( r.has_value() );
}

TEST( WidgetsV3FileDialog, SaveFileDialogReturnsNilWithoutEventLoop )
{
    std::vector< aaa::ui::FileDialogFilter > filters;
    filters.push_back( { "Lua scripts", { "lua" } } );
    auto const r = aaa::ui::save_file_dialog( "Test save", "preset.lua", filters );
    EXPECT_FALSE( r.has_value() );
}

// (file-watcher tests live in tests/unit/aaa_file_watcher_test.cpp.)

// =======================================================================
// Preset save/load (4 tests)
// =======================================================================

TEST( WidgetsV3Preset, SavePresetWritesValidLuaFile )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        aaa::meu::Runner runner( backend );
        runner.set_widget_system( &ws );

        //  Load a script that registers a slider label so the runner
        //  tracks it.
        std::string const lua = write_temp_script( "preset_save",
            "function aaa.on_frame(w, h, f)\n"
            "  aaa.ui.begin_panel('p', 10, 10, 200, 200)\n"
            "  aaa.ui.slider('intensity', 0.5, 0.0, 1.0)\n"
            "  aaa.ui.end_panel()\n"
            "end\n" );
        ASSERT_TRUE( runner.load_script( lua ) );

        ws.begin_frame( 512, 384, -1.0, -1.0, false, false );
        ws.begin_panel( "p", 10.0f, 10.0f, 200.0f, 200.0f );
        //  Drive via the binding-path by calling the runner ; render_frame
        //  with no backend pass works because the widget system only
        //  emits draws at end_frame.
        runner.render_frame( 512, 384, 0 );
        ws.end_panel();
        ws.end_frame();

        auto const preset =
            std::filesystem::temp_directory_path() / "aaaseed_v3_preset_save.lua";
        EXPECT_TRUE( runner.save_preset( preset.string() ) );

        //  File must be readable + parse as Lua. We only assert non-empty
        //  here ; load_preset round-trips elsewhere.
        std::ifstream in( preset );
        std::string contents( ( std::istreambuf_iterator< char >( in ) ),
                              std::istreambuf_iterator< char >() );
        EXPECT_NE( contents.find( "slider_state" ), std::string::npos );

        std::error_code ec;
        std::filesystem::remove( preset, ec );
        std::filesystem::remove( lua, ec );
    }
    delete backend;
}

TEST( WidgetsV3Preset, LoadPresetRestoresSliderValues )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        aaa::meu::Runner runner( backend );
        runner.set_widget_system( &ws );

        std::string const lua = write_temp_script( "preset_load",
            "function aaa.on_frame(w, h, f) end\n" );
        ASSERT_TRUE( runner.load_script( lua ) );

        auto const preset =
            std::filesystem::temp_directory_path() / "aaaseed_v3_preset_load.lua";
        std::ofstream{ preset }
            << "return {\n"
            << "  slider_state = { [\"intensity\"] = 0.75 },\n"
            << "  hsv_picker_state = {},\n"
            << "  color_well_state = {},\n"
            << "  text_input_state = {},\n"
            << "  panel_expanded_state = {},\n"
            << "}\n";

        EXPECT_TRUE( runner.load_preset( preset.string() ) );
        EXPECT_NEAR( ws.slider_value( "intensity" ), 0.75f, 1e-5f );

        std::error_code ec;
        std::filesystem::remove( preset, ec );
        std::filesystem::remove( lua, ec );
    }
    delete backend;
}

TEST( WidgetsV3Preset, LoadPresetMissingFileReturnsFalse )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        aaa::meu::Runner runner( backend );
        runner.set_widget_system( &ws );

        std::string const lua = write_temp_script( "preset_missing",
            "function aaa.on_frame(w, h, f) end\n" );
        ASSERT_TRUE( runner.load_script( lua ) );

        EXPECT_FALSE( runner.load_preset(
            "/no/such/dir/preset_definitely_not_there.lua" ) );

        std::error_code ec;
        std::filesystem::remove( lua, ec );
    }
    delete backend;
}

TEST( WidgetsV3Preset, RoundTripPreservesAllWidgetStateTypes )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        aaa::meu::Runner runner( backend );
        runner.set_widget_system( &ws );

        std::string const lua = write_temp_script( "preset_round",
            "function aaa.on_frame(w, h, f)\n"
            "  aaa.ui.slider('s1', 0.0, 0.0, 1.0)\n"
            "  aaa.ui.hsv_color_picker('hp', 0.5, 0.5, 0.5, 1.0)\n"
            "  aaa.ui.color_well('cw', 0.0, 0.0, 0.0, 1.0)\n"
            "  aaa.ui.text_input('ti', '', 32)\n"
            "  aaa.ui.begin_collapsing_panel('cp', 10, 10, 200, 200)\n"
            "  aaa.ui.end_collapsing_panel()\n"
            "end\n" );
        ASSERT_TRUE( runner.load_script( lua ) );

        //  Drive one frame to populate _ui_label_kinds.
        ws.begin_frame( 512, 384, -1.0, -1.0, false, false );
        runner.render_frame( 512, 384, 0 );
        ws.end_frame();

        //  Set distinct values directly on the widget state.
        ws.set_slider_value( "s1", 0.42f );
        ws.set_hsv_picker_value( "hp",
            aaa::ui::widgets::Color4f{ 0.9f, 0.1f, 0.2f, 0.8f } );
        ws.set_color_well_index( "cw", 3 );
        ws.set_text_input_value( "ti", "hello" );
        ws.set_panel_expanded( "cp", false );

        auto const preset =
            std::filesystem::temp_directory_path() / "aaaseed_v3_round.lua";
        ASSERT_TRUE( runner.save_preset( preset.string() ) );

        //  Stomp the values so we know load_preset restores them.
        ws.set_slider_value( "s1", 0.0f );
        ws.set_text_input_value( "ti", "" );
        ws.set_color_well_index( "cw", 0 );
        ws.set_panel_expanded( "cp", true );

        ASSERT_TRUE( runner.load_preset( preset.string() ) );

        EXPECT_NEAR( ws.slider_value( "s1" ), 0.42f, 1e-5f );
        EXPECT_EQ( ws.color_well_index( "cw" ), 3 );
        EXPECT_EQ( ws.text_input_value( "ti" ), "hello" );
        EXPECT_FALSE( ws.panel_expanded( "cp" ) );
        //  HSV is lossy through round-trip via RGB->HSV->RGB ; loose
        //  tolerance like the c148-A HSV tests.
        aaa::ui::widgets::Color4f const got = ws.hsv_picker_value( "hp" );
        EXPECT_NEAR( got.r, 0.9f, 1e-2f );
        EXPECT_NEAR( got.a, 0.8f, 1e-5f );

        std::error_code ec;
        std::filesystem::remove( preset, ec );
        std::filesystem::remove( lua, ec );
    }
    delete backend;
}

// =======================================================================
// Nested collapsing panels (3 tests)
// =======================================================================

TEST( WidgetsV3NestedPanels, NestedCollapsingPanelDepthTracked )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        ws.begin_frame( 512, 384, -1.0, -1.0, false, false );
        EXPECT_EQ( ws.collapsing_nest_depth(), 0 );

        ws.begin_collapsing_panel( "outer", 10.0f, 10.0f, 200.0f, 200.0f );
        EXPECT_EQ( ws.collapsing_nest_depth(), 1 );

        ws.begin_collapsing_panel( "inner", 10.0f, 50.0f, 180.0f, 150.0f );
        EXPECT_EQ( ws.collapsing_nest_depth(), 2 );

        ws.end_collapsing_panel();
        EXPECT_EQ( ws.collapsing_nest_depth(), 1 );

        ws.end_collapsing_panel();
        EXPECT_EQ( ws.collapsing_nest_depth(), 0 );

        ws.end_frame();
    }
    delete backend;
}

TEST( WidgetsV3NestedPanels, CollapsedParentSuppressesChildWidgets )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );

        //  Pre-set the outer panel to collapsed via the public setter.
        ws.set_panel_expanded( "outer", false );

        ws.begin_frame( 512, 384, -1.0, -1.0, false, false );
        bool const outer_expanded =
            ws.begin_collapsing_panel( "outer", 10.0f, 10.0f, 200.0f, 200.0f );
        EXPECT_FALSE( outer_expanded );

        //  Nested begin_collapsing_panel called inside a collapsed
        //  parent must return false (inherits suppression).
        bool const inner_expanded =
            ws.begin_collapsing_panel( "inner", 10.0f, 50.0f, 180.0f, 150.0f );
        EXPECT_FALSE( inner_expanded );

        ws.end_collapsing_panel();
        ws.end_collapsing_panel();
        ws.end_frame();
    }
    delete backend;
}

TEST( WidgetsV3NestedPanels, ExpandedParentRendersChildren )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );
    {
        aaa::ui::widgets::WidgetSystem ws( backend );
        ws.set_panel_expanded( "outer", true );
        ws.set_panel_expanded( "inner", true );

        ws.begin_frame( 512, 384, -1.0, -1.0, false, false );
        bool const outer_expanded =
            ws.begin_collapsing_panel( "outer", 10.0f, 10.0f, 200.0f, 200.0f );
        EXPECT_TRUE( outer_expanded );

        bool const inner_expanded =
            ws.begin_collapsing_panel( "inner", 10.0f, 50.0f, 180.0f, 150.0f );
        EXPECT_TRUE( inner_expanded );

        ws.end_collapsing_panel();
        ws.end_collapsing_panel();
        ws.end_frame();
    }
    delete backend;
}
