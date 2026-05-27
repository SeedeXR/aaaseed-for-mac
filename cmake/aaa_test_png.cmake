# cmake/aaa_test_png.cmake
#
# Build-time fixture generator. Compiles `tools/make_test_png/` into a
# host executable that calls stbi_write_png to emit a 64x64 RGBA test
# pattern. Output : `${CMAKE_BINARY_DIR}/test_assets/test_pattern.png`.
#
# The .app bundle's post-build copy step (src/ui/macos/CMakeLists.txt)
# picks the PNG up and drops it into `Contents/Resources/`. The .app
# loads it at runtime via NSBundle resource lookup + stbi_load.
#
# Continuation 46 -- end-to-end Phase 5 asset pipeline proof at the
# .app level.

add_executable(aaa_make_test_png
    "${CMAKE_SOURCE_DIR}/tools/make_test_png/make_test_png.c"
)
target_link_libraries(aaa_make_test_png PRIVATE aaaseed_stb)

set(AAA_TEST_PNG_PATH "${CMAKE_BINARY_DIR}/test_assets/test_pattern.png")

#  Custom command : run the generator after it's built ; output is the
#  PNG file. The .app target's post-build copy step DEPENDS on this
#  output, so CMake schedules the generator run before the copy.
add_custom_command(
    OUTPUT "${AAA_TEST_PNG_PATH}"
    DEPENDS aaa_make_test_png
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/test_assets"
    COMMAND aaa_make_test_png "${AAA_TEST_PNG_PATH}"
    COMMENT "Generating Phase 5 test_pattern.png via aaa_make_test_png"
    VERBATIM
)

#  A phony target groups the .png + its dependency. Consumers (e.g.
#  aaaseed_app) depend on this target.
add_custom_target(aaa_test_png_fixture
    DEPENDS "${AAA_TEST_PNG_PATH}"
)
