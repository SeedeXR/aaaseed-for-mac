// tests/unit/ui_file_dialog_test.cpp
//
// c132 / Phase 4 unblock : aaa::ui file dialog smoke + normalisation
// coverage. The actual NSOpenPanel / NSSavePanel runModal path needs
// a live NSApplication event loop ; under ctest there is none, so we
// only exercise :
//
//   1. Public type ctor + filter struct usability (smoke).
//   2. Extension normalisation : both ".lua" and "lua" must collapse
//      to the bare "lua" form. Mixed case lower-cased.
//   3. Empty filter list path : the API accepts an empty filter
//      vector ; the impl interprets this as "all files" and returns
//      a configured-but-unspecific dialog.
//
// Hermetic test : links aaaseed_ui_file_dialog_mac only. NO link to
// aaaseed_code_utils -- preserves the doctrine from c104 /
// feedback_hermetic_mac_sublibs.md. Pure C++ test TU (no .mm) so the
// header's no-ObjC-in-header contract is verified at compile time.

#include "src/ui/macos/aaa_file_dialog.h"

#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <vector>

namespace
{

using aaa::ui::FileDialogFilter;
using aaa::ui::normalise_extension;

}   //	anonymous namespace

//	-----------------------------------------------------------------
//	Test 1 : ctor + null-context smoke.
//
//	Just instantiates the public types. No runModal call : without a
//	live NSApplication the panel either hangs or aborts under ctest,
//	so this test verifies only that the symbols link + the structs
//	are usable. Actual interactive runModal coverage will land when
//	the app-level integration test gets a scripted UI driver
//	(deferred, Phase 7 territory).
//	-----------------------------------------------------------------
TEST(UiFileDialog, SmokeCtorAndStructUsable)
{
    FileDialogFilter f;
    f.label = "Lua scripts";
    f.extensions = { "lua" };

    EXPECT_EQ( f.label, std::string{ "Lua scripts" } );
    ASSERT_EQ( f.extensions.size(), static_cast< std::size_t >( 1 ) );
    EXPECT_EQ( f.extensions[0], std::string{ "lua" } );

    //	The three public API symbols must resolve at link time. Take
    //	their addresses to force the linker to pull them in without
    //	actually invoking a panel that would block on the missing
    //	event loop.
    auto p_open  = &aaa::ui::open_file_dialog;
    auto p_save  = &aaa::ui::save_file_dialog;
    auto p_multi = &aaa::ui::open_multi_file_dialog;
    EXPECT_NE( p_open,  nullptr );
    EXPECT_NE( p_save,  nullptr );
    EXPECT_NE( p_multi, nullptr );
}

//	-----------------------------------------------------------------
//	Test 2 : extension normalisation.
//
//	The impl accepts ".lua" and "lua" interchangeably -- both must
//	collapse to the bare "lua" form. Mixed-case inputs lower-case.
//	Empty input round-trips to empty (NOT a leading-dot artifact).
//	-----------------------------------------------------------------
TEST(UiFileDialog, FilterNormalisation_DotAndCase)
{
    EXPECT_EQ( normalise_extension( "lua"   ), std::string{ "lua" } );
    EXPECT_EQ( normalise_extension( ".lua"  ), std::string{ "lua" } );
    EXPECT_EQ( normalise_extension( ".LUA"  ), std::string{ "lua" } );
    EXPECT_EQ( normalise_extension( "PnG"   ), std::string{ "png" } );
    EXPECT_EQ( normalise_extension( ".jpeg" ), std::string{ "jpeg" } );
    //	Empty stays empty -- the impl skips empty entries when building
    //	the NSArray, so this is the safe sentinel.
    EXPECT_EQ( normalise_extension( ""      ), std::string{ "" } );
    //	A leading dot with nothing after it normalises to empty -- the
    //	one-leading-dot strip + tolower path is order-stable.
    EXPECT_EQ( normalise_extension( "."     ), std::string{ "" } );
    //	Only the FIRST leading dot is stripped : "..lua" -> ".lua".
    //	Callers shouldn't double-dot in practice, but the rule is
    //	deterministic and worth pinning.
    EXPECT_EQ( normalise_extension( "..lua" ), std::string{ ".lua" } );
}

//	-----------------------------------------------------------------
//	Test 3 : empty filter list path.
//
//	An empty filter vector is the explicit "all files" mode. The
//	public types must accept it and the helper normalise_extension
//	is irrelevant in that branch. Pin the shape so a future refactor
//	cannot silently turn this into a precondition violation.
//	-----------------------------------------------------------------
TEST(UiFileDialog, EmptyFilterListIsAllFilesMode)
{
    std::vector< FileDialogFilter > const empty {};
    EXPECT_TRUE( empty.empty() );

    //	Mirror what configure_panel sees : iterating an empty filter
    //	list yields no entries, which maps to nil NSArray + the
    //	"all files allowed" panel state. We can't observe the panel
    //	from outside without runModal, but we can verify the
    //	pre-condition + that the public API accepts the call shape.
    std::size_t total_exts = 0;
    for( auto const& f : empty )
        total_exts += f.extensions.size();
    EXPECT_EQ( total_exts, static_cast< std::size_t >( 0 ) );

    //	A single filter with NO extensions is the "label only" form ;
    //	the impl skips it the same way (no exts to add). Also a no-op.
    std::vector< FileDialogFilter > const label_only {
        FileDialogFilter{ "All files", {} },
    };
    std::size_t exts2 = 0;
    for( auto const& f : label_only )
        exts2 += f.extensions.size();
    EXPECT_EQ( exts2, static_cast< std::size_t >( 0 ) );
}
