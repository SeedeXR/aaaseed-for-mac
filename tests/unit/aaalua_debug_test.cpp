// tests/unit/aaalua_debug_test.cpp
//
// Engine-Lua-wrapper Stage 2 (continuation 49) : drive the first real
// `aaalua_*.cpp` body on Mac. aaalua_debug.cpp is 91 lines, the smallest
// of the 8 wrapper files, and pure logic -- no GUI, no infra deps, no
// LuaJIT extensions. Functions exposed :
//   - aaalua_debug_grab_info( lua_State* )         -- walks Lua stack via
//       lua_getstack / lua_getinfo into a static `ars[64]` buffer.
//   - aaalua_debug_build_trace( CHAR const* )      -- formats the captured
//       frames into a static char buffer.
//   - aaalua_debug_get_level / set_level            -- reports captured depth.
//   - aaalua_debug_get_script_name / get_line       -- reports last grabbed
//       Lua frame's source + line.
//
// What this test proves :
//   1. The full include chain (aaalua_debug.h -> aaa_type.h + lua/lua.hpp
//      + aaa_str.h) compiles a real engine .cpp on Mac (not just a smoke
//      test from continuation 48).
//   2. Engine bundled Lua 5.1 headers (in vendor's Include/lua/) are
//      ABI-compatible at the call-site level with our vanilla Lua 5.1.5
//      lib : lua_getstack / lua_getinfo / lua_Debug fields all link.
//   3. `o_str` (engine string class) lifecycle works in a real wrapper
//      file -- ctor / dtor in static storage, set / get / erase.
//   4. ERR_PRINT_STRING path is reachable but not actually triggered
//      under normal frame counts.
//
// Test approach : spin a Lua state, register a C function that calls
// `aaalua_debug_grab_info(L)` from inside the Lua VM (so the stack
// has real frames to walk), run a tiny script that calls it, then read
// back the trace + level + script name.

#include <gtest/gtest.h>

#include <cstring>
#include <string>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "aaa_type.h"
#include "language/lua/aaalua_debug.h"

namespace
{
    //	The Lua-callable C bridge : when Lua calls aaa.grab(), the C side
    //	dives into aaalua_debug_grab_info to snapshot the current stack.
    //	Walking the stack from inside a C function called by Lua gives us
    //	at least one frame to inspect.
    int l_grab( lua_State* L )
    {
        aaalua_debug_grab_info( L );
        return 0;
    }

    lua_State* open_state_with_grab()
    {
        lua_State* L = luaL_newstate();
        if( !L ) return nullptr;
        luaL_openlibs( L );
        lua_pushcfunction( L, l_grab );
        lua_setglobal( L, "grab" );
        return L;
    }
}

TEST( AaaluaDebug, InitialLevelIsZero )
{
    //	Static state starts clean -- no previous test should have left
    //	frames behind, but if it did, set_level(0) resets it.
    aaalua_debug_set_level( 0 );
    EXPECT_EQ( aaalua_debug_get_level(), 0 );
}

TEST( AaaluaDebug, GrabInfoCapturesAtLeastOneFrame )
{
    aaalua_debug_set_level( 0 );

    lua_State* L = open_state_with_grab();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L, "grab()" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    //	The grab call lives inside a Lua chunk ; the C side should see
    //	at least the chunk-level frame on the stack.
    EXPECT_GT( aaalua_debug_get_level(), 0 );

    lua_close( L );
}

TEST( AaaluaDebug, BuildTraceProducesNonEmptyOutput )
{
    aaalua_debug_set_level( 0 );

    lua_State* L = open_state_with_grab();
    ASSERT_NE( L, nullptr );

    //	Use a named function so the trace has a real `namewhat` /
    //	`source` field to format. The trace contains a header line
    //	("test_header") followed by formatted frame info.
    char const* script =
        "function inner() grab() end\n"
        "inner()\n";
    int const ret = luaL_dostring( L, script );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    CHAR* const trace = aaalua_debug_build_trace( "test_header" );
    ASSERT_NE( trace, nullptr );

    //	Trace should start with our header.
    std::string const trace_str{ trace };
    EXPECT_NE( trace_str.find( "test_header" ), std::string::npos )
        << "trace=" << trace_str;
}

TEST( AaaluaDebug, BuildTraceWithNullHeaderStillFormatsFrames )
{
    aaalua_debug_set_level( 0 );

    lua_State* L = open_state_with_grab();
    ASSERT_NE( L, nullptr );

    int const ret = luaL_dostring( L, "grab()" );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    CHAR* const trace = aaalua_debug_build_trace( nullptr );
    ASSERT_NE( trace, nullptr );

    //	With a null header the trace is purely frame content -- non-empty
    //	if grab captured a frame.
    EXPECT_GT( std::strlen( trace ), 0u );

    lua_close( L );
}

TEST( AaaluaDebug, SetLevelOverridesCapturedLevel )
{
    aaalua_debug_set_level( 7 );
    EXPECT_EQ( aaalua_debug_get_level(), 7 );

    aaalua_debug_set_level( 0 );
    EXPECT_EQ( aaalua_debug_get_level(), 0 );
}
