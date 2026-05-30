// tests/unit/phase3_per_frame_perf_test.cpp
//
// Phase 3 EXIT per-frame perf baseline (continuation 121-B).
//
// The Phase 3 EXIT criterion (memory/todo.md L209) demands "frame-time
// within 1.2x Windows reference" on a representative path. c120 nailed
// the Mac-half of the pixel-comparable golden-frame contract via the
// `ps_Maa_add_scale.metal` shader through MetalBackend at 256x256.
//
// This test takes the SAME shader (same Path A catalog member, same
// fully-deterministic ABI) and drives it at 1280x720 BGRA8 -- the
// resolution the .app actually presents -- for N=100 iterations to
// establish the Mac-side per-frame budget number. Once the Windows
// reference lands (c126+), Phase 3 EXIT compares mean-Mac to mean-Win
// and the 1.2x envelope can be asserted cross-platform.
//
// Three-artifact perf doctrine (philosophy.md 2.95) :
//   1. CPU os_signpost interval per-frame (Instruments / Time Profiler
//      under subsystem com.seedexr.aaaseed). Reuses the proven shim wiring
//      from gol_metal_perf_test.cpp + nsight_signpost_test.cpp.
//   2. Wall-clock per-frame timing with std::chrono::steady_clock.
//      We collect per-frame samples to derive mean / median / p95 so
//      the headline characterizes BOTH steady-state and tail latency.
//   3. Budget assertion : mean is bounded by an Apple-M4-reasonable
//      envelope [10us, 5000us]. The upper bound (5ms = 200fps lower
//      bound) is the actual EXIT bar for a single shader pass at
//      1280x720 on Apple Silicon ; the lower bound rules out broken
//      setup (e.g. timing skipped, GPU never bound).
//
// Why ps_Maa_add_scale (not full-screen-red) :
//   - Real catalog shader. Full-screen-red has no uniform/texture
//     bindings ; the budget would not characterize a real-MEU frame.
//   - Deterministic. No iTime, no noise, no feedback. The shader's
//     per-frame work is pixel-stable, so the timing variance is purely
//     dispatch overhead -- exactly what we want to baseline.
//   - Already proven path : c38 (path_a_runtime_test) + c120 (golden
//     regression) both exercise this shader successfully.
//
// CTest label : `unit;perf;phase3` -- counted under `ctest -L perf`.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <os/log.h>
#include <os/signpost.h>
#include <sstream>
#include <string>
#include <vector>

#ifndef AAA_SHADERS_MSL_DIR
#error "AAA_SHADERS_MSL_DIR must be defined by CMake : path to src/shaders/msl/"
#endif

namespace
{
    //	Mirror the AaaFuVec4s ABI from path_a_runtime_test.cpp / c120.
    //	16 vec4 slots ; slot 0 = offset, slot 1 = scale.
    struct AaaFuVec4s
    {
        float values[ 16 ][ 4 ];
    };

    std::string slurp( std::filesystem::path const& p )
    {
        std::ifstream in( p, std::ios::binary );
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    //	Per-process signpost log handle. Matches the subsystem used by
    //	the NSightEvents.h Mac branch and gol_metal_perf_test so Instruments
    //	groups all aaaseed perf markers together.
    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "com.seedexr.aaaseed", "performance" );
        return log;
    }
}

