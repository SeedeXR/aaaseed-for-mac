// tests/unit/ui_dialog_test.cpp
//
// c136 / Phase 4 (todo.md L301) : aaa::ui::dialog smoke coverage.
// The actual NSAlert runModal path needs a live NSApplication event
// loop ; under ctest there is none. The dialog module's headless
// guard (`has_live_event_loop`) returns false there, so the modal
// entry points return AlertResult::Cancel / nullopt without blocking
// -- exactly the contract this test pins.
//
// Hermetic test : links aaaseed_ui_dialog_mac only. NO link to
// aaaseed_code_utils -- preserves the doctrine from c104 /
// feedback_hermetic_mac_sublibs.md. Pure C++ test TU (no .mm) so the
// header's no-ObjC-in-header contract is verified at compile time
// (c131-B + c134-A pattern).
//
// Test surface (7 cases) :
//   1. alert() returns Cancel under no-NSApp.
//   2. confirm() with custom labels returns Cancel under no-NSApp.
//   3. prompt() returns nullopt under no-NSApp.
//   4. make_modeless_panel(positive dims) returns a non-null handle.
//   5. make_modeless_panel(zero/negative) returns null handle.
//   6. show -> hide -> destroy lifecycle does not crash.
//   7. alert() with empty title + message still returns sensibly.

#include "src/ui/macos/aaa_dialog.h"

#include <gtest/gtest.h>

#include <optional>
#include <string>

namespace
{

using aaa::ui::dialog::AlertResult;
using aaa::ui::dialog::AlertStyle;
using aaa::ui::dialog::PanelHandle;

}   //	anonymous namespace

//	-----------------------------------------------------------------
//	Test 1 : alert() returns Cancel under no-NSApp.
//
//	Without a live NSApplication event loop the dialog module's
//	headless guard kicks in. The contract : runModal is NOT called,
//	the function returns Cancel cleanly, no blocking. This is the
//	c131-B no-runModal-under-ctest pattern, made explicit here via
//	the `has_live_event_loop` probe.
//	-----------------------------------------------------------------
TEST(UiDialog, AlertReturnsCancelUnderNoEventLoop)
{
    AlertResult const r = aaa::ui::dialog::alert(
            "Smoke Title",
            "Smoke body message.",
            AlertStyle::Informational );
    EXPECT_EQ( r, AlertResult::Cancel );

    //	Confirm warning + critical styles also gracefully degrade.
    AlertResult const r_warn = aaa::ui::dialog::alert(
            "Warning Title",
            "Warning body.",
            AlertStyle::Warning );
    EXPECT_EQ( r_warn, AlertResult::Cancel );

    AlertResult const r_crit = aaa::ui::dialog::alert(
            "Critical Title",
            "Critical body.",
            AlertStyle::Critical );
    EXPECT_EQ( r_crit, AlertResult::Cancel );
}

//	-----------------------------------------------------------------
//	Test 2 : confirm() with custom labels under no-NSApp.
//
//	Custom OK/Cancel labels must not trigger the modal path either.
//	Returns Cancel without blocking. We also verify the default-
//	argument shape (just title + message + default ok/cancel).
//	-----------------------------------------------------------------
TEST(UiDialog, ConfirmCustomLabelsReturnsCancelUnderNoEventLoop)
{
    AlertResult const r = aaa::ui::dialog::confirm(
            "Delete File?",
            "Are you sure you want to delete this file?",
            "Delete",
            "Keep" );
    EXPECT_EQ( r, AlertResult::Cancel );

    //	Default labels path -- separate symbol resolution check.
    AlertResult const r2 = aaa::ui::dialog::confirm(
            "Confirm",
            "Proceed with the operation?" );
    EXPECT_EQ( r2, AlertResult::Cancel );
}

//	-----------------------------------------------------------------
//	Test 3 : prompt() returns nullopt under no-NSApp.
//
//	prompt's headless-guard contract is "nullopt", not "empty
//	string". An empty string is a valid OK-with-no-input result ;
//	nullopt is the unambiguous cancel sentinel.
//	-----------------------------------------------------------------
TEST(UiDialog, PromptReturnsNulloptUnderNoEventLoop)
{
    std::optional< std::string > const v = aaa::ui::dialog::prompt(
            "Enter Name",
            "Please enter your name:",
            "default_value",
            "OK",
            "Cancel" );
    EXPECT_FALSE( v.has_value() );

    //	Defaulted optional-args path.
    std::optional< std::string > const v2 = aaa::ui::dialog::prompt(
            "Enter Value",
            "Please enter a value:" );
    EXPECT_FALSE( v2.has_value() );
}

