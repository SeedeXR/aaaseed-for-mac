// tests/unit/aaalua_util_compile_test.cpp
//
// Compile-only beachhead for aaalua_util.cpp on Mac, mirroring the
// c105/c106 approach used for aaalua_master.cpp and aaalua_wrap.cpp.
// The CMake target `aaaseed_aaalua_util_compile_tests` is an OBJECT
// library that compiles the engine TU through AppleClang. Link is
// intentionally NOT attempted -- the TU has unresolved factory /
// the_factory / push_caller / pop_caller / o_str method references
// at link time (the multi-session trio cascade, Task #94). The
// compile alone is the deliverable.

#include <gtest/gtest.h>

TEST( AaaluaUtilCompile, TranslationUnitCompiles )
{
    //  Nothing to assert -- the value of this test is the compile of
    //  vendor/aaaseed-engine/Src/language/lua/aaalua_util.cpp as a
    //  TU of this object library. If we got here, compile-green is real.
    SUCCEED();
}
