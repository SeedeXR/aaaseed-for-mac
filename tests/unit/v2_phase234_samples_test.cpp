//	tests/unit/v2_phase234_samples_test.cpp
//
//	c148 v2 Phases 2-4 : regression-guard for the THREE new sample
//	MEUs landed under bundle/macos/meu/Samples/ alongside the new
//	subsections in docs/AUTHORING_MEUS_ON_MAC.md . Echoes the
//	c147 `v2_widgets_sample_test.cpp` pattern (hermetic, pure C++,
//	no engine link, no Metal, no Objective-C ; source-root injected
//	via compile-time AAASEED_SOURCE_ROOT). Per
//	memory/feedback_regression_guard_tests.md doctrine : assert the
//	v2-Phase-2/3/4 deliverables exist + are well-formed at the file-
//	system + content level, so a future session can't silently delete
//	a sample, break a README, or remove a doc subsection.
//
//	Six scenarios :
//	 Test 1 -- v2_color_picker dir + files exist.
//	 Test 2 -- v2_modal_textinput dir + files exist.
//	 Test 3 -- v2_collapse_reload dir + files exist.
//	 Test 4 -- each demo .lua references the appropriate aaa.ui.*
//	           bindings (grep).
//	 Test 5 -- each demo .lua passes `luac -p` syntax check (skip
//	           if luac is absent on the host).
//	 Test 6 -- docs/AUTHORING_MEUS_ON_MAC.md has subsections for
//	           hsv_color_picker + begin_modal + text_input +
//	           hot_reload_button + begin_collapsing_panel.

#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

