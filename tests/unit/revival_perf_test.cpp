// tests/unit/revival_perf_test.cpp
//
// Phase 3 Path A revival perf budgets (continuation : c135-A, c138-A, c139-A
// follow-up). Each of the three goldens already validated correctness at
// 256x256 ; this file establishes the per-frame wall-clock budget on the
// Mac-side at the .app's actual presentation resolution 1280x720 BGRA8.
//
// Why a NEW file (not extending phase3_per_frame_perf_test.cpp) :
//   - That file's scope is the c121-B baseline shader ps_Maa_add_scale ;
//     adding three more shaders to it bloats the per-target boundary
//     and complicates the link line (each revival pulls a different MSL
//     uniform layout). Keeping revivals in their own test target preserves
//     the c121-B baseline as a stable reference.
//   - The project doctrine ("Performance profiling is embedded in every
//     Metal backend change ... a CTest under label `perf` that asserts
//     the path stays within budget") was satisfied for c121-B but
//     deferred for c135-A, c138-A, c139-A. This file closes that gap.
//
// Three perf-labelled tests, 1280x720 BGRA8 single-pipeline each :
//   1. RevivalPerf.AaaNoiseReal_UnderBudget       (c135-A) -- single pass,
//      4-quadrant Perlin + Simplex + FBM. 30 iterations, mean < 12ms.
//   2. RevivalPerf.AaaBloomReal_UnderBudget       (c138-A) -- 4-pass
//      threshold + Hblur + Vblur + composite. 20 iterations, mean < 20ms.
//   3. RevivalPerf.AaaMotionBlurReal_UnderBudget  (c139-A) -- single pass,
//      N=16 taps along projected velocity. 30 iterations, mean < 15ms.
//
// Budgets are debug-mode envelopes on Apple Silicon. Each test :
//   - 3 warmup frames (pipeline-state-object cache + first-launch
//     amortization), matches c121-B convention.
//   - os_signpost interval per measured frame (cross-tool Instruments
//     correlation under subsystem ai.bsa.aaaseed). 3 perf tests x 1
//     per-frame interval = 3 interval kinds.
//   - Mean + p95 collected and printed on every run, EXPECT_LT vs budget.
//
// CTest label : "perf;unit;revival;phase3" -- perf-first per the
// [[ctest-label-first-only]] doctrine (gtest_discover_tests honours only
// the first label on this CMake/AppleClang toolchain, so `ctest -L perf`
// requires `perf` to be the first list element).
//
// No collision with c140-A : that session works on
// src/shaders/msl/aaa_dof_hex_bokeh_real.metal +
// tests/regression/phase3_golden_frame_regression_test.cpp +
// tests/unit/path_a_catalog_test.cpp. This file's only c++ touch is
// THIS new file + an addition block to tests/unit/CMakeLists.txt.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
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
    //	Engine-ABI uniform structs mirrored from the regression test.
    //	Layouts MUST match the corresponding MSL `struct AaaFu*` exactly.
    struct AaaFuFloats { float        values[ 16 ];      };
    struct AaaFuVec4s  { float        values[ 16 ][ 4 ]; };
    struct AaaFuInts   { std::int32_t values[ 16 ];      };

    //	Mirror of MSL `struct AaaCamReal` -- 288 bytes, no trailing pad.
    struct AaaCamReal_Cpu
    {
        float view      [ 16 ];
        float proj      [ 16 ];
        float view_inv  [ 16 ];
        float vp        [ 16 ];
        float eye_aspect[  4 ];
        float fov_clip  [  4 ];
    };
    static_assert( sizeof( AaaCamReal_Cpu ) == 288,
                   "AaaCamReal_Cpu layout drifted from aaa_cam_real.metal" );

    //	Render dimensions : c121-B benchmark size = actual .app present size.
    constexpr std::uint32_t kWidth        = 1280;
    constexpr std::uint32_t kHeight       =  720;
    constexpr std::size_t   kBytesPerRow  = std::size_t( kWidth ) * 4;
    constexpr std::size_t   kBufferBytes  = kBytesPerRow * kHeight;

    std::string slurp( std::filesystem::path const& p )
    {
        std::ifstream in( p, std::ios::binary );
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    //	Per-process signpost log handle. Matches the subsystem used by the
    //	c121-B baseline + gol_metal_perf_test so Instruments groups all
    //	aaaseed perf markers together.
    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "ai.bsa.aaaseed", "performance" );
        return log;
    }

    //	Compute mean + p95 over a frame-time vector (microseconds in,
    //	milliseconds out). p95 = sorted[ floor(0.95*N) ] ; conservative
    //	for small N (e.g. N=20 -> index 19, the worst sample).
    struct PerfStats
    {
        double mean_ms;
        double p95_ms;
    };
    PerfStats compute_stats( std::vector< double > const& frame_us )
    {
        double sum = 0.0;
        for( double v : frame_us ) sum += v;
        double const mean_us = sum / double( frame_us.size() );
        std::vector< double > sorted = frame_us;
        std::sort( sorted.begin(), sorted.end() );
        std::size_t const p95_idx =
            std::min( sorted.size() - 1,
                      std::size_t( std::floor( 0.95 * double( sorted.size() ) ) ) );
        double const p95_us  = sorted[ p95_idx ];
        return PerfStats{ mean_us / 1000.0, p95_us / 1000.0 };
    }

    //	-------- Minimal matrix helpers (mirrored from the regression test;
    //	this file deliberately duplicates rather than #include the
    //	regression cpp's anonymous-namespace helpers).
    void mat4_mul_cm( float const* A, float const* B, float* out )
    {
        for( int c = 0; c < 4; ++c )
            for( int r = 0; r < 4; ++r )
            {
                float s = 0.0f;
                for( int k = 0; k < 4; ++k )
                    s += A[ k * 4 + r ] * B[ c * 4 + k ];
                out[ c * 4 + r ] = s;
            }
    }
    void make_view_lookat_rh( float ex, float ey, float ez,
                              float cx, float cy, float cz,
                              float ux, float uy, float uz,
                              float* out_cm )
    {
        float fx = cx - ex, fy = cy - ey, fz = cz - ez;
        float fl = std::sqrt( fx*fx + fy*fy + fz*fz );
        fx /= fl; fy /= fl; fz /= fl;
        float sx = fy*uz - fz*uy;
        float sy = fz*ux - fx*uz;
        float sz = fx*uy - fy*ux;
        float sl = std::sqrt( sx*sx + sy*sy + sz*sz );
        sx /= sl; sy /= sl; sz /= sl;
        float u2x = sy*fz - sz*fy;
        float u2y = sz*fx - sx*fz;
        float u2z = sx*fy - sy*fx;
        out_cm[ 0]= sx ; out_cm[ 1]= u2x; out_cm[ 2]=-fx ; out_cm[ 3]=0.0f;
        out_cm[ 4]= sy ; out_cm[ 5]= u2y; out_cm[ 6]=-fy ; out_cm[ 7]=0.0f;
        out_cm[ 8]= sz ; out_cm[ 9]= u2z; out_cm[10]=-fz ; out_cm[11]=0.0f;
        out_cm[12]= -( sx*ex + sy*ey + sz*ez );
        out_cm[13]= -( u2x*ex + u2y*ey + u2z*ez );
        out_cm[14]=  ( fx*ex + fy*ey + fz*ez );
        out_cm[15]= 1.0f;
    }
    void make_proj_perspective_rh( float fov_y_rad, float aspect,
                                   float near_c, float far_c, float* out_cm )
    {
        float const f = 1.0f / std::tan( fov_y_rad * 0.5f );
        for( int i = 0; i < 16; ++i ) out_cm[ i ] = 0.0f;
        out_cm[ 0]= f / aspect;
        out_cm[ 5]= f;
        out_cm[10]= far_c / ( near_c - far_c );
        out_cm[11]= -1.0f;
        out_cm[14]= ( near_c * far_c ) / ( near_c - far_c );
    }
    void make_view_inv_for_lookat_rh( float const* view_cm, float* out_cm )
    {
        out_cm[ 0]=view_cm[ 0]; out_cm[ 1]=view_cm[ 4]; out_cm[ 2]=view_cm[ 8]; out_cm[ 3]=0.0f;
        out_cm[ 4]=view_cm[ 1]; out_cm[ 5]=view_cm[ 5]; out_cm[ 6]=view_cm[ 9]; out_cm[ 7]=0.0f;
        out_cm[ 8]=view_cm[ 2]; out_cm[ 9]=view_cm[ 6]; out_cm[10]=view_cm[10]; out_cm[11]=0.0f;
        float tx = view_cm[12], ty = view_cm[13], tz = view_cm[14];
        out_cm[12] = -( out_cm[0]*tx + out_cm[4]*ty + out_cm[8]*tz );
        out_cm[13] = -( out_cm[1]*tx + out_cm[5]*ty + out_cm[9]*tz );
        out_cm[14] = -( out_cm[2]*tx + out_cm[6]*ty + out_cm[10]*tz );
        out_cm[15] = 1.0f;
    }

    //	Procedural gradient scene used as the input texture for the
    //	bloom + motion-blur perf tests (both shaders sample a scene tex).
    std::vector< std::uint8_t > make_gradient_rgba8( std::uint32_t W,
                                                    std::uint32_t H )
    {
        std::vector< std::uint8_t > p( std::size_t( W ) * H * 4, 0 );
        for( std::uint32_t y = 0; y < H; ++y )
            for( std::uint32_t x = 0; x < W; ++x )
            {
                std::size_t const i = ( y * W + x ) * 4;
                p[ i + 0 ] = std::uint8_t( ( x * 255u ) / ( W - 1 ) );
                p[ i + 1 ] = std::uint8_t( ( y * 255u ) / ( H - 1 ) );
                p[ i + 2 ] = std::uint8_t( ( ( x + y ) * 255u ) / ( W + H - 2 ) );
                p[ i + 3 ] = 255;
            }
        return p;
    }

    //	Print one line on PASS to expose observed mean / p95 in the
    //	ctest output. Visible without --verbose because gtest forwards
    //	std::cout to the test log.
    void report_perf( char const* name,
                      PerfStats const& s,
                      double budget_ms )
    {
        std::cout << "[perf] " << name
                  << " : mean=" << s.mean_ms << "ms"
                  << " p95="    << s.p95_ms  << "ms"
                  << " budget=" << budget_ms << "ms\n";
        std::cout.flush();
    }
}

