// tests/unit/aaaseed_app_perf_test.cpp
//
// .app-level per-frame performance test (continuation 56). Launches
// the `aaaseed_app` bundle binary with `--max-frames 60`, captures
// its stdout, parses the `[AAASeedApp.Perf]` line emitted by
// AAASeedMTKView's drawInMTKView on termination, and asserts the
// per-frame averages stay within budget.
//
// Three-artifact perf doctrine (philosophy.md 2.95) :
//   1. CPU-side timing is gathered inside AAASeedMTKView via
//      `std::chrono::steady_clock` per drawInMTKView call (warmup
//      frame 0 excluded). Already-existing `os_signpost` per-frame
//      and Metal `pushDebugGroup` markers around the render pass
//      (continuation 27) cover the Instruments / Frame Capture
//      surfaces ; this test adds the in-process aggregate.
//   2. Wall-clock measurement = aggregate computed inside the .app
//      across the measured frames, reported as avg/max/min ms.
//   3. Budget assertion : avg-per-frame < 16 ms (60 Hz) ; max-per-
//      frame < 50 ms (any single frame must not stutter beyond 3x
//      the 60 Hz target).
//
// Why this is the right test shape : the existing gol_metal_perf
// test exercises a HEADLESS render path (no window, no MTKView, no
// NSApplication). This one exercises the SAME code path users see
// when they double-click aaaseed_app.app -- window-driven Metal
// drawable acquire + present cycle. Different cost profile ; both
// matter.
//
// CTest label : `perf;app` -- runs under `ctest -L perf` and also
// surfaces under `ctest -L app`.

#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <regex>
#include <string>

#ifndef AAA_APP_BINARY_PATH
#error "AAA_APP_BINARY_PATH must be defined by CMake : path to the aaaseed_app binary inside the .app bundle"
#endif

namespace
{
    //	Run the .app binary with --max-frames N, capture stdout. Return
    //	combined stdout text on success, empty string on launch failure.
    std::string run_app_capture_stdout( int max_frames )
    {
        std::string cmd;
        cmd.reserve( 256 );
        cmd.append( "\"" );
        cmd.append( AAA_APP_BINARY_PATH );
        cmd.append( "\" --max-frames " );
        cmd.append( std::to_string( max_frames ) );
        cmd.append( " 2>&1" );

        std::unique_ptr< FILE, decltype( &pclose ) > pipe( popen( cmd.c_str(), "r" ), &pclose );
        if( !pipe )
            return {};

        std::string out;
        std::array< char, 1024 > buf;
        while( std::fgets( buf.data(), buf.size(), pipe.get() ) != nullptr )
            out.append( buf.data() );
        return out;
    }
}

TEST( AaaseedAppPerf, PerFrameBudgetWithSixtyFrames )
{
    //	Run 60 frames. Frame 0 is the warmup and excluded ; we get 59
    //	measured frames. 60 Hz target = 16 ms / frame ; the .app does a
    //	single full-screen catalog-shader draw on each frame which on
    //	M4 should land in single-digit ms.
    //	Budget headroom is generous to tolerate debug-build noise and
    //	CI-runner contention -- a real frame-rate regression will still
    //	blow past these.
    constexpr int    kFrames            = 60;
    constexpr double kAvgBudgetMs       = 16.0;
    constexpr double kMaxBudgetMs       = 50.0;
    constexpr int    kRunTimeoutSeconds = 10;

    std::string const captured = run_app_capture_stdout( kFrames );
    ASSERT_FALSE( captured.empty() )
        << "aaaseed_app binary failed to launch or produce stdout. "
        << "Check AAA_APP_BINARY_PATH = " << AAA_APP_BINARY_PATH;

    //	Find the perf report line. Format pinned by AAASeedMTKView.mm :
    //	"[AAASeedApp.Perf] frames=X measured=Y avg_ms=Z.zzz max_ms=W.www min_ms=V.vvv\n"
    std::regex const re(
        R"(\[AAASeedApp\.Perf\]\s+frames=(\d+)\s+measured=(\d+)\s+)"
        R"(avg_ms=([0-9.]+)\s+max_ms=([0-9.]+)\s+min_ms=([0-9.]+))" );
    std::smatch m;
    ASSERT_TRUE( std::regex_search( captured, m, re ) )
        << "Could not find [AAASeedApp.Perf] line in captured stdout :\n"
        << captured;

    int    const frames   = std::atoi( m[ 1 ].str().c_str() );
    int    const measured = std::atoi( m[ 2 ].str().c_str() );
    double const avg_ms   = std::atof( m[ 3 ].str().c_str() );
    double const max_ms   = std::atof( m[ 4 ].str().c_str() );
    double const min_ms   = std::atof( m[ 5 ].str().c_str() );

    std::printf( "[AaaseedAppPerf] frames=%d measured=%d "
                 "avg=%.4f ms (budget %.1f) "
                 "max=%.4f ms (budget %.1f) "
                 "min=%.4f ms\n",
                 frames, measured, avg_ms, kAvgBudgetMs,
                 max_ms, kMaxBudgetMs, min_ms );

    //	Sanity-check : the warmup-excluded frame count should be exactly
    //	one less than total. If it isn't, the test is measuring something
    //	different than intended.
    EXPECT_EQ( frames, kFrames );
    EXPECT_EQ( measured, kFrames - 1 );

    EXPECT_LT( avg_ms, kAvgBudgetMs )
        << "Average frame " << avg_ms << " ms exceeds budget "
        << kAvgBudgetMs << " ms across " << measured << " measured frames.";
    EXPECT_LT( max_ms, kMaxBudgetMs )
        << "Slowest frame " << max_ms << " ms exceeds max budget "
        << kMaxBudgetMs << " ms. Indicates a frame stutter, not just a slowdown.";

    //	(void) on the variables not directly asserted -- avoids unused
    //	warnings under aggressive -Wall configs ; values still appear in
    //	the printf above.
    (void) min_ms;
    (void) kRunTimeoutSeconds;
}
