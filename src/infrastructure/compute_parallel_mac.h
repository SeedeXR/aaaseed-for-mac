// src/infrastructure/compute_parallel_mac.h
//
// Mac (Apple Silicon) shim for the vendor's Win-only header at
// vendor/aaaseed-engine/Src/infrastructure/compute_parallel.h.
//
// The vendor header unconditionally pulls Intel TBB or Microsoft PPL
// (#include <Tbb/parallel_for.h> or #include <ppl.h>) and uses
// `PARALLEL_LIB::parallel_for(...)` where PARALLEL_LIB is a typedef-alias
// for `tbb` or `concurrency`. Neither library exists on macOS.
//
// c116-B (2026-05-26) -- per c115-C audit. The only known consumer on Mac
// at this stage is `mem::memcpy_parallel` (still un-ported, lives in
// c116-A's scope at src/code_utils/memcpy_mac.cpp). This shim provides
// the Mac-native equivalent surface via Grand Central Dispatch (GCD),
// the Apple-native primitive that ships with every macOS install. No
// third-party dependency is needed.
//
// Public surface (matches c115-C audit) :
//
//   UINT32  aaa::parallel::get_thread_nb_def();
//       Returns the recommended worker count (delegated to
//       std::thread::hardware_concurrency() at first call; cached).
//
//   void    aaa::parallel::parallel_for( INT32 start, INT32 end,
//                                        std::function<void(INT32)> fn );
//       Invokes fn(i) for i in [start, end). Internally dispatches via
//       dispatch_apply on the global concurrent queue at
//       QOS_CLASS_DEFAULT, then converts back to INT32 indices.
//
//   PARALLEL_LIB                — namespace alias resolving to
//                                 `aaa::parallel`, so existing call
//                                 sites of `PARALLEL_LIB::parallel_for`
//                                 keep working unchanged.
//
// Hermetic doctrine : this lib has NO dependency on aaa_str.h / o_str /
// aaa_mem.h / c_cpu / spy. It uses ONLY std::thread + std::function +
// dispatch_apply. That deliberately sidesteps the blocked memcpy /
// aaa_mem / c_cpu cascade that c116-A owns. Consumers that want the
// full vendor surface (b_use_ui flag, lib_name o_str, SPY_PUSH_RANGE
// instrumentation, the templated aaa::parallel::call() wrapper) need a
// separate header once the cascade is unblocked ; this shim is the
// minimal subset that `mem::memcpy_parallel` needs.

#ifdef AAA_COMPUTE_PARALLEL_MAC_H
#error "COMPUTE_PARALLEL_MAC_H included more than once."
#endif
#define AAA_COMPUTE_PARALLEL_MAC_H 1

// c117 : guard with ifndef so consumers (e.g. src/code_utils/memcpy_mac.cpp)
// that already pull aaa_type.h via aaa_mem.h don't trigger its
// single-include `#error "AAA_TYPE_H included more than once."` guard.
#ifndef AAA_AAA_TYPE_H
#   include "aaa_type.h"   // INT32 / UINT32
#endif

#include <functional>

namespace aaa
{
    namespace parallel
    {
        // Returns the recommended worker thread count.
        // First call queries std::thread::hardware_concurrency() and caches.
        // Subsequent calls are O(1) atomic load.
        UINT32  get_thread_nb_def();

        // Mac-native parallel_for. Invokes fn(i) for every i in [start, end).
        // Backed by GCD's dispatch_apply on the global default-QoS queue,
        // which uses the system's libdispatch thread pool (auto-sized to
        // the number of active cores). Behaves as a synchronous barrier :
        // returns only after every invocation of fn has completed.
        void    parallel_for( INT32 start, INT32 end,
                              std::function<void(INT32)> fn );
    }
}

// Namespace alias so `PARALLEL_LIB::parallel_for(s,e, lambda)` call sites
// from the vendor tree resolve to aaa::parallel::parallel_for on Mac.
// The vendor header uses `#define PARALLEL_LIB tbb` (or concurrency), but
// a namespace alias is the cleaner Mac substitute since both `tbb` and
// `concurrency` are top-level namespaces in their respective libraries.
namespace PARALLEL_LIB = aaa::parallel;
