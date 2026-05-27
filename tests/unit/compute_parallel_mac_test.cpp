// tests/unit/compute_parallel_mac_test.cpp
//
// c116-B (2026-05-26) -- unit test for the Mac-native compute_parallel
// shim at src/infrastructure/compute_parallel_mac.{h,cpp}. The vendor
// source vendor/aaaseed-engine/Src/infrastructure/compute_parallel.h is
// Win-only (TBB / PPL) ; the Mac sub-lib provides the matching surface
// via dispatch_apply (Grand Central Dispatch).
//
// What we check :
//   1. get_thread_nb_def() returns >= 1 and is stable across calls
//      (cached lazy init).
//   2. parallel_for( 0, N, fn ) visits every index in [0, N) exactly
//      once (atomic counter + per-index visit bitmap).
//   3. parallel_for( start, end, fn ) with start != 0 hits the correct
//      index range, proving the start-offset arithmetic.
//   4. parallel_for( 0, 0, fn ) and parallel_for( 5, 5, fn ) are no-ops
//      (empty range early-out).
//   5. parallel_for( 10, 5, fn ) (inverted range) is a no-op.
//   6. parallel_for actually parallelizes : with a reasonable iteration
//      count we observe a non-trivial number of distinct std::thread::id
//      values during execution.

#include <gtest/gtest.h>

#include "compute_parallel_mac.h"

#include <atomic>
#include <set>
#include <thread>
#include <mutex>
#include <vector>

TEST( ComputeParallelMac, ThreadNbDefIsPositiveAndStable )
{
    UINT32 const a = aaa::parallel::get_thread_nb_def();
    UINT32 const b = aaa::parallel::get_thread_nb_def();
    EXPECT_GE( a, 1u );
    EXPECT_EQ( a, b );  // cached -> identical
}

TEST( ComputeParallelMac, ParallelForVisitsEveryIndexFromZero )
{
    INT32 const N = 100;
    std::vector<std::atomic<INT32>> visited( N );
    for( auto& v : visited )
        v.store( 0 );

    std::atomic<INT32> count{ 0 };

    aaa::parallel::parallel_for( 0, N, [&]( INT32 i ) {
        visited[i].fetch_add( 1, std::memory_order_relaxed );
        count.fetch_add( 1, std::memory_order_relaxed );
    } );

    EXPECT_EQ( count.load(), N );
    for( INT32 i = 0; i < N; ++i )
        EXPECT_EQ( visited[i].load(), 1 ) << "index " << i;
}

TEST( ComputeParallelMac, ParallelForStartOffsetWorks )
{
    INT32 const START = 42;
    INT32 const END   = 73;
    std::mutex mtx;
    std::set<INT32> seen;

    aaa::parallel::parallel_for( START, END, [&]( INT32 i ) {
        std::lock_guard<std::mutex> g( mtx );
        seen.insert( i );
    } );

    EXPECT_EQ( seen.size(), static_cast<size_t>( END - START ) );
    EXPECT_EQ( *seen.begin(), START );
    EXPECT_EQ( *seen.rbegin(), END - 1 );
}

TEST( ComputeParallelMac, EmptyAndInvertedRangeIsNoOp )
{
    std::atomic<INT32> count{ 0 };
    auto inc = [&]( INT32 ) { count.fetch_add( 1 ); };

    aaa::parallel::parallel_for( 0, 0, inc );
    aaa::parallel::parallel_for( 5, 5, inc );
    aaa::parallel::parallel_for( 10, 5, inc );   // inverted

    EXPECT_EQ( count.load(), 0 );
}

TEST( ComputeParallelMac, ParallelForActuallyParallelizes )
{
    // Heuristic check : with enough iterations and a tiny sleep per
    // iteration, GCD should fan out across multiple threads. On Apple
    // Silicon (>=8 cores) we expect at least 2 distinct thread ids ;
    // single-thread fallback would yield exactly 1.
    INT32 const N = 256;
    std::mutex mtx;
    std::set<std::thread::id> tids;

    aaa::parallel::parallel_for( 0, N, [&]( INT32 ) {
        {
            std::lock_guard<std::mutex> g( mtx );
            tids.insert( std::this_thread::get_id() );
        }
        // Tiny pause to give GCD a chance to spread work across workers.
        std::this_thread::yield();
    } );

    EXPECT_GE( tids.size(), 2u )
        << "parallel_for appeared to serialise on one thread "
        << "(saw " << tids.size() << " distinct ids)";
}
