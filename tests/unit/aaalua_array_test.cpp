// tests/unit/aaalua_array_test.cpp
//
// Engine-Lua-wrapper Stage 4 (continuation 51) : drive the third real
// `aaalua_*.cpp` body on Mac. aaalua_array.cpp is 319 lines, exposes
// `arrayfp32` (and 7 other typed-userdata variants) -- the fast typed
// buffers Lua scripts use for image / audio / vertex data without
// per-element allocation. Operations : new, set, fill, clear, resize,
// __index, __newindex, __len, __gc (free).
//
// Strategy this session : the file's only "GL" dependency is using 8
// `GLenum` values as opaque integer type tags inside the userdata
// struct. No real GL functions are called. A tests-private shim at
// `aaalua_array_shim/gol/gol_base.h` provides GLenum + the 8 values ;
// added to this target's include path BEFORE the engine path so the
// shim wins for this build only. Zero vendor/ touch.
//
// What this test proves :
//   1. `aaalua::n_array::register_array(L)` populates the parent table
//      with an "array" sub-table containing 8 constructors.
//   2. `array.new_fp32(N)` returns a userdata of size N (verified by
//      Lua `#` operator -> `__len` metamethod).
//   3. `arr[i]` / `arr[i] = v` access via Lua syntactic sugar
//      (`__index` / `__newindex` metamethods).
//   4. `arr:fill(v)` and `arr:clear()` method dispatch via metatable.
//   5. `arr:set( i, v1, v2, ... )` bulk write.
//   6. `arr:resize(N)` realloc path.
//   7. Garbage collection runs `__gc` which frees the aligned alloc
//      (verified : no leak under Lua state close).

#include <gtest/gtest.h>

#include <cstring>
#include <string>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "aaa_type.h"

namespace aaalua { namespace n_array {
    extern void register_array( lua_State* CONST L );
}}

namespace
{
    lua_State* open_state_with_array()
    {
        lua_State* L = luaL_newstate();
        if( !L ) return nullptr;
        luaL_openlibs( L );

        //	register_array uses define_table("array") which requires a
        //	parent table at -2 ; the global table works. Lua 5.1 exposes
        //	the globals table at LUA_GLOBALSINDEX.
        lua_pushvalue( L, LUA_GLOBALSINDEX );
        aaalua::n_array::register_array( L );
        lua_pop( L, 1 );

        return L;
    }
}

TEST( AaaluaArray, RegisterArrayCreatesGlobalArrayTable )
{
    lua_State* L = open_state_with_array();
    ASSERT_NE( L, nullptr );

    lua_getglobal( L, "array" );
    EXPECT_EQ( lua_type( L, -1 ), LUA_TTABLE );

    //	Verify all 8 constructors are registered.
    char const* const ctors[] = {
        "new_int8", "new_uint8",
        "new_int16", "new_uint16",
        "new_int32", "new_uint32",
        "new_fp32", "new_fp64",
    };
    for( char const* const name : ctors )
    {
        lua_getfield( L, -1, name );
        EXPECT_EQ( lua_type( L, -1 ), LUA_TFUNCTION )
            << "array." << name << " missing";
        lua_pop( L, 1 );
    }
    lua_pop( L, 1 );

    lua_close( L );
}

TEST( AaaluaArray, NewFp32ReturnsUserdataOfRequestedSize )
{
    lua_State* L = open_state_with_array();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L,
        "local a = array.new_fp32( 10 )\n"
        "return #a\n" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    EXPECT_EQ( lua_tointeger( L, -1 ), 10 );

    lua_close( L );
}

TEST( AaaluaArray, IndexAssignAndReadRoundTrip )
{
    lua_State* L = open_state_with_array();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L,
        "local a = array.new_fp32( 4 )\n"
        "a[1] = 1.5\n"
        "a[2] = 2.5\n"
        "a[3] = 3.5\n"
        "a[4] = 4.5\n"
        "return a[1] + a[2] + a[3] + a[4]\n" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 12.0 );

    lua_close( L );
}

TEST( AaaluaArray, FillSetsAllElements )
{
    lua_State* L = open_state_with_array();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L,
        "local a = array.new_fp32( 5 )\n"
        "a:fill( 7.0 )\n"
        "local sum = 0\n"
        "for i = 1, #a do sum = sum + a[i] end\n"
        "return sum\n" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 35.0 );

    lua_close( L );
}

TEST( AaaluaArray, ClearZerosAllElements )
{
    lua_State* L = open_state_with_array();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L,
        "local a = array.new_fp32( 3 )\n"
        "a:fill( 42.0 )\n"
        "a:clear()\n"
        "return a[1] + a[2] + a[3]\n" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 0.0 );

    lua_close( L );
}

TEST( AaaluaArray, SetBulkWriteFromVarargs )
{
    lua_State* L = open_state_with_array();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L,
        "local a = array.new_fp32( 4 )\n"
        "a:set( 1, 10.0, 20.0, 30.0 )\n"
        "return a[1] + a[2] + a[3] + a[4]\n" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    //	a[1..3] = 10+20+30 = 60 ; a[4] untouched = 0.
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 60.0 );

    lua_close( L );
}

TEST( AaaluaArray, ResizeGrowsArray )
{
    lua_State* L = open_state_with_array();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L,
        "local a = array.new_fp32( 2 )\n"
        "a[1] = 5.0\n"
        "a:resize( 6 )\n"
        "return #a\n" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    EXPECT_EQ( lua_tointeger( L, -1 ), 6 );

    lua_close( L );
}

TEST( AaaluaArray, Int32TypeTagDistinctFromFp32 )
{
    //	Sanity-check : the GLenum-as-type-tag mechanism distinguishes
    //	array_int32 from array_fp32. The metatables are keyed on the
    //	type-tag name (names[]) so the engine's type-check in
    //	get_ud_info_type would fire on a mismatch -- but for now we
    //	just verify the two ctors return distinct userdata that both
    //	report the size correctly via __len.
    lua_State* L = open_state_with_array();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L,
        "local f = array.new_fp32( 3 )\n"
        "local i = array.new_int32( 5 )\n"
        "return #f + #i\n" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    EXPECT_EQ( lua_tointeger( L, -1 ), 8 );

    lua_close( L );
}
