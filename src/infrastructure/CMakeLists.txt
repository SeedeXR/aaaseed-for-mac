# src/infrastructure/CMakeLists.txt
#
# Stage 2 of the obj_ui port : compile the bottom of the dependency stack
# (factory + registry + param + params + param_data + param_def + namer +
# obj). These are all the .cpp files that obj_ui.cpp transitively needs to
# link, MINUS animation (trax, connex, traxs), drawing (param_draw),
# UI focus (param_focus), and Lua bindings (param_lua, obj_lua).
#
# Stage 3 lands obj_ui.cpp itself once this lib links clean.

add_library(aaaseed_infrastructure STATIC
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/factory/factory.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/factory/registry.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/param/param.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/param/params.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/param/param_data.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/param/param_def.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/namer.cpp"
)

aaa_apply_engine_compile_flags(aaaseed_infrastructure)

set_target_properties(aaaseed_infrastructure PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)


# c116-B (2026-05-26) -- Mac-native port of the Win-only
# vendor/aaaseed-engine/Src/infrastructure/compute_parallel.h. That vendor
# header unconditionally pulls TBB or PPL ; neither exists on macOS.
# This sub-lib owns the matching `aaa::parallel::get_thread_nb_def` and
# `aaa::parallel::parallel_for` symbols via Grand Central Dispatch
# (libdispatch / dispatch_apply), the Apple-native equivalent.
#
# Kept as a SEPARATE static lib (rather than folded into aaaseed_infrastructure
# above) per the hermetic-Mac-sub-lib doctrine : compute_parallel_mac.cpp
# uses ONLY std::thread + std::atomic + dispatch_apply, no o_str / aaa_mem /
# c_cpu / spy dependency, so consumers that link it don't get pulled into
# the wider blocked memcpy / aaa_mem cascade (c116-A's domain).
add_library(aaaseed_infrastructure_mac STATIC
    compute_parallel_mac.cpp
)

aaa_apply_engine_compile_flags(aaaseed_infrastructure_mac)

target_include_directories(aaaseed_infrastructure_mac PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(aaaseed_infrastructure_mac PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