//	==================== c135-A : aaa_noise_real perf ======================
TEST( RevivalPerf, AaaNoiseReal_UnderBudget )
{
    constexpr int    kIterations = 30;
    constexpr int    kWarmup     =  3;
    //	3x margin over c135-A's observed warm ~6ms. Debug-mode budget.
    constexpr double kBudgetMs   = 12.0;

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path const msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_noise_real.metal";
    std::string const msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main",
        GOL::TextureFormat::BGRA8 );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                GOL::TextureFormat::BGRA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    AaaFuFloats uf{};
    AaaFuVec4s  uv4{};
    AaaFuInts   ui{};
    //	mode = 1 : real 4-quadrant Perlin + Simplex + FBM noise.
    ui.values[ 0 ] = 1;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f,  GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i,  GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "perf.revival.aaa_noise_real";

    auto render_one_frame = [ & ]()
    {
        ASSERT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_buffer( ub_f,  0, 0 );
        backend.bind_fragment_buffer( ub_v4, 1, 0 );
        backend.bind_fragment_buffer( ub_i,  2, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();
    };

    for( int i = 0; i < kWarmup; ++i ) render_one_frame();

    std::vector< double > frame_us;
    frame_us.reserve( kIterations );
    for( int i = 0; i < kIterations; ++i )
    {
        os_signpost_id_t const sid = os_signpost_id_generate( perf_log() );
        os_signpost_interval_begin( perf_log(), sid,
                                    "perf.revival.aaa_noise_real.frame",
                                    "i=%d", i );
        auto const t0 = std::chrono::steady_clock::now();
        render_one_frame();
        auto const t1 = std::chrono::steady_clock::now();
        os_signpost_interval_end( perf_log(), sid,
                                  "perf.revival.aaa_noise_real.frame",
                                  "i=%d", i );
        frame_us.push_back(
            std::chrono::duration< double, std::micro >( t1 - t0 ).count() );
    }

    PerfStats const s = compute_stats( frame_us );
    report_perf( "RevivalPerf.AaaNoiseReal_UnderBudget", s, kBudgetMs );

    EXPECT_LT( s.mean_ms, kBudgetMs )
        << "aaa_noise_real mean frame time " << s.mean_ms
        << "ms exceeds budget " << kBudgetMs
        << "ms (1280x720 BGRA8 single-pass real 4-quadrant Perlin+Simplex+FBM).";

    backend.delete_buffer(  ub_i  );
    backend.delete_buffer(  ub_v4 );
    backend.delete_buffer(  ub_f  );
    backend.delete_texture( rt    );
    backend.delete_program( prog  );
}

