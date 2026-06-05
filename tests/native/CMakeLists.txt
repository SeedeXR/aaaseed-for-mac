# tests/native/CMakeLists.txt
#
# second_todo.md (c153) : ALWAYS-ON test tree for the wave-2 native Mac
# subsystems (clipboard / midi / net / syphon-directory). The legacy
# tests/unit + tests/regression trees are quarantined behind
# AAA_BUILD_LEGACY_TESTS (root CMakeLists L96) while the Qt cutover settles ;
# this tree is added UNCONDITIONALLY (like tests/qt) so the new subsystems
# ship with a runnable test pyramid that ctest picks up by default.
#
# Test layout per subsystem :
#   - unit       : pure deterministic logic (encode/flip/auth/json) -- always runs.
#   - integration: real-OS-resource round-trips (NSPasteboard, DN discovery,
#                  CoreMIDI enumeration) -- runs headless, no hardware needed.
#   - regression : stability of the deterministic transforms (covered by the
#                  pure-logic unit cases ; flagged with the `regression` label).
# Hardware / network-dependent paths GTEST_SKIP() with an explicit logged
# reason (never silent) per feedback_regression_guard_tests.md.

include(GoogleTest)

#  ----------------------------------------------------------------------
#  Clipboard (S6)
#  ----------------------------------------------------------------------
add_executable(aaaseed_clipboard_native_tests
    clipboard_mac_test.mm
)
set_source_files_properties(clipboard_mac_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_clipboard_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_clipboard
)
target_include_directories(aaaseed_clipboard_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_clipboard_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_clipboard_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;clipboard"
)

#  ----------------------------------------------------------------------
#  MIDI (S3)
#  ----------------------------------------------------------------------
find_library(COREMIDI_FRAMEWORK_MIDI_TEST CoreMIDI REQUIRED)
add_executable(aaaseed_midi_native_tests
    midi_mac_test.mm
)
set_source_files_properties(midi_mac_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_midi_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_midi
    "${COREMIDI_FRAMEWORK_MIDI_TEST}"
)
target_include_directories(aaaseed_midi_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_midi_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_midi_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;midi"
)

#  ----------------------------------------------------------------------
#  Net : auth-header + live-network guard (S5)
#  ----------------------------------------------------------------------
add_executable(aaaseed_net_native_tests
    net_mac_test.mm
)
set_source_files_properties(net_mac_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_net_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_net
)
target_include_directories(aaaseed_net_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_net_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_net_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;net"
)

#  ----------------------------------------------------------------------
#  Net : JSON -> Lua table binding (S5)
#  ----------------------------------------------------------------------
add_executable(aaaseed_net_json_lua_native_tests
    net_json_lua_test.cpp
)
target_link_libraries(aaaseed_net_json_lua_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_net
    aaaseed_lua
)
target_include_directories(aaaseed_net_json_lua_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_net_json_lua_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_net_json_lua_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;net"
)

#  ----------------------------------------------------------------------
#  Syphon : directory discovery + vertical-flip (S4)
#  ----------------------------------------------------------------------
find_library(METAL_FRAMEWORK_SYPHON_DIR_TEST       Metal          REQUIRED)
find_library(FOUNDATION_FRAMEWORK_SYPHON_DIR_TEST  Foundation     REQUIRED)
find_library(COREFOUNDATION_FRAMEWORK_SYPHON_DIR_TEST CoreFoundation REQUIRED)
find_library(IOSURFACE_FRAMEWORK_SYPHON_DIR_TEST   IOSurface      REQUIRED)
add_executable(aaaseed_syphon_directory_native_tests
    syphon_directory_mac_test.mm
)
set_source_files_properties(syphon_directory_mac_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_syphon_directory_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_syphon
    "${METAL_FRAMEWORK_SYPHON_DIR_TEST}"
    "${FOUNDATION_FRAMEWORK_SYPHON_DIR_TEST}"
    "${COREFOUNDATION_FRAMEWORK_SYPHON_DIR_TEST}"
    "${IOSURFACE_FRAMEWORK_SYPHON_DIR_TEST}"
)
target_include_directories(aaaseed_syphon_directory_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_syphon_directory_native_tests PRIVATE cxx_std_20)
#  RESOURCE_LOCK on the shared DN bus so the directory discovery test does
#  not race the existing syphon DN tests (feedback_distnoted_dual_center.md).
gtest_discover_tests(aaaseed_syphon_directory_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES
        LABELS "integration;syphon"
        RESOURCE_LOCK "aaaseed_syphon_dn_bus"
)

