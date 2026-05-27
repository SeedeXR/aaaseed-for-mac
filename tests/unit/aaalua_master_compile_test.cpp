// tests/unit/aaalua_master_compile_test.cpp
//
// Compile-only beachhead for aaalua_master.cpp on Mac. This file does
// not exercise any runtime behavior of `c_lua_master` ; it simply pulls
// the engine TU into a translation unit through the test executable so
// the AppleClang front-end has to chew through the full include chain
// (aaalua_master.h, aaalua_wrap.h, aaalua_util.h, aaalua_debug.h,
//  aaalua_glue.h, infrastructure/param/trax.h, obj_ui/com/net.h,
//  infrastructure/seedfile.h, infrastructure/param/param_declare.h,
//  math/v_base.h, spy.h, system/shared/SystemUtils.h, aaa/execute.h).
//
// The CMake target `aaaseed_aaalua_master_compile_tests` compiles the
// vendored .cpp as part of the test binary -- if it links it links, if
// it doesn't we still get the compile errors. This is Stage X (TBD) of
// the multi-session aaalua_master.cpp port (Task #94).

#include <gtest/gtest.h>

TEST( AaaluaMasterCompile, TranslationUnitCompiles )
{
    //  Nothing to assert -- the value of this test is the compile of
    //  vendor/aaaseed-engine/Src/language/lua/aaalua_master.cpp as a
    //  TU of this executable. If we got here, compile-green is real.
    SUCCEED();
}
