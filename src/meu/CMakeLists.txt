# src/meu/CMakeLists.txt
#
# c142-B (finished by harness after the in-flight agent hit an API 529
# overload) : Mac-native MEU runner shim. Loads a Lua script + drives the
# Path A 172-shader catalog directly through MetalBackend. Sidesteps the
# c119-A layer-subsystem doctrine wall per project_v1_ship_gate.md.
#
# Hermetic Mac sub-lib per feedback_hermetic_mac_sublibs.md :
#   - Pure ObjC++. No o_str / aaa_str.h / aaa_mem.h.
#   - Lua bindings via raw lua_pushcfunction (c124-A precedent).
#   - Links aaaseed_lua + aaaseed_gol_metal + Foundation.

find_library(FOUNDATION_FRAMEWORK_MEU Foundation REQUIRED)
#  c149-A v3 Feature 3 : FSEvents lives in CoreServices.
find_library(CORESERVICES_FRAMEWORK_MEU CoreServices REQUIRED)

# c149-A v3 Feature 3 : hermetic FileWatcher sub-lib. Pure CoreServices
# FSEventStream wrapper ; no o_str / aaa_str.h. Builds standalone so
# tests can link it without pulling the full runner.
add_library(aaaseed_meu_file_watcher STATIC
    aaa_file_watcher_mac.mm
)

set_source_files_properties(aaa_file_watcher_mac.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_meu_file_watcher PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_meu_file_watcher PUBLIC
    "${FOUNDATION_FRAMEWORK_MEU}"
    "${CORESERVICES_FRAMEWORK_MEU}"
)

target_compile_features(aaaseed_meu_file_watcher PUBLIC cxx_std_20)

set_target_properties(aaaseed_meu_file_watcher PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

add_library(aaaseed_meu_runner STATIC
    aaa_meu_runner_mac.mm
)

set_source_files_properties(aaa_meu_runner_mac.mm
    PROPERTIES
    LANGUAGE OBJCXX
    #  Matches the rest of the Mac sub-libs (event_bridge, syphon, ui/macos).
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_meu_runner PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_meu_runner PUBLIC
    "${FOUNDATION_FRAMEWORK_MEU}"
    aaaseed_lua
    aaaseed_gol_metal
    #  c147-A : aaa.ui.* Lua bindings dispatch into the WidgetSystem.
    #  PUBLIC so downstream consumers (aaaseed_app, integration tests)
    #  pick up the widget symbols transitively without restating the
    #  link line.
    aaaseed_ui_widgets_mac
    #  c149-A v3 Feature 2 : aaa.io.open_file_dialog / save_file_dialog
    #  Lua bindings dispatch into the existing c131-B NSOpenPanel adapter.
    aaaseed_ui_file_dialog_mac
    #  c149-A v3 Feature 3 : aaa.io.* hot-reload watcher wraps FSEvents.
    aaaseed_meu_file_watcher
)

#  Surface the Path A catalog source dir to the .mm for the test-runtime
#  fallback path (`AAA_SHADERS_MSL_DIR` define). The .app bundle path
#  is preferred at runtime via NSBundle.
target_compile_definitions(aaaseed_meu_runner PRIVATE
    AAA_SHADERS_MSL_DIR="${CMAKE_SOURCE_DIR}/src/shaders/msl"
)

target_compile_features(aaaseed_meu_runner PUBLIC cxx_std_20)

set_target_properties(aaaseed_meu_runner PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