#  ----------------------------------------------------------------------
#  Audio : pure beat detector (S1) -- deterministic, no hardware
#  ----------------------------------------------------------------------
add_executable(aaaseed_audio_beat_detector_native_tests
    audio_beat_detector_test.cpp
)
target_link_libraries(aaaseed_audio_beat_detector_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_audio
)
target_include_directories(aaaseed_audio_beat_detector_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_audio_beat_detector_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_audio_beat_detector_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;audio"
)

#  ----------------------------------------------------------------------
#  Audio : device enum + Player crash-safety (S1) ; live mic gated
#  ----------------------------------------------------------------------
add_executable(aaaseed_audio_mac_native_tests
    audio_mac_test.mm
)
set_source_files_properties(audio_mac_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_audio_mac_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_audio
)
target_include_directories(aaaseed_audio_mac_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_audio_mac_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_audio_mac_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;audio"
)

#  ----------------------------------------------------------------------
#  Video : zero-copy CVPixelBuffer -> MTLTexture bridge (S2) -- the
#  headline unified-memory test. CI-safe (synthesized pixel buffer).
#  ----------------------------------------------------------------------
find_library(METAL_FRAMEWORK_VIDEO_TEST     Metal     REQUIRED)
find_library(COREVIDEO_FRAMEWORK_VIDEO_TEST CoreVideo REQUIRED)
add_executable(aaaseed_video_metal_bridge_native_tests
    video_metal_bridge_test.mm
)
set_source_files_properties(video_metal_bridge_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_video_metal_bridge_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_video
    "${METAL_FRAMEWORK_VIDEO_TEST}"
    "${COREVIDEO_FRAMEWORK_VIDEO_TEST}"
)
target_include_directories(aaaseed_video_metal_bridge_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_video_metal_bridge_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_video_metal_bridge_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;video"
)

#  ----------------------------------------------------------------------
#  Video : Movie + Capture crash-safety / enum (S2) ; live camera gated
#  ----------------------------------------------------------------------
find_library(COREVIDEO_FRAMEWORK_VIDEO_MAC_TEST CoreVideo REQUIRED)
add_executable(aaaseed_video_mac_native_tests
    video_mac_test.mm
)
set_source_files_properties(video_mac_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_video_mac_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_video
    "${COREVIDEO_FRAMEWORK_VIDEO_MAC_TEST}"
)
target_include_directories(aaaseed_video_mac_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_video_mac_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_video_mac_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;video"
)

#  ----------------------------------------------------------------------
#  Menu : NSMenu context-menu builder (S7) -- headless-introspectable
#  ----------------------------------------------------------------------
add_executable(aaaseed_menu_native_tests
    menu_mac_test.mm
)
set_source_files_properties(menu_mac_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_menu_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_menu
)
target_include_directories(aaaseed_menu_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_menu_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_menu_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;menu"
)

