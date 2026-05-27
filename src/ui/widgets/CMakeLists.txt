# src/ui/widgets/CMakeLists.txt
#
# c147-A : Mac-native immediate-mode widget UI. v2 Phase 1 of the
# authoring surface (closes c146 ship-gate "Authoring Surface DEFERRED
# v2" item per c145-C Option A).
#
# Hermetic Mac sub-lib per feedback_hermetic_mac_sublibs.md :
#   - Pure ObjC++. No o_str / aaa_str.h / aaa_mem.h.
#   - Links aaaseed_gol_metal + Foundation only.

find_library(FOUNDATION_FRAMEWORK_WIDGETS Foundation REQUIRED)

add_library(aaaseed_ui_widgets_mac STATIC
    aaa_widgets_mac.mm
)

set_source_files_properties(aaa_widgets_mac.mm
    PROPERTIES
    LANGUAGE OBJCXX
    #  Match the rest of the Mac sub-libs (manual reference counting,
    #  pairs with metal-cpp's manual ARC pattern).
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_ui_widgets_mac PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_ui_widgets_mac PUBLIC
    "${FOUNDATION_FRAMEWORK_WIDGETS}"
    aaaseed_gol_metal
)

target_compile_features(aaaseed_ui_widgets_mac PUBLIC cxx_std_20)

set_target_properties(aaaseed_ui_widgets_mac PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
