// tests/unit/aaalua_master_shim/lib_use.h
//
// Tests-private SHIM for vendor/aaaseed-engine/Src/code_utils/lib_use.h,
// used ONLY by the aaalua_master.cpp compile-only test target. Added to
// the target's include path BEFORE the engine include path so this file
// wins for the test build only ; the engine's real lib_use.h still
// resolves everywhere else.
//
// Why a shim : the real lib_use.h relies on MSVC's `__pragma( comment(
// lib, ... ) )` to emit a linker directive in the .obj file. AppleClang
// supports `_Pragma` but not the MSVC `__pragma` extension, and even if
// it did, `#pragma comment(lib, ...)` is a MSVC linker directive that
// has no analog in ld64. On Mac, libraries are linked via CMake's
// target_link_libraries (or -framework / -l flags). So the right
// substitute is : make AAA_LIB_USE / AAA_LIB_USE_D no-ops.
//
// The string-pasting issue (`"./lib/"##s` is invalid since both sides
// are string literals, not pp-tokens) is the immediate compile error,
// but the deeper reason is the platform mismatch above.

#ifdef AAA_LIB_USE_H
#error "LIB_USE_H included more than once."
#endif
#define AAA_LIB_USE_H 1

//	No-op on Mac : library link is driven by CMake, not by source-level
//	pragma directives.
#define AAA_LIB_USE_MESSAGE(s)
#define AAA_LIB_USE32(s)
#define AAA_LIB_USE64(s)
#define AAA_LIB_USE(s)
#define AAA_LIB_USE_D(s)
#define AAA_LIB_USE_PLATFORM()		"macos-arm64"
#define AAA_LIB_USE_PLATFORM_SHORT()	"arm64"
#if AAA_DEBUG()
#	define AAA_LIB_USE_CONFIGURATION()	"Debug"
#else
#	define AAA_LIB_USE_CONFIGURATION()	"Release"
#endif