#  ----------------------------------------------------------------------
#  Display : pure multi-display geometry (S8) -- framework-free
#  ----------------------------------------------------------------------
add_executable(aaaseed_display_layout_native_tests
    display_layout_test.cpp
)
target_link_libraries(aaaseed_display_layout_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_include_directories(aaaseed_display_layout_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_display_layout_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_display_layout_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;display"
)

#  ----------------------------------------------------------------------
#  Display : live NSScreen enum + MultiDisplay aux windows (S8) ; gated
#  ----------------------------------------------------------------------
add_executable(aaaseed_display_mac_native_tests
    display_mac_test.mm
)
set_source_files_properties(display_mac_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_display_mac_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_display
)
target_include_directories(aaaseed_display_mac_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_display_mac_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_display_mac_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;display"
)

#  ----------------------------------------------------------------------
#  Display : sub-rect present primitive (S8) -- CI-safe offscreen blit
#  ----------------------------------------------------------------------
find_library(METAL_FRAMEWORK_DISPLAY_PRESENT_TEST Metal REQUIRED)
add_executable(aaaseed_display_present_native_tests
    display_present_test.mm
)
set_source_files_properties(display_present_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_display_present_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_display
    "${METAL_FRAMEWORK_DISPLAY_PRESENT_TEST}"
)
target_include_directories(aaaseed_display_present_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_display_present_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_display_present_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;display"
)

#  ----------------------------------------------------------------------
#  Perlin sample MEU : pure-Lua unit (c155) -- the embedded Perlin math
#  (determinism / lattice zeros / range) + the on_frame draw contract,
#  via a recording stub aaa table. No Metal, no runner.
#  ----------------------------------------------------------------------
add_executable(aaaseed_perlin_meu_script_native_tests
    perlin_meu_script_test.cpp
)
target_link_libraries(aaaseed_perlin_meu_script_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
)
target_compile_definitions(aaaseed_perlin_meu_script_native_tests PRIVATE
    "AAA_PERLIN_SAMPLE_LUA=\"${CMAKE_SOURCE_DIR}/bundle/macos/meu/Samples/perlin_noise/perlin_noise.lua\""
)
target_include_directories(aaaseed_perlin_meu_script_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_perlin_meu_script_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_perlin_meu_script_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu"
)

#  ----------------------------------------------------------------------
#  Perlin sample MEU : integration (c155) -- real Runner + MetalBackend
#  render offscreen frames ; has_on_frame() diagnostic for module-only
#  scripts (the "my perlin.lua didn't work" failure mode).
#  ----------------------------------------------------------------------
add_executable(aaaseed_meu_runner_perlin_native_tests
    meu_runner_perlin_test.mm
)
set_source_files_properties(meu_runner_perlin_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_meu_runner_perlin_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_meu_runner
    aaaseed_gol_metal
)
target_compile_definitions(aaaseed_meu_runner_perlin_native_tests PRIVATE
    "AAA_PERLIN_SAMPLE_LUA=\"${CMAKE_SOURCE_DIR}/bundle/macos/meu/Samples/perlin_noise/perlin_noise.lua\""
)
target_include_directories(aaaseed_meu_runner_perlin_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_meu_runner_perlin_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_meu_runner_perlin_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;meu"
)

#  ----------------------------------------------------------------------
#  Particle portal sample : pure-Lua unit (c157) -- MEU contract + force
#  uniforms + slider panel + embedded perlin, via recording stubs.
#  ----------------------------------------------------------------------
add_executable(aaaseed_particle_portal_script_native_tests
    particle_portal_script_test.cpp
)
target_link_libraries(aaaseed_particle_portal_script_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
)
target_compile_definitions(aaaseed_particle_portal_script_native_tests PRIVATE
    "AAA_PORTAL_SAMPLE_LUA=\"${CMAKE_SOURCE_DIR}/bundle/macos/meu/Samples/particle_portal/particle_portal.lua\""
)
target_include_directories(aaaseed_particle_portal_script_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_particle_portal_script_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_particle_portal_script_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu"
)

#  ----------------------------------------------------------------------
#  Particle portal : integration (c157) -- real Runner compiles the new
#  catalog shader, renders offscreen, and PIXEL-PROVES the c157 uniform-
#  ABI fix (mode int must reach buffer(2) for the bright field). Plus the
#  legacy vec4@0 ABI regression guard (ps_Maa_add_scale still draws).
#  ----------------------------------------------------------------------
add_executable(aaaseed_particle_portal_runner_native_tests
    particle_portal_runner_test.mm
)
set_source_files_properties(particle_portal_runner_test.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_particle_portal_runner_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_meu_runner
    aaaseed_gol_metal
)
target_compile_definitions(aaaseed_particle_portal_runner_native_tests PRIVATE
    "AAA_PORTAL_SAMPLE_LUA=\"${CMAKE_SOURCE_DIR}/bundle/macos/meu/Samples/particle_portal/particle_portal.lua\""
)
target_include_directories(aaaseed_particle_portal_runner_native_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_particle_portal_runner_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_particle_portal_runner_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;meu"
)

#  ----------------------------------------------------------------------
#  Runtime --script flag : end-to-end (c156) -- spawns the REAL
#  aaaseed_runtime with the perlin sample + a frame cap and asserts from
#  its log that the script loaded and frames rendered. Skips (loudly) on
#  a headless runner where no window can be created.
#  ----------------------------------------------------------------------
add_executable(aaaseed_runtime_script_flag_native_tests
    runtime_script_flag_test.cpp
)
add_dependencies(aaaseed_runtime_script_flag_native_tests aaaseed_runtime)
target_link_libraries(aaaseed_runtime_script_flag_native_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_definitions(aaaseed_runtime_script_flag_native_tests PRIVATE
    "AAA_RUNTIME_BIN=\"$<TARGET_FILE:aaaseed_runtime>\""
    "AAA_PERLIN_SAMPLE_LUA=\"${CMAKE_SOURCE_DIR}/bundle/macos/meu/Samples/perlin_noise/perlin_noise.lua\""
)
target_compile_features(aaaseed_runtime_script_flag_native_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_runtime_script_flag_native_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES
        LABELS "integration;meu"
        TIMEOUT 120
        #  Two runtime spawns share the GPU + window server ; don't
        #  parallelize against other window-creating tests.
        RESOURCE_LOCK "aaaseed_runtime_window"
)
