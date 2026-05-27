// tests/unit/aaalua_wrap_compile_test.cpp
//
// Compile-only beachhead for aaalua_wrap.cpp on Mac, mirroring the
// c105 approach used for aaalua_master.cpp. The CMake target
// `aaaseed_aaalua_wrap_compile_tests` is an OBJECT library that
// compiles the engine TU through AppleClang. Link is intentionally
// NOT attempted -- the TU has unresolved factory / param / obj_ui
// references at link time. The compile alone is the deliverable.

#include <gtest/gtest.h>

TEST( AaaluaWrapCompile, TranslationUnitCompiles )
{
    //  Nothing to assert -- the value of this test is the compile of
    //  vendor/aaaseed-engine/Src/language/lua/aaalua_wrap.cpp as a
    //  TU of this object library. If we got here, compile-green is real.
    SUCCEED();
}
