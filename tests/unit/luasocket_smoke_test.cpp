// tests/unit/luasocket_smoke_test.cpp
//
// Phase 5 fourth beachhead smoke test (continuation 62) : verify the
// engine's vendored LuaSocket 3.1.0 compiles + links + opens as a
// real Lua module on Mac. Last `aaalua_util.cpp` blocker resolved
// when this is green.
//
// Strategy : open a Lua state, push `luaopen_socket_core` as a C
// function, call it with zero args. luaopen_* returns 1 on success
// (the module table is the single return value). Inspect the table
// for the canonical LuaSocket functions that aaalua_util.cpp /
// `socket.*` Lua scripts will call (`socket.tcp`, `socket.udp`,
// `socket.select`, `socket.gettime`).
//
// Doctrine reminder : LuaSocket is the Mac-side networking beachhead.
// Windows engine uses the same source tree ; the only delta is
// wsocket.c (Win32) vs usocket.c (Unix) -- both ship in upstream.
//
// CTest label : unit;luasocket.

#include <gtest/gtest.h>

#include <cstring>
#include <string>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
    //	luasocket is compiled as plain C (continuation 62) ; its
    //	luaopen_* exports have C linkage matching the aaaseed_lua lib's
    //	bare-extern Lua API.
    int luaopen_socket_core( lua_State* L );
    int luaopen_mime_core(   lua_State* L );
}

namespace
{
    lua_State* open_state()
    {
        lua_State* L = luaL_newstate();
        if( !L ) return nullptr;
        luaL_openlibs( L );
        return L;
    }

    //	Call luaopen_<name>_core, register the returned table as a
    //	global named `<name>` so Lua scripts can `require'<name>'`
    //	via the conventional pattern. Returns 1 on success, 0 on
    //	failure (table not produced).
    int register_lua_module( lua_State* L, char const* name,
                             lua_CFunction loader )
    {
        lua_pushcfunction( L, loader );
        if( lua_pcall( L, 0, 1, 0 ) != 0 )
            return 0;
        if( !lua_istable( L, -1 ) )
        {
            lua_pop( L, 1 );
            return 0;
        }
        //	Stack : ... [module_table]. Stash as global with `name`.
        lua_setglobal( L, name );
        return 1;
    }
}

TEST( LuaSocketSmoke, LuaopenSocketCoreReturnsTable )
{
    lua_State* L = open_state();
    ASSERT_NE( L, nullptr );

    ASSERT_EQ( register_lua_module( L, "socket", &luaopen_socket_core ), 1 )
        << "luaopen_socket_core failed : " << lua_tostring( L, -1 );

    //	Globals.socket should be a table.
    lua_getglobal( L, "socket" );
    EXPECT_EQ( lua_type( L, -1 ), LUA_TTABLE );

    //	Spot-check canonical LuaSocket functions exposed by the C side
    //	(some are added by the Lua-side `socket.lua` wrapper which we
    //	don't load here ; we check the C-side ones only).
    //	`socket.gettime` is one of the most commonly grepped C
    //	bindings ; `socket.select`, `socket.tcp`, `socket.udp` are
    //	the other essentials.
    char const* const expected_fns[] = {
        "gettime", "select", "tcp", "udp", "tcp4", "udp4"
    };
    int found = 0;
    for( char const* fn : expected_fns )
    {
        lua_getfield( L, -1, fn );
        if( lua_type( L, -1 ) == LUA_TFUNCTION )
            ++found;
        lua_pop( L, 1 );
    }
    EXPECT_GE( found, 4 )
        << "Expected >= 4 of (gettime, select, tcp, udp, tcp4, udp4) ; got "
        << found;

    lua_pop( L, 1 );  // pop the socket table
    lua_close( L );
}

TEST( LuaSocketSmoke, LuaopenMimeCoreReturnsTable )
{
    lua_State* L = open_state();
    ASSERT_NE( L, nullptr );

    ASSERT_EQ( register_lua_module( L, "mime", &luaopen_mime_core ), 1 )
        << "luaopen_mime_core failed : " << lua_tostring( L, -1 );

    lua_getglobal( L, "mime" );
    EXPECT_EQ( lua_type( L, -1 ), LUA_TTABLE );

    //	mime.core provides at least these C-side functions :
    char const* const expected_fns[] = {
        "b64", "unb64", "qp", "unqp", "qpwrp", "wrp",
        "eol", "dot"
    };
    int found = 0;
    for( char const* fn : expected_fns )
    {
        lua_getfield( L, -1, fn );
        if( lua_type( L, -1 ) == LUA_TFUNCTION )
            ++found;
        lua_pop( L, 1 );
    }
    EXPECT_GE( found, 4 )
        << "Expected >= 4 mime.core functions ; got " << found;

    lua_pop( L, 1 );  // pop the mime table
    lua_close( L );
}

TEST( LuaSocketSmoke, SocketGettimeIsCallable )
{
    //	The simplest functional smoke : socket.gettime returns a number
    //	(seconds since some epoch). Just verify the call doesn't error
    //	and the result is positive.
    lua_State* L = open_state();
    ASSERT_NE( L, nullptr );
    ASSERT_EQ( register_lua_module( L, "socket", &luaopen_socket_core ), 1 );

    int const ret = luaL_dostring( L,
        "return socket.gettime()" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    ASSERT_TRUE( lua_isnumber( L, -1 ) );
    double const t = lua_tonumber( L, -1 );
    EXPECT_GT( t, 0.0 )
        << "socket.gettime() returned non-positive : " << t;

    lua_close( L );
}
