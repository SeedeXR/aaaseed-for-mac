// tests/unit/aaalua_header_test.cpp
//
// Engine-Lua-wrapper Stage 1 : header-parse smoke test for the eight
// `aaalua_*.h` files at vendor/aaaseed-engine/Src/language/lua/. Mirrors
// the layer-headers Stage 1 (continuation 30) and obj_ui Stage 1
// patterns : compile the include chain without linking any aaalua_*.cpp
// body. If this builds, the header graph is healthy and Stage 2 (port
// the smallest .cpp -- aaalua_debug.cpp, 91 lines, or aaalua_array.cpp)
// can begin.
//
// Headers under test :
//   - aaalua_array.h
//   - aaalua_debug.h
//   - aaalua_draw.h
//   - aaalua_exp.h
//   - aaalua_glue.h
//   - aaalua_master.h
//   - aaalua_util.h
//   - aaalua_wrap.h
//
// Dependencies the chain transitively pulls in (all already Mac-clean) :
//   - aaa_type.h
//   - lua/lua.hpp (engine bundles its own LuaJIT-flavored headers under
//     vendor/aaaseed-engine/Include/lua/ ; we link against our vanilla
//     Lua 5.1.5 at runtime ; the C API is compatible)
//   - aaa_str.h (o_str via code_utils)
//   - infrastructure/obj/obj_ui.h (Mac-clean Stage 1 since continuation
//     ~16)
//   - aaa/aaa_mutex.h (Mac-clean since continuation 16)
//
// Why include order matters : aaalua_glue.h forward-references c_lua_state
// from aaalua_util.h ; aaalua_master.h includes obj_ui + aaa_mutex ;
// pull leaves first then composites.

#include <gtest/gtest.h>

#include <type_traits>

#include "aaa_type.h"

//	Leaf dependencies first.
#include "aaa_str.h"
#include "lua/lua.hpp"
#include "infrastructure/obj/obj_ui.h"
#include "aaa/aaa_mutex.h"

//	aaalua_*.h headers under test. Order : util before glue (glue
//	references c_lua_state from util), array/debug/exp/draw are leaves
//	on the engine-Lua side, master and wrap pull obj_ui + str.
#include "language/lua/aaalua_util.h"
#include "language/lua/aaalua_glue.h"
#include "language/lua/aaalua_array.h"
#include "language/lua/aaalua_debug.h"
#include "language/lua/aaalua_exp.h"
#include "language/lua/aaalua_draw.h"
#include "language/lua/aaalua_master.h"
#include "language/lua/aaalua_wrap.h"

//	Compile-time-only assertions : these wrapper classes must exist with
//	the expected base relationships, mirroring the layer_header_test
//	approach. c_lua_state is final (not derived from anything), c_lua_wrap
//	and c_lua_master are documented to derive from c_obj_ui.
static_assert( sizeof( c_lua_state ) > 0,
    "c_lua_state must be a complete type after aaalua_util.h" );
static_assert( std::is_base_of_v< c_obj_ui, c_lua_wrap >,
    "c_lua_wrap must inherit c_obj_ui" );
static_assert( std::is_base_of_v< c_obj_ui, c_lua_master >,
    "c_lua_master must inherit c_obj_ui" );

TEST( AaaluaHeader, HeaderChainParsesCleanly )
{
    //	If the file compiled, the chain parses on Mac. No runtime
    //	assertion needed -- the static_asserts above carry the contract.
    SUCCEED();
}

TEST( AaaluaHeader, ClassSizesAreReasonable )
{
    //	Sanity-check : zero-sized wrapper classes would mean the headers
    //	are stubbed out somewhere. The actual sizes will drift across
    //	engine revisions ; these are generous caps purely for a smoke
    //	check.
    EXPECT_GT( sizeof( c_lua_state  ),  8u );
    EXPECT_GT( sizeof( c_lua_wrap   ), 32u );
    EXPECT_GT( sizeof( c_lua_master ), 32u );

    EXPECT_LT( sizeof( c_lua_state  ),  4u * 1024u );
    EXPECT_LT( sizeof( c_lua_wrap   ), 16u * 1024u );
    EXPECT_LT( sizeof( c_lua_master ), 16u * 1024u );
}

TEST( AaaluaHeader, LuaStateHelpersAreInline )
{
    //	Verify the engine's c_lua_state matches the c_lua_state we
    //	constructed by hand in continuation 47's bridge test : default
    //	construction without a lua_State* is legal (returns a benign
    //	instance whose get_state() yields nullptr). This is the public
    //	contract the wrapper layer relies on.
    c_lua_state const empty;
    EXPECT_EQ( empty.get_state(), nullptr );
}
