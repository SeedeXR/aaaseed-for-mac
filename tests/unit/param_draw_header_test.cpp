// tests/unit/param_draw_header_test.cpp
//
// Task #30 (header-parse beachhead, c113 Agent A audit follow-up).
// Stage 1 of the param_draw port : prove the include chain rooted at
// `infrastructure/param/param_draw.h` parses cleanly under AppleClang
// on arm64. The .cpp body defers to post-GOL state-stack ; this test
// only verifies the header surface compiles + the documented free
// function + flat namespace exist with the expected signatures.
//
// Header under test :
//   - infrastructure/param/param_draw.h
//
// Transitive deps already Mac-clean (parsed by sibling tests) :
//   - aaa_type.h
//   - infrastructure/obj/obj_ui.h (obj_ui_header_test, c~16)
//   - infrastructure/param/param.h (forward-decl of c_param is enough
//     here ; the .h doesn't need the full param.h)
//
// Mirrors aaalua_header_test : compile-only, no link to param_draw.cpp.

#include <gtest/gtest.h>

#include <type_traits>

#include "aaa_type.h"
#include "infrastructure/obj/obj_ui.h"
#include "infrastructure/param/param_draw.h"

//	Compile-time-only assertion : the free function `param_draw(c_obj_ui*)`
//	must be declared with the expected signature. Take its address and
//	verify it casts to the documented function pointer type.
static_assert(
    std::is_same_v< decltype( &param_draw ), void(*)( c_obj_ui* ) >,
    "param_draw(c_obj_ui*) free function must exist with the documented signature" );

//	The `aaa::param::flat` namespace must exist and expose the
//	scale_update() member ; touching its address is enough to force
//	the namespace lookup at compile time.
static_assert(
    std::is_same_v< decltype( &aaa::param::flat::scale_update ), void(*)() >,
    "aaa::param::flat::scale_update() must exist" );

TEST( ParamDrawHeader, HeaderParsesCleanly )
{
    //	If the file compiled, the chain parses. The static_asserts above
    //	carry the contract.
    SUCCEED();
}
