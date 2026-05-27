// tests/unit/lua_smoke_test.cpp
//
// Phase 5 Lua-vendoring smoke test (continuation 45). Verifies the
// `aaaseed_lua` static lib produces a usable Lua 5.1 runtime on Apple
// Silicon. Exercises the C API the engine's *_lua.cpp bindings depend
// on : open / close state, load standard libs, eval a script, read
// the stack, register a C function, call it back from Lua.
//
// AAASeed pins to Lua 5.1 (philosophy.md, project_context.md). Do not
// upgrade to 5.2/5.3/5.4 in this port -- the engine bindings assume
// the 5.1 ABI / API.
//
// Lua is MIT-licensed. AAASeed is MIT. License-compatible.
//
// CTest label : unit;lua.

#include <gtest/gtest.h>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include <cstring>

TEST( LuaSmoke, OpenStateAndCloseClean )
{
    lua_State* L = luaL_newstate();
    ASSERT_NE( L, nullptr ) << "luaL_newstate returned null";
    luaL_openlibs( L );
    lua_close( L );
    SUCCEED();
}

TEST( LuaSmoke, EvalArithmeticReturnsExpectedNumber )
{
    lua_State* L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    //	`luaL_dostring` is the all-in-one loadstring + pcall.
    //	The script returns 42 -- a load + run failure surfaces via
    //	non-zero return.
    int const ret = luaL_dostring( L, "return 6 * 7" );
    ASSERT_EQ( ret, 0 ) << "luaL_dostring failed : "
                       << lua_tostring( L, -1 );

    //	The return value is at the top of the stack. lua_tointeger
    //	returns 0 on type mismatch ; for numeric values it returns
    //	the integer-truncated form.
    ASSERT_EQ( lua_isnumber( L, -1 ), 1 ) << "Top of stack isn't a number";
    EXPECT_EQ( lua_tointeger( L, -1 ), 42 );

    lua_pop( L, 1 );
    lua_close( L );
}

TEST( LuaSmoke, StringLibStringFormat )
{
    lua_State* L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    int const ret = luaL_dostring( L,
        "return string.format( '%d-%s', 7, 'aaa' )" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    ASSERT_EQ( lua_isstring( L, -1 ), 1 );
    char const* s = lua_tostring( L, -1 );
    ASSERT_NE( s, nullptr );
    EXPECT_STREQ( s, "7-aaa" );

    lua_pop( L, 1 );
    lua_close( L );
}

//	C function registered via lua_pushcfunction + lua_setglobal.
//	Returns its single argument plus one. Engine binding TUs use this
//	pattern (with AAA's `AAALUACALL` macro wrappers) extensively.
static int test_plus_one( lua_State* L )
{
    lua_Integer const n = lua_tointeger( L, 1 );
    lua_pushinteger( L, n + 1 );
    return 1;
}

TEST( LuaSmoke, CFunctionRegisteredAndCalledFromLua )
{
    lua_State* L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    lua_pushcfunction( L, test_plus_one );
    lua_setglobal( L, "plus_one" );

    int const ret = luaL_dostring( L, "return plus_one( 41 )" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    ASSERT_EQ( lua_isnumber( L, -1 ), 1 );
    EXPECT_EQ( lua_tointeger( L, -1 ), 42 );

    lua_pop( L, 1 );
    lua_close( L );
}

TEST( LuaSmoke, BadSyntaxReportsErrorWithoutCrashing )
{
    lua_State* L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    int const ret = luaL_dostring( L, "this is not valid lua --::--" );
    EXPECT_NE( ret, 0 ) << "Expected non-zero return for invalid input";

    char const* err = lua_tostring( L, -1 );
    ASSERT_NE( err, nullptr );
    EXPECT_GT( std::strlen( err ), size_t( 0 ) );

    lua_close( L );
}

TEST( LuaSmoke, LuaVersionMatches501 )
{
    //	Confirm we vendored the right version. LUA_VERSION_NUM was
    //	introduced in 5.1 as 501.
    EXPECT_EQ( LUA_VERSION_NUM, 501 );
}
