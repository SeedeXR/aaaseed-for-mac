// tests/unit/bdd_cell_draw_header_test.cpp
//
// Task #30 (header-parse beachhead, c113 Agent A audit follow-up).
// Stage 1 of the bdd_cell_draw port : prove the include chain rooted
// at `obj_ui/bdd/bdd_cell_draw.h` parses cleanly under AppleClang on
// arm64. The .cpp body defers to post-GOL state-stack ; this test only
// verifies the header surface compiles + the documented inheritance
// (`c_bdd_cell_draw : c_bdd`) is intact.
//
// Header under test :
//   - obj_ui/bdd/bdd_cell_draw.h (which transitively pulls
//     obj_ui/bdd/util/bdd.h -> obj_ui.h, draw/multiple.h,
//     infrastructure/param/param_declare.h, ui/strsymbo.h).
//
// Mirrors aaalua_header_test : compile-only, no link to bdd_cell_draw.cpp.

#include <gtest/gtest.h>

#include <type_traits>

#include "aaa_type.h"
#include "obj_ui/bdd/util/bdd.h"
#include "obj_ui/bdd/bdd_cell_draw.h"

//	Compile-time-only contract : c_bdd_cell_draw must derive from c_bdd.
//	If the inheritance ever drifts (e.g. someone hoists it to c_obj_ui
//	directly), the static_assert fires at compile time -- much sharper
//	than a runtime test.
static_assert( std::is_base_of_v< c_bdd, c_bdd_cell_draw >,
    "c_bdd_cell_draw must derive from c_bdd" );

//	Bonus contract : c_bdd_cell_draw is declared `final` in the engine
//	header. Lock that in -- the rendering pipeline relies on the leaf
//	being a closed type (no further subclassing in the bdd hierarchy).
static_assert( std::is_final_v< c_bdd_cell_draw >,
    "c_bdd_cell_draw must remain a `final` class" );

TEST( BddCellDrawHeader, HeaderParsesCleanly )
{
    //	If the file compiled, the chain parses. The static_asserts above
    //	carry the contract.
    SUCCEED();
}
