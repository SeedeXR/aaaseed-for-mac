# tests/qt/CMakeLists.txt
#
# c152-E : Qt::Test coverage for the Qt6 + QML AAASeed Studio data
# layer. Pure C++ tests via QTEST_GUILESS_MAIN -- no QML scene loaded,
# no QGuiApplication required for most tests. The CameraController
# tests don't actually start a capture session (would trigger the
# macOS camera permission dialog under CI / headless runs).
#
# Each test binary is a single .cpp file with its own QObject + #include
# of the generated .moc. AUTOMOC handles the rest.

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test Multimedia Qml Quick)

# Shared link surface : the Qt port's panel adapters + the studio
# data model + Qt6::Test.
set(AAA_QT_TEST_LIBS
    aaaseed_ui_studio
    aaaseed_lua
    Qt6::Core
    Qt6::Gui
    Qt6::Test
    Qt6::Multimedia
    Qt6::Qml
)

# We compile the adapter .cpp files directly into each test binary
# rather than linking against the studio app target ; AUTOMOC on these
# files runs ONCE per test target (Qt's recommended pattern for stand-
# alone tests).
set(AAA_QT_ADAPTER_SOURCES
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_studio_model.cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_studio_model.h"
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_panel_models.cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_panel_models.h"
)

# ── StudioModel test ──────────────────────────────────────────────────────
add_executable(aaa_qt_studio_model_test
    aaa_studio_model_test.cpp
    ${AAA_QT_ADAPTER_SOURCES}
)
target_include_directories(aaa_qt_studio_model_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(aaa_qt_studio_model_test PRIVATE ${AAA_QT_TEST_LIBS})
target_compile_features(aaa_qt_studio_model_test PRIVATE cxx_std_20)
target_compile_options(aaa_qt_studio_model_test PRIVATE -Wno-deprecated-declarations)
add_test(NAME aaa_qt_studio_model_test COMMAND aaa_qt_studio_model_test)
set_tests_properties(aaa_qt_studio_model_test PROPERTIES
    LABELS "qt;unit"
    TIMEOUT 60
)

# ── Panel models test (Sound / Camera / Tasks) ────────────────────────────
add_executable(aaa_qt_panel_models_test
    aaa_panel_models_test.cpp
    ${AAA_QT_ADAPTER_SOURCES}
)
target_include_directories(aaa_qt_panel_models_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(aaa_qt_panel_models_test PRIVATE ${AAA_QT_TEST_LIBS})
target_compile_features(aaa_qt_panel_models_test PRIVATE cxx_std_20)
target_compile_options(aaa_qt_panel_models_test PRIVATE -Wno-deprecated-declarations)
add_test(NAME aaa_qt_panel_models_test COMMAND aaa_qt_panel_models_test)
set_tests_properties(aaa_qt_panel_models_test PROPERTIES
    LABELS "qt;unit"
    TIMEOUT 60
)

# ── Settings model test (c152-N : theme/editor/auto-save round-trip) ──
add_executable(aaa_qt_settings_model_test
    aaa_settings_model_test.cpp
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_settings_model.cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_settings_model.h"
)
target_include_directories(aaa_qt_settings_model_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(aaa_qt_settings_model_test PRIVATE
    Qt6::Core
    Qt6::Test
)
target_compile_features(aaa_qt_settings_model_test PRIVATE cxx_std_20)
target_compile_options(aaa_qt_settings_model_test PRIVATE -Wno-deprecated-declarations)
add_test(NAME aaa_qt_settings_model_test COMMAND aaa_qt_settings_model_test)
set_tests_properties(aaa_qt_settings_model_test PROPERTIES
    LABELS "qt;unit"
    TIMEOUT 20
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Lua helper test (c152-M : highlighter attach + lint + classify) ──
add_executable(aaa_qt_lua_helper_test
    aaa_lua_helper_test.cpp
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_lua_helper.cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_lua_helper.h"
)
target_include_directories(aaa_qt_lua_helper_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(aaa_qt_lua_helper_test PRIVATE
    aaaseed_lua
    Qt6::Core
    Qt6::Gui
    Qt6::Test
    Qt6::Qml
    Qt6::Quick
)
target_compile_features(aaa_qt_lua_helper_test PRIVATE cxx_std_20)
target_compile_options(aaa_qt_lua_helper_test PRIVATE -Wno-deprecated-declarations)
add_test(NAME aaa_qt_lua_helper_test COMMAND aaa_qt_lua_helper_test)
set_tests_properties(aaa_qt_lua_helper_test PROPERTIES
    LABELS "qt;unit"
    TIMEOUT 30
)

# ── c154 : native-feature bridge + Display-menu integration ─────────────────
# The c153 hermetic sub-libs the bridge surfaces to QML. Linked into the
# bridge-driven tests (the regression test links none of these -- it only
# reads files).
set(AAA_NATIVE_FEATURE_LIBS
    aaaseed_midi
    aaaseed_audio
    aaaseed_video
    aaaseed_syphon
    aaaseed_clipboard
    aaaseed_net
    aaaseed_display
)

# Unit : the bridge QObjects (guiless ; no QML scene, no camera/mic open).
add_executable(aaa_qt_native_bridge_test
    aaa_native_bridge_test.cpp
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_native_bridge.cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_native_bridge.h"
)
target_include_directories(aaa_qt_native_bridge_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(aaa_qt_native_bridge_test PRIVATE
    ${AAA_NATIVE_FEATURE_LIBS}
    Qt6::Core Qt6::Gui Qt6::Test Qt6::Qml
)
target_compile_features(aaa_qt_native_bridge_test PRIVATE cxx_std_20)
target_compile_options(aaa_qt_native_bridge_test PRIVATE -Wno-deprecated-declarations)
add_test(NAME aaa_qt_native_bridge_test COMMAND aaa_qt_native_bridge_test)
set_tests_properties(aaa_qt_native_bridge_test PROPERTIES
    LABELS "qt;unit"
    TIMEOUT 60
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# Integration : load NativeDevicesPanel.qml with the bridge context
# properties bound, assert it reaches Ready (the feature "appears").
add_executable(aaa_qt_native_panel_qml_test
    aaa_native_panel_qml_test.cpp
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_native_bridge.cpp"
    "${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_native_bridge.h"
)
target_include_directories(aaa_qt_native_panel_qml_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(aaa_qt_native_panel_qml_test PRIVATE
    ${AAA_NATIVE_FEATURE_LIBS}
    Qt6::Core Qt6::Gui Qt6::Test Qt6::Qml Qt6::Quick Qt6::QuickControls2
)
target_compile_definitions(aaa_qt_native_panel_qml_test PRIVATE
    "AAA_NATIVE_DEVICES_PANEL_QML=\"${CMAKE_SOURCE_DIR}/src/ui/qt/qml/panels/NativeDevicesPanel.qml\""
)
target_compile_features(aaa_qt_native_panel_qml_test PRIVATE cxx_std_20)
target_compile_options(aaa_qt_native_panel_qml_test PRIVATE -Wno-deprecated-declarations)
add_test(NAME aaa_qt_native_panel_qml_test COMMAND aaa_qt_native_panel_qml_test)
set_tests_properties(aaa_qt_native_panel_qml_test PROPERTIES
    LABELS "qt;integration"
    TIMEOUT 60
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# c156 : Run Script (Cmd+R) editor-execution pipeline (guiless ; spawn
# disabled via the test seam).
add_executable(aaa_qt_run_script_test
    aaa_run_script_test.cpp
    ${AAA_QT_ADAPTER_SOURCES}
)
target_include_directories(aaa_qt_run_script_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(aaa_qt_run_script_test PRIVATE ${AAA_QT_TEST_LIBS})
target_compile_features(aaa_qt_run_script_test PRIVATE cxx_std_20)
target_compile_options(aaa_qt_run_script_test PRIVATE -Wno-deprecated-declarations)
add_test(NAME aaa_qt_run_script_test COMMAND aaa_qt_run_script_test)
set_tests_properties(aaa_qt_run_script_test PROPERTIES
    LABELS "qt;unit"
    TIMEOUT 60
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# Regression : file-content guards (plist permissions + QML/qrc/cpp wiring).
add_executable(aaa_qt_integration_regression_test
    aaa_integration_regression_test.cpp
)
target_include_directories(aaa_qt_integration_regression_test PRIVATE "${CMAKE_SOURCE_DIR}")
target_link_libraries(aaa_qt_integration_regression_test PRIVATE
    Qt6::Core Qt6::Test
)
target_compile_definitions(aaa_qt_integration_regression_test PRIVATE
    "AAA_INFO_PLIST_IN=\"${CMAKE_SOURCE_DIR}/bundle/macos/Info.plist.in\""
    "AAA_ENTITLEMENTS_PLIST=\"${CMAKE_SOURCE_DIR}/bundle/macos/entitlements.plist\""
    "AAA_QT_CMAKELISTS=\"${CMAKE_SOURCE_DIR}/src/ui/qt/CMakeLists.txt\""
    "AAA_MAIN_QML=\"${CMAKE_SOURCE_DIR}/src/ui/qt/qml/Main.qml\""
    "AAA_QT_QRC=\"${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_qt_qml.qrc\""
    "AAA_QT_MAIN_CPP=\"${CMAKE_SOURCE_DIR}/src/ui/qt/aaa_qt_main.cpp\""
    "AAA_CODE_EDITOR_QML=\"${CMAKE_SOURCE_DIR}/src/ui/qt/qml/panels/CodeEditorPanel.qml\""
    "AAA_PORTAL_SHADER_METAL=\"${CMAKE_SOURCE_DIR}/src/shaders/msl/aaa_particle_portal.metal\""
)
target_compile_features(aaa_qt_integration_regression_test PRIVATE cxx_std_20)
target_compile_options(aaa_qt_integration_regression_test PRIVATE -Wno-deprecated-declarations)
add_test(NAME aaa_qt_integration_regression_test COMMAND aaa_qt_integration_regression_test)
set_tests_properties(aaa_qt_integration_regression_test PROPERTIES
    LABELS "qt;regression"
    TIMEOUT 30
)
