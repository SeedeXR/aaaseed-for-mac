# tests/integration/CMakeLists.txt
#
# c144 : First integration test target for the AAASeed Mac port. Hosts
# AAASeedMTKView.mm + AAASeedInputView.mm directly (rather than linking
# the aaaseed_app executable) so gtest can drive the view + delegate
# in-process without launching NSApplication.

find_library(COCOA_FRAMEWORK_IT      Cocoa      REQUIRED)
find_library(METALKIT_FRAMEWORK_IT   MetalKit   REQUIRED)
find_library(METAL_FRAMEWORK_IT      Metal      REQUIRED)
find_library(FOUNDATION_FRAMEWORK_IT Foundation REQUIRED)
find_library(QUARTZCORE_FRAMEWORK_IT QuartzCore REQUIRED)

# c144 : MEU-runner-in-app integration test. Compiles AAASeedMTKView.mm
# and AAASeedInputView.mm directly into the test binary -- same pattern
# the c19+ unit tests use for engine-side .cpp files that haven't yet
# graduated to their own static lib. The view ivars + delegate methods
# need an ObjC++ TU so the test source is .mm too.
add_executable(aaaseed_meu_runner_in_app_tests
    meu_runner_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
)

set_source_files_properties(
    meu_runner_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
    PROPERTIES
    LANGUAGE OBJCXX
    #  Matches src/ui/macos/CMakeLists.txt -- the AAASeed*.mm sources are
    #  written under manual reference counting to match metal-cpp.
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_meu_runner_in_app_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/metal-cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/macos"
)

target_link_libraries(aaaseed_meu_runner_in_app_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
    aaaseed_meu_runner
    aaaseed_ui_studio_mac
    aaaseed_event_bridge
    aaaseed_text
    aaaseed_stb
    "${COCOA_FRAMEWORK_IT}"
    "${METALKIT_FRAMEWORK_IT}"
    "${METAL_FRAMEWORK_IT}"
    "${FOUNDATION_FRAMEWORK_IT}"
    "${QUARTZCORE_FRAMEWORK_IT}"
)

#  c144 : same hello_world.lua fallback path the .app uses when NSBundle
#  lookup misses. Under ctest the binary lives outside the .app bundle,
#  so NSBundle resolves to the test executable's bundle which doesn't
#  ship hello_world.lua. The compile-time fallback closes that gap.
target_compile_definitions(aaaseed_meu_runner_in_app_tests PRIVATE
    AAA_MEU_HELLO_WORLD_LUA_PATH="${CMAKE_SOURCE_DIR}/bundle/macos/meu/hello_world.lua"
)

target_compile_features(aaaseed_meu_runner_in_app_tests PRIVATE cxx_std_20)

include(GoogleTest)

#  Per feedback_ctest_label_first_only.md : on this CMake/ctest toolchain
#  the FIRST label is the only one honored for `ctest -L` filters. Put
#  the primary key (`integration`) FIRST -- the spec's `LABELS` order
#  matters semantically.
gtest_discover_tests(aaaseed_meu_runner_in_app_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;ui;meu;phase4"
)

# c146 : MEU HUD overlay integration test. Same hosting model as the
# c144 meu_runner_in_app_tests target -- compiles AAASeedMTKView.mm +
# AAASeedInputView.mm directly so we can drive [view draw] without
# launching NSApplication. Verifies the Lua aaa.draw_hud_text path
# fires per frame AND the captured text produces non-zero glyph
# quads via aaa::text::layout_text_quads.
add_executable(aaaseed_meu_hud_in_app_tests
    meu_hud_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
)

