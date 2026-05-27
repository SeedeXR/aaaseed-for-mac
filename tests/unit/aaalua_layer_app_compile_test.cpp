// tests/unit/aaalua_layer_app_compile_test.cpp
//
// Compile-only beachhead for vendor/aaaseed-engine/Src/infrastructure/layer/app.cpp on Mac.
// Task #146 Layer Stage 2 (c119, 2026-05-26). Mirrors the c105 OBJECT-lib pattern
// used for aaalua_master / aaalua_wrap / aaalua_util : the CMake target
// `aaaseed_layer_app_compile_tests` is an OBJECT library that compiles the
// vendored app.cpp as a translation unit ; link is NOT attempted.
//
// app.cpp is the smallest leaf of the 4-level render-graph hierarchy
// (c_layer < c_layers < c_module < c_modules) at 447 LOC. It owns g_app
// and forward-declares c_modules. Goal of this stage : surface the exact
// compile-time blockers (Win-leaky headers in its transitive cone) and
// resolve them via the faked-class / umbrella-shim doctrine WITHOUT any
// vendor edit.
//
// SUCCEED-only sentinel ; the deliverable is the .o file.

#include <gtest/gtest.h>

TEST( AaaluaLayerAppCompile, TranslationUnitCompiles )
{
    //  Nothing to assert. If this compiles, app.cpp compiles. The .o
    //  produced for app.cpp on disk is the deliverable.
    SUCCEED();
}
