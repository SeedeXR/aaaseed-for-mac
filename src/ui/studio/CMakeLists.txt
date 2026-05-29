# src/ui/studio/CMakeLists.txt
#
# c152-C : after ImGui retirement, this static lib carries ONLY the
# pure-C++ AAASeed Studio data model. The Qt6 UI (aaaseed_app_qt)
# drives it through the public Studio:: API.
#
# Produces : aaaseed_ui_studio (static lib, platform-neutral)

add_library(aaaseed_ui_studio STATIC
    aaa_studio.cpp           # platform-neutral data model
)

# aaa_studio.cpp is pure C++ ; references lua.h for the project-file
# parser. Linked PUBLIC so consumers (Qt app, tests) get the same lib.
set_source_files_properties(aaa_studio.cpp
    PROPERTIES
    LANGUAGE CXX
)

target_include_directories(aaaseed_ui_studio PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_ui_studio PUBLIC
    aaaseed_lua   # for the project-file .lua parser
)

target_compile_features(aaaseed_ui_studio PUBLIC cxx_std_20)

set_target_properties(aaaseed_ui_studio PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
