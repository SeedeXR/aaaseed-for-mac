// src/code_utils/memcpy_mac.cpp
//
// Mac (Apple Silicon) port of vendor/aaaseed-engine/Src/code_utils/memcpy.cpp.
//
// The Windows source (1548 LOC) implements a runtime-dispatched memcpy stack
// (movsb / SSE4.1 streaming / AVX2 streaming / aligned / parallel) gated on
// 14 mem:: data flags resolved by c_cpu capability detection.
//
// On Apple Silicon (arm64) all four SIMD branches are x86-only intrinsics
// and can never fire ; libc's memcpy / memmove is itself NEON-tuned by
// Apple (`_platform_memmove`) and beats any naive arm64 port we could
// write. Per the project's "never stub when a real native port exists"
// policy AND the c115-C audit, the Mac port :
//
//   1. Forwards mem::memmove / mem::memcpy / mem::memcpy_monothread
//      to libc straight through (single static call, no dispatch).
//   2. Provides real out-of-line defs for the 14 mem:: dispatch data
//      symbols enumerated by c115-C, with arm64-appropriate defaults
//      (the SIMD flags are false ; the engine "asked" flags stay true
//      so a future x86_64 cross-build could re-enable them ; thread_nb
//      reflects the actual core count ; threshold_kb stays at 1024).
//   3. Stubs mem::memcpy_parallel by forwarding to memcpy_monothread.
//      The real parallel impl is gated on c117 compute_parallel port.
//      Since b_cpy_parallel defaults to false on Mac, the path is
//      unreachable from mem::memcpy anyway -- but the strong symbol
//      is still needed to resolve the linker reference.
//
// This file SUPERSEDES the c109 weak stubs in
// tests/unit/aaalua_mem_dispatch_stub.cpp at link time (the stubs were
// `__attribute__((weak))` exactly for this transition).
//
// Wired via src/code_utils/CMakeLists.txt ; the vendor memcpy.cpp is NOT
// added to aaaseed_code_utils so there's no duplicate-symbol clash.

#include "aaa_mem.h"

#include <cstring>      // std::memmove, std::memcpy
#include <thread>       // std::thread::hardware_concurrency

//  ------------------------------------------------------------------
//  Dispatch data symbols (14 audited by c115-C).
//
//  Mirrors the original definitions in vendor/.../memcpy.cpp lines
//  1398-1410. Defaults adjusted for Apple Silicon :
//      * The three b_cpy_use_* SIMD flags stay false (x86 intrinsics
//        never fire on arm64).
//      * The three b_cpy_use_*_asked flags stay true (engine default ;
//        future x86_64 cross-build keeps the same engine semantics).
//      * b_memset_use_avx false / asked true (same rationale).
//      * b_cpy_use_cache_L2_size_ui / L3 stay false (user-UI overrides
//        of the cache sizes ; c_cpu detection on Mac will fill the
//        cache sizes itself when c117 lands).
//      * cpy_use_cache_L2_size / L3 stay 0 (c_cpu will set them).
//      * b_cpy_parallel false on Mac : the parallel impl is stubbed
//        until c117's compute_parallel port lands.
//      * cpy_parallel_thread_nb : hardware_concurrency() at link time
//        (resolved during static-init). Falls back to 1 if the runtime
//        returns 0 (rare, but POSIX permits it).
//      * cpy_parallel_threshold_kb : 1024 (vendor default).
//  ------------------------------------------------------------------

namespace
{
    UINT32 detect_hardware_concurrency_() noexcept
    {
        unsigned int const hc = std::thread::hardware_concurrency();
        return hc == 0 ? UINT32(1) : UINT32(hc);
    }
}

bool    mem::b_cpy_use_movsb            = false;
bool    mem::b_cpy_use_avx2             = false;
bool    mem::b_cpy_use_sse41            = false;
bool    mem::b_cpy_use_movsb_asked      = true;
bool    mem::b_cpy_use_avx2_asked       = true;
bool    mem::b_cpy_use_sse41_asked      = true;
bool    mem::b_memset_use_avx           = false;
bool    mem::b_memset_use_avx_asked     = true;
bool    mem::b_cpy_use_cache_L2_size_ui = false;
bool    mem::b_cpy_use_cache_L3_size_ui = false;
size_t  mem::cpy_use_cache_L2_size      = 0;
size_t  mem::cpy_use_cache_L3_size      = 0;
bool    mem::b_cpy_parallel             = false;
UINT32  mem::cpy_parallel_thread_nb     = detect_hardware_concurrency_();
UINT32  mem::cpy_parallel_threshold_kb  = 1024;


