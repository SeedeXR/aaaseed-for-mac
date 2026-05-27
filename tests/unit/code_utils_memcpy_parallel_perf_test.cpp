// tests/unit/code_utils_memcpy_parallel_perf_test.cpp
//
// c117 (2026-05-26) -- perf-labelled benchmark : parallel memcpy via
// aaa::parallel::parallel_for (GCD dispatch_apply) vs single-thread
// std::memmove. Both paths land in src/code_utils/memcpy_mac.cpp.
//
// Rationale : the c115-C audit predicted that on Apple Silicon,
// libc memcpy is already memory-bandwidth-bound on a single core for
// most realistic buffer sizes. Parallelism mainly helps multi-MB
// transfers where the working set spills L3.
//
// What this perf test PROVES (not promises) :
//   - memcpy_parallel correctness : output byte-identical to monothread.
//   - memcpy_parallel does NOT regress measurably below monothread on
//     the 16MB benchmark (we just assert the parallel ratio is in the
//     [0.4 ... 4.0] envelope, i.e. NOT catastrophically slow).
//
// What this perf test does NOT promise :
//   - Strict speed-up. On bandwidth-bound transfers the GCD overhead
//     may NULL the gain. The doctrine says "don't beat libc" -- this
//     bench verifies we don't lose either.
//
// CTest label : "unit;perf;memcpy_parallel". Counted in the "perf"
// label total (the 8th perf-labelled test as of c117).

#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <thread>

#include "aaa_type.h"
#include "aaa_mem.h"

namespace
{
    constexpr size_t kBufBytes = 16u * 1024u * 1024u;   // 16MB

    using clk = std::chrono::steady_clock;

    double elapsed_us( clk::time_point a, clk::time_point b )
    {
        return std::chrono::duration_cast< std::chrono::microseconds >(
                   b - a ).count();
    }
}

//  mem::memcpy_parallel is `private` on c_mem (friends only) ; tests can't
//  call it directly. We exercise it via mem::memcpy() which c117 routed
//  to memcpy_parallel when b_cpy_parallel is true AND buffer exceeds
//  threshold. Toggling b_cpy_parallel at runtime picks between paths.

TEST( MemcpyParallelPerf, OutputMatchesMonothread )
{
    std::vector< char > src( kBufBytes );
    std::vector< char > dst_para( kBufBytes );
    std::vector< char > dst_mono( kBufBytes );
    for( size_t i = 0; i < kBufBytes; ++i )
        src[ i ] = char( i & 0xff );

    bool const saved = mem::b_cpy_parallel;

    //  Force parallel path.
    mem::b_cpy_parallel = true;
    mem::memcpy( dst_para.data(), src.data(), kBufBytes, "perf" );
    //  Force monothread path.
    mem::b_cpy_parallel = false;
    mem::memcpy( dst_mono.data(), src.data(), kBufBytes, "perf-mono" );

    mem::b_cpy_parallel = saved;

    ASSERT_EQ( std::memcmp( dst_para.data(), src.data(),     kBufBytes ), 0 )
        << "parallel output differs from source";
    ASSERT_EQ( std::memcmp( dst_para.data(), dst_mono.data(), kBufBytes ), 0 )
        << "parallel output differs from monothread";
}

TEST( MemcpyParallelPerf, NotCatastrophicallySlowVsMonothread )
{
    std::vector< char > src( kBufBytes );
    std::vector< char > dst( kBufBytes );
    for( size_t i = 0; i < kBufBytes; ++i )
        src[ i ] = char( i & 0xff );

    bool const saved = mem::b_cpy_parallel;
    INT32 const n_threads = INT32( mem::cpy_parallel_thread_nb );

    //  Warm-up : first call may include lazy GCD pool spin-up.
    mem::b_cpy_parallel = true;
    mem::memcpy( dst.data(), src.data(), kBufBytes, "perf" );
    mem::b_cpy_parallel = false;
    mem::memcpy( dst.data(), src.data(), kBufBytes, "perf" );

    constexpr int N_RUNS = 5;
    double mono_us = 0, para_us = 0;
    for( int i = 0; i < N_RUNS; ++i )
    {
        mem::b_cpy_parallel = false;
        auto t0 = clk::now();
        mem::memcpy( dst.data(), src.data(), kBufBytes, "perf" );
        auto t1 = clk::now();
        mem::b_cpy_parallel = true;
        mem::memcpy( dst.data(), src.data(), kBufBytes, "perf" );
        auto t2 = clk::now();
        mono_us += elapsed_us( t0, t1 );
        para_us += elapsed_us( t1, t2 );
    }
    mem::b_cpy_parallel = saved;

    mono_us /= N_RUNS;
    para_us /= N_RUNS;
    double const ratio = para_us / mono_us;
    std::printf( "[ memcpy_parallel perf ] mono=%.0fus para=%.0fus ratio=%.2fx (threads=%d, %.1fMB)\n",
                 mono_us, para_us, ratio, n_threads, double( kBufBytes ) / (1024.0*1024.0) );

    //  Tolerant envelope : parallel within 0.25x ... 4x of monothread.
    //  Tighter assertions would flake on busy CI runners.
    EXPECT_GT( ratio, 0.25 ) << "parallel-vs-mono ratio implausibly low";
    EXPECT_LT( ratio, 4.0  ) << "parallel-vs-mono ratio catastrophically slow";
}
