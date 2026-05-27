// tests/unit/meu_compat_test.cpp
//
// Phase 6 partial unblock smoke test for src/lua/meu_compat.lua.
//
// Opens a Lua 5.1 state, injects a stub `aaa.platform_name()` (since
// the full engine boot is not wired into the test harness yet), loads
// the shim from disk via `dofile`, calls `meu_compat.detect()`, and
// verifies it returns a non-empty string token. Also exercises the
// spout <-> syphon bridge install path on the simulated Mac case to
// catch regressions in the table-forwarding logic.
//
// The shim path is plumbed in at CMake-configure time via the
// AAA_MEU_COMPAT_LUA define so the test stays hermetic on out-of-tree
// builds.
//
// CTest label : unit;meu_compat.

#include <gtest/gtest.h>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include <cstring>
#include <string>

namespace
{
    //	Stub : pushes "mac" so `meu_compat.detect()` exercises the Mac
    //	branch. The shim accepts any of mac/macos/darwin/osx and folds
    //	them to "mac".
    int l_platform_name_mac( lua_State* L )
    {
        lua_pushstring( L, "mac" );
        return 1;
    }

    //	Helper : push an `aaa` table with `platform_name = <fn>` and an
    //	empty `syphon.send` stub so the install path can find a target.
    void install_aaa_table( lua_State* L, lua_CFunction platform_fn )
    {
        lua_newtable( L );                          //  aaa = {}

        lua_pushcfunction( L, platform_fn );
        lua_setfield( L, -2, "platform_name" );     //  aaa.platform_name = fn

        //	Provide a syphon.send no-op so install() has something to
        //	bridge from on the simulated Mac platform.
        lua_newtable( L );                          //  syphon = {}
        lua_pushcfunction( L, []( lua_State* )->int { return 0; } );
        lua_setfield( L, -2, "send" );              //  syphon.send = fn
        lua_setfield( L, -2, "syphon" );            //  aaa.syphon = syphon

        lua_setglobal( L, "aaa" );
    }
}

TEST( MeuCompat, DetectReturnsNonEmptyStringOnMac )
{
    lua_State* L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    install_aaa_table( L, l_platform_name_mac );

    //	Load + execute the shim. dofile returns the module table on the
    //	stack ; store it as a global so the test scripts can reach it.
    int const ret = luaL_dofile( L, AAA_MEU_COMPAT_LUA );
    ASSERT_EQ( ret, 0 ) << "dofile failed : " << lua_tostring( L, -1 );
    lua_setglobal( L, "meu_compat" );

    int const ret2 = luaL_dostring( L, "return meu_compat.detect()" );
    ASSERT_EQ( ret2, 0 ) << lua_tostring( L, -1 );

    ASSERT_EQ( lua_isstring( L, -1 ), 1 ) << "detect() did not return a string";
    char const* s = lua_tostring( L, -1 );
    ASSERT_NE( s, nullptr );
    EXPECT_GT( std::strlen( s ), size_t( 0 ) ) << "detect() returned an empty string";
    EXPECT_STREQ( s, "mac" );

    lua_pop( L, 1 );
    lua_close( L );
}

TEST( MeuCompat, InstallBridgesSpoutToSyphonOnMac )
{
    lua_State* L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    install_aaa_table( L, l_platform_name_mac );

    int const ret = luaL_dofile( L, AAA_MEU_COMPAT_LUA );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
    lua_setglobal( L, "meu_compat" );

    //	After install() on Mac, `aaa.spout.send` should be a callable
    //	function forwarding into `aaa.syphon.send`.
    int const ret2 = luaL_dostring( L,
        "meu_compat.install() ; "
        "return type(aaa.spout) == 'table' and type(aaa.spout.send) == 'function'" );
    ASSERT_EQ( ret2, 0 ) << lua_tostring( L, -1 );

    ASSERT_EQ( lua_isboolean( L, -1 ), 1 );
    EXPECT_EQ( lua_toboolean( L, -1 ), 1 ) << "spout bridge not installed";

    lua_pop( L, 1 );
    lua_close( L );
}
