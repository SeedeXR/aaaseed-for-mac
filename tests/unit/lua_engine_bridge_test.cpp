// tests/unit/lua_engine_bridge_test.cpp
//
// First end-to-end Lua --> C --> engine integration test (continuation
// 47). Wraps three engine math helpers (CLAMP / MIN / MAX from
// `code_utils/aaa_util.h`) as Lua-callable C functions, registers them
// in the global Lua table, and calls them from Lua scripts. Verifies
// return values match the engine semantics.
//
// Why this test : it's the smallest possible proof that the vendored
// Lua (continuation 45) can drive engine code on Mac. Documents the
// pattern future `*_lua.cpp` binding ports follow when the engine's
// `aaalua_*.h/cpp` wrapper layer lands (the wrapper's `AAALUACALL`
// macros expand to roughly the same boilerplate as the static
// functions below).
//
// CTest label : unit;lua_engine.

#include <gtest/gtest.h>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "aaa_util.h"   //  CLAMP / MIN / MAX templates ; force-included
                        //  build config provides the typedefs they need.

namespace
{
    //	Lua-callable wrappers. Each pops args from the stack, calls the
    //	templated engine helper at lua_Number (= double on AppleClang),
    //	pushes the result, returns 1 (one return value).
    int l_clamp( lua_State* L )
    {
        double const a   = lua_tonumber( L, 1 );
        double const lo  = lua_tonumber( L, 2 );
        double const hi  = lua_tonumber( L, 3 );
        double const ret = CLAMP< double >( a, lo, hi );
        lua_pushnumber( L, ret );
        return 1;
    }

    int l_min( lua_State* L )
    {
        double const a   = lua_tonumber( L, 1 );
        double const b   = lua_tonumber( L, 2 );
        double const ret = MIN< double >( a, b );
        lua_pushnumber( L, ret );
        return 1;
    }

    int l_max( lua_State* L )
    {
        double const a   = lua_tonumber( L, 1 );
        double const b   = lua_tonumber( L, 2 );
        double const ret = MAX< double >( a, b );
        lua_pushnumber( L, ret );
        return 1;
    }

    //	Test-side helper : opens a Lua state with stdlibs + the three
    //	engine bindings registered as globals. Returns the new state ;
    //	caller closes.
    lua_State* open_state_with_bridges()
    {
        lua_State* L = luaL_newstate();
        if( !L ) return nullptr;
        luaL_openlibs( L );

        lua_pushcfunction( L, l_clamp ); lua_setglobal( L, "clamp" );
        lua_pushcfunction( L, l_min   ); lua_setglobal( L, "min"   );
        lua_pushcfunction( L, l_max   ); lua_setglobal( L, "max"   );
        return L;
    }
}

TEST( LuaEngineBridge, ClampWithinRangeReturnsInput )
{
    lua_State* L = open_state_with_bridges();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L, "return clamp( 5.0, 0.0, 10.0 )" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 5.0 );
    lua_pop( L, 1 );

    lua_close( L );
}

TEST( LuaEngineBridge, ClampBelowLowerReturnsLower )
{
    lua_State* L = open_state_with_bridges();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L, "return clamp( -3.0, 0.0, 10.0 )" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 0.0 );
    lua_pop( L, 1 );

    lua_close( L );
}

TEST( LuaEngineBridge, ClampAboveUpperReturnsUpper )
{
    lua_State* L = open_state_with_bridges();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L, "return clamp( 99.0, 0.0, 10.0 )" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 10.0 );
    lua_pop( L, 1 );

    lua_close( L );
}

TEST( LuaEngineBridge, MinAndMaxComposeInLua )
{
    lua_State* L = open_state_with_bridges();
    ASSERT_NE( L, nullptr );

    //	Compose calls to demonstrate Lua can chain engine functions
    //	the way real MEU scripts will.
    int const ret = luaL_dostring(
        L, "return max( min( 7.0, 5.0 ), 3.0 )" );  // = max(5, 3) = 5
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 5.0 );
    lua_pop( L, 1 );

    lua_close( L );
}

TEST( LuaEngineBridge, ClampEquivalentToLuaSideExpression )
{
    //	Cross-validate : a Lua-only clamp implementation should agree
    //	with the engine binding on a sweep of values. Catches drift if
    //	the engine semantics ever change.
    lua_State* L = open_state_with_bridges();
    ASSERT_NE( L, nullptr );

    char const* script =
        "local lo, hi = -2.0, 4.0\n"
        "local function lua_clamp( a )\n"
        "    if a < lo then return lo end\n"
        "    if a > hi then return hi end\n"
        "    return a\n"
        "end\n"
        "local samples = { -5, -2, -1, 0, 1, 3, 4, 5, 100 }\n"
        "for _, v in ipairs( samples ) do\n"
        "    if clamp( v, lo, hi ) ~= lua_clamp( v ) then\n"
        "        return false, v\n"
        "    end\n"
        "end\n"
        "return true\n";

    int const ret = luaL_dostring( L, script );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    //	The script returns true on a clean sweep, false + the bad value
    //	otherwise. Top of stack is the boolean.
    ASSERT_EQ( lua_isboolean( L, -1 ), 1 );
    EXPECT_TRUE( lua_toboolean( L, -1 ) )
        << "engine CLAMP disagreed with Lua-side reference at value "
        << ( lua_isnumber( L, -2 ) ? lua_tonumber( L, -2 ) : -999.0 );

    lua_close( L );
}
