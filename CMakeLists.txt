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

set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "macOS deployment target (Ventura+)")

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
add_subdirectory(src/macos)
add_subdirectory(src/meu)
add_subdirectory(src/bdd)
# c147-A : Mac-native immediate-mode widget UI. v2 Phase 1 authoring
# surface. Sub-lib lives at src/ui/widgets/ ; consumed by aaaseed_app +
# the MEU runner (aaa.ui.* bindings) + tests/unit/aaa_widgets_mac_test.
add_subdirectory(src/ui/widgets)
add_subdirectory(src/ui/macos)

# Phase 8 DMG packaging target (c137-B, 2026-05-27, todo.md L379-L380).
# Defines `aaaseed_dmg` custom_target -- stages AAASeed.app + Applications
# symlink + .background placeholder into ${CMAKE_BINARY_DIR}/dmg_staging/
# and produces AAASeed-${PROJECT_VERSION}.dmg via hdiutil (unsigned).
# Must come AFTER add_subdirectory(src/ui/macos) so the aaaseed_app target
# is already defined when dmg.cmake captures it as a dependency.
include(dmg)

# Phase 8 env-gated codesign + notarize wiring (c138-B, 2026-05-27,
# todo.md L385-L387). Defines aaaseed_codesign_target() +
# aaaseed_notarize_dmg() functions ; both are NO-OPs unless the
# operator has exported CODESIGN_IDENTITY / NOTARY_API_KEY_* env
# vars. Must come AFTER include(dmg) so `aaaseed_dmg` exists when
# aaaseed_notarize_dmg() captures it as a dependency.
include(codesign)
aaaseed_codesign_target(aaaseed_app)
aaaseed_notarize_dmg()

add_subdirectory(tests/unit)
add_subdirectory(tests/regression)
#  c144 : integration test family. First member is the MEU-runner-in-app
#  wiring test. Lives in `tests/integration/CMakeLists.txt` -- mirrors
#  the unit + regression sibling pattern.
add_subdirectory(tests/integration)

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