//	==================== c138-A : aaa_bloom_real perf ======================
TEST( RevivalPerf, AaaBloomReal_UnderBudget )
{
    constexpr int    kIterations = 20;       //	heavier : 4-pass per frame
    constexpr int    kWarmup     =  3;
    //	5ms/pass average debug-mode budget for the 4-pass pipeline.
    constexpr double kBudgetMs   = 20.0;

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path const msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_bloom_real.metal";
    std::string const msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main",
        GOL::TextureFormat::BGRA8 );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId scene_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                       GOL::TextureFormat::RGBA8 );
    ASSERT_NE( scene_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > const scene = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( scene_tex, scene.data(), kBytesPerRow );

    GOL::TextureId rt_threshold = backend.gen_texture_2d( kWidth, kHeight,
                                                          GOL::TextureFormat::BGRA8 );
    GOL::TextureId rt_hblur     = backend.gen_texture_2d( kWidth, kHeight,
                                                          GOL::TextureFormat::BGRA8 );
    GOL::TextureId rt_vblur     = backend.gen_texture_2d( kWidth, kHeight,
                                                          GOL::TextureFormat::BGRA8 );
    GOL::TextureId rt_composite = backend.gen_texture_2d( kWidth, kHeight,
                                                          GOL::TextureFormat::BGRA8 );
    ASSERT_NE( rt_threshold, GOL::kInvalidTextureId );
    ASSERT_NE( rt_hblur,     GOL::kInvalidTextureId );
    ASSERT_NE( rt_vblur,     GOL::kInvalidTextureId );
    ASSERT_NE( rt_composite, GOL::kInvalidTextureId );

    AaaFuFloats uf{};
    AaaFuVec4s  uv4{};
    AaaFuInts   ui{};
    uf.values[ 0 ] = 0.6f;
    uf.values[ 1 ] = 0.4f;
    uf.values[ 2 ] = 1.0f / float( kWidth  );
    uf.values[ 3 ] = 1.0f / float( kHeight );
    ui.values[ 0 ] = 1;   // mode = 1 (real 4-pass bloom)

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    ASSERT_NE( ub_f,  GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );

    GOL::BufferId ub_i_p0 = backend.gen_buffer();
    GOL::BufferId ub_i_p1 = backend.gen_buffer();
    GOL::BufferId ub_i_p2 = backend.gen_buffer();
    GOL::BufferId ub_i_p3 = backend.gen_buffer();
    ASSERT_NE( ub_i_p0, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i_p1, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i_p2, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i_p3, GOL::kInvalidBufferId );
    AaaFuInts ui_p0 = ui; ui_p0.values[ 3 ] = 0;
    AaaFuInts ui_p1 = ui; ui_p1.values[ 3 ] = 1;
    AaaFuInts ui_p2 = ui; ui_p2.values[ 3 ] = 2;
    AaaFuInts ui_p3 = ui; ui_p3.values[ 3 ] = 3;
    backend.buffer_data( ub_i_p0, sizeof( ui_p0 ), &ui_p0, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i_p1, sizeof( ui_p1 ), &ui_p1, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i_p2, sizeof( ui_p2 ), &ui_p2, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i_p3, sizeof( ui_p3 ), &ui_p3, GOL::BufferUsage::Static );

    auto run_pass = [ & ]( GOL::BufferId  ub_i,
                           GOL::TextureId tex0,
                           GOL::TextureId tex1,
                           GOL::TextureId target,
                           char const*    debug_label )
    {
        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = target;
        rpd.load_action      = GOL::LoadAction::Clear;
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 0.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = debug_label;
        ASSERT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_texture( tex0, 0 );
        backend.bind_fragment_texture(
            tex1 != GOL::kInvalidTextureId ? tex1 : scene_tex, 1 );
        backend.bind_fragment_buffer( ub_f,  0, 0 );
        backend.bind_fragment_buffer( ub_v4, 1, 0 );
        backend.bind_fragment_buffer( ub_i,  2, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        //  c140-B follow-up : commit between passes. Each begin_render_pass()
        //  allocates a fresh MTLCommandBuffer (c140-A finding) ; without a
        //  present() per pass the perf loop accumulates 4 outstanding command
        //  buffers per iteration and saturates Metal's command queue across
        //  23 iterations (3 warmup + 20 measured), causing the next
        //  begin_render_pass to block indefinitely.
        backend.present();
    };

    auto render_one_frame = [ & ]()
    {
        run_pass( ub_i_p0, scene_tex,    GOL::kInvalidTextureId, rt_threshold,
                  "perf.revival.aaa_bloom_real.p0_threshold" );
        run_pass( ub_i_p1, rt_threshold, GOL::kInvalidTextureId, rt_hblur,
                  "perf.revival.aaa_bloom_real.p1_hblur"     );
        run_pass( ub_i_p2, rt_hblur,     GOL::kInvalidTextureId, rt_vblur,
                  "perf.revival.aaa_bloom_real.p2_vblur"     );
        run_pass( ub_i_p3, scene_tex,    rt_vblur,               rt_composite,
                  "perf.revival.aaa_bloom_real.p3_composite"  );
        //  Each pass already presents above — no tail present needed.
    };

    for( int i = 0; i < kWarmup; ++i ) render_one_frame();

    std::vector< double > frame_us;
    frame_us.reserve( kIterations );
    for( int i = 0; i < kIterations; ++i )
    {
        os_signpost_id_t const sid = os_signpost_id_generate( perf_log() );
        os_signpost_interval_begin( perf_log(), sid,
                                    "perf.revival.aaa_bloom_real.frame",
                                    "i=%d", i );
        auto const t0 = std::chrono::steady_clock::now();
        render_one_frame();
        auto const t1 = std::chrono::steady_clock::now();
        os_signpost_interval_end( perf_log(), sid,
                                  "perf.revival.aaa_bloom_real.frame",
                                  "i=%d", i );
        frame_us.push_back(
            std::chrono::duration< double, std::micro >( t1 - t0 ).count() );
    }

    PerfStats const s = compute_stats( frame_us );
    report_perf( "RevivalPerf.AaaBloomReal_UnderBudget", s, kBudgetMs );

    EXPECT_LT( s.mean_ms, kBudgetMs )
        << "aaa_bloom_real mean frame time " << s.mean_ms
        << "ms exceeds budget " << kBudgetMs
        << "ms (1280x720 BGRA8 four-pass : threshold + Hblur + Vblur + composite).";

    backend.delete_buffer(  ub_i_p3 );
    backend.delete_buffer(  ub_i_p2 );
    backend.delete_buffer(  ub_i_p1 );
    backend.delete_buffer(  ub_i_p0 );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt_composite );
    backend.delete_texture( rt_vblur     );
    backend.delete_texture( rt_hblur     );
    backend.delete_texture( rt_threshold );
    backend.delete_texture( scene_tex    );
    backend.delete_program( prog         );
}

