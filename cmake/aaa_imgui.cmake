# cmake/aaa_imgui.cmake
#
# Vendors Dear ImGui v1.91.x via FetchContent and builds a hermetic
# static lib `aaaseed_imgui` with:
#   - Core imgui (no renderer / platform backend)
#   - imgui_demo (for the developer demo window, stripped in Release)
#   - imgui_stdlib (std::string helpers)
#
# The Metal + Cocoa backends (backends/imgui_impl_metal.mm +
# backends/imgui_impl_osx.mm) are compiled into a SEPARATE target
# `aaaseed_imgui_metal_backend` so the core lib stays renderer-agnostic
# and the hermetic Mac sub-libs doctrine is satisfied.
#
# Consumers:
#   target_link_libraries(my_target PRIVATE aaaseed_imgui_metal_backend)
# pulls in both the backend and the core automatically via transitive deps.
#
# Hermetic Mac sub-lib rules (feedback_hermetic_mac_sublibs.md):
#   - No o_str / aaa_mem / aaa_str / aaaseed_code_utils link.
#   - std::string / std::vector / std::unordered_map / <cstdint> only from
#     AAASeed side.
#   - ImGui itself uses its own allocator; fine.

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.6
    GIT_SHALLOW    TRUE
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui)

# ── Core imgui static lib ──────────────────────────────────────────────────

add_library(aaaseed_imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)

target_include_directories(aaaseed_imgui PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/misc/cpp"
    "${imgui_SOURCE_DIR}/backends"
)

target_compile_features(aaaseed_imgui PUBLIC cxx_std_20)

set_target_properties(aaaseed_imgui PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# Suppress upstream warnings -- we don't own this code.
target_compile_options(aaaseed_imgui PRIVATE
    -Wno-all -Wno-extra -Wno-unused-parameter
)

# ── Metal + Cocoa backend static lib ──────────────────────────────────────

find_library(METAL_FRAMEWORK_IMGUI       Metal           REQUIRED)
find_library(METALKIT_FRAMEWORK_IMGUI    MetalKit        REQUIRED)
find_library(QUARTZCORE_IMGUI            QuartzCore      REQUIRED)
find_library(APPKIT_IMGUI                AppKit          REQUIRED)
# c151-A : imgui_impl_osx.mm references GCController for gamepad input
# (NSGameController integration since ImGui 1.84).
find_library(GAMECONTROLLER_IMGUI        GameController  REQUIRED)

add_library(aaaseed_imgui_metal_backend STATIC
    "${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_osx.mm"
)

set_source_files_properties(
    "${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_osx.mm"
    PROPERTIES
    LANGUAGE OBJCXX
    # c151-A : the upstream backend (see comment at imgui_impl_metal.mm
    # line 20) is written for ARC -- @property strong, __bridge casts,
    # autoreleased return values. Compiling with -fno-objc-arc broke
    # MetalContext.device's retain semantics so newLibraryWithSource ran
    # against a dangling device pointer and aborted the app smoke test.
    # ARC is local to these two backend files ; metal-cpp's manual
    # retain/release on OUR side of the bridge is unaffected.
    COMPILE_FLAGS "-fobjc-arc -Wno-all"
)

target_include_directories(aaaseed_imgui_metal_backend PUBLIC
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
)

target_link_libraries(aaaseed_imgui_metal_backend PUBLIC
    aaaseed_imgui
    "${METAL_FRAMEWORK_IMGUI}"
    "${METALKIT_FRAMEWORK_IMGUI}"
    "${QUARTZCORE_IMGUI}"
    "${APPKIT_IMGUI}"
    "${GAMECONTROLLER_IMGUI}"
)

target_compile_features(aaaseed_imgui_metal_backend PUBLIC cxx_std_20)

set_target_properties(aaaseed_imgui_metal_backend PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
