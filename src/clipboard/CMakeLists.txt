# src/clipboard/CMakeLists.txt
#
# second_todo.md S6 (c153) : native macOS clipboard copy/paste sub-lib.
# Mirrors the src/syphon hermetic pattern : pure ObjC++, links AppKit only,
# no engine link / no o_str / no aaa_mem cascade. Manual reference counting
# (-fno-objc-arc) to stay consistent with the other Mac sub-libs.

find_library(APPKIT_FRAMEWORK_CLIPBOARD AppKit REQUIRED)

add_library(aaaseed_clipboard STATIC
    clipboard_mac.mm
)

set_source_files_properties(clipboard_mac.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_clipboard PUBLIC
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_clipboard PUBLIC
    "${APPKIT_FRAMEWORK_CLIPBOARD}"
)

target_compile_features(aaaseed_clipboard PUBLIC cxx_std_20)

set_target_properties(aaaseed_clipboard PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
