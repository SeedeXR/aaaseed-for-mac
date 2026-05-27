//  tests/unit/meu_hud_test.cpp
//
//  c146 : Unit tests for the MEU runner's HUD text overlay queue
//  (aaa.draw_hud_text Lua binding + Runner::get_pending_hud_text host
//  accessor). Closes the c143-C "interactive Space-press visual
//  confirmation : UNVERIFIED" gap by giving Lua scripts a way to push
//  on-screen text identifying the currently-active shader.
//
//  Hermetic doctrine : plain .cpp (not .mm), only pulls the public
//  Runner header (no AppKit / Lua leak into the test TU). MetalBackend
//  init mirrors the c142-B meu_runner_test.cpp pattern.

#include "src/gol/metal/metal_backend.h"
#include "src/meu/aaa_meu_runner_mac.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

namespace
{

//  Write `body` to a fresh temp .lua file and return its absolute path.
//  Same helper shape as meu_runner_test.cpp ; copied (not shared) to
//  keep this TU hermetic per feedback_hermetic_mac_sublibs.md.
std::filesystem::path write_temp_lua( std::string const& stem,
                                      std::string const& body )
{
    auto const tmp = std::filesystem::temp_directory_path() /
                     ( "aaaseed_meu_hud_test_" + stem + ".lua" );
    std::ofstream out{ tmp };
    out << body;
    out.close();
    return tmp;
}

GOL::MetalBackend* make_backend()
{
    auto* backend = new GOL::MetalBackend();
    if( !backend->init() )
    {
        delete backend;
        return nullptr;
    }
    return backend;
}

}   //  namespace

//  -----------------------------------------------------------------------
//  Test 1 : aaa.draw_hud_text binding exists. If the binding were
//  missing, Lua would raise "attempt to call a nil value" at parse
//  time and load_script would return false.
TEST( MeuHud, DrawHudTextBindingExists )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );

    constexpr char const* kScript =
        "function aaa.on_frame(w, h, f)\n"
        "  aaa.draw_hud_text('hi')\n"
        "end\n";
    auto const tmp = write_temp_lua( "binding_exists", kScript );

    aaa::meu::Runner runner( backend );
    EXPECT_TRUE( runner.load_script( tmp.string() ) )
        << "draw_hud_text binding missing or script syntax broke.";

    //  One frame to exercise the binding.
    runner.render_frame( 256, 256, 0 );

    std::filesystem::remove( tmp );
    delete backend;
}

//  Test 2 : aaa.draw_hud_text("x") surfaces to host via
//  Runner::get_pending_hud_text(). End-to-end Lua -> C surface check.
TEST( MeuHud, PendingTextSurfacesToHost )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );

    constexpr char const* kScript =
        "function aaa.on_frame(w, h, f)\n"
        "  aaa.draw_hud_text('x')\n"
        "end\n";
    auto const tmp = write_temp_lua( "surfaces", kScript );

    aaa::meu::Runner runner( backend );
    ASSERT_TRUE( runner.load_script( tmp.string() ) );

    //  Before render, the queue is empty (no on_frame has run yet).
    EXPECT_TRUE( runner.get_pending_hud_text().empty() );

    runner.render_frame( 256, 256, 0 );
    EXPECT_EQ( runner.get_pending_hud_text(), std::string( "x" ) );

    std::filesystem::remove( tmp );
    delete backend;
}

//  Test 3 : aaa.draw_hud_text("") is valid (clears the HUD). Defensive
//  against a script that wants to suppress HUD on a particular frame
//  without omitting the call entirely.
TEST( MeuHud, EmptyTextIsValid )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );

    constexpr char const* kScript =
        "function aaa.on_frame(w, h, f)\n"
        "  aaa.draw_hud_text('')\n"
        "end\n";
    auto const tmp = write_temp_lua( "empty_valid", kScript );

    aaa::meu::Runner runner( backend );
    ASSERT_TRUE( runner.load_script( tmp.string() ) );

    runner.render_frame( 256, 256, 0 );
    EXPECT_EQ( runner.get_pending_hud_text(), std::string() )
        << "draw_hud_text('') should leave the queue empty.";

    std::filesystem::remove( tmp );
    delete backend;
}

//  Test 4 : the pending HUD text resets at the start of each frame. A
//  script that calls draw_hud_text on frame 0 then omits the call on
//  frame 1 must NOT see frame 0's stale text on frame 1.
TEST( MeuHud, PendingTextClearsBetweenFrames )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );

    //  Script that emits HUD on every EVEN frame and skips it on odd.
    constexpr char const* kScript =
        "function aaa.on_frame(w, h, f)\n"
        "  if (f % 2) == 0 then\n"
        "    aaa.draw_hud_text('even')\n"
        "  end\n"
        "end\n";
    auto const tmp = write_temp_lua( "clear_between", kScript );

    aaa::meu::Runner runner( backend );
    ASSERT_TRUE( runner.load_script( tmp.string() ) );

    //  Frame 0 : f=0 (even) -> 'even'.
    runner.render_frame( 256, 256, 0 );
    EXPECT_EQ( runner.get_pending_hud_text(), std::string( "even" ) );

    //  Frame 1 : f=1 (odd) -> no call ; queue cleared at frame start.
    runner.render_frame( 256, 256, 0 );
    EXPECT_EQ( runner.get_pending_hud_text(), std::string() )
        << "Pending HUD text didn't clear between frames.";

    //  Frame 2 : f=2 (even) -> 'even' again.
    runner.render_frame( 256, 256, 0 );
    EXPECT_EQ( runner.get_pending_hud_text(), std::string( "even" ) );

    std::filesystem::remove( tmp );
    delete backend;
}

//  Test 5 : a long (~200 char) string doesn't crash. Guards against a
//  fixed-size internal buffer ; std::string accepts arbitrary lengths
//  so the call must succeed and the host must read back the same bytes.
TEST( MeuHud, LongTextDoesntCrash )
{
    auto* backend = make_backend();
    ASSERT_NE( backend, nullptr );

    //  200 'a' chars in Lua source.
    std::string const big( 200, 'a' );
    std::string const script =
        std::string( "function aaa.on_frame(w, h, f)\n" )
        + "  aaa.draw_hud_text('" + big + "')\n"
        + "end\n";
    auto const tmp = write_temp_lua( "long_text", script );

    aaa::meu::Runner runner( backend );
    ASSERT_TRUE( runner.load_script( tmp.string() ) );

    runner.render_frame( 256, 256, 0 );
    EXPECT_EQ( runner.get_pending_hud_text().size(), big.size() );
    EXPECT_EQ( runner.get_pending_hud_text(), big );

    std::filesystem::remove( tmp );
    delete backend;
}
