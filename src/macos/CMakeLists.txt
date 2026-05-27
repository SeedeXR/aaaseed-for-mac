# src/macos/CMakeLists.txt
#
# Phase 8 (todo.md L365-L383, c136-B, 2026-05-27) :
# aaaseed_bundle_meta -- hermetic Mac sub-lib exposing .app
# bundle Info.plist metadata as a pure C++ API.
#
# Hermetic doctrine (c104 / feedback_hermetic_mac_sublibs.md) :
#   - Foundation only.
#   - std::string + std::optional in the header.
#   - No `o_str`, no `aaa_mem.h`, no `aaa_str.h`.
#   - No link to `aaaseed_code_utils`.
#
# Consumers : aaaseed_app (runtime "About" panel, crash-report headers),
# tests/unit/macos_bundle_meta_test.cpp.

find_library(FOUNDATION_FRAMEWORK_BUNDLE_META Foundation REQUIRED)

add_library(aaaseed_bundle_meta STATIC
    aaa_bundle_meta.mm
)
set_source_files_properties(aaa_bundle_meta.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_include_directories(aaaseed_bundle_meta PUBLIC
    "${CMAKE_SOURCE_DIR}"
)
target_link_libraries(aaaseed_bundle_meta PUBLIC
    "${FOUNDATION_FRAMEWORK_BUNDLE_META}"
)
target_compile_features(aaaseed_bundle_meta PUBLIC cxx_std_20)