//  ------------------------------------------------------------------
//  Forwarders to libc.
//
//  Apple's `_platform_memmove` (the implementation behind both
//  std::memmove AND std::memcpy on Apple Silicon) is hand-tuned NEON
//  + cache-prefetch + correct overlap handling. Forwarding through it
//  is the fastest correct primitive available.
//
//  mem::memcpy ignores `signature` on Mac : the SPY range macros it
//  drove in the Windows path are no-ops here (spy.cpp not in the
//  Mac code_utils lib yet).
//  ------------------------------------------------------------------

void    mem::memmove( void* CONST dst, void CONST * CONST src, size_t byte_nb ) NOEXCEPT
{
    if( byte_nb == 0 )
        return;
    std::memmove( dst, src, byte_nb );
}

void    mem::memcpy_monothread( void* CONST dst, void CONST * CONST src, size_t nb ) NOEXCEPT
{
    if( nb == 0 )
        return;
    std::memcpy( dst, src, nb );
}

void    mem::memcpy( void* CONST dst, void CONST * CONST src, size_t byte_nb, C_PCHAR_C signature ) NOEXCEPT
{
    if( !dst || !src )
    {
        //  Vendor path calls debug_break() ; err.cpp isn't in the Mac
        //  code_utils lib yet (c115-D deferral). Silent early-out
        //  matches the safer-than-crash branch.
        return;
    }
    if( byte_nb == 0 )
        return;
    //  c117 : dispatch to parallel impl when caller has opted in via
    //  b_cpy_parallel AND buffer exceeds threshold. On Apple Silicon
    //  libc std::memcpy is single-core bandwidth-bound for most realistic
    //  sizes ; parallel helps only for multi-MB transfers. Default Mac
    //  init keeps b_cpy_parallel=false so the fast path is unchanged.
    if( b_cpy_parallel && byte_nb >= size_t( cpy_parallel_threshold_kb ) * 1024u )
    {
        memcpy_parallel( dst, src, byte_nb, INT32( cpy_parallel_thread_nb ), signature );
        return;
    }
    std::memcpy( dst, src, byte_nb );
}

//  Parallel memcpy : c117 wired to compute_parallel_mac.h's parallel_for
//  (GCD dispatch_apply backed). Splits the copy across `thread_nb` workers,
//  each handling a contiguous chunk. On Apple Silicon libc memcpy is
//  already memory-bandwidth-bound on a single core for most realistic
//  buffer sizes ; parallelism mainly helps for very large (multi-MB)
//  contiguous transfers where the working set spills L3.
//
//  Fallback to memcpy_monothread when :
//    - thread_nb <= 1 (no work to split).
//    - byte_nb < threshold (overhead > parallelism win).
//    - dst or src is null (defensive ; libc UB).
#include "infrastructure/compute_parallel_mac.h"

void    mem::memcpy_parallel( void* CONST dst, void CONST * CONST src, size_t nb, INT32 CONST thread_nb, C_PCHAR_C /*signature*/ ) NOEXCEPT
{
    if( !dst || !src || nb == 0 )
        return;
    INT32 const n_workers = ( thread_nb > 0 ) ? thread_nb
                           : INT32( cpy_parallel_thread_nb );
    if( n_workers <= 1 )
    {
        memcpy_monothread( dst, src, nb );
        return;
    }
    size_t const chunk = nb / size_t( n_workers );
    if( chunk == 0 )
    {
        memcpy_monothread( dst, src, nb );
        return;
    }
    char*       dst_c = static_cast< char* >( dst );
    char const* src_c = static_cast< char const* >( src );
    aaa::parallel::parallel_for( 0, n_workers, [&]( INT32 i ) {
        size_t const off  = size_t( i ) * chunk;
        size_t const this_nb = ( i + 1 == n_workers ) ? ( nb - off ) : chunk;
        std::memcpy( dst_c + off, src_c + off, this_nb );
    } );
}
