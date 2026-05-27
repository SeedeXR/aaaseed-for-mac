// tests/unit/stb_image_perf_test.cpp
//
// Performance-profiling test for the vendored stb_image PNG decoder.
// Closes the perf-coverage gap on the asset pipeline landed end-to-end
// in the .app (continuation 46) : the round-trip unit test
// (stb_image_test.cpp) proves correctness ; this one proves the
// throughput stays in budget.
//
// Three-artifact perf doctrine (philosophy.md 2.95) :
//   1. CPU-side `os_signpost` interval around the measured loop
//      (visible in Instruments / Time Profiler under subsystem
//      ai.bsa.aaaseed, category "performance").
//   2. Wall-clock measurement with std::chrono::steady_clock.
//   3. Budget assertion : per-decode latency for a 256x256 RGBA PNG
//      must stay under 20 ms on M4 debug build. Real-world decodes
//      typically land 0.5-2 ms ; the loose budget catches a 10x
//      regression without flapping on debug-build noise.
//
// Why 256x256 : matches the smallest "real" texture size the .app
// might load (test_pattern.png is 64x64 ; production assets are
// larger). Keeps the test under a second of wall clock while still
// exercising the inflate path beyond the trivial case.
//
// What this guards :
//   - stb_image header upgrade regressing the decode path.
//   - Apple Clang miscompiling stb's SIMD inflate inner loop after a
//     toolchain bump.
//   - A future port mistakenly disabling SSE2 / NEON shortcuts in
//     stb_image_impl.c.
//
// CTest label : `perf;stb_image`.

#include <gtest/gtest.h>

#include "stb_image.h"
#include "stb_image_write.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <vector>

#include <os/log.h>
#include <os/signpost.h>

namespace
{
    constexpr int kWidth        = 256;
    constexpr int kHeight       = 256;
    constexpr int kChannels     = 4;

    //	Generate a deterministic 256x256 RGBA pattern. Gradient-style
    //	so the PNG's deflate stream has some real work to do (a flat
    //	color image compresses to almost nothing and would skew the
    //	decode throughput measurement).
    std::vector< std::uint8_t > make_pattern_256()
    {
        std::vector< std::uint8_t > p( kWidth * kHeight * kChannels, 0 );
        for( int y = 0; y < kHeight; ++y )
        {
            for( int x = 0; x < kWidth; ++x )
            {
                std::size_t const i = ( y * kWidth + x ) * kChannels;
                p[ i + 0 ] = std::uint8_t( x );
                p[ i + 1 ] = std::uint8_t( y );
                p[ i + 2 ] = std::uint8_t( ( x + y ) & 0xff );
                p[ i + 3 ] = 255;
            }
        }
        return p;
    }

    void png_write_callback( void* user, void* data, int size )
    {
        auto* out = static_cast< std::vector< std::uint8_t >* >( user );
        std::uint8_t const* p = static_cast< std::uint8_t const* >( data );
        out->insert( out->end(), p, p + size );
    }

    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "ai.bsa.aaaseed", "performance" );
        return log;
    }
}

TEST( StbImagePerf, DecodeStaysUnderBudget )
{
    //	Encode once into an in-memory PNG ; the measured loop only
    //	pays the DECODE cost, not encode-then-decode.
    std::vector< std::uint8_t > const original = make_pattern_256();
    std::vector< std::uint8_t > png_bytes;
    int const enc_ok = stbi_write_png_to_func(
        &png_write_callback, &png_bytes,
        kWidth, kHeight, kChannels,
        original.data(),
        kWidth * kChannels );
    ASSERT_NE( enc_ok, 0 );
    ASSERT_GT( png_bytes.size(), size_t( 1024 ) )
        << "PNG suspiciously small ; gradient should compress to >1KB";

    //	Sanity-check : one decode succeeds before the loop.
    {
        int w = 0, h = 0, c = 0;
        std::uint8_t* probe = stbi_load_from_memory(
            png_bytes.data(), int( png_bytes.size() ),
            &w, &h, &c, kChannels );
        ASSERT_NE( probe, nullptr ) << stbi_failure_reason();
        ASSERT_EQ( w, kWidth );
        ASSERT_EQ( h, kHeight );
        stbi_image_free( probe );
    }

    //	Budget : 20 ms per decode on M4 debug. Production target is
    //	well under 5 ms. The 4x margin catches real regressions while
    //	tolerating debug-build noise.
    constexpr int    kIterations         = 200;
    constexpr double kBudgetMsPerDecode  = 20.0;
    constexpr int    kWarmupDecodes      = 5;

    //	Warmup : the first few decodes warm the icache + branch
    //	predictor for stb's inflate inner loop. Discard their time.
    for( int i = 0; i < kWarmupDecodes; ++i )
    {
        int w = 0, h = 0, c = 0;
        std::uint8_t* decoded = stbi_load_from_memory(
            png_bytes.data(), int( png_bytes.size() ),
            &w, &h, &c, kChannels );
        ASSERT_NE( decoded, nullptr );
        stbi_image_free( decoded );
    }

    os_signpost_id_t spid = os_signpost_id_generate( perf_log() );
    os_signpost_interval_begin( perf_log(), spid, "stb_image.decode.loop",
                                "iterations=%d size=%dx%d",
                                kIterations, kWidth, kHeight );

    auto const t0 = std::chrono::steady_clock::now();

    for( int i = 0; i < kIterations; ++i )
    {
        int w = 0, h = 0, c = 0;
        std::uint8_t* decoded = stbi_load_from_memory(
            png_bytes.data(), int( png_bytes.size() ),
            &w, &h, &c, kChannels );
        ASSERT_NE( decoded, nullptr ) << stbi_failure_reason();
        stbi_image_free( decoded );
    }

    auto const t1 = std::chrono::steady_clock::now();
    os_signpost_interval_end( perf_log(), spid, "stb_image.decode.loop",
                              "iterations=%d size=%dx%d",
                              kIterations, kWidth, kHeight );

    double const total_ms = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
    double const avg_ms   = total_ms / kIterations;
    //	Throughput in pixels-per-second -- the headline number for
    //	asset-pipeline planning.
    double const pix_per_sec = double( kWidth * kHeight ) * 1000.0 / avg_ms;

    std::printf( "[StbImagePerf] %d decodes of %dx%d RGBA PNG (%zu bytes) "
                 "in %.2f ms (avg %.3f ms ; %.1f Mpix/sec ; budget %.1f ms)\n",
                 kIterations, kWidth, kHeight, png_bytes.size(),
                 total_ms, avg_ms, pix_per_sec / 1e6,
                 kBudgetMsPerDecode );

    EXPECT_LT( avg_ms, kBudgetMsPerDecode )
        << "Average decode " << avg_ms
        << " ms exceeds budget " << kBudgetMsPerDecode << " ms.";
}
