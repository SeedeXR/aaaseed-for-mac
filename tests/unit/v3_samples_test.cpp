//	tests/unit/v3_samples_test.cpp
//
//	c149-B v3 : regression-guard for the THREE new v3 sample MEUs
//	(bundle/macos/meu/Samples/v3_drag_drop/ ,
//	v3_preset_save_load/ , v3_nested_panels/) + the new section 4
//	in docs/AUTHORING_MEUS_ON_MAC.md + the "v3 -- LANDED (c149)"
//	subsection in memory/project_v1_ship_gate.md . Echoes the
//	c148 `v2_phase234_samples_test.cpp` pattern (hermetic, pure
//	C++, no engine link, no Metal, no Objective-C ; source-root
//	injected via compile-time AAASEED_SOURCE_ROOT). Per
//	memory/feedback_regression_guard_tests.md doctrine : assert
//	the v3 deliverables exist + are well-formed at the file-
//	system + content level, so a future session can't silently
//	delete a sample, break a README, remove a doc section, or
//	drop the v3 LANDED subsection from the ship-gate.
//
//	Five scenarios :
//	 Test 1 -- 3 new sample dirs exist with <name>.lua + README.md.
//	 Test 2 -- each demo .lua references the new `aaa.io.*` or
//	           nested-collapsing tokens (grep).
//	 Test 3 -- each demo .lua passes `luac -p` syntax check
//	           (skip if luac is absent on the host).
//	 Test 4 -- docs/AUTHORING_MEUS_ON_MAC.md has section 4
//	           `aaa.io.*` AND lists all 5 new bindings.
//	 Test 5 -- memory/project_v1_ship_gate.md has the
//	           "v3 -- LANDED" section AND mentions IME deferral.

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

fs::path drag_drop_dir()        { return samples_root() / "v3_drag_drop"; }
fs::path preset_save_load_dir() { return samples_root() / "v3_preset_save_load"; }
fs::path nested_panels_dir()    { return samples_root() / "v3_nested_panels"; }

fs::path drag_drop_lua()
{
	return drag_drop_dir() / "drag_drop_demo.lua";
}

fs::path preset_demo_lua()
{
	return preset_save_load_dir() / "preset_demo.lua";
}

fs::path nested_panels_lua()
{
	return nested_panels_dir() / "nested_panels_demo.lua";
}

fs::path guide_path()
{
	return fs::path( AAASEED_SOURCE_ROOT )
	     / "docs" / "AUTHORING_MEUS_ON_MAC.md";
}

fs::path ship_gate_path()
{
	return fs::path( AAASEED_SOURCE_ROOT )
	     / "memory" / "project_v1_ship_gate.md";
}

