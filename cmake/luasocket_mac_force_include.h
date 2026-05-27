/*
 * cmake/luasocket_mac_force_include.h
 *
 * Mac-only force-include for the aaaseed_luasocket static lib.
 * Continuation 62.
 *
 * Problem we solve : luasocket's compat.h declares
 *     void luasocket_setfuncs( lua_State*, const luaL_Reg*, int );
 *     void *luasocket_testudata( lua_State*, int, const char* );
 * and gates the `#define luaL_setfuncs luasocket_setfuncs` substitution
 * on `LUA_VERSION_NUM==501`. compat.h does NOT include lua.h itself,
 * so it cannot be force-included before the .c files' own lua.h
 * include.
 *
 * Solution : this wrapper pulls lua.h + lauxlib.h FIRST, sets
 * LUA_VERSION_NUM (in case it isn't already), THEN includes compat.h.
 * Force-including THIS header instead of compat.h directly puts
 * everything in the right order.
 *
 * Behaviour-preserving on Windows : never compiled on that platform
 * (the engine uses LuaJIT which provides luaL_setfuncs / luaL_testudata
 * natively).
 */
#ifndef AAA_LUASOCKET_MAC_FORCE_INCLUDE_H
#define AAA_LUASOCKET_MAC_FORCE_INCLUDE_H

#ifndef LUA_VERSION_NUM
#define LUA_VERSION_NUM 501
#endif

#include "lua.h"
#include "lauxlib.h"

#include "compat.h"

#endif
