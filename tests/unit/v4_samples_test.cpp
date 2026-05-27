//  tests/unit/v4_samples_test.cpp
//
//  c150 v4 : regression guards for the v4 sample MEUs + AUTHORING guide
//  + ship-gate doctrine closure. c150-B agent stalled before writing
//  these ; written manually after c150-B's samples landed at
//  bundle/macos/meu/Samples/v4_ime_text/ + v4_multiline/.

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#ifndef AAASEED_SOURCE_ROOT
#error "AAASEED_SOURCE_ROOT must be defined by the CMake target."
#endif

namespace fs = std::filesystem;

namespace
{

fs::path source_root()
{
    return fs::path( AAASEED_SOURCE_ROOT );
}

fs::path samples_root()
{
    return source_root() / "bundle" / "macos" / "meu" / "Samples";
}

std::string read_file_to_string( fs::path const& p )
{
    std::ifstream in( p, std::ios::binary );
    if( !in ) return std::string();
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

}   //  namespace

//  ---------------------------------------------------------------------
//  Test 1 : both v4 sample directories exist with .lua + README.md.
TEST( V4Samples, BothSampleDirsExistWithLuaAndReadme )
{
    fs::path const ime_dir   = samples_root() / "v4_ime_text";
    fs::path const multi_dir = samples_root() / "v4_multiline";
    EXPECT_TRUE( fs::is_directory( ime_dir   ) ) << ime_dir;
    EXPECT_TRUE( fs::is_directory( multi_dir ) ) << multi_dir;
    EXPECT_TRUE( fs::is_regular_file( ime_dir   / "ime_text_demo.lua" ) );
    EXPECT_TRUE( fs::is_regular_file( ime_dir   / "README.md" ) );
    EXPECT_TRUE( fs::is_regular_file( multi_dir / "multiline_demo.lua" ) );
    EXPECT_TRUE( fs::is_regular_file( multi_dir / "README.md" ) );
}

//  Test 2 : each demo .lua references its respective v4 binding.
TEST( V4Samples, EachDemoReferencesIts_v4_Bindings )
{
    auto const ime = read_file_to_string(
        samples_root() / "v4_ime_text" / "ime_text_demo.lua" );
    EXPECT_NE( ime.find( "aaa.ime." ), std::string::npos )
        << "ime_text_demo.lua must reference aaa.ime.* binding";

    auto const multi = read_file_to_string(
        samples_root() / "v4_multiline" / "multiline_demo.lua" );
    EXPECT_NE( multi.find( "aaa.ui.text_area" ), std::string::npos )
        << "multiline_demo.lua must reference aaa.ui.text_area";
}

//  Test 3 : each demo .lua passes `luac -p` syntax check.
TEST( V4Samples, AllDemosPassLuacSyntaxCheck )
{
    if( std::system( "which luac > /dev/null 2>&1" ) != 0 )
    {
        GTEST_SKIP() << "luac not on PATH ; skipping syntax check";
        return;
    }

    fs::path const luas[] = {
        samples_root() / "v4_ime_text"  / "ime_text_demo.lua",
        samples_root() / "v4_multiline" / "multiline_demo.lua",
    };
    for( auto const& p : luas )
    {
        std::string cmd = "luac -p '" + p.string() + "' 2>&1";
        int const rc = std::system( cmd.c_str() );
        EXPECT_EQ( rc, 0 ) << "luac -p failed for " << p;
    }
}

//  Test 4 : AUTHORING_MEUS_ON_MAC.md mentions text_area + IME composition.
TEST( V4Samples, AuthoringGuideMentionsTextAreaAndIme )
{
    fs::path const guide = source_root() / "docs" /
                           "AUTHORING_MEUS_ON_MAC.md";
    if( !fs::is_regular_file( guide ) )
    {
        GTEST_SKIP() << "docs/AUTHORING_MEUS_ON_MAC.md not present ; "
                        "this is fine until c150-B docs land.";
        return;
    }
    auto const body = read_file_to_string( guide );
    EXPECT_NE( body.find( "text_area" ),    std::string::npos );
    //  IME / marked-text mentioned by either keyword.
    bool const has_ime =
        body.find( "IME" )         != std::string::npos ||
        body.find( "ime" )         != std::string::npos ||
        body.find( "marked" )      != std::string::npos ||
        body.find( "composition" ) != std::string::npos;
    EXPECT_TRUE( has_ime )
        << "guide must mention IME / marked / composition somewhere";
}

//  Test 5 : project_v1_ship_gate.md has v4 LANDED section AND a clear
//  closure note ("PROJECT CLOSURE" or "feature-complete" wording).
//  Per user "no more versions after v4" — gate the doctrine update.
TEST( V4Samples, ShipGateHas_v4_AndProjectClosureWording )
{
    fs::path const gate = source_root() / "memory" /
                          "project_v1_ship_gate.md";
    if( !fs::is_regular_file( gate ) )
    {
        GTEST_SKIP() << "ship-gate doctrine memo not present ; expected "
                        "after c150-B but tolerable if behind.";
        return;
    }
    auto const body = read_file_to_string( gate );
    EXPECT_NE( body.find( "v4" ), std::string::npos )
        << "ship-gate must mention v4 milestone";
    bool const has_closure =
        body.find( "PROJECT CLOSURE" )    != std::string::npos ||
        body.find( "project closure" )    != std::string::npos ||
        body.find( "feature-complete" )   != std::string::npos ||
        body.find( "feature complete" )   != std::string::npos ||
        body.find( "no more versions" )   != std::string::npos;
    EXPECT_TRUE( has_closure )
        << "ship-gate must record the user mandate of v4 = final ; "
           "either 'PROJECT CLOSURE', 'feature-complete', or "
           "'no more versions' wording";
}
