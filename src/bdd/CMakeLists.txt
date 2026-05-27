# src/bdd/CMakeLists.txt
#
# c145 (Task #30 honest closure) : Mac-native port of the PURE-C++
# portion of vendor `Src/obj_ui/bdd/bdd_cell_draw.cpp` + abstract
# renderer interface for the v2 Metal-side BDD subsystem.
#
# Hermetic Mac sub-lib per feedback_hermetic_mac_sublibs.md :
#   - Pure C++. No Metal touch (the abstract Renderer header has no
#     impl). No Objective-C in the public surface.
#   - std::string / std::vector / <cmath> only. NO o_str, NO
#     aaa_mem.h, NO aaa_str.h, NO aaa_type.h.
#   - NO link to aaaseed_code_utils. NO vendor edits.
#
# Renderer Metal-side impl lives in `src/bdd/metal/` (NOT created in
# this commit -- that is v2 work, est. 3-4 weeks).

add_library(aaaseed_bdd STATIC
    aaa_bdd_cell_draw_mac.cpp
)

target_include_directories(aaaseed_bdd PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_features(aaaseed_bdd PUBLIC cxx_std_20)

set_target_properties(aaaseed_bdd PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
