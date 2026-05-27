// tests/unit/aaalua_mem_dispatch_stub.cpp
//
// c109 (2026-05-26) — weak-symbol stubs for the 12 mem:: link symbols
// referenced by the aaalua trio (and ultimately by aaa_mem.cpp -- which
// is in aaaseed_code_utils -- which references but does NOT define the
// dispatch flags).
//
// Why : the 11 dispatch flags + mem::memmove are defined in
// vendor/aaaseed-engine/Src/code_utils/memcpy.cpp (lines 629/1398-1407
// for flags ; line 14 for memmove). memcpy.cpp is unported on Mac
// (Task #98, 1548 LOC, blocks o_str stub retirement).
//
// Scope : this TU is wired ONLY into the `aaaseed_aaalua_trio_link_audit_tests`
// target. It is NOT linked into aaaseed_code_utils itself, so production
// builds remain unaffected. Weak attributes are belt-and-suspenders : if
// memcpy.cpp ever lands in aaaseed_code_utils, its strong defs win and
// these stubs become inactive automatically.
//
// Behavior : all flags default to false / 0 -- meaning the dispatch
// system selects the libc memmove fallback path everywhere (correct for
// tests and for any code path that respects the flags). mem::memmove
// forwards to std::memmove.
//
// Reference : c107 link-cascade audit identified this bucket as the
// 2nd-highest leverage (12 syms, 13% of cascade) after c_obj_ui (c108).

#include <cstring>

namespace mem
{
    //  Dispatch flags (defined in vendor/.../memcpy.cpp lines 629/1398-1407).
    //  Mirror the engine defaults exactly so any code path that triggers
    //  flag-aware dispatch picks the safe libc fallback.
    __attribute__((weak)) bool   b_cpy_use_movsb           = false;
    __attribute__((weak)) bool   b_cpy_use_avx2            = false;
    __attribute__((weak)) bool   b_cpy_use_sse41           = false;
    __attribute__((weak)) bool   b_cpy_use_movsb_asked     = true;
    __attribute__((weak)) bool   b_cpy_use_sse41_asked     = true;
    __attribute__((weak)) bool   b_memset_use_avx          = false;
    __attribute__((weak)) bool   b_memset_use_avx_asked    = true;
    __attribute__((weak)) bool   b_cpy_use_cache_L2_size_ui = false;
    __attribute__((weak)) bool   b_cpy_use_cache_L3_size_ui = false;
    __attribute__((weak)) size_t cpy_use_cache_L2_size     = 0;
    __attribute__((weak)) size_t cpy_use_cache_L3_size     = 0;

    //  mem::memmove (defined in vendor/.../memcpy.cpp line 14).
    //  Real impl picks among SSE / AVX2 / SSE4.1 / movsb / libc based on
    //  the dispatch flags above. The fallback IS libc memmove, so on Mac
    //  we just forward unconditionally -- correctness preserved, perf is
    //  whatever libc memmove delivers (which on Apple Silicon is heavily
    //  optimized via the standard C library's SIMD-aware implementation).
    __attribute__((weak)) void memmove( void* dst, void const* src, size_t nb ) noexcept
    {
        std::memmove( dst, src, nb );
    }
}
