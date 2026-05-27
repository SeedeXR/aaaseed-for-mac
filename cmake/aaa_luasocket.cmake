# cmake/aaa_luasocket.cmake
#
# Static lib wrapping the engine's vendored LuaSocket 3.1.0 sources
# (vendor/aaaseed-engine/Src/language/lua/lua_socket/). Continuation 62
# -- last `aaalua_util.cpp` blocker.
#
# 13 .c files compile on Mac (12 portable + usocket.c for Unix
# sockets) ; wsocket.c is the Win32 variant, skipped. The luasocket
# headers use `extern "C" { #include "lua/lua.h" }` (no __cplusplus
# guard) so the .c files compile as C++ -- same convention the engine
# uses on Windows (see CLAUDE.md "`.c` files compile as C++").
#
# Convention :
#   target_link_libraries(<consumer> PRIVATE aaaseed_luasocket)
#   then `lua_pushcfunction(L, luaopen_socket_core);` etc.
#
# LuaSocket is MIT-licensed (Diego Nehab) ; engine vendor tree carries
# the COPYRIGHT.

set(AAA_LUASOCKET_DIR
    "${AAASEED_ENGINE_SRC}/language/lua/lua_socket")

#  Mac-relevant sources. Order doesn't matter for the link, but listing
#  each file explicitly avoids accidentally pulling future additions to
#  the directory we haven't validated.
set(AAA_LUASOCKET_SOURCES
    "${AAA_LUASOCKET_DIR}/luasocket.c"
    "${AAA_LUASOCKET_DIR}/auxiliar.c"
    "${AAA_LUASOCKET_DIR}/buffer.c"
    "${AAA_LUASOCKET_DIR}/compat.c"
    "${AAA_LUASOCKET_DIR}/except.c"
    "${AAA_LUASOCKET_DIR}/inet.c"
    "${AAA_LUASOCKET_DIR}/io.c"
    "${AAA_LUASOCKET_DIR}/mime.c"
    "${AAA_LUASOCKET_DIR}/options.c"
    "${AAA_LUASOCKET_DIR}/select.c"
    "${AAA_LUASOCKET_DIR}/tcp.c"
    "${AAA_LUASOCKET_DIR}/timeout.c"
    "${AAA_LUASOCKET_DIR}/udp.c"
    #  Mac substitute for wsocket.c (Win32). The engine's authoritative
    #  sources.cmake ships wsocket.c on Windows ; we swap in usocket.c.
    "${AAA_LUASOCKET_DIR}/usocket.c"
    #  Excluded :
    #    wsocket.c     -- Win32-only.
    #    unix.c / unixstream.c / unixdgram.c / serial.c / context.c
    #                  -- Unix-domain sockets + serial port extras ;
    #                     engine doesn't reference these on Windows
    #                     and aaalua_util.cpp only calls luaopen_socket_core
    #                     and luaopen_mime_core. Skip until a real
    #                     consumer surfaces.
)

#  Compile as plain C. Our vendored Lua 5.1.5 declares its API with
#  bare `extern` (no `extern "C"` wrap in lua.h), so compiling the .c
#  files as C++ would emit C++-mangled symbols that the C-compiled
#  aaaseed_lua lib can't resolve. luasocket.h's `extern "C"` block is
#  gated with `#ifdef __cplusplus` (one-line upstream cleanup,
#  continuation 62) so it stays valid for C++ consumers (like
#  aaalua_util.cpp) while compiling as plain C here.

add_library(aaaseed_luasocket STATIC ${AAA_LUASOCKET_SOURCES})

#  Headers expected by the .c files :
#    "lua/lua.h"   -> vendor/aaaseed-engine/Include/lua/lua.h
#    "auxiliar.h"  -> sibling in lua_socket/
target_include_directories(aaaseed_luasocket PUBLIC
    "${AAA_LUASOCKET_DIR}"
    "${AAASEED_ENGINE_ROOT}/Include"
)

#  Link against our vendored Lua. Public so consumers pick up the C API.
target_link_libraries(aaaseed_luasocket PUBLIC
    aaaseed_lua
)

#  Force-include the Mac shim wrapper for every TU. The shim pulls
#  lua.h + lauxlib.h FIRST then includes luasocket's compat.h, which
#  injects `luaL_setfuncs` -> `luasocket_setfuncs` / `luaL_testudata`
#  -> `luasocket_testudata` macros (gated on LUA_VERSION_NUM==501,
#  which we set first). On Windows the engine links LuaJIT which
#  natively provides those symbols, so this shim is Mac-only.
target_compile_options(aaaseed_luasocket PRIVATE
    -include "${CMAKE_SOURCE_DIR}/cmake/luasocket_mac_force_include.h"
)

#  Win32 macro neutralization : `FAR` is a Win16 holdover that lingers
#  in upstream as `(const char FAR *)&yes` ; on Mac it must expand to
#  nothing. The engine's Windows build relies on <windows.h> providing
#  the empty macro ; on Mac we provide it manually here. Behaviour-
#  preserving on Windows -- we don't add this define on that platform.
target_compile_definitions(aaaseed_luasocket PRIVATE
    FAR=
)

#  Silence upstream warnings we don't own. LuaSocket targets old C
#  conventions ; AppleClang lights up a handful of pedantic flags.
target_compile_options(aaaseed_luasocket PRIVATE
    -Wno-deprecated-declarations
    -Wno-implicit-function-declaration
    -Wno-incompatible-pointer-types
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-sign-compare
    -Wno-shorten-64-to-32
    -Wno-strict-prototypes
)

set_target_properties(aaaseed_luasocket PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
