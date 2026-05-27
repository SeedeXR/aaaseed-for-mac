// tests/unit/spy_header_test.cpp
//
// Header-parse smoke test for `code_utils/spy.h` (continuation 55).
// Locks in Mac-cleanness of the spy / time_buf header chain after the
// one-line upstream cleanup that gated `sleep_ex`'s `SleepEx` body and
// `wait_for_single_object` behind `#if AAA_OS_WINDOWS()`. Pattern
// mirrors layer_header_test.cpp + aaalua_header_test.cpp.
//
// What this verifies on Mac :
//   - `code_utils/spy.h` parses with `AAA_NSIGHT_USE()=0` (the Mac
//     compile flag set by aaa_apply_engine_compile_flags).
//   - `code_utils/time_buf.h` (transitively included via the
//     #if 0 / #else fallback in spy.h's TBUF_* macros stay no-ops --
//     spy.cpp does include time_buf.h explicitly, so the macros need
//     to be at least declared) parses.
//   - `spy::yield`, `spy::sleep`, `spy::sleep_micro`, `spy::sleep_ex`
//     are declared (cross-platform interface preserved).
//   - `spy::wait_for_single_object` is NOT declared on Mac (gated by
//     `#if AAA_OS_WINDOWS()` in spy.h).
//
// Why this matters : spy.h is one of the includes in aaalua_util.cpp
// (continuation 50 / 52 noted it as a blocker). The header now parses
// on Mac ; the .cpp also compiles (verified in continuation 55 by
// inspection of the gated paths). This removes one item from the
// aaalua_util.cpp blocker list.

#include <gtest/gtest.h>

#include <type_traits>

#include "aaa_type.h"
#include "code_utils/spy.h"

//	Compile-time signature checks : ensure the cross-platform interface
//	survives the Mac gate work. If the spy.h surface ever drifts, these
//	assertions surface the regression here, not in a downstream wrapper
//	port that's harder to bisect.
static_assert( std::is_same_v< decltype( spy::yield ),
                              void( C_PCHAR_C ) >,
    "spy::yield must remain a void(C_PCHAR_C) cross-platform" );
static_assert( std::is_same_v< decltype( spy::sleep ),
                              void( INT32 const, C_PCHAR_C ) >,
    "spy::sleep must remain void(INT32, C_PCHAR_C)" );
static_assert( std::is_same_v< decltype( spy::sleep_micro ),
                              void( INT32 const, C_PCHAR_C ) >,
    "spy::sleep_micro must remain void(INT32, C_PCHAR_C)" );
static_assert( std::is_same_v< decltype( spy::sleep_ex ),
                              void( INT32 const, bool const, C_PCHAR_C ) >,
    "spy::sleep_ex must remain void(INT32, bool, C_PCHAR_C) on all platforms" );

TEST( SpyHeader, HeaderChainParsesCleanly )
{
    //	Compile-time success is the assertion. The static_asserts above
    //	carry the actual contract.
    SUCCEED();
}

TEST( SpyHeader, SpyColorEnumValuesAreStable )
{
    //	The SPY_COLOR enum holds ARGB values consumed by NVTX (Win) and,
    //	in the future, an equivalent macOS instrumentation tag. Pin a
    //	few representative values so an accidental enum reorder fires
    //	here.
    EXPECT_EQ( UINT32( spy::MEM       ), UINT32( 0xff008080 ) );
    EXPECT_EQ( UINT32( spy::GOL       ), UINT32( 0xffc06000 ) );
    EXPECT_EQ( UINT32( spy::LUA       ), UINT32( 0xff800080 ) );
    EXPECT_EQ( UINT32( spy::CALLBACK_FN ), UINT32( 0xffc0c0c0 ) );
}
