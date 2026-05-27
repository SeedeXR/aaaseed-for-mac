# tests/regression/CMakeLists.txt
#
# Regression-corpus tests. Each test renders a deterministic scene
# through the GOL Metal backend, reads back the full pixel buffer, and
# compares against a stored golden. Per philosophy.md Part 2.95 + Phase 7
# in todo.md, a regression failure blocks the porting step from being
# marked done.
#
# Goldens live in tests/regression/golden/. For trivial deterministic
# renders the golden is described inline in C++ (no external file). When
# the corpus grows to Windows-reference MEU frames, we will vendor
# stb_image / stb_image_write and the harness will load .png goldens.

add_executable(aaaseed_regression_tests
    full_screen_red_regression_test.cpp
)
target_link_libraries(aaaseed_regression_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_regression_tests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(aaaseed_regression_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "regression;gol_metal"
)

# Phase 3 EXIT golden-frame regression (continuation 120 / Task #156).
# Drives MetalBackend offscreen against the Path A `ps_Maa_add_scale`
# shader + a procedural gradient texture + fixed uniforms, reads back
# the BGRA8 buffer, compares against tests/regression/golden/phase3_*.png.
# First-run mode auto-captures the golden for commit. Headless-safe :
# uses the offscreen MTLTexture render path, NOT MTKView (which doesn't
# work without a window-server connection on CI runners).
add_executable(aaaseed_regression_phase3_tests
    phase3_golden_frame_regression_test.cpp
)
target_link_libraries(aaaseed_regression_phase3_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
    aaaseed_stb
)
target_compile_features(aaaseed_regression_phase3_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_regression_phase3_tests PRIVATE
    "AAA_SHADERS_MSL_DIR=\"${CMAKE_SOURCE_DIR}/src/shaders/msl\""
    "AAA_REGRESSION_GOLDEN_DIR=\"${CMAKE_SOURCE_DIR}/tests/regression/golden\""
)
gtest_discover_tests(aaaseed_regression_phase3_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "regression;phase3"
)
