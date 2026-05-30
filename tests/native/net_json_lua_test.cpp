// tests/native/net_json_lua_test.cpp
//
// Deterministic coverage for the JSON-string -> Lua-table binding
// (aaa.net.parse_json / parse_json_to_lua). No network is touched : we
// open a bare lua_State, register the binding, feed it literal JSON
// strings, and verify the resulting Lua values by walking the stack
// and by running small Lua snippets via luaL_dostring.
//
// Links aaaseed_lua. Builds as a plain .cpp (the binding header is
// C++-friendly ; the .mm implementation owns all the ObjC).

#include <gtest/gtest.h>

#include <string>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "src/net/net_json_lua.h"

namespace
{
//	Open a fresh lua_State with the standard libs + our binding. Caller
//	owns the returned state and must lua_close() it.
lua_State * open_state()
{
    lua_State * L = luaL_newstate();
    if( L == nullptr )
        return nullptr;
    luaL_openlibs( L );
    aaa::net::register_lua_bindings( L );
    return L;
}
}   //	anonymous namespace

TEST( NetJsonLua, RegisterBindings )
{
    lua_State * L = open_state();
    ASSERT_NE( L, nullptr );

    lua_getglobal( L, "aaa" );
    ASSERT_TRUE( lua_istable( L, -1 ) ) << "global `aaa` is not a table";

    lua_getfield( L, -1, "net" );
    ASSERT_TRUE( lua_istable( L, -1 ) ) << "`aaa.net` is not a table";

    lua_getfield( L, -1, "parse_json" );
    EXPECT_TRUE( lua_isfunction( L, -1 ) )
        << "`aaa.net.parse_json` is not a function";

    lua_pop( L, 3 );
    lua_close( L );
}

//	Scalars + a flat object via the test-only direct entry point.
TEST( NetJsonLua, ScalarsAndFlatObject )
{
    lua_State * L = open_state();
    ASSERT_NE( L, nullptr );

    //	String scalar.
    {
        int n = aaa::net::parse_json_to_lua( L, "\"hello\"" );
        ASSERT_EQ( n, 1 );
        ASSERT_TRUE( lua_isstring( L, -1 ) );
        EXPECT_STREQ( lua_tostring( L, -1 ), "hello" );
        lua_pop( L, 1 );
    }

    //	Number scalar.
    {
        int n = aaa::net::parse_json_to_lua( L, "42.5" );
        ASSERT_EQ( n, 1 );
        ASSERT_TRUE( lua_isnumber( L, -1 ) );
        EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 42.5 );
        lua_pop( L, 1 );
    }

    //	Boolean true / false must be Lua booleans, NOT numbers.
    {
        int n = aaa::net::parse_json_to_lua( L, "true" );
        ASSERT_EQ( n, 1 );
        ASSERT_TRUE( lua_isboolean( L, -1 ) );
        EXPECT_EQ( lua_toboolean( L, -1 ), 1 );
        lua_pop( L, 1 );

        n = aaa::net::parse_json_to_lua( L, "false" );
        ASSERT_EQ( n, 1 );
        ASSERT_TRUE( lua_isboolean( L, -1 ) );
        EXPECT_EQ( lua_toboolean( L, -1 ), 0 );
        lua_pop( L, 1 );
    }

    lua_close( L );
}

