// tests/unit/path_a_compile_perf_test.cpp
//
// Performance-profiling test for the Path A regression catalog --
// measures per-shader Metal compile time across every `.metal` file
// in src/shaders/msl/. Embedded perf doctrine (philosophy.md 2.95) :
//   1. CPU-side `os_signpost` interval per shader, plus one wrapping
//      the whole sweep (visible in Instruments / Time Profiler under
//      subsystem com.seedexr.aaaseed, category "performance").
//   2. Wall-clock measurement with std::chrono::steady_clock per
//      shader + total.
//   3. Budget assertion : no single shader exceeds the per-shader cap.
//
// Why : the Path A catalog (continuation 31+) auto-discovers ports
// dropped into src/shaders/msl/. Compile time is the iteration cost
// on every dev rebuild + first-launch of aaaseed_app.app. A perf test
// catches :
//   - Apple shipping a Metal compiler regression that doubles compile
//     time for AAASeed-style fragment shaders.
//   - A future port that's pathologically large (eg, 10x bigger than
//     average) and would inflate startup.
//   - The .metallib pre-compile path (Phase 8 todo) eventually
//     winning -- this test will then assert the runtime path is
//     SLOWER than .metallib, justifying the switch.
//
// CTest label : `perf;path_a` -- runs under `ctest -L perf` and also
// surfaces under `ctest -L path_a` alongside the catalog regression.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <os/log.h>
#include <os/signpost.h>

#ifndef AAA_SHADERS_MSL_DIR
#error "AAA_SHADERS_MSL_DIR must be defined by CMake -- points at src/shaders/msl/"
#endif

namespace
{
    std::string slurp( std::filesystem::path const& p )
    {
        std::ifstream in( p, std::ios::binary );
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    std::vector< std::filesystem::path > collect_metal_files()
    {
        std::vector< std::filesystem::path > out;
        std::filesystem::path const root( AAA_SHADERS_MSL_DIR );
        if( !std::filesystem::is_directory( root ) )
            return out;
        for( auto const& entry : std::filesystem::directory_iterator( root ) )
        {
            if( entry.is_regular_file() && entry.path().extension() == ".metal" )
                out.push_back( entry.path() );
        }
        std::sort( out.begin(), out.end() );
        return out;
    }

    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "com.seedexr.aaaseed", "performance" );
        return log;
    }
}

TEST( PathACompilePerf, EveryShaderCompilesUnderBudget )
{
    //	Per-shader budget : 250 ms on M4 debug build. Real shaders today
    //	compile in 5-30 ms ; the budget catches an order-of-magnitude
    //	regression without flapping on measurement noise.
    constexpr double kPerShaderBudgetMs = 250.0;
    //	Whole-sweep budget : 2 s on M4 debug for ~25 shaders. Scales as
    //	the catalog grows -- target is "all shaders compile within a
    //	dev's coffee-sip wait". GitHub-hosted macos-14 runners are
    //	~3-4x slower than a local M-series machine (shared CI host,
    //	virtualized Metal driver), so we relax the sweep budget on CI
    //	to keep the test meaningful without flapping on hardware noise.
    //	The per-shader budget (250 ms) stays strict on both : individual
    //	shaders still compile well under it even on CI (~42 ms avg with
    //	169 shaders at 7 s sweep).
    bool   const is_ci               = std::getenv( "GITHUB_ACTIONS" ) != nullptr;
    double const kTotalSweepBudgetMs = is_ci ? 12000.0 : 2000.0;

    auto const metal_files = collect_metal_files();
    ASSERT_GE( metal_files.size(), size_t( 21 ) )
        << "Expected >=21 .metal files ; found " << metal_files.size();

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    //	Warmup : the FIRST newLibrary call pays a one-shot init cost on
    //	the Metal compiler thread (typical 30-100 ms on top of the real
    //	compile). Compile + discard the first shader so the measured
    //	loop sees steady-state cost only.
    {
        std::string const warmup_source = slurp( metal_files.front() );
        GOL::ProgramId const wp = backend.create_program_msl(
            warmup_source.c_str(), "vs_main", "fs_main" );
        if( wp != GOL::kInvalidProgramId )
            backend.delete_program( wp );
    }

    os_signpost_id_t sweep_spid = os_signpost_id_generate( perf_log() );
    os_signpost_interval_begin( perf_log(), sweep_spid, "path_a.compile.sweep",
                                "shaders=%zu", metal_files.size() );

    auto const t_sweep0 = std::chrono::steady_clock::now();

    double max_ms = 0.0;
    std::string max_name;
    double total_ms = 0.0;

    for( auto const& p : metal_files )
    {
        SCOPED_TRACE( "shader = " + p.filename().string() );

        std::string const source = slurp( p );
        ASSERT_FALSE( source.empty() );

        os_signpost_id_t spid = os_signpost_id_generate( perf_log() );
        os_signpost_interval_begin( perf_log(), spid, "path_a.compile",
                                    "name=%s", p.filename().string().c_str() );

        auto const t0 = std::chrono::steady_clock::now();
        GOL::ProgramId const prog = backend.create_program_msl(
            source.c_str(), "vs_main", "fs_main" );
        auto const t1 = std::chrono::steady_clock::now();

        os_signpost_interval_end( perf_log(), spid, "path_a.compile",
                                  "name=%s", p.filename().string().c_str() );

        ASSERT_NE( prog, GOL::kInvalidProgramId )
            << "MetalBackend rejected " << p.filename().string()
            << " : " << backend.get_last_error();

        double const ms = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
        total_ms += ms;
        if( ms > max_ms )
        {
            max_ms = ms;
            max_name = p.filename().string();
        }

        EXPECT_LT( ms, kPerShaderBudgetMs )
            << "Shader " << p.filename().string()
            << " compile " << ms << " ms exceeds per-shader budget "
            << kPerShaderBudgetMs << " ms.";

        backend.delete_program( prog );
    }

    auto const t_sweep1 = std::chrono::steady_clock::now();
    os_signpost_interval_end( perf_log(), sweep_spid, "path_a.compile.sweep",
                              "shaders=%zu", metal_files.size() );

    double const sweep_ms = std::chrono::duration< double, std::milli >( t_sweep1 - t_sweep0 ).count();
    double const avg_ms   = total_ms / double( metal_files.size() );

    std::printf( "[PathACompilePerf] %zu shaders in %.2f ms "
                 "(avg %.2f ms ; max %.2f ms = %s ; sweep wall %.2f ms ; "
                 "per-shader budget %.0f ms ; sweep budget %.0f ms)\n",
                 metal_files.size(), total_ms, avg_ms, max_ms,
                 max_name.c_str(), sweep_ms,
                 kPerShaderBudgetMs, kTotalSweepBudgetMs );

    EXPECT_LT( sweep_ms, kTotalSweepBudgetMs )
        << "Total catalog compile " << sweep_ms
        << " ms exceeds sweep budget " << kTotalSweepBudgetMs << " ms.";
}
