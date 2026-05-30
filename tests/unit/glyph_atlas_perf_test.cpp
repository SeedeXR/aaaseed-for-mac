// tests/unit/glyph_atlas_perf_test.cpp
//
// Performance-profiling test for aaa::text::build_printable_ascii_atlas
// (continuation 74). Closes the perf-coverage gap on the c58 atlas
// helper. Mirrors the c54 stb_image / c73 tinyexr perf shape : load
// font ONCE, then build the atlas N=100 times in a tight loop with
// os_signpost interval + chrono + budget assertion.
//
// What this measures : the cost the .app pays once at boot to build
// the FPS HUD's glyph atlas. The atlas builder wraps stb_truetype's
// pack API (stbtt_PackBegin / PackFontRange / PackEnd) -- a future
// stb_truetype upgrade or pack-algorithm change would surface here.
//
// Headline number : ms / atlas-build. Compare to the encode/decode
// throughput perf tests (52.7 Mpix/sec PNG decode, 4.63 Mpix/sec EXR
// decode) -- this one is "per-build" not per-pixel because the atlas
// is a fixed-size artifact computed once.
//
// CTest label : `perf;glyph_atlas`.

#include <gtest/gtest.h>

#include "src/text/glyph_atlas.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <vector>

#include <os/log.h>
#include <os/signpost.h>

#ifndef AAA_STB_TT_FONT_PATH
#error "AAA_STB_TT_FONT_PATH must be defined by CMake"
#endif

namespace
{
    std::vector< std::uint8_t > load_file_bytes( char const* path )
    {
        std::ifstream in( path, std::ios::binary | std::ios::ate );
        if( !in ) return {};
        auto const size = in.tellg();
        in.seekg( 0, std::ios::beg );
        std::vector< std::uint8_t > out( static_cast< std::size_t >( size ) );
        if( !in.read( reinterpret_cast< char* >( out.data() ), size ) )
            return {};
        return out;
    }

    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "com.seedexr.aaaseed", "performance" );
        return log;
    }
}

TEST( GlyphAtlasPerf, BuildStaysUnderBudget )
{
    //	Load font once ; the measured loop only pays the BUILD cost.
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), std::size_t( 1024 ) );

    //	Sanity-check one build succeeds.
    {
        aaa::text::GlyphAtlas probe;
        ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
            font_bytes.data(), font_bytes.size(),
            16.0f, 256, 256, probe ) );
        EXPECT_EQ( probe.width, 256 );
        EXPECT_EQ( probe.height, 256 );
    }

    //	Budget : 30 ms per build on M4 debug. The .app boots once and
    //	pays this once ; 30 ms is well under the user-visible "instant
    //	startup" threshold. Real-world target is ~5-15 ms based on
    //	stb_truetype's pack API characteristics ; 2-5x margin tolerates
    //	debug-build noise + future stb_truetype upgrades.
    constexpr int    kIterations         = 100;
    constexpr double kBudgetMsPerBuild   = 30.0;
    constexpr int    kWarmupBuilds       = 3;

    //	Warmup : first builds warm the malloc heap + branch predictor
    //	for stbtt's pack inner loop.
    for( int i = 0; i < kWarmupBuilds; ++i )
    {
        aaa::text::GlyphAtlas atlas;
        ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
            font_bytes.data(), font_bytes.size(),
            16.0f, 256, 256, atlas ) );
    }

    os_signpost_id_t spid = os_signpost_id_generate( perf_log() );
    os_signpost_interval_begin( perf_log(), spid, "glyph_atlas.build.loop",
                                "iterations=%d pixel_height=%d",
                                kIterations, 16 );

    auto const t0 = std::chrono::steady_clock::now();

    for( int i = 0; i < kIterations; ++i )
    {
        aaa::text::GlyphAtlas atlas;
        ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
            font_bytes.data(), font_bytes.size(),
            16.0f, 256, 256, atlas ) );
    }

    auto const t1 = std::chrono::steady_clock::now();
    os_signpost_interval_end( perf_log(), spid, "glyph_atlas.build.loop",
                              "iterations=%d pixel_height=%d",
                              kIterations, 16 );

    double const total_ms = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
    double const avg_ms   = total_ms / kIterations;

    std::printf( "[GlyphAtlasPerf] %d builds of 256x256 atlas @ 16px from "
                 "SourceCodePro-Medium.ttf in %.2f ms (avg %.3f ms ; budget %.1f ms)\n",
                 kIterations, total_ms, avg_ms, kBudgetMsPerBuild );

    EXPECT_LT( avg_ms, kBudgetMsPerBuild )
        << "Average atlas build " << avg_ms
        << " ms exceeds budget " << kBudgetMsPerBuild << " ms.";
}
