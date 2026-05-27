//	tests/unit/v2_widgets_sample_test.cpp
//
//	c147 v2 Phase 1 : regression-guard for the canonical widgets demo
//	MEU + the new "aaa.ui.* widget API" section in the authoring guide.
//	Echoes the c145-D `meu_samples_test.cpp` pattern (hermetic, pure
//	C++, no engine link, no Metal, no Objective-C ; source-root
//	injected via compile-time AAASEED_SOURCE_ROOT). Per
//	memory/feedback_regression_guard_tests.md doctrine : assert the
//	v2-Phase-1 deliverables exist + are well-formed at the file
//	system + content level, so a future session can't silently delete
//	the sample, break the README, or remove the doc section.
//
//	Four scenarios :
//	 Test 1 -- sample directory + files exist.
//	 Test 2 -- sample .lua references every one of the 5 `aaa.ui.*`
//	           bindings (grep).
//	 Test 3 -- sample .lua passes `luac -p` syntax check (skip if
//	           luac is absent on the host).
//	 Test 4 -- authoring guide has the new "aaa.ui.* widget API"
//	           section heading.

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

fs::path widgets_sample_dir()
{
	return fs::path( AAASEED_SOURCE_ROOT )
	     / "bundle" / "macos" / "meu" / "Samples" / "v2_widgets";
}

fs::path widgets_lua_path()
{
	return widgets_sample_dir() / "widgets_demo.lua";
}

fs::path widgets_readme_path()
{
	return widgets_sample_dir() / "README.md";
}

fs::path guide_path()
{
	return fs::path( AAASEED_SOURCE_ROOT ) / "docs" / "AUTHORING_MEUS_ON_MAC.md";
}

std::string slurp( fs::path const& p )
{
	std::ifstream f( p );
	std::stringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

}	//	namespace

//	Test 1 : the sample directory exists + contains both `widgets_demo.lua`
//	and `README.md`. Mirrors meu_samples_test Test 2 ; ensures the c147
//	deliverable did not silently regress to "lua-only" or "readme-only".
TEST( V2WidgetsSample, SampleDirectoryHasLuaAndReadme )
{
	fs::path const dir = widgets_sample_dir();
	ASSERT_TRUE( fs::exists( dir ) ) << dir;
	ASSERT_TRUE( fs::is_directory( dir ) ) << dir;

	fs::path const lua = widgets_lua_path();
	fs::path const md  = widgets_readme_path();
	EXPECT_TRUE( fs::is_regular_file( lua ) ) << lua;
	EXPECT_TRUE( fs::is_regular_file( md  ) ) << md;

	//	Non-trivial content guard so an empty-file regression also fails.
	std::string const lua_body = slurp( lua );
	std::string const md_body  = slurp( md );
	EXPECT_GT( lua_body.size(), static_cast< size_t >( 256 ) )
		<< "widgets_demo.lua suspiciously small : " << lua_body.size() << " bytes";
	EXPECT_GT( md_body.size(), static_cast< size_t >( 256 ) )
		<< "v2_widgets/README.md suspiciously small : " << md_body.size() << " bytes";
}

//	Test 2 : the sample lua references ALL FIVE `aaa.ui.*` bindings.
//	If c147-A drops a binding or renames one, this test fires so the
//	sample can be updated in lockstep. Plain substring grep ; no Lua
//	interpreter required.
TEST( V2WidgetsSample, LuaReferencesAllFiveUiBindings )
{
	fs::path const lua = widgets_lua_path();
	ASSERT_TRUE( fs::is_regular_file( lua ) ) << lua;

	std::string const body = slurp( lua );
	ASSERT_FALSE( body.empty() );

	std::array< std::string_view, 5 > const bindings = {
		"aaa.ui.begin_panel",
		"aaa.ui.button",
		"aaa.ui.slider",
		"aaa.ui.color_well",
		"aaa.ui.end_panel",
	};

	for( auto const& b : bindings )
	{
		EXPECT_NE( body.find( b ), std::string::npos )
			<< "widgets_demo.lua missing `" << b << "` reference";
	}
}

//	Test 3 : the sample lua passes `luac -p` (parse-only) syntax check.
//	Skipped on hosts without luac on PATH so this target doesn't
//	regress minimal CI images. Homebrew lua on the dev workstation
//	provides it.
TEST( V2WidgetsSample, LuaPassesLuacSyntaxCheck )
{
	int const probe = std::system( "luac -v >/dev/null 2>&1" );
	if( probe != 0 )
	{
		GTEST_SKIP() << "luac not on PATH ; skipping syntax-check test";
	}

	fs::path const lua = widgets_lua_path();
	std::string cmd = "luac -p \"";
	cmd += lua.string();
	cmd += "\" >/dev/null 2>&1";
	int const rc = std::system( cmd.c_str() );
	EXPECT_EQ( rc, 0 ) << "luac -p failed for " << lua;
}

//	Test 4 : the authoring guide gained the new "aaa.ui.* widget API"
//	section AND names each of the 5 bindings inside that section. If
//	a future doc rewrite drops the section heading or any binding,
//	this test fires so the guide stays the canonical reference.
TEST( V2WidgetsSample, AuthoringGuideHasWidgetApiSection )
{
	fs::path const guide = guide_path();
	ASSERT_TRUE( fs::is_regular_file( guide ) ) << guide;

	std::string const body = slurp( guide );
	ASSERT_FALSE( body.empty() );

	//	The new section heading. The actual heading is
	//	`## 3. \`aaa.ui.*\` widget API (v2 Phase 1)` (markdown backticks
	//	around the namespace), so we grep for the trailing distinctive
	//	substring that uniquely identifies the section without being
	//	sensitive to the inline-code formatting.
	EXPECT_NE( body.find( "widget API (v2 Phase 1)" ),
	           std::string::npos )
		<< "AUTHORING_MEUS_ON_MAC.md missing v2 Phase 1 widget section heading";

	//	Same five bindings as Test 2, but in the doc this time.
	std::array< std::string_view, 5 > const bindings = {
		"aaa.ui.begin_panel",
		"aaa.ui.button",
		"aaa.ui.slider",
		"aaa.ui.color_well",
		"aaa.ui.end_panel",
	};

	for( auto const& b : bindings )
	{
		EXPECT_NE( body.find( b ), std::string::npos )
			<< "AUTHORING_MEUS_ON_MAC.md missing binding `" << b << "`";
	}
}