//	-----------------------------------------------------------------
//	Test 4 : make_modeless_panel returns a non-null handle for
//	          sensible dimensions.
//
//	A 400 x 300 point panel is well within AppKit's accepted size
//	range. The handle's `opaque` field must be non-null so callers
//	can distinguish from the degenerate path (test 5). We then
//	destroy_panel to release the retain count, leaving no leak.
//	-----------------------------------------------------------------
TEST(UiDialog, MakeModelessPanelReturnsNonNullForSensibleDims)
{
    PanelHandle h = aaa::ui::dialog::make_modeless_panel(
            "Smoke Panel",
            400,
            300 );
    EXPECT_NE( h.opaque, nullptr );
    //	Clean up retain count.
    aaa::ui::dialog::destroy_panel( h );
}

//	-----------------------------------------------------------------
//	Test 5 : make_modeless_panel rejects zero/negative dimensions.
//
//	Zero or negative width/height is a caller bug ; the module
//	returns { nullptr } so the caller can detect-and-recover
//	without crashing. The dimensions check is symmetric in width
//	and height.
//	-----------------------------------------------------------------
TEST(UiDialog, MakeModelessPanelRejectsZeroOrNegativeDims)
{
    PanelHandle h_zero_w = aaa::ui::dialog::make_modeless_panel(
            "Zero Width",
            0,
            300 );
    EXPECT_EQ( h_zero_w.opaque, nullptr );

    PanelHandle h_zero_h = aaa::ui::dialog::make_modeless_panel(
            "Zero Height",
            400,
            0 );
    EXPECT_EQ( h_zero_h.opaque, nullptr );

    PanelHandle h_neg_w = aaa::ui::dialog::make_modeless_panel(
            "Negative Width",
            -10,
            300 );
    EXPECT_EQ( h_neg_w.opaque, nullptr );

    PanelHandle h_neg_h = aaa::ui::dialog::make_modeless_panel(
            "Negative Height",
            400,
            -20 );
    EXPECT_EQ( h_neg_h.opaque, nullptr );

    //	destroy_panel on a degenerate handle must be a safe no-op.
    aaa::ui::dialog::destroy_panel( h_zero_w );
    aaa::ui::dialog::destroy_panel( h_neg_h );
}

//	-----------------------------------------------------------------
//	Test 6 : show -> hide -> destroy lifecycle works without crash.
//
//	The orderFront / orderOut calls are AppKit-internal and do not
//	require a running NSApp ; they manipulate window-server state
//	directly. We exercise the full lifecycle to catch any release-
//	count imbalance or AppKit assertion that would surface as a
//	crash. No content / draw is verified -- that is integration-
//	test territory.
//	-----------------------------------------------------------------
TEST(UiDialog, ShowHideDestroyLifecycleSmoke)
{
    PanelHandle h = aaa::ui::dialog::make_modeless_panel(
            "Lifecycle Panel",
            320,
            240 );
    ASSERT_NE( h.opaque, nullptr );

    //	show + hide are no-ops on a degenerate handle, so this
    //	covers the happy path. Each call should be crash-free.
    aaa::ui::dialog::show_panel( h );
    aaa::ui::dialog::hide_panel( h );
    aaa::ui::dialog::show_panel( h );
    aaa::ui::dialog::hide_panel( h );

    aaa::ui::dialog::destroy_panel( h );

    //	Also exercise show/hide on the degenerate path -- must
    //	not crash.
    PanelHandle h_null{ nullptr };
    aaa::ui::dialog::show_panel( h_null );
    aaa::ui::dialog::hide_panel( h_null );
    aaa::ui::dialog::destroy_panel( h_null );
}

//	-----------------------------------------------------------------
//	Test 7 : alert() with empty title + empty message returns
//	          sensibly under no-NSApp.
//
//	Degenerate input must not trigger UB or assertion. Empty
//	strings flow through `ns_from_std` -> @"" which NSAlert accepts.
//	With no event loop, the headless guard still returns Cancel.
//	-----------------------------------------------------------------
TEST(UiDialog, AlertWithEmptyTitleAndMessageIsSensible)
{
    AlertResult const r = aaa::ui::dialog::alert(
            "",
            "",
            AlertStyle::Informational );
    EXPECT_EQ( r, AlertResult::Cancel );

    //	Empty title + non-empty message and vice versa, for
    //	completeness of the degenerate-input fan-out.
    AlertResult const r_empty_title = aaa::ui::dialog::alert(
            "",
            "Body only.",
            AlertStyle::Warning );
    EXPECT_EQ( r_empty_title, AlertResult::Cancel );

    AlertResult const r_empty_body = aaa::ui::dialog::alert(
            "Title only",
            "",
            AlertStyle::Critical );
    EXPECT_EQ( r_empty_body, AlertResult::Cancel );
}
