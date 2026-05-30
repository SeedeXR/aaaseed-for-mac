cmake_minimum_required(VERSION 3.27)

project(AAASeed_mac
    VERSION 0.0.1
    DESCRIPTION "AAASeed native macOS port (Apple Silicon, Metal backend)"
    LANGUAGES CXX C OBJCXX
)

if(NOT APPLE)
    message(FATAL_ERROR "aaaseed-for-mac is the macOS port. Use the Windows tree for non-Apple builds.")
endif()

if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    message(WARNING "Mac v1 targets Apple Silicon (arm64). Detected: ${CMAKE_SYSTEM_PROCESSOR}.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "macOS deployment target (Sonoma+) -- matches Homebrew Qt6 framework min-OS so the dylib warnings stay clean.")

# Phase 8 universal-binary support (c143-B, 2026-05-27, todo.md L379-L387).
# Default is Apple Silicon (matches v1 ship + the c141-A DMG). The ship
# pipeline (`scripts/ship-dmg.sh`) drives BOTH arm64 + x86_64 by passing
# -DCMAKE_OSX_ARCHITECTURES=<arch> per arch and lipo-fusing the
# resulting executables. We honour an externally-set value verbatim --
# the cache line below only sets a default when nothing was passed.
if(NOT DEFINED CMAKE_OSX_ARCHITECTURES OR CMAKE_OSX_ARCHITECTURES STREQUAL "")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Default to Apple Silicon ; ship pipeline overrides per-arch.")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(aaa_compile_flags)
include(aaa_stb)
include(aaa_lua)
include(aaa_luasocket)
include(aaa_tinyexr)
include(aaa_test_png)
# c152-C : Dear ImGui retired. The Qt6 + QML port is now the sole UI.
include(aaa_qt6)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
    EXCLUDE_FROM_ALL
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(src/code_utils)
add_subdirectory(src/aaa)
add_subdirectory(src/math)
add_subdirectory(src/infrastructure)
add_subdirectory(src/gol/metal)
add_subdirectory(src/text)
add_subdirectory(src/syphon)
# c153 (second_todo.md) : wave-2 native Mac feature sub-libs ported from the
# engine author's mac-port branch. Hermetic (std:: + Apple frameworks only,
# no engine link). Clipboard (S6), MIDI (S3), Net/HTTP+JSON (S5). The Syphon
# directory + vertical-flip (S4) extend src/syphon in place above.
add_subdirectory(src/clipboard)
add_subdirectory(src/midi)
add_subdirectory(src/net)
# c153 wave-2 tier 2 (second_todo.md S1/S2) : Audio (CoreAudio/AVAudioEngine +
# beat detector) and Video (AVFoundation -> CVMetalTextureCache zero-copy
# MTLTexture, the unified-memory showcase + movie playback + camera capture).
add_subdirectory(src/audio)
add_subdirectory(src/video)
# c153 wave-2 tier 3 (second_todo.md S7/S8) : native NSMenu context menu and
# multi-display (NSScreen geometry + borderless aux windows w/ CAMetalLayers).
add_subdirectory(src/menu)
add_subdirectory(src/display)
add_subdirectory(src/macos)
add_subdirectory(src/meu)
add_subdirectory(src/bdd)
# c147-A : Mac-native immediate-mode widget UI. v2 Phase 1 authoring
# surface. Sub-lib lives at src/ui/widgets/ ; consumed by aaaseed_app +
# the MEU runner (aaa.ui.* bindings) + tests/unit/aaa_widgets_mac_test.
add_subdirectory(src/ui/widgets)
# c148 : ImGui Studio authoring surface.
add_subdirectory(src/ui/studio)
# c152-D : src/ui/macos rebuilt as the engine RUNTIME binary
# (aaaseed_runtime) -- pure playback, no Studio. The Qt port
# (aaaseed_app_qt) is the authoring shell. Both ship in the DMG.
add_subdirectory(src/ui/macos)
add_subdirectory(src/ui/qt)

# c152-P : DMG packaging is now scripts/ship-qt-dmg.sh (macdeployqt +
# hdiutil) -- the legacy in-tree dmg.cmake produced an ImGui-era
# AAASeed.app and is retired. Codesign integration stays in-tree
# since aaaseed_app_qt is signed in-place when CODESIGN_IDENTITY is
# set ; the notarize hook is now a no-op (the DMG is produced
# out-of-tree so the CMake target it watched doesn't exist).
include(codesign)
aaaseed_codesign_target(aaaseed_app_qt)

# c152-C : tests temporarily quarantined while the Qt cutover lands.
# Many .mm tests compiled AAASeedMTKView.mm directly (now retired) ;
# many .cpp tests link aaaseed_app or aaaseed_ui_studio_mac (renamed).
# Re-introduce on the Qt side panel-by-panel. Run with -DAAA_BUILD_LEGACY_TESTS=ON
# to attempt the old tree.
option(AAA_BUILD_LEGACY_TESTS "Build the pre-Qt tree of tests." OFF)
if(AAA_BUILD_LEGACY_TESTS)
    add_subdirectory(tests/unit)
    add_subdirectory(tests/regression)
endif()
# c152-E : Qt::Test coverage for the new data layer. Builds whenever
# Qt6 is enabled (same as aaaseed_app_qt).
add_subdirectory(tests/qt)
# c153 (second_todo.md) : ALWAYS-ON test tree for the wave-2 native Mac
# subsystems (clipboard / midi / net / syphon-directory). Independent of the
# quarantined AAA_BUILD_LEGACY_TESTS tree -- ctest picks these up by default.
add_subdirectory(tests/native)
# c152-C : tests/integration retired with the .mm in-app test family --
# every test there compiled AAASeedMTKView.mm. Re-introduce on the Qt
# side once an engine viewport ships.

message(STATUS "")
message(STATUS "AAASeed_mac configuration:")
message(STATUS "  CMake             ${CMAKE_VERSION}")
message(STATUS "  Generator         ${CMAKE_GENERATOR}")
message(STATUS "  Compiler          ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  C++ standard      ${CMAKE_CXX_STANDARD}")
message(STATUS "  macOS deploy      ${CMAKE_OSX_DEPLOYMENT_TARGET}")
message(STATUS "  Architectures     ${CMAKE_OSX_ARCHITECTURES}")
message(STATUS "  Build type        ${CMAKE_BUILD_TYPE}")
message(STATUS "")