namespace
{

namespace fs = std::filesystem;

#ifndef AAASEED_SOURCE_ROOT
#	error "AAASEED_SOURCE_ROOT must be defined by the CMake target."
#endif

fs::path samples_root()
{
	return fs::path( AAASEED_SOURCE_ROOT )
	     / "bundle" / "macos" / "meu" / "Samples";
}

fs::path color_picker_dir()    { return samples_root() / "v2_color_picker"; }
fs::path modal_textinput_dir() { return samples_root() / "v2_modal_textinput"; }
fs::path collapse_reload_dir() { return samples_root() / "v2_collapse_reload"; }

fs::path color_picker_lua()
{
	return color_picker_dir() / "color_picker_demo.lua";
}

fs::path modal_textinput_lua()
{
	return modal_textinput_dir() / "modal_textinput_demo.lua";
}

fs::path collapse_reload_lua()
{
	return collapse_reload_dir() / "collapse_reload_demo.lua";
}

fs::path guide_path()
{
	return fs::path( AAASEED_SOURCE_ROOT )
	     / "docs" / "AUTHORING_MEUS_ON_MAC.md";
}

std::string slurp( fs::path const& p )
{
	std::ifstream f( p );
	std::stringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

//	Per-sample dir + files + non-trivial size guard. Mirrors the c147
//	v2_widgets_sample_test pattern so any future regression hits the
//	same family of assertions.
void expect_sample_dir_and_files( fs::path const& dir,
                                  fs::path const& lua,
                                  fs::path const& md )
{
	ASSERT_TRUE( fs::exists( dir ) ) << dir;
	ASSERT_TRUE( fs::is_directory( dir ) ) << dir;

	EXPECT_TRUE( fs::is_regular_file( lua ) ) << lua;
	EXPECT_TRUE( fs::is_regular_file( md  ) ) << md;

	std::string const lua_body = slurp( lua );
	std::string const md_body  = slurp( md );
	EXPECT_GT( lua_body.size(), static_cast< size_t >( 256 ) )
		<< lua << " suspiciously small : " << lua_body.size() << " bytes";
	EXPECT_GT( md_body.size(), static_cast< size_t >( 256 ) )
		<< md << " suspiciously small : " << md_body.size() << " bytes";
}

}	//	namespace

//	Test 1 : v2_color_picker dir + canonical files exist + non-trivial.
TEST( V2Phase234Samples, ColorPickerDirHasLuaAndReadme )
{
	expect_sample_dir_and_files(
		color_picker_dir(),
		color_picker_lua(),
		color_picker_dir() / "README.md" );
}

//	Test 2 : v2_modal_textinput dir + canonical files exist + non-trivial.
TEST( V2Phase234Samples, ModalTextInputDirHasLuaAndReadme )
{
	expect_sample_dir_and_files(
		modal_textinput_dir(),
		modal_textinput_lua(),
		modal_textinput_dir() / "README.md" );
}

//	Test 3 : v2_collapse_reload dir + canonical files exist + non-trivial.
TEST( V2Phase234Samples, CollapseReloadDirHasLuaAndReadme )
{
	expect_sample_dir_and_files(
		collapse_reload_dir(),
		collapse_reload_lua(),
		collapse_reload_dir() / "README.md" );
}

//	Test 4 : each demo .lua references the appropriate `aaa.ui.*`
//	bindings (grep). If c148-A renames or drops a binding, the
//	matching sample firing here points at the right primitive to fix
//	in lockstep. 8 bindings split across the 3 demos :
//	  Phase 2 (1) : hsv_color_picker.
//	  Phase 3 (4) : show_modal, begin_modal, end_modal, text_input.
//	  Phase 4 (3) : begin_collapsing_panel, end_collapsing_panel,
//	                hot_reload_button.
TEST( V2Phase234Samples, EachDemoReferencesItsBindings )
{
	//	Phase 2 -- color picker demo.
	{
		std::string const body = slurp( color_picker_lua() );
		ASSERT_FALSE( body.empty() );
		EXPECT_NE( body.find( "aaa.ui.hsv_color_picker" ),
		           std::string::npos )
			<< "color_picker_demo.lua missing `aaa.ui.hsv_color_picker`";
	}

	//	Phase 3 -- modal + text input demo.
	{
		std::string const body = slurp( modal_textinput_lua() );
		ASSERT_FALSE( body.empty() );
		std::array< std::string_view, 4 > const bindings = {
			"aaa.ui.show_modal",
			"aaa.ui.begin_modal",
			"aaa.ui.end_modal",
			"aaa.ui.text_input",
		};
		for( auto const& b : bindings )
		{
			EXPECT_NE( body.find( b ), std::string::npos )
				<< "modal_textinput_demo.lua missing `" << b << "`";
		}
	}

	//	Phase 4 -- collapsing panel + hot reload demo.
	{
		std::string const body = slurp( collapse_reload_lua() );
		ASSERT_FALSE( body.empty() );
		std::array< std::string_view, 3 > const bindings = {
			"aaa.ui.begin_collapsing_panel",
			"aaa.ui.end_collapsing_panel",
			"aaa.ui.hot_reload_button",
		};
		for( auto const& b : bindings )
		{
			EXPECT_NE( body.find( b ), std::string::npos )
				<< "collapse_reload_demo.lua missing `" << b << "`";
		}
	}
}

//	Test 5 : each demo .lua passes `luac -p` (parse-only). Skipped on
//	hosts without luac on PATH so the target doesn't regress minimal
//	CI images. Homebrew lua on the dev workstation provides it.
TEST( V2Phase234Samples, AllThreeDemosPassLuacSyntaxCheck )
{
	int const probe = std::system( "luac -v >/dev/null 2>&1" );
	if( probe != 0 )
	{
		GTEST_SKIP() << "luac not on PATH ; skipping syntax-check test";
	}

	std::array< fs::path, 3 > const luas = {
		color_picker_lua(),
		modal_textinput_lua(),
		collapse_reload_lua(),
	};

	for( auto const& lua : luas )
	{
		std::string cmd = "luac -p \"";
		cmd += lua.string();
		cmd += "\" >/dev/null 2>&1";
		int const rc = std::system( cmd.c_str() );
		EXPECT_EQ( rc, 0 ) << "luac -p failed for " << lua;
	}
}

//	Test 6 : docs/AUTHORING_MEUS_ON_MAC.md gained the new Phase 2-4
//	subsections AND names each of the 5 NEW canonical bindings inside
//	the doc. If a future doc rewrite drops a subsection or any
//	binding, this test fires so the guide stays the canonical
//	reference. Note : Phase 1's 5 bindings are still expected to be
//	there too (per v2_widgets_sample_test Test 4) ; we don't re-check
//	them here to avoid duplicating that target's contract.
TEST( V2Phase234Samples, AuthoringGuideHasPhase234Subsections )
{
	fs::path const guide = guide_path();
	ASSERT_TRUE( fs::is_regular_file( guide ) ) << guide;

	std::string const body = slurp( guide );
	ASSERT_FALSE( body.empty() );

	//	The 5 new canonical bindings -- one per Phase-2/3/4 family
	//	highlighted by the spec.
	std::array< std::string_view, 5 > const new_bindings = {
		"aaa.ui.hsv_color_picker",
		"aaa.ui.begin_modal",
		"aaa.ui.text_input",
		"aaa.ui.hot_reload_button",
		"aaa.ui.begin_collapsing_panel",
	};

	for( auto const& b : new_bindings )
	{
		EXPECT_NE( body.find( b ), std::string::npos )
			<< "AUTHORING_MEUS_ON_MAC.md missing binding `" << b << "`";
	}
}