std::string slurp( fs::path const& p )
{
	std::ifstream f( p );
	std::stringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

//	Per-sample dir + files + non-trivial size guard. Mirrors the
//	c148 v2_phase234_samples_test pattern so any future regression
//	hits the same family of assertions.
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

//	Test 1 : all 3 new v3 sample dirs exist with <name>.lua + README.md.
//	If a future agent deletes a sample dir or renames a file this fires
//	in isolation so the failure points at the right place.
TEST( V3Samples, AllThreeSampleDirsExistWithLuaAndReadme )
{
	expect_sample_dir_and_files(
		drag_drop_dir(),
		drag_drop_lua(),
		drag_drop_dir() / "README.md" );

	expect_sample_dir_and_files(
		preset_save_load_dir(),
		preset_demo_lua(),
		preset_save_load_dir() / "README.md" );

	expect_sample_dir_and_files(
		nested_panels_dir(),
		nested_panels_lua(),
		nested_panels_dir() / "README.md" );
}

//	Test 2 : each demo .lua references at least one new `aaa.io.*`
//	binding or the nested-collapsing call. If c149-A renames or drops
//	a binding, the matching sample firing here points at the right
//	primitive to fix in lockstep. Coverage split across the 3 demos :
//	  drag_drop      : aaa.io.open_file_dialog .
//	  preset_demo    : aaa.io.save_file_dialog +
//	                   aaa.io.save_preset + aaa.io.load_preset +
//	                   aaa.io.open_file_dialog .
//	  nested_panels  : two `aaa.ui.begin_collapsing_panel` calls
//	                   nested inside a parent collapsing panel
//	                   (the v3 nesting capability).
TEST( V3Samples, EachDemoReferencesIts_v3_Bindings )
{
	//	drag-drop demo.
	{
		std::string const body = slurp( drag_drop_lua() );
		ASSERT_FALSE( body.empty() );
		EXPECT_NE( body.find( "aaa.io.open_file_dialog" ),
		           std::string::npos )
			<< "drag_drop_demo.lua missing `aaa.io.open_file_dialog`";
	}

	//	preset save/load demo -- 4 of the 5 new aaa.io.* bindings.
	{
		std::string const body = slurp( preset_demo_lua() );
		ASSERT_FALSE( body.empty() );
		std::array< std::string_view, 4 > const bindings = {
			"aaa.io.save_file_dialog",
			"aaa.io.open_file_dialog",
			"aaa.io.save_preset",
			"aaa.io.load_preset",
		};
		for( auto const& b : bindings )
		{
			EXPECT_NE( body.find( b ), std::string::npos )
				<< "preset_demo.lua missing `" << b << "`";
		}
	}

	//	nested-panels demo -- the v3 nesting capability. Assert there
	//	are AT LEAST 3 `begin_collapsing_panel` calls (1 parent
	//	"Effects" + 2 nested children "Color" + "Bloom", plus a
	//	sibling "Atmosphere") so a regression that collapses the demo
	//	to a single panel fires here.
	{
		std::string const body = slurp( nested_panels_lua() );
		ASSERT_FALSE( body.empty() );

		std::string_view const tok = "aaa.ui.begin_collapsing_panel";
		size_t pos = 0;
		int count = 0;
		while( ( pos = body.find( tok, pos ) ) != std::string::npos )
		{
			++count;
			pos += tok.size();
		}
		EXPECT_GE( count, 3 )
			<< "nested_panels_demo.lua should call "
			<< "`aaa.ui.begin_collapsing_panel` at least 3 times "
			<< "(parent + 2 nested) ; saw " << count;
	}
}

//	Test 3 : each demo .lua passes `luac -p` (parse-only). Skipped on
//	hosts without luac on PATH so the target doesn't regress minimal
//	CI images. Homebrew lua on the dev workstation provides it.
TEST( V3Samples, AllThreeDemosPassLuacSyntaxCheck )
{
	int const probe = std::system( "luac -v >/dev/null 2>&1" );
	if( probe != 0 )
	{
		GTEST_SKIP() << "luac not on PATH ; skipping syntax-check test";
	}

	std::array< fs::path, 3 > const luas = {
		drag_drop_lua(),
		preset_demo_lua(),
		nested_panels_lua(),
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

//	Test 4 : docs/AUTHORING_MEUS_ON_MAC.md gained the new section 4
//	"aaa.io.* file I/O API (v3)" AND names each of the 5 new bindings
//	inside the doc. If a future doc rewrite drops the section or any
//	binding, this test fires so the guide stays the canonical
//	reference.
TEST( V3Samples, AuthoringGuideHasSection4AndAllFiveBindings )
{
	fs::path const guide = guide_path();
	ASSERT_TRUE( fs::is_regular_file( guide ) ) << guide;

	std::string const body = slurp( guide );
	ASSERT_FALSE( body.empty() );

	//	Section 4 heading -- exact text.
	EXPECT_NE(
		body.find( "## 4. `aaa.io.*` file I/O API (v3)" ),
		std::string::npos )
		<< "AUTHORING_MEUS_ON_MAC.md missing section 4 heading";

	//	The 5 canonical new bindings -- one per spec entry.
	std::array< std::string_view, 5 > const new_bindings = {
		"aaa.io.drop_file",
		"aaa.io.open_file_dialog",
		"aaa.io.save_file_dialog",
		"aaa.io.save_preset",
		"aaa.io.load_preset",
	};

	for( auto const& b : new_bindings )
	{
		EXPECT_NE( body.find( b ), std::string::npos )
			<< "AUTHORING_MEUS_ON_MAC.md missing binding `" << b << "`";
	}
}

//	Test 5 : memory/project_v1_ship_gate.md has the "v3 -- LANDED"
//	subsection AND mentions the IME v4 deferral. The pair of
//	assertions catches both "agent forgot to land v3 record" and
//	"agent forgot to defer IME formally" regressions in one place.
TEST( V3Samples, ShipGateHas_v3_LandedSection_And_v4_IME_Deferral )
{
	fs::path const sg = ship_gate_path();
	ASSERT_TRUE( fs::is_regular_file( sg ) ) << sg;

	std::string const body = slurp( sg );
	ASSERT_FALSE( body.empty() );

	//	v3 LANDED section heading -- accept either ASCII hyphen
	//	form (`v3 --` or `v3 -`) but require the LANDED keyword +
	//	the c149 session tag.
	EXPECT_NE( body.find( "v3" ), std::string::npos )
		<< "ship-gate missing any v3 reference";
	EXPECT_NE( body.find( "LANDED (c149)" ), std::string::npos )
		<< "ship-gate missing `LANDED (c149)` tag for v3 record";

	//	IME deferral memo : require both the IME keyword AND the
	//	v4 milestone reference so a "we'll get to IME someday"
	//	hand-wave isn't enough -- the deferral has to be formal.
	EXPECT_NE( body.find( "IME" ), std::string::npos )
		<< "ship-gate missing IME keyword for v4 deferral";
	EXPECT_NE( body.find( "v4" ), std::string::npos )
		<< "ship-gate missing v4 milestone reference for IME deferral";
	EXPECT_NE( body.find( "NSTextInputClient" ), std::string::npos )
		<< "ship-gate missing NSTextInputClient protocol citation "
		<< "for the IME v4 deferral rationale";
}