set_source_files_properties(
    meu_hud_in_app_test.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
#  AAASeedMTKView.mm + AAASeedInputView.mm already have these properties
#  set above for the c144 target -- CMake honors per-target overrides so
#  re-applying here is redundant. The new .mm source needs it explicitly.

target_include_directories(aaaseed_meu_hud_in_app_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/metal-cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/macos"
)

target_link_libraries(aaaseed_meu_hud_in_app_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
    aaaseed_meu_runner
    aaaseed_ui_studio_mac
    aaaseed_event_bridge
    aaaseed_text
    aaaseed_stb
    "${COCOA_FRAMEWORK_IT}"
    "${METALKIT_FRAMEWORK_IT}"
    "${METAL_FRAMEWORK_IT}"
    "${FOUNDATION_FRAMEWORK_IT}"
    "${QUARTZCORE_FRAMEWORK_IT}"
)

#  c144 fallback : hello_world.lua path. c146 addition : the bundled
#  TTF source location, surfaced via AAA_MEU_HUD_FONT_PATH so the test
#  can rebuild the same atlas the .app does. Under ctest the test
#  binary lives outside the .app bundle so NSBundle has no Resources/.
target_compile_definitions(aaaseed_meu_hud_in_app_tests PRIVATE
    AAA_MEU_HELLO_WORLD_LUA_PATH="${CMAKE_SOURCE_DIR}/bundle/macos/meu/hello_world.lua"
    AAA_MEU_HUD_FONT_PATH="${CMAKE_SOURCE_DIR}/vendor/aaaseed-runtime/AAAKernel/Fonts/Google/Source_Code_Pro/static/SourceCodePro-Medium.ttf"
)

target_compile_features(aaaseed_meu_hud_in_app_tests PRIVATE cxx_std_20)

gtest_discover_tests(aaaseed_meu_hud_in_app_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;ui;meu;hud;phase4"
)

# c147-A : widget system in-app integration test. Same hosting pattern
# as the c144 / c146 cousin tests : compile AAASeedMTKView.mm +
# AAASeedInputView.mm directly into the test binary so the .app's
# delegate is driven without launching NSApplication. Two tests :
#   1. WidgetsRenderInDrawInMTKView : a Lua script calling
#      aaa.ui.slider() produces last_frame_widget_count == 1 per frame.
#   2. MouseClickReachesWidgets : a synthesized mouseDown inside the
#      slider's rect flips last_frame_had_interaction true.
add_executable(aaaseed_widgets_in_app_tests
    widgets_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
)

set_source_files_properties(
    widgets_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_widgets_in_app_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/metal-cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/macos"
)

target_link_libraries(aaaseed_widgets_in_app_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
    aaaseed_meu_runner
    aaaseed_ui_studio_mac
    aaaseed_event_bridge
    aaaseed_text
    aaaseed_stb
    aaaseed_ui_widgets_mac
    "${COCOA_FRAMEWORK_IT}"
    "${METALKIT_FRAMEWORK_IT}"
    "${METAL_FRAMEWORK_IT}"
    "${FOUNDATION_FRAMEWORK_IT}"
    "${QUARTZCORE_FRAMEWORK_IT}"
)

#  hello_world.lua fallback so AAASeedMTKView's init can construct + load
#  the runner even when running outside the .app bundle (under ctest).
target_compile_definitions(aaaseed_widgets_in_app_tests PRIVATE
    AAA_MEU_HELLO_WORLD_LUA_PATH="${CMAKE_SOURCE_DIR}/bundle/macos/meu/hello_world.lua"
)

target_compile_features(aaaseed_widgets_in_app_tests PRIVATE cxx_std_20)

gtest_discover_tests(aaaseed_widgets_in_app_tests
    DISCOVERY_TIMEOUT 30
    # feedback_ctest_label_first_only.md : `integration` is the primary
    # filter key and goes first. `phase4` is retained because Phase 4
    # is where the input + render pipeline this exercises was completed.
    PROPERTIES LABELS "integration;ui;widgets;phase4"
)

# c148-A v2 Phases 2-4 integration tests : 3 cases driving the new
# widget primitives through the full MTKView + InputView + MEU runner
# stack. Same hosting pattern as aaaseed_widgets_in_app_tests above.
add_executable(aaaseed_widgets_v2_in_app_tests
    widgets_v2_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
)

set_source_files_properties(
    widgets_v2_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_widgets_v2_in_app_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/metal-cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/macos"
)

target_link_libraries(aaaseed_widgets_v2_in_app_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
    aaaseed_meu_runner
    aaaseed_ui_studio_mac
    aaaseed_event_bridge
    aaaseed_text
    aaaseed_stb
    aaaseed_ui_widgets_mac
    "${COCOA_FRAMEWORK_IT}"
    "${METALKIT_FRAMEWORK_IT}"
    "${METAL_FRAMEWORK_IT}"
    "${FOUNDATION_FRAMEWORK_IT}"
    "${QUARTZCORE_FRAMEWORK_IT}"
)

target_compile_definitions(aaaseed_widgets_v2_in_app_tests PRIVATE
    AAA_MEU_HELLO_WORLD_LUA_PATH="${CMAKE_SOURCE_DIR}/bundle/macos/meu/hello_world.lua"
)

target_compile_features(aaaseed_widgets_v2_in_app_tests PRIVATE cxx_std_20)

gtest_discover_tests(aaaseed_widgets_v2_in_app_tests
    DISCOVERY_TIMEOUT 30
    # `integration` first per feedback_ctest_label_first_only.md ; `v2`
    # marks the Phase 2-4 batch + `phase4` keeps the cohort alignment.
    PROPERTIES LABELS "integration;ui;widgets;v2;phase4"
)

# c149-A v3 integration tests : 3 cases driving drag-drop + file-watch +
# preset round-trip through the full MTKView + InputView + MEU runner
# stack. Same hosting pattern as aaaseed_widgets_in_app_tests.
add_executable(aaaseed_v3_in_app_tests
    v3_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
)

set_source_files_properties(
    v3_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_v3_in_app_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/metal-cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/macos"
)

target_link_libraries(aaaseed_v3_in_app_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
    aaaseed_meu_runner
    aaaseed_ui_studio_mac
    aaaseed_event_bridge
    aaaseed_text
    aaaseed_stb
    aaaseed_ui_widgets_mac
    aaaseed_ui_file_dialog_mac
    aaaseed_meu_file_watcher
    "${COCOA_FRAMEWORK_IT}"
    "${METALKIT_FRAMEWORK_IT}"
    "${METAL_FRAMEWORK_IT}"
    "${FOUNDATION_FRAMEWORK_IT}"
    "${QUARTZCORE_FRAMEWORK_IT}"
)

target_compile_definitions(aaaseed_v3_in_app_tests PRIVATE
    AAA_MEU_HELLO_WORLD_LUA_PATH="${CMAKE_SOURCE_DIR}/bundle/macos/meu/hello_world.lua"
)

target_compile_features(aaaseed_v3_in_app_tests PRIVATE cxx_std_20)

gtest_discover_tests(aaaseed_v3_in_app_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;ui;v3;phase4"
)

# c148 : ImGui Studio in-app integration tests.
# Spins up a real MetalBackend + AAASeedMTKViewDelegate and exercises
# the Studio via its C++ data model API headlessly.
add_executable(aaaseed_studio_in_app_tests
    studio_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
)

set_source_files_properties(
    studio_in_app_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedMTKView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_studio_in_app_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/metal-cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/macos"
)

target_link_libraries(aaaseed_studio_in_app_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
    aaaseed_meu_runner
    aaaseed_event_bridge
    aaaseed_text
    aaaseed_stb
    aaaseed_ui_widgets_mac
    aaaseed_ui_studio_mac
    "${COCOA_FRAMEWORK_IT}"
    "${METALKIT_FRAMEWORK_IT}"
    "${METAL_FRAMEWORK_IT}"
    "${FOUNDATION_FRAMEWORK_IT}"
    "${QUARTZCORE_FRAMEWORK_IT}"
)

target_compile_features(aaaseed_studio_in_app_tests PRIVATE cxx_std_20)

gtest_discover_tests(aaaseed_studio_in_app_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;studio"
)
