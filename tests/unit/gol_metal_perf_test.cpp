// gol_metal_perf_test.cpp
//
// Performance-profiling test for the GOL Metal backend. Demonstrates the
// three-artifact doctrine from philosophy.md Part 2.95 :
//   1. CPU-side `os_signpost` interval around the render loop (visible in
//      Instruments / Time Profiler under subsystem ai.bsa.aaaseed).
//   2. Wall-clock measurement with std::chrono::steady_clock.
//   3. Budget assertion : average frame time must stay within budget for
//      a trivial full-screen-red render. Budget chosen generously for the
//      debug build on Apple M-series ; release builds will be much tighter.
//
// This is the SCAFFOLD for the perf doctrine. Later perf tests follow
// the same shape (signpost + measure + assert), with subsystem-specific
// budgets. When MetalBackend grows pushDebugGroup support (Phase 3 todo),
// the GPU-side debug-group call lands inside `begin_render_pass` itself
// and this test gets a labelled GPU-Frame-Capture entry for free.
//
// CTest label : `perf`.

#include <gtest/gtest.h>

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <chrono>
#include <cstdint>
#include <os/log.h>
#include <os/signpost.h>

namespace
{
    //	Same trivial MSL shader as gol_metal_render_test.cpp's full-screen
    //	red ; we don't care about visual output here, only frame-time.
    constexpr char const* kFullScreenRedMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[3];
    verts[0] = float2( -1.0, -3.0 );
    verts[1] = float2( -1.0,  1.0 );
    verts[2] = float2(  3.0,  1.0 );
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    return out;
}

fragment float4 fs_main()
{
    return float4( 1.0, 0.0, 0.0, 1.0 );
}
)MSL";

    //	Per-process signpost log handle. Matches the subsystem already
    //	used by the NSightEvents.h Mac branch so Instruments groups CPU
    //	markers from tests next to markers from a live AAASeed run.
    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "ai.bsa.aaaseed", "performance" );
        return log;
    }
}

TEST( GolMetalPerf, FullScreenRedFrameStaysUnderBudget )
{
    //	Budget : 16 ms / frame in debug build on Apple M-series. This is
    //	the 60 Hz frame budget. Trivial full-screen-red should be well
    //	under this ; we set the budget generously to catch only real
    //	regressions, not measurement noise.
    constexpr int      kIterations         = 200;
    constexpr double   kBudgetMsPerFrame   = 16.0;
    //	Warmup : the first frame pays MTL::Library compile + pipeline-state
    //	creation cost, which is one-shot. Exclude N warmup frames from the
    //	average so we measure steady-state, not init.
    constexpr int      kWarmupFrames       = 10;

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 256;
    constexpr std::uint32_t H = 256;
    GOL::TextureId  target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( target, GOL::kInvalidTextureId );

    GOL::ProgramId prog = backend.create_program_msl( kFullScreenRedMsl,
                                                     "vs_main",
                                                     "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "shader compile failed : " << backend.get_last_error();

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    //	GPU debug-marker integration : the encoder pushes a group named
    //	"perf.full_screen_red" so the band appears labelled in Xcode's
    //	GPU Frame Capture / Instruments Metal System Trace. CPU side is
    //	already covered by the os_signpost intervals above.
    rpd.debug_label = "perf.full_screen_red";

    //	Warmup pass : not measured, not signposted.
    for( int i = 0; i < kWarmupFrames; ++i )
    {
        ASSERT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
        backend.bind_program( prog );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();
    }

    //	Measured pass : wrap in a single os_signpost interval named
    //	"perf.full_screen_red.loop". In Instruments this shows as one
    //	band covering all N frames.
    os_signpost_id_t spid = os_signpost_id_generate( perf_log() );
    os_signpost_interval_begin( perf_log(), spid, "perf.loop",
                                "iterations=%d", kIterations );

    auto const t0 = std::chrono::steady_clock::now();

    for( int i = 0; i < kIterations; ++i )
    {
        //	Per-frame signpost too — lets us see frame-time variance
        //	in Instruments without re-running the test.
        os_signpost_id_t fid = os_signpost_id_generate( perf_log() );
        os_signpost_interval_begin( perf_log(), fid, "perf.frame", "i=%d", i );

        ASSERT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
        backend.bind_program( prog );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        os_signpost_interval_end( perf_log(), fid, "perf.frame", "i=%d", i );
    }

    auto const t1 = std::chrono::steady_clock::now();
    os_signpost_interval_end( perf_log(), spid, "perf.loop",
                              "iterations=%d", kIterations );

    auto const total_ms  = std::chrono::duration< double, std::milli >( t1 - t0 ).count();
    double const avg_ms  = total_ms / kIterations;

    std::printf( "[GolMetalPerf] %d frames in %.3f ms (avg %.4f ms/frame ; budget %.1f ms)\n",
                 kIterations, total_ms, avg_ms, kBudgetMsPerFrame );

    EXPECT_LT( avg_ms, kBudgetMsPerFrame )
        << "Average frame time " << avg_ms
        << " ms exceeds budget " << kBudgetMsPerFrame << " ms.";

    backend.delete_program( prog );
    backend.delete_texture( target );
}
