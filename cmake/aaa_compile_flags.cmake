# aaa_compile_flags.cmake
#
# Sets up the compile environment required to compile engine source from
# `vendor/aaaseed-engine/Src/` (snapshot of Maa's upstream Windows tree) under
# AppleClang on arm64.
#
# Provides one function: aaa_apply_engine_compile_flags(<target>).
# Call it on any target that pulls in files from the vendored engine tree.

# Path to the vendored engine source tree. The repo ships a snapshot at
# vendor/aaaseed-engine so that `git clone aaaseed-for-mac && cmake --preset
# macos-arm64-debug` works without any external dependency. To work against
# an alternative tree (e.g. a fresh upstream sync), override on configure :
#     cmake --preset macos-arm64-debug -DAAASEED_ENGINE_ROOT=/path/to/other
set(AAASEED_ENGINE_ROOT "${CMAKE_SOURCE_DIR}/vendor/aaaseed-engine"
    CACHE PATH "Path to the vendored aaaseed-engine snapshot")

if(NOT EXISTS "${AAASEED_ENGINE_ROOT}/Src/aaa_build_config.h")
    message(FATAL_ERROR
        "aaa_compile_flags.cmake : cannot find aaa_build_config.h at "
        "${AAASEED_ENGINE_ROOT}/Src/. The vendored tree should ship inside "
        "the repository at vendor/aaaseed-engine/. If you have removed it, "
        "re-vendor from a known-good upstream commit. See VENDORING.md.")
endif()

function(aaa_apply_engine_compile_flags target)
    # Force-include aaa_build_config.h on every TU. Mirrors the Windows vcxproj
    # <ForcedIncludeFiles> directive. On Clang the equivalent is -include.
    target_compile_options(${target} PRIVATE
        -include "${AAASEED_ENGINE_ROOT}/Src/aaa_build_config.h"
    )

    # Mirror the Windows <IncludePath> set so #include "aaa_type.h" etc resolve.
    target_include_directories(${target} PRIVATE
        "${AAASEED_ENGINE_ROOT}/Src"
        "${AAASEED_ENGINE_ROOT}/Src/code_utils"
        "${AAASEED_ENGINE_ROOT}/Src/system"
        "${AAASEED_ENGINE_ROOT}/Src/math"
        "${AAASEED_ENGINE_ROOT}/Include"
    )

    # Defines used by the engine code, with Mac-appropriate values.
    target_compile_definitions(${target} PRIVATE
        NDEBUG=$<NOT:$<CONFIG:Debug>>   # match Windows convention
    )

    # Function-style macro overrides for engine features that don't apply on
    # macOS / Apple Silicon. Sent via raw -D in target_compile_options because
    # target_compile_definitions silently drops function-style preprocessor
    # defs ("many compilers do not support it" — CMake docs). Clang supports
    # them ; the corresponding upstream headers have #ifndef guards added on
    # 2026-05-25 so these overrides actually take effect.
    #   AAA_USE_XNA()=0    — DirectX XNA Math (v.h, v_xna.h). x86-Windows only.
    #   AAA_NSIGHT_USE()=0 — NVIDIA NSight profiling (draw/NSightEvents.h).
    target_compile_options(${target} PRIVATE
        "-DAAA_USE_XNA()=0"
        "-DAAA_NSIGHT_USE()=0"
    )

    # Warnings: keep enabled but allow the most common upstream-noise warnings
    # to fail-soft for now. Each gets revisited per-subsystem as we port.
    target_compile_options(${target} PRIVATE
        -Wall
        -Wno-unused-parameter
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-unused-private-field
        -Wno-deprecated-declarations
        -Wno-unknown-pragmas             # ignore #pragma warning(disable:...) from MSVC headers
        -Wno-pragma-pack                 # tolerate /Zp16-style pack directives in headers
        -Wno-undefined-inline            # inline forwarders in v_xna.h reference inline _v3r in v.cpp; linker resolves fine
    )

    # ASCII-only source policy is a runtime-output convention, not a compile
    # flag, so nothing to add here. See memory/philosophy.md sect 1.5.
endfunction()

# Convenience : expose the per-source path so subsystem CMakeLists files can
# resolve sources relative to it without re-finding it. `include()` runs in
# the caller's scope, so plain set() is enough to make AAASEED_ENGINE_SRC
# visible in the root CMakeLists.txt and all add_subdirectory children below.
set(AAASEED_ENGINE_SRC "${AAASEED_ENGINE_ROOT}/Src")
