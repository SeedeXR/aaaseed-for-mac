
#ifdef AAA_AAA_BUILD_CONFIG_H
#error "AAA_BUILD_CONFIG_H included more than once."
#endif
#define AAA_AAA_BUILD_CONFIG_H 1


// Force-included by the build system in every translation unit:
//   - vcxproj:  <ForcedIncludeFiles>Src/aaa_build_config.h;%(ForcedIncludeFiles)</ForcedIncludeFiles>
//   - CMake:    target_compile_options(... /FI<header>)   (MSVC)
//               target_compile_options(... -include <header>) (Clang/GCC)
//
// Lets vendor sources reach project-wide feature flags as function-like macros
// without including any project header (zero coupling vendor -> project).


// AAASEED() == 1 when compiled as part of the AAASeed build.
// Used by vendor sources (ftgl, MSA, OpenFramework, fbx, ...) to guard local
// adaptations made for AAASeed via the pattern:
//     #if AAASEED() ... AAASeed-specific code ... #else ... vendor original ... #endif
#define AAASEED()				1


// Project feature flags (formerly in vcxproj <PreprocessorDefinitions>):
//   - AAA_NEW_DESIGN()  -- new UI / scene design (was driven by AAA_NEW_DESIGN_PRIVATE)
//   - AAA_TRACKER()     -- tracker subsystem (was driven by AAA_TRACKER_PRIVATE)
// Function-like form for consistency with AAA_DEBUG(), AAA_WIN64(), AAA_OS_WINDOWS().
#define AAA_NEW_DESIGN()		1
#define AAA_TRACKER()			1


// MSA library binding flag, consumed by Src/MSA/MSACore/MSACore.h and
// Src/MSA/MSAFluid/MSAFluid.h via `#if defined(MSA_HOST_AAASEED)`.
// Object-like macro because the vendor checks with `defined()`.
#define MSA_HOST_AAASEED		1
