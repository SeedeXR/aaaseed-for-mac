// src/infrastructure/compute_parallel_mac.cpp
//
// Mac-native implementation of the compute_parallel_mac.h shim.
//
// Backed by Grand Central Dispatch (GCD), the Apple-native parallel
// primitive that ships in libdispatch on every macOS install (since
// 10.6). `dispatch_apply` is the literal moral equivalent of TBB's
// tbb::parallel_for and Microsoft PPL's concurrency::parallel_for : a
// synchronous parallel loop driven by the system's worker thread pool.
//
// Why GCD instead of std::thread :
//   - Zero allocation per call (libdispatch reuses its global pool).
//   - Auto-sized worker pool ; the kernel scales it to active core count
//     and updates dynamically as the system load changes.
//   - First-class macOS support : every macOS profile / Instruments tool
//     understands libdispatch traces natively.
//   - std::thread would spin up + tear down OS threads on every call,
//     defeating the purpose for the hot `mem::memcpy_parallel` consumer.
//
// c116-B (2026-05-26).

#include "compute_parallel_mac.h"

#include <atomic>
#include <thread>

#include <dispatch/dispatch.h>

namespace aaa
{
    namespace parallel
    {
        namespace
        {
            // Cached worker count, lazily filled on first get_thread_nb_def().
            // Atomic so concurrent first-callers see a consistent value
            // without a mutex. `0` is the "not yet computed" sentinel ;
            // hardware_concurrency() never returns 0 on macOS arm64 (Apple
            // Silicon ships with at least 8 cores), but we still guard
            // against the std-spec'd zero-return for portability.
            std::atomic<UINT32> g_thread_nb_def_cached{ 0 };
        }

        UINT32  get_thread_nb_def()
        {
            UINT32 cached = g_thread_nb_def_cached.load( std::memory_order_acquire );
            if( cached != 0 )
                return cached;

            UINT32 const hw = static_cast<UINT32>( std::thread::hardware_concurrency() );
            UINT32 const v  = (hw == 0) ? 8u : hw;   // 8 mirrors the vendor's default fallback
            g_thread_nb_def_cached.store( v, std::memory_order_release );
            return v;
        }

        void    parallel_for( INT32 start, INT32 end,
                              std::function<void(INT32)> fn )
        {
            // Empty / inverted range : no-op, matches tbb / concurrency semantics.
            if( end <= start )
                return;

            size_t const iterations = static_cast<size_t>( end - start );

            // QOS_CLASS_DEFAULT global queue : libdispatch's general-purpose
            // pool. dispatch_apply blocks the calling thread until every i
            // has been dispatched and completed, mirroring the synchronous
            // barrier of tbb::parallel_for / concurrency::parallel_for.
            dispatch_queue_t queue =
                dispatch_get_global_queue( QOS_CLASS_DEFAULT, 0 );

            dispatch_apply( iterations, queue, ^( size_t i )
            {
                fn( start + static_cast<INT32>( i ) );
            } );
        }
    }
}
