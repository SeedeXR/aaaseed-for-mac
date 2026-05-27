// tests/unit/layer_header_test.cpp
//
// Group D Stage 1 : header-only smoke test for the five core
// layer-subsystem headers. Mirrors the obj_ui Stage 1 pattern
// (continuation ~16) : compile the include chain without linking any
// engine .cpp. If this builds, the header graph is healthy and Stage 2
// (compile `app.cpp` -- 447 lines, simplest leaf) can begin.
//
// Headers under test :
//   - infrastructure/layer/app.h
//   - infrastructure/layer/layer.h
//   - infrastructure/layer/layers.h
//   - infrastructure/layer/module.h
//   - infrastructure/layer/modules.h
//
// Deliberately NOT included here (Stage 1 scope) :
//   - layer_att.h / layers_att.h -- pull in obj_ui/bdd/util/bdd.h and
//     draw/seeddraw.h / seedcam.h / lights.h , each of which is its
//     own transitive cone.
//   - layer_lua.h / layers_lua.h / module_lua.h / modules_lua.h --
//     require the Lua state wrapper, deferred.
//
// Why a unit test and not a CMake `add_library` smoke target : we need
// the engine force-include + include-path setup (aaa_apply_engine_
// compile_flags()) that the existing test executables already plumb.
// Re-using that scaffold is cheaper than spinning a new lib for what is
// fundamentally a parse check.

#include <gtest/gtest.h>

#include <type_traits>

#include "aaa_type.h"

//	Order matters : layer.h references c_obj_ui, layers.h references
//	layer + cameras, modules.h references module + global data, app.h
//	references modules. Include the leaves first.
#include "infrastructure/layer/layer.h"
#include "infrastructure/cameras.h"
#include "infrastructure/data/aaa_global.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/layer/app.h"

//	Compile-time-only assertion : the type names must exist and inherit
//	from `c_obj_ui` as the CLAUDE.md claims. Cheaper than calling
//	anything (which would need link-time symbols we have not built).
static_assert( std::is_base_of_v< c_obj_ui, c_layer  >, "c_layer must inherit c_obj_ui" );
static_assert( std::is_base_of_v< c_obj_ui, c_layers >, "c_layers must inherit c_obj_ui" );
static_assert( std::is_base_of_v< c_obj_ui, c_module >, "c_module must inherit c_obj_ui" );
static_assert( std::is_base_of_v< c_obj_ui, c_modules>, "c_modules must inherit c_obj_ui" );
static_assert( std::is_base_of_v< c_obj_ui, c_app    >, "c_app must inherit c_obj_ui" );

TEST( LayerHeader, HeaderChainParsesCleanly )
{
    //	If the file compiled, the chain parses. No runtime assertion needed.
    SUCCEED();
}

TEST( LayerHeader, ClassSizesAreReasonable )
{
    //	Sanity-check : zero-sized engine-graph classes would mean the
    //	headers are stubbed out somewhere. The actual numbers don't
    //	matter (will drift across engine revisions), but they should be
    //	non-trivial. Caps are generous -- this is a smoke check.
    EXPECT_GT( sizeof( c_layer   ), 32u );
    EXPECT_GT( sizeof( c_layers  ), 32u );
    EXPECT_GT( sizeof( c_module  ), 32u );
    EXPECT_GT( sizeof( c_modules ), 32u );
    EXPECT_GT( sizeof( c_app     ), 32u );

    //	Upper caps catch obvious accidental ballooning (the windows
    //	engine numbers are well under these).
    EXPECT_LT( sizeof( c_layer   ), 16u * 1024u );
    EXPECT_LT( sizeof( c_layers  ), 16u * 1024u );
    EXPECT_LT( sizeof( c_module  ), 32u * 1024u );
    EXPECT_LT( sizeof( c_modules ), 16u * 1024u );
    EXPECT_LT( sizeof( c_app     ), 16u * 1024u );
}
