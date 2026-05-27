// Stage 1 of the engine port: prove that AAASeed's platform-detection headers
// (aaa_type.h, Native_System.h, aaa_def.h) and the force-include of
// aaa_build_config.h all resolve and produce the expected Mac flags.
//
// No engine .cpp is compiled at this stage. This is purely a header-graph
// smoke test against the cross-tree includes set up by
// aaa_apply_engine_compile_flags() in cmake/aaa_compile_flags.cmake.

#include <gtest/gtest.h>

#include "aaa_def.h"
#include "aaa_type.h"
#include "Native_System.h"

TEST( AAAPlatform, BuildConfigForceIncluded )
{
    EXPECT_EQ( AAASEED(), 1 );
    EXPECT_EQ( AAA_NEW_DESIGN(), 1 );
    EXPECT_EQ( AAA_TRACKER(), 1 );
}

TEST( AAAPlatform, NativeSystemDarwinMacrosOnApple )
{
#ifdef NATIVE_OS_DARWIN
    SUCCEED() << "NATIVE_OS_DARWIN defined as expected.";
#else
    FAIL() << "NATIVE_OS_DARWIN should be defined under Native_System.h on __APPLE__.";
#endif

#ifdef NATIVE_OS_DARWIN64
    SUCCEED() << "NATIVE_OS_DARWIN64 defined (arm64 is LP64).";
#else
    FAIL() << "NATIVE_OS_DARWIN64 should be defined on __LP64__.";
#endif

#ifdef NATIVE_OS_WIN32
    FAIL() << "NATIVE_OS_WIN32 must NOT be defined on Mac.";
#endif

#ifdef NATIVE_OS_WIN64
    FAIL() << "NATIVE_OS_WIN64 must NOT be defined on Mac.";
#endif

#ifdef NATIVE_OS_BSD4
    SUCCEED() << "NATIVE_OS_BSD4 defined (Darwin is BSD-derived).";
#else
    FAIL() << "NATIVE_OS_BSD4 should also be defined on Darwin.";
#endif
}

TEST( AAAPlatform, NativeOs64IsTrueOnArm64 )
{
#if defined( NATIVE_OS_64 ) && NATIVE_OS_64 == 1
    SUCCEED() << "NATIVE_OS_64 == 1 on arm64 (LP64).";
#else
    FAIL() << "NATIVE_OS_64 should be 1 on Apple Silicon.";
#endif
}

TEST( AAAPlatform, AAADefMacrosAtDefaults )
{
    EXPECT_EQ( AAA_DEMO(), 0 );
    EXPECT_EQ( AAA_DEMO_INTERGRAPH(), 0 );
    EXPECT_EQ( AAA_MENU_LOCKED(), 0 );
    EXPECT_EQ( AAA_MENU_FOCUS_LOCKED(), 0 );
    EXPECT_EQ( AAA_MOUSE_LOCKED(), 0 );
    EXPECT_EQ( AAA_CHECKSUM_ENV_LOCKED(), 0 );
    EXPECT_EQ( AAA_CHECKSUM_PARAM_DO(), 0 );
    EXPECT_EQ( AAA_EDIT(), 1 );
    EXPECT_EQ( AAA_WATCHDOG(), 0 );
}

TEST( AAAPlatform, AppleSiliconIsArm64 )
{
#if defined( __aarch64__ ) || defined( __arm64__ )
    SUCCEED() << "arm64 confirmed.";
#else
    FAIL() << "Mac v1 ships arm64-only.";
#endif
}

TEST( AAAPlatform, AAATypeRecognizesMac )
{
    EXPECT_EQ( AAA_OS_MAC(),     1 );
    EXPECT_EQ( AAA_OS_WINDOWS(), 0 );
    EXPECT_EQ( AAA_OS_LINUX(),   0 );
}

TEST( AAAPlatform, AAATypeNotWin64OnMac )
{
    EXPECT_EQ( AAA_WIN64(), 0 );
    EXPECT_EQ( AAA_BIT64(), 0 );
}

TEST( AAAPlatform, AAATypeLittleEndianOnArm64 )
{
    EXPECT_EQ( AAA_ENDIAN_LITTLE(), 1 );
}
