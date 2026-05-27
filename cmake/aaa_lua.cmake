# cmake/aaa_lua.cmake
#
# Static lib wrapping vendored Lua 5.1.5. Phase 5 second beachhead
# (continuation 45). The Mac port pins to Lua 5.1 to match the engine's
# Windows build -- per philosophy.md / project_context.md the version
# is NOT bumped. AAASeed's bindings (`*_lua.cpp` files in the engine
# tree) assume the 5.1 C API surface.
#
# Build : every .c in third_party/lua-5.1/ EXCEPT `lua.c` (interactive
# CLI entry point) and `luac.c` (bytecode compiler CLI entry point).
# Those are end-user programs, not the engine library.
#
# MIT-license-compatible (Lua is MIT). COPYRIGHT file vendored alongside.
#
# Convention :
#   target_link_libraries(<consumer> PRIVATE aaaseed_lua)
#   then `#include "lua.h"` etc. (header search path public on the lib).

set(AAA_LUA_DIR "${CMAKE_SOURCE_DIR}/third_party/lua-5.1")

#  Enumerate the library sources. Order doesn't matter for the link ;
#  listing each file explicitly avoids accidentally picking up future
#  files in the directory (e.g. test code) that don't belong.
set(AAA_LUA_SOURCES
    "${AAA_LUA_DIR}/lapi.c"
    "${AAA_LUA_DIR}/lauxlib.c"
    "${AAA_LUA_DIR}/lbaselib.c"
    "${AAA_LUA_DIR}/lcode.c"
    "${AAA_LUA_DIR}/ldblib.c"
    "${AAA_LUA_DIR}/ldebug.c"
    "${AAA_LUA_DIR}/ldo.c"
    "${AAA_LUA_DIR}/ldump.c"
    "${AAA_LUA_DIR}/lfunc.c"
    "${AAA_LUA_DIR}/lgc.c"
    "${AAA_LUA_DIR}/linit.c"
    "${AAA_LUA_DIR}/liolib.c"
    "${AAA_LUA_DIR}/llex.c"
    "${AAA_LUA_DIR}/lmathlib.c"
    "${AAA_LUA_DIR}/lmem.c"
    "${AAA_LUA_DIR}/loadlib.c"
    "${AAA_LUA_DIR}/lobject.c"
    "${AAA_LUA_DIR}/lopcodes.c"
    "${AAA_LUA_DIR}/loslib.c"
    "${AAA_LUA_DIR}/lparser.c"
    "${AAA_LUA_DIR}/lstate.c"
    "${AAA_LUA_DIR}/lstring.c"
    "${AAA_LUA_DIR}/lstrlib.c"
    "${AAA_LUA_DIR}/ltable.c"
    "${AAA_LUA_DIR}/ltablib.c"
    "${AAA_LUA_DIR}/ltm.c"
    "${AAA_LUA_DIR}/lundump.c"
    "${AAA_LUA_DIR}/lvm.c"
    "${AAA_LUA_DIR}/lzio.c"
    "${AAA_LUA_DIR}/print.c"
    # Note: lua.c (interactive CLI) and luac.c (bytecode compiler CLI)
    # intentionally excluded -- those are application binaries, not
    # part of the library.
)

add_library(aaaseed_lua STATIC ${AAA_LUA_SOURCES})

target_include_directories(aaaseed_lua PUBLIC
    "${AAA_LUA_DIR}"
)

#  Apple-specific Lua build flags. LUA_USE_MACOSX enables dlopen-based
#  package.loadlib + readline support flags (we don't ship the readline
#  features in the static lib but the define still gates the right code
#  paths in loadlib.c / loslib.c).
target_compile_definitions(aaaseed_lua PRIVATE
    LUA_USE_MACOSX
)

#  Lua 5.1 source emits a handful of -Wdeprecated-declarations / sign
#  warnings on modern AppleClang. Silence them ; we don't own this code.
target_compile_options(aaaseed_lua PRIVATE
    -Wno-deprecated-declarations
    -Wno-empty-body
    -Wno-string-plus-int
)

set_target_properties(aaaseed_lua PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
