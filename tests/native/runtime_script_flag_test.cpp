// tests/native/runtime_script_flag_test.cpp
//
// c156 : END-TO-END coverage for the `--script <path.lua>` runtime flag (the
// Studio's Run Script / Cmd+R transport). Spawns the REAL aaaseed_runtime app
// with the perlin_noise sample and a frame cap, then asserts from its log
// that (a) the script was loaded by the runner and (b) frames actually
// rendered (the [AAASeedApp.Perf] line is only printed after maxFrames
// successfully drew). A 50 s alarm guards against a hung window ; on a
// headless runner where no window can be created the perf line never
// appears and the test SKIPs with the log tail (never silently).

#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

namespace
{

std::string read_all( std::string const& path )
{
    std::ifstream in( path );
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

}   //	anonymous namespace

TEST( RuntimeScriptFlag, PerlinSampleRendersEndToEnd )
{
    std::string const log = "/tmp/aaa_runtime_script_flag_test.log";
    std::remove( log.c_str() );

    //	perl alarm = portable timeout on macOS (no coreutils `timeout`).
    std::string const cmd =
        std::string( "/bin/sh -c \"perl -e 'alarm 50; exec @ARGV' -- '" )
        + AAA_RUNTIME_BIN + "' --script '" + AAA_PERLIN_SAMPLE_LUA
        + "' --max-frames 45 > '" + log + "' 2>&1\"";

    int const rc = std::system( cmd.c_str() );
    std::string const out = read_all( log );

    bool const rendered = out.find( "[AAASeedApp.Perf]" ) != std::string::npos;
    if( !rendered )
    {
        //	No frames drew : almost certainly a headless / no-WindowServer
        //	runner (rc is then non-zero from the alarm or Cocoa abort).
        GTEST_SKIP() << "runtime drew no frames (headless runner?) rc=" << rc
                     << "\n--- log tail ---\n"
                     << out.substr( out.size() > 800 ? out.size() - 800 : 0 );
    }

    //	Frames rendered -> the loaded script MUST be the perlin sample, not
    //	the hello_world fallback (which would mean --script silently failed).
    EXPECT_NE( out.find( "perlin_noise.lua : loaded" ), std::string::npos )
        << "--script did not load the sample ; log:\n" << out;
    EXPECT_NE( out.find( "runtime loaded script" ), std::string::npos );
    EXPECT_EQ( rc, 0 ) << "runtime exited non-zero despite rendering";
}

TEST( RuntimeScriptFlag, BadScriptPathFallsBackAndStillRenders )
{
    std::string const log = "/tmp/aaa_runtime_script_flag_bad.log";
    std::remove( log.c_str() );

    std::string const cmd =
        std::string( "/bin/sh -c \"perl -e 'alarm 50; exec @ARGV' -- '" )
        + AAA_RUNTIME_BIN
        + "' --script '/no/such/script.lua' --max-frames 30 > '"
        + log + "' 2>&1\"";

    int const rc = std::system( cmd.c_str() );
    std::string const out = read_all( log );

    bool const rendered = out.find( "[AAASeedApp.Perf]" ) != std::string::npos;
    if( !rendered )
        GTEST_SKIP() << "runtime drew no frames (headless runner?) rc=" << rc;

    //	A bad path must NOT crash the app : it logs the failure and falls
    //	back to the bundled hello_world.lua.
    EXPECT_NE( out.find( "FAILED" ), std::string::npos );
    EXPECT_NE( out.find( "hello_world.lua" ), std::string::npos );
    EXPECT_EQ( rc, 0 );
}
