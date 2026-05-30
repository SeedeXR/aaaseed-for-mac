# src/display/CMakeLists.txt
#
# second_todo.md S8 (c153) : native macOS multi-display sub-lib. NSScreen
# enumeration + borderless aux NSWindows backed by CAMetalLayers. The pure
# geometry core is header-only (display_layout.h). Hermetic : pure ObjC++,
# links AppKit + QuartzCore + Metal, no engine link / no o_str / no aaa_mem.
# -fno-objc-arc. Symbol shape : aaa::display::* (display_mac.h,
# display_layout.h).

find_library(APPKIT_FRAMEWORK_DISPLAY     AppKit     REQUIRED)
find_library(QUARTZCORE_FRAMEWORK_DISPLAY QuartzCore REQUIRED)
find_library(METAL_FRAMEWORK_DISPLAY      Metal      REQUIRED)

add_library(aaaseed_display STATIC
    display_mac.mm
    #  c153 S8 : per-aux-display sub-rect present primitive (the blit core the
    #  engine present loop calls once per aux CAMetalLayer each frame).
    display_present_mac.mm
)

set_source_files_properties(display_mac.mm display_present_mac.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_display PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_display PUBLIC
    "${APPKIT_FRAMEWORK_DISPLAY}"
    "${QUARTZCORE_FRAMEWORK_DISPLAY}"
    "${METAL_FRAMEWORK_DISPLAY}"
)

target_compile_features(aaaseed_display PUBLIC cxx_std_20)
set_target_properties(aaaseed_display PROPERTIES POSITION_INDEPENDENT_CODE ON)
