# src/net/CMakeLists.txt
#
# second_todo.md S5 (c153) : native macOS HTTP + JSON sub-lib. NSURLSession
# async client + NSJSONSerialization -> Lua table bridge, replacing the
# CPR/RapidJSON path the Windows engine uses. Mirrors the src/syphon hermetic
# pattern : pure ObjC++, links Foundation + CoreFoundation, plus the hermetic
# Lua static lib (the JSON->table binding references lua.h). No engine link,
# no o_str, no aaa_mem. Manual reference counting (-fno-objc-arc).
#
# Symbol shape for callers : aaa::net::* (headers net_mac.h, net_json_lua.h).

find_library(FOUNDATION_FRAMEWORK_NET     Foundation     REQUIRED)
find_library(COREFOUNDATION_FRAMEWORK_NET CoreFoundation REQUIRED)

add_library(aaaseed_net STATIC
    net_mac.mm
    net_json_lua.mm
)

set_source_files_properties(net_mac.mm net_json_lua.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_net PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_net PUBLIC
    "${FOUNDATION_FRAMEWORK_NET}"
    "${COREFOUNDATION_FRAMEWORK_NET}"
    #  net_json_lua.mm references lua.h / lauxlib.h. Link the hermetic Lua
    #  static lib publicly so consumers inherit the include path + symbols.
    aaaseed_lua
)

target_compile_features(aaaseed_net PUBLIC cxx_std_20)

set_target_properties(aaaseed_net PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