//	Nested dict + array + number + bool + null + string, verified by
//	running Lua against the parsed table stored as a global.
TEST( NetJsonLua, NestedStructureViaLua )
{
    lua_State * L = open_state();
    ASSERT_NE( L, nullptr );

    char const * json =
        "{"
        "  \"name\": \"seed\","
        "  \"count\": 3,"
        "  \"enabled\": true,"
        "  \"disabled\": false,"
        "  \"missing\": null,"
        "  \"items\": [10, 20, 30],"
        "  \"nested\": { \"inner\": \"deep\" }"
        "}";

    int n = aaa::net::parse_json_to_lua( L, json );
    ASSERT_EQ( n, 1 );
    ASSERT_TRUE( lua_istable( L, -1 ) );

    //	Stash the parsed table as a global so Lua snippets can read it.
    lua_setglobal( L, "t" );

    //	String field.
    ASSERT_EQ( luaL_dostring( L, "return t.name" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_STREQ( lua_tostring( L, -1 ), "seed" );
    lua_pop( L, 1 );

    //	Number field.
    ASSERT_EQ( luaL_dostring( L, "return t.count" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 3.0 );
    lua_pop( L, 1 );

    //	Booleans.
    ASSERT_EQ( luaL_dostring( L, "return t.enabled" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_TRUE( lua_isboolean( L, -1 ) );
    EXPECT_EQ( lua_toboolean( L, -1 ), 1 );
    lua_pop( L, 1 );

    ASSERT_EQ( luaL_dostring( L, "return t.disabled" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_TRUE( lua_isboolean( L, -1 ) );
    EXPECT_EQ( lua_toboolean( L, -1 ), 0 );
    lua_pop( L, 1 );

    //	JSON null -> key unset -> Lua nil.
    ASSERT_EQ( luaL_dostring( L, "return t.missing == nil" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_TRUE( lua_isboolean( L, -1 ) );
    EXPECT_EQ( lua_toboolean( L, -1 ), 1 );
    lua_pop( L, 1 );

    //	Array : 1-indexed, length 3, values 10/20/30.
    ASSERT_EQ( luaL_dostring( L,
        "return #t.items, t.items[1], t.items[2], t.items[3]" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_EQ( lua_tointeger( L, -4 ), 3 );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -3 ), 10.0 );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -2 ), 20.0 );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), 30.0 );
    lua_pop( L, 4 );

    //	Nested dictionary.
    ASSERT_EQ( luaL_dostring( L, "return t.nested.inner" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_STREQ( lua_tostring( L, -1 ), "deep" );
    lua_pop( L, 1 );

    lua_close( L );
}

//	Top-level array converts to a 1-indexed table.
TEST( NetJsonLua, TopLevelArray )
{
    lua_State * L = open_state();
    ASSERT_NE( L, nullptr );

    int n = aaa::net::parse_json_to_lua( L, "[\"a\", \"b\", \"c\"]" );
    ASSERT_EQ( n, 1 );
    ASSERT_TRUE( lua_istable( L, -1 ) );
    lua_setglobal( L, "arr" );

    ASSERT_EQ( luaL_dostring( L,
        "return #arr, arr[1], arr[3]" ), 0 ) << lua_tostring( L, -1 );
    EXPECT_EQ( lua_tointeger( L, -3 ), 3 );
    EXPECT_STREQ( lua_tostring( L, -2 ), "a" );
    EXPECT_STREQ( lua_tostring( L, -1 ), "c" );
    lua_pop( L, 3 );

    lua_close( L );
}

//	Malformed JSON : parse_json_to_lua must push nil + an error string
//	(2 values), and the Lua-callable form must return the same shape.
TEST( NetJsonLua, MalformedJsonError )
{
    lua_State * L = open_state();
    ASSERT_NE( L, nullptr );

    //	Direct entry point : 2 values, nil then string.
    int n = aaa::net::parse_json_to_lua( L, "{ this is not json ]" );
    ASSERT_EQ( n, 2 );
    EXPECT_TRUE( lua_isnil( L, -2 ) );
    EXPECT_TRUE( lua_isstring( L, -1 ) );
    EXPECT_GT( std::string( lua_tostring( L, -1 ) ).size(), 0u );
    lua_pop( L, 2 );

    //	Through the Lua callable : `local v, err = aaa.net.parse_json(..)`
    //	-> v is nil, err is a non-empty string.
    ASSERT_EQ( luaL_dostring( L,
        "local v, err = aaa.net.parse_json('{bad') "
        "return v == nil, type(err) == 'string' and #err > 0" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_TRUE( lua_toboolean( L, -2 ) ) << "expected nil value";
    EXPECT_TRUE( lua_toboolean( L, -1 ) ) << "expected error string";
    lua_pop( L, 2 );

    lua_close( L );
}