//	==================== c139-A : aaa_motion_blur_real perf ================
TEST( RevivalPerf, AaaMotionBlurReal_UnderBudget )
{
    constexpr int    kIterations = 30;
    constexpr int    kWarmup     =  3;
    //	N=16 taps debug-mode budget for a 1280x720 single pass.
    constexpr double kBudgetMs   = 15.0;

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path const msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_motion_blur_real.metal";
    std::string const msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main",
        GOL::TextureFormat::BGRA8 );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId scene_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                       GOL::TextureFormat::RGBA8 );
    ASSERT_NE( scene_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > const scene = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( scene_tex, scene.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                GOL::TextureFormat::BGRA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    AaaFuFloats uf{};
    AaaFuVec4s  uv4{};
    AaaFuInts   ui{};
    uf.values[ 0 ] = 0.5f;      // blur_intensity
    uf.values[ 1 ] = 64.0f;     // max_velocity_pixels
    uf.values[ 2 ] = 0.5f;      // linear_depth
    ui.values[ 0 ] = 1;         // mode = 1 (real motion blur)
    ui.values[ 1 ] = 16;        // N = 16 taps along velocity

    //	Two cams : horizontal-pan delta produces a real velocity field.
    AaaCamReal_Cpu cam{};
    AaaCamReal_Cpu cam_prev{};
    {
        float const fov_y_rad = 60.0f * 3.14159265358979323846f / 180.0f;
        float const aspect    = float( kWidth ) / float( kHeight );
        float const near_c    = 0.1f;
        float const far_c     = 100.0f;
        make_view_lookat_rh( 0.0f, 0.0f, 5.0f,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, cam.view );
        make_proj_perspective_rh( fov_y_rad, aspect, near_c, far_c, cam.proj );
        make_view_inv_for_lookat_rh( cam.view, cam.view_inv );
        mat4_mul_cm( cam.proj, cam.view, cam.vp );
        cam.eye_aspect[0] = 0.0f;
        cam.eye_aspect[1] = 0.0f;
        cam.eye_aspect[2] = 5.0f;
        cam.eye_aspect[3] = aspect;
        cam.fov_clip  [0] = fov_y_rad;
        cam.fov_clip  [1] = near_c;
        cam.fov_clip  [2] = far_c;
        cam.fov_clip  [3] = 0.0f;

        make_view_lookat_rh( 1.0f, 0.0f, 5.0f,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, cam_prev.view );
        make_proj_perspective_rh( fov_y_rad, aspect, near_c, far_c, cam_prev.proj );
        make_view_inv_for_lookat_rh( cam_prev.view, cam_prev.view_inv );
        mat4_mul_cm( cam_prev.proj, cam_prev.view, cam_prev.vp );
        cam_prev.eye_aspect[0] = 1.0f;
        cam_prev.eye_aspect[1] = 0.0f;
        cam_prev.eye_aspect[2] = 5.0f;
        cam_prev.eye_aspect[3] = aspect;
        cam_prev.fov_clip  [0] = fov_y_rad;
        cam_prev.fov_clip  [1] = near_c;
        cam_prev.fov_clip  [2] = far_c;
        cam_prev.fov_clip  [3] = 0.0f;
    }

    GOL::BufferId ub_f       = backend.gen_buffer();
    GOL::BufferId ub_v4      = backend.gen_buffer();
    GOL::BufferId ub_i       = backend.gen_buffer();
    GOL::BufferId ub_cam     = backend.gen_buffer();
    GOL::BufferId ub_camprev = backend.gen_buffer();
    ASSERT_NE( ub_f,       GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4,      GOL::kInvalidBufferId );
    ASSERT_NE( ub_i,       GOL::kInvalidBufferId );
    ASSERT_NE( ub_cam,     GOL::kInvalidBufferId );
    ASSERT_NE( ub_camprev, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,       sizeof( uf       ), &uf,       GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4,      sizeof( uv4      ), &uv4,      GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,       sizeof( ui       ), &ui,       GOL::BufferUsage::Static );
    backend.buffer_data( ub_cam,     sizeof( cam      ), &cam,      GOL::BufferUsage::Static );
    backend.buffer_data( ub_camprev, sizeof( cam_prev ), &cam_prev, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "perf.revival.aaa_motion_blur_real";

    auto render_one_frame = [ & ]()
    {
        ASSERT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_texture( scene_tex, 0 );
        backend.bind_fragment_buffer( ub_f,       0, 0 );
        backend.bind_fragment_buffer( ub_v4,      1, 0 );
        backend.bind_fragment_buffer( ub_i,       2, 0 );
        backend.bind_fragment_buffer( ub_cam,     3, 0 );
        backend.bind_fragment_buffer( ub_camprev, 4, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();
    };

    for( int i = 0; i < kWarmup; ++i ) render_one_frame();

    std::vector< double > frame_us;
    frame_us.reserve( kIterations );
    for( int i = 0; i < kIterations; ++i )
    {
        os_signpost_id_t const sid = os_signpost_id_generate( perf_log() );
        os_signpost_interval_begin( perf_log(), sid,
                                    "perf.revival.aaa_motion_blur_real.frame",
                                    "i=%d", i );
        auto const t0 = std::chrono::steady_clock::now();
        render_one_frame();
        auto const t1 = std::chrono::steady_clock::now();
        os_signpost_interval_end( perf_log(), sid,
                                  "perf.revival.aaa_motion_blur_real.frame",
                                  "i=%d", i );
        frame_us.push_back(
            std::chrono::duration< double, std::micro >( t1 - t0 ).count() );
    }

    PerfStats const s = compute_stats( frame_us );
    report_perf( "RevivalPerf.AaaMotionBlurReal_UnderBudget", s, kBudgetMs );

    EXPECT_LT( s.mean_ms, kBudgetMs )
        << "aaa_motion_blur_real mean frame time " << s.mean_ms
        << "ms exceeds budget " << kBudgetMs
        << "ms (1280x720 BGRA8 single-pass N=16-tap velocity-projected blur).";

    backend.delete_buffer(  ub_camprev );
    backend.delete_buffer(  ub_cam     );
    backend.delete_buffer(  ub_i       );
    backend.delete_buffer(  ub_v4      );
    backend.delete_buffer(  ub_f       );
    backend.delete_texture( rt         );
    backend.delete_texture( scene_tex  );
    backend.delete_program( prog       );
}