TEST( Phase3PerFramePerf, PsMaaAddScale_1280x720_BgraStaysUnderBudget )
{
    constexpr std::uint32_t kWidth         = 1280;
    constexpr std::uint32_t kHeight        = 720;
    constexpr std::uint32_t kSrcW          = 256;   // small source ; tile-sampled
    constexpr std::uint32_t kSrcH          = 256;
    constexpr int           kIterations    = 100;
    constexpr int           kWarmupFrames  = 5;
    //	Envelope per spec : implausibly low (broken setup / skipped work)
    //	OR catastrophically high (>5ms per frame == sub-200fps lower bound
    //	for a single shader pass at 720p on Apple Silicon).
    constexpr double        kMinMeanUs     = 10.0;
    constexpr double        kMaxMeanUs     = 5000.0;

    //	-------- Backend + program ---------------------------------------
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_add_scale.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    //	Pass BGRA8 as the program's target color format so the pipeline-
    //	state-object matches the render target's pixel layout. Mismatch
    //	yields a Metal validation error at draw time.
    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main",
        GOL::TextureFormat::BGRA8 );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    //	-------- Source texture (small, tile-sampled) --------------------
    GOL::TextureId src_tex = backend.gen_texture_2d( kSrcW, kSrcH,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > src_px( std::size_t( kSrcW ) * kSrcH * 4, 0 );
    for( std::uint32_t y = 0; y < kSrcH; ++y )
        for( std::uint32_t x = 0; x < kSrcW; ++x )
        {
            std::size_t const i = ( y * kSrcW + x ) * 4;
            src_px[ i + 0 ] = std::uint8_t( ( x * 255u ) / ( kSrcW - 1 ) );
            src_px[ i + 1 ] = std::uint8_t( ( y * 255u ) / ( kSrcH - 1 ) );
            src_px[ i + 2 ] = 128;
            src_px[ i + 3 ] = 255;
        }
    backend.texture_data_2d( src_tex, src_px.data(), kSrcW * 4 );

    //	-------- Render target : BGRA8 1280x720 -- the .app's pixel fmt ---
    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::BGRA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	-------- Uniform buffer (same offset+scale as c120 golden) -------
    AaaFuVec4s u{};
    u.values[ 0 ][ 0 ] = 0.10f;
    u.values[ 0 ][ 1 ] = 0.05f;
    u.values[ 0 ][ 2 ] = 0.20f;
    u.values[ 0 ][ 3 ] = 0.00f;
    u.values[ 1 ][ 0 ] = 0.70f;
    u.values[ 1 ][ 1 ] = 0.80f;
    u.values[ 1 ][ 2 ] = 0.60f;
    u.values[ 1 ][ 3 ] = 1.00f;

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    //	-------- Render pass descriptor ----------------------------------
    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment   = rt;
    rpd.load_action        = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ]   = 0.0f;
    rpd.clear_color[ 1 ]   = 0.0f;
    rpd.clear_color[ 2 ]   = 0.0f;
    rpd.clear_color[ 3 ]   = 1.0f;
    rpd.debug_label        = "phase3.perf.ps_Maa_add_scale";

    auto render_one_frame = [ & ]()
    {
        ASSERT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        //	Shader declares 4 texture slots ; bind src to all to satisfy
        //	Metal validation (c120 pattern).
        backend.bind_fragment_texture( src_tex, 0 );
        backend.bind_fragment_texture( src_tex, 1 );
        backend.bind_fragment_texture( src_tex, 2 );
        backend.bind_fragment_texture( src_tex, 3 );
        backend.bind_fragment_buffer(  ub, 0, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();
    };

    //	Warmup : excluded from measurement. First frames pay pipeline-
    //	state-object construction + caching costs that are amortized.
    for( int i = 0; i < kWarmupFrames; ++i )
        render_one_frame();

    //	-------- Measured loop -------------------------------------------
    os_signpost_id_t loop_id = os_signpost_id_generate( perf_log() );
    os_signpost_interval_begin( perf_log(), loop_id, "phase3.perf.loop",
                                "iterations=%d", kIterations );

    std::vector< double > frame_us;
    frame_us.reserve( kIterations );

    for( int i = 0; i < kIterations; ++i )
    {
        os_signpost_id_t fid = os_signpost_id_generate( perf_log() );
        os_signpost_interval_begin( perf_log(), fid, "phase3.perf.frame",
                                    "i=%d", i );

        auto const t0 = std::chrono::steady_clock::now();
        render_one_frame();
        auto const t1 = std::chrono::steady_clock::now();

        os_signpost_interval_end( perf_log(), fid, "phase3.perf.frame",
                                  "i=%d", i );

        double const us = std::chrono::duration< double, std::micro >(
                              t1 - t0 ).count();
        frame_us.push_back( us );
    }

    os_signpost_interval_end( perf_log(), loop_id, "phase3.perf.loop",
                              "iterations=%d", kIterations );

    //	-------- Stats : mean / median / p95 -----------------------------
    ASSERT_EQ( int( frame_us.size() ), kIterations );

    double sum = 0.0;
    for( double v : frame_us ) sum += v;
    double const mean_us = sum / double( kIterations );

    std::vector< double > sorted = frame_us;
    std::sort( sorted.begin(), sorted.end() );
    double const median_us = sorted[ kIterations / 2 ];
    //	p95 : the value at the 95th percentile index. For N=100 this is
    //	index 95 (0-based). The 5 slowest frames sit above this value.
    double const p95_us    = sorted[ ( kIterations * 95 ) / 100 ];

    std::printf( "[ phase3 perf ] mean=%.1fus median=%.1fus p95=%.1fus "
                 "N=%d (1280x720, ps_Maa_add_scale)\n",
                 mean_us, median_us, p95_us, kIterations );

    //	-------- Budget assertion ----------------------------------------
    EXPECT_GT( mean_us, kMinMeanUs )
        << "Mean frame time " << mean_us << "us is implausibly low (< "
        << kMinMeanUs << "us). The render path likely skipped work -- "
        << "broken setup, GPU command never enqueued, or the timing "
        << "window missed the draw.";
    EXPECT_LT( mean_us, kMaxMeanUs )
        << "Mean frame time " << mean_us << "us exceeds the Phase 3 EXIT "
        << "Mac-side budget bar of " << kMaxMeanUs << "us (sub-200fps for "
        << "a single shader pass at 1280x720 on Apple Silicon).";

    //	-------- Cleanup -------------------------------------------------
    backend.delete_buffer(  ub      );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}
