// tests/unit/tinyexr_perf_test.cpp
//
// Performance-profiling test for tinyexr decode throughput (continuation
// 73). Closes the perf-coverage gap on the Phase 5 fifth beachhead
// (tinyexr, landed c67). Mirrors the c54 stb_image perf shape : encode
// a known float-RGBA pattern to EXR in memory ONCE, decode it N=100
// times in a tight loop with os_signpost interval + chrono + budget
// assertion.
//
// Three-artifact perf doctrine (philosophy.md 2.95) :
//   1. CPU-side `os_signpost` interval (visible in Instruments under
//      ai.bsa.aaaseed / performance / tinyexr.decode.loop).
//   2. Wall-clock std::chrono::steady_clock measurement.
//   3. Budget assertion : 50 ms/decode on M4 debug (EXR ZIP decompress
//      + 16/32-bit-per-channel float reconstruction is meaningfully
//      heavier than stb_image's PNG path).
//
// Headline number : Mpix/sec throughput. Same unit as the c54 stb_image
// perf test for direct comparison ; future asset-pipeline planning
// compares image / EXR / TTF rasterize all in pixels-per-second.
//
// CTest label : `perf;tinyexr`.

#include <gtest/gtest.h>

#include "tinyexr.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include <os/log.h>
#include <os/signpost.h>

namespace
{
    //	128x128 = 16384 pixels. Larger than c54's stb_image (256x256 =
    //	65536) for PNG but EXR's per-channel float math makes 128x128
    //	already meaningfully sized ; total decode work is similar.
    constexpr int kWidth    = 128;
    constexpr int kHeight   = 128;
    constexpr int kChannels = 4;

    //	Deterministic 128x128 FP32 RGBA gradient. Distinct float
    //	values per pixel + per channel so ZIP-compression has real
    //	entropy to chew (a flat-color image would compress to nothing
    //	and not exercise the decode path properly).
    std::vector< float > make_pattern_128()
    {
        std::vector< float > p( kWidth * kHeight * kChannels, 0.0f );
        for( int y = 0; y < kHeight; ++y )
        {
            for( int x = 0; x < kWidth; ++x )
            {
                std::size_t const i = ( y * kWidth + x ) * kChannels;
                p[ i + 0 ] = float( x ) / float( kWidth );
                p[ i + 1 ] = float( y ) / float( kHeight );
                p[ i + 2 ] = float( ( x + y ) % 17 ) / 17.0f;
                p[ i + 3 ] = 1.0f;
            }
        }
        return p;
    }

    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "ai.bsa.aaaseed", "performance" );
        return log;
    }
}

TEST( TinyExrPerf, DecodeStaysUnderBudget )
{
    //	Encode once into an in-memory EXR ; the measured loop pays
    //	only the DECODE cost.
    std::vector< float > const original = make_pattern_128();
    unsigned char* exr_bytes  = nullptr;
    char const*    save_err   = nullptr;
    int const      bytes_writ = SaveEXRToMemory(
        original.data(),
        kWidth, kHeight,
        kChannels,
        /*save_as_fp16*/ 0,   // FP32 = lossless
        &exr_bytes,
        &save_err );
    ASSERT_GT( bytes_writ, 0 )
        << "SaveEXRToMemory failed : "
        << ( save_err ? save_err : "(no err)" );
    ASSERT_NE( exr_bytes, nullptr );

    //	Sanity-check one decode succeeds before the measured loop.
    {
        float* probe = nullptr;
        int    w = 0, h = 0;
        char const* err = nullptr;
        int const ok = LoadEXRFromMemory( &probe, &w, &h,
                                          exr_bytes, (std::size_t) bytes_writ, &err );
        ASSERT_EQ( ok, TINYEXR_SUCCESS )
            << "LoadEXRFromMemory probe failed : " << ( err ? err : "(no err)" );
        ASSERT_NE( probe, nullptr );
        ASSERT_EQ( w, kWidth );
        ASSERT_EQ( h, kHeight );
        std::free( probe );
    }

    //	Budget : 50 ms per decode on M4 debug. Real-world target is
    //	~5-10 ms for FP32 ZIP-compressed 128x128 ; the 5-10x margin
    //	accommodates debug-build noise + future tinyexr / stb_zlib
    //	upgrades that might add overhead.
    constexpr int    kIterations         = 100;
    constexpr double kBudgetMsPerDecode  = 50.0;
    constexpr int    kWarmupDecodes      = 5;

    //	Warmup : first decodes pay icache + branch-predictor warmup
    //	for the ZIP inflate + EXR scanline assembler. Discard their
    //	time.
    for( int i = 0; i < kWarmupDecodes; ++i )
    {
        float* decoded = nullptr;
        int    w = 0, h = 0;
        char const* err = nullptr;
        int const ok = LoadEXRFromMemory( &decoded, &w, &h,
                                          exr_bytes, (std::size_t) bytes_writ, &err );
        ASSERT_EQ( ok, TINYEXR_SUCCESS );
        std::free( decoded );
    }

    os_signpost_id_t spid = os_signpost_id_generate( perf_log() );
    os_signpost_interval_begin( perf_log(), spid, "tinyexr.decode.loop",
                                "iterations=%d size=%dx%d",
                                kIterations, kWidth, kHeight );

    auto const t0 = std::chrono::steady_clock::now();

    for( int i = 0; i < kIterations; ++i )
    {
        float* decoded = nullptr;
        int    w = 0, h = 0;
        char const* err = nullptr;
        int const ok = LoadEXRFromMemory( &decoded, &w, &h,
                                          exr_bytes, (std::size_t) bytes_writ, &err );
        ASSERT_EQ( ok, TINYEXR_SUCCESS )
            << "iter " << i << " : " << ( err ? err : "(no err)" );
        std::free( decoded );
    }

    auto const t1 = std::chrono::steady_clock::now();
    os_signpost_interval_end( perf_log(), spid, "tinyexr.decode.loop",
                              "iterations=%d size=%dx%d",
                              kIterations, kWidth, kHeight );

    double const total_ms = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
    double const avg_ms   = total_ms / kIterations;
    double const pix_per_sec = double( kWidth * kHeight ) * 1000.0 / avg_ms;

    std::printf( "[TinyExrPerf] %d decodes of %dx%d FP32 RGBA EXR (%d bytes) "
                 "in %.2f ms (avg %.3f ms ; %.2f Mpix/sec ; budget %.1f ms)\n",
                 kIterations, kWidth, kHeight, bytes_writ,
                 total_ms, avg_ms, pix_per_sec / 1e6,
                 kBudgetMsPerDecode );

    EXPECT_LT( avg_ms, kBudgetMsPerDecode )
        << "Average decode " << avg_ms
        << " ms exceeds budget " << kBudgetMsPerDecode << " ms.";

    std::free( exr_bytes );
}
