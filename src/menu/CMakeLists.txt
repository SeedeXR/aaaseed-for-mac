# src/menu/CMakeLists.txt
#
# second_todo.md S7 (c153) : native macOS context-menu (NSMenu) sub-lib.
# Hermetic : pure ObjC++, links AppKit only, no engine link / no o_str /
# no aaa_mem. -fno-objc-arc. Symbol shape : aaa::menu::* (menu_mac.h).

find_library(APPKIT_FRAMEWORK_MENU AppKit REQUIRED)

add_library(aaaseed_menu STATIC
    menu_mac.mm
)

set_source_files_properties(menu_mac.mm
    PROPERTIES LANGUAGE OBJCXX COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_menu PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_menu PUBLIC
    "${APPKIT_FRAMEWORK_MENU}"
)

target_compile_features(aaaseed_menu PUBLIC cxx_std_20)
set_target_properties(aaaseed_menu PROPERTIES POSITION_INDEPENDENT_CODE ON)
