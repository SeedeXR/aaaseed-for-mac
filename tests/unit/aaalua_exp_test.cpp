// tests/unit/aaalua_exp_test.cpp
//
// Engine-Lua-wrapper Stage 3 (continuation 50) : drive the second real
// `aaalua_*.cpp` body on Mac. aaalua_exp.cpp is 160 lines but most are
// inside `#if 0` (legacy luaI_openlib) or `#if !AAA_LIB_LUA_STATIC()`
// (Lua-lib polyfills) ; AAA_LIB_LUA_STATIC is `1` in aaalua_util.h:15
// so the actually-compiled portion is ~15 lines :
//   - `tag_error( lua_State*, narg, tag )` global free function
//   - `aaalua::n_exp::register_exp( lua_State* )` namespace member
//     that creates an empty "exp" sub-table on the current parent.
//
// What this test proves :
//   1. aaalua_exp.cpp body compiles + links against our vendored Lua
//      5.1.5 (luaL_typerror, lua_typename are real, not from the
//      AAA_LIB_LUA_STATIC=0 polyfills which are excluded by `#if`).
//   2. The `LUAAAA_START(L, __FUNCTION__)` macro from aaalua_glue.h
//      expands to a c_lua_state construction that resolves at link
//      time (proves the inline-only c_lua_state path).
//   3. `c_lua_state::define_table` (inline) + `pop` (inline) operate
//      correctly on a real Lua stack.
//   4. `tag_error` raises a catchable Lua error -- documents the
//      pattern that engine wrapper code uses for argument type checks.
//
// This is Stage 3 of the engine Lua wrapper port. Stage 2 (aaalua_
// debug.cpp) landed in continuation 49. Pattern : compile the engine
// .cpp directly into a test target, link aaaseed_lua + aaaseed_code_
// utils + obj.cpp + the slim engine-stub split.

#include <gtest/gtest.h>

#include <cstring>
#include <string>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "aaa_type.h"

//	tag_error is declared at file scope in aaalua_exp.cpp ; expose its
//	signature here so the test can call it directly.
extern void tag_error( lua_State* L, int narg, int tag );

//	register_exp lives in aaalua::n_exp namespace.
namespace aaalua { namespace n_exp {
    extern void register_exp( lua_State* L );
}}

namespace
{
    //	C bridge : when called from Lua, calls tag_error(L, 1, LUA_TNUMBER).
    //	tag_error -> luaL_typerror -> luaL_argerror -> luaL_error ->
    //	lua_error (long-jumps out of the C function). Caller must use
    //	lua_pcall / luaL_dostring to catch it.
    int l_force_tag_error( lua_State* L )
    {
        tag_error( L, 1, LUA_TNUMBER );
        return 0; //  unreachable -- lua_error never returns.
    }

    lua_State* open_state()
    {
        lua_State* L = luaL_newstate();
        if( !L ) return nullptr;
        luaL_openlibs( L );
        return L;
    }
}

TEST( AaaluaExp, RegisterExpCreatesEmptySubtable )
{
    lua_State* L = open_state();
    ASSERT_NE( L, nullptr );

    //	register_exp uses `l.define_table( "exp" )` which performs
    //	`new_table + lua_setfield(-2, "exp") + lua_getfield(-1, "exp")`.
    //	The setfield writes into the table at -2 ; provide one.
    lua_newtable( L );

    int const top_before = lua_gettop( L );
    aaalua::n_exp::register_exp( L );
    int const top_after = lua_gettop( L );

    //	register_exp's net stack effect is zero : new_table pushes T1,
    //	setfield pops T1, getfield pushes T1, pop(1) removes T1. The
    //	parent table remains at -1.
    EXPECT_EQ( top_after, top_before );

    //	Verify the parent now has an "exp" field that is a table.
    lua_getfield( L, -1, "exp" );
    EXPECT_EQ( lua_type( L, -1 ), LUA_TTABLE );

    //	"exp" table should be empty (the only ADD_FN is commented out).
    //	lua_objlen on a table returns its array-part length ; 0 here.
    EXPECT_EQ( lua_objlen( L, -1 ), 0u );

    lua_close( L );
}

TEST( AaaluaExp, TagErrorRaisesCatchableLuaError )
{
    lua_State* L = open_state();
    ASSERT_NE( L, nullptr );

    lua_pushcfunction( L, l_force_tag_error );
    lua_setglobal( L, "force_tag_error" );

    //	Calling force_tag_error("hello") -- pass a string where the
    //	C side will report "number expected". lua_pcall (inside
    //	luaL_dostring) catches the long-jump.
    int const ret = luaL_dostring( L, "force_tag_error( 'hello' )" );
    ASSERT_NE( ret, 0 ) << "expected Lua error, got success";

    //	The error message should mention the expected type ("number"
    //	from lua_typename + LUA_TNUMBER).
    char const* const msg = lua_tostring( L, -1 );
    ASSERT_NE( msg, nullptr );
    std::string const m{ msg };
    EXPECT_NE( m.find( "number" ), std::string::npos )
        << "expected error message to mention 'number', got: " << m;

    lua_close( L );
}

TEST( AaaluaExp, RegisterExpDoesNotPolluteUnrelatedTable )
{
    //	Sanity-check : register_exp only touches the table at -2.
    //	An unrelated table earlier on the stack is unaffected.
    lua_State* L = open_state();
    ASSERT_NE( L, nullptr );

    lua_newtable( L );                  //	table A at -1
    lua_pushstring( L, "A_marker" );
    lua_setfield( L, -2, "marker" );    //	A.marker = "A_marker"

    lua_newtable( L );                  //	table B at -1, A at -2
    aaalua::n_exp::register_exp( L );   //	operates on B

    //	B has "exp" sub-table.
    lua_getfield( L, -1, "exp" );
    EXPECT_EQ( lua_type( L, -1 ), LUA_TTABLE );
    lua_pop( L, 1 );

    //	A is at -2 and untouched : its marker is intact, no "exp" field.
    lua_getfield( L, -2, "marker" );
    char const* const a_marker = lua_tostring( L, -1 );
    ASSERT_NE( a_marker, nullptr );
    EXPECT_STREQ( a_marker, "A_marker" );
    lua_pop( L, 1 );

    lua_getfield( L, -2, "exp" );
    EXPECT_EQ( lua_type( L, -1 ), LUA_TNIL );
    lua_pop( L, 1 );

    lua_close( L );
}
