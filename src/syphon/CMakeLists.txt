# src/syphon/CMakeLists.txt
#
# Phase 6 beachhead (c122) : Mac-side Syphon C-binding static lib. Mirrors
# the src/aaa/CMakeLists.txt hermetic pattern (c104). Pure ObjC++ ; links
# Foundation + Metal + IOSurface frameworks only. No engine link, no
# vendor code, no o_str/aaa_mem cascade.
#
# Symbol shape for callers : aaa::syphon::ServerMac (header syphon_mac.h).
# The full Syphon protocol port (Mach bootstrap, CFMessagePort, JSON
# server info dict) is c123+ work ; this lib only freezes the API
# signature so the Lua-side dispatch (meu_compat.lua c115) has something
# to call into.

find_library(FOUNDATION_FRAMEWORK_SYPHON     Foundation     REQUIRED)
find_library(COREFOUNDATION_FRAMEWORK_SYPHON CoreFoundation REQUIRED)
find_library(METAL_FRAMEWORK_SYPHON          Metal          REQUIRED)
find_library(IOSURFACE_FRAMEWORK_SYPHON      IOSurface      REQUIRED)
#  c123-A : CFMessagePort + Distributed Notifications + xpc_object are
#  all in libSystem / CoreFoundation, no extra framework needed beyond
#  CoreFoundation. IOSurfaceCreateXPCObject lives in IOSurface itself.

add_library(aaaseed_syphon STATIC
    syphon_mac.mm
    syphon_lua_mac.mm
    #  c124-C : Phase 6 reciprocity LAST HOP -- Mac-side Syphon CLIENT.
    #  Pairs with ServerMac's c123-A DN publishing. Registers a DN
    #  observer on the same notification name, looks up the published
    #  IOSurfaceID, and hands the recovered IOSurfaceRef (or wrapped
    #  MTLTexture) to the consumer.
    syphon_client_mac.mm
)

set_source_files_properties(syphon_mac.mm syphon_lua_mac.mm syphon_client_mac.mm
    PROPERTIES
    LANGUAGE OBJCXX
    #  Manual reference counting matches the rest of the Mac sub-libs
    #  (event_bridge, ui/macos) ; don't mix ARC with metal-cpp objects
    #  in the same TU.
    COMPILE_FLAGS "-fno-objc-arc"
)

target_include_directories(aaaseed_syphon PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(aaaseed_syphon PUBLIC
    "${FOUNDATION_FRAMEWORK_SYPHON}"
    "${COREFOUNDATION_FRAMEWORK_SYPHON}"
    "${METAL_FRAMEWORK_SYPHON}"
    "${IOSURFACE_FRAMEWORK_SYPHON}"
    #  c124 : the new syphon_lua_mac.mm references lua.h / lauxlib.h.
    #  Link the hermetic Lua static lib publicly so consumers don't have
    #  to re-add it themselves.
    aaaseed_lua
)

target_compile_features(aaaseed_syphon PUBLIC cxx_std_20)

set_target_properties(aaaseed_syphon PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
