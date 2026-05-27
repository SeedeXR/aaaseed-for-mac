//	tests/unit/meu_samples_test.cpp
//
//	c145-D : regression-guard for the v1 MEU sample library +
//	authoring guide. Asserts the 5 starter MEUs at
//	`bundle/macos/meu/Samples/` are well-formed and the authoring guide
//	at `docs/AUTHORING_MEUS_ON_MAC.md` references every binding the
//	runner installs. Pure C++ ; no engine link, no Metal, no
//	Objective-C. Hermetic per memory/feedback_hermetic_mac_sublibs.md .
//
//	The source-root location is compile-time injected via
//	`AAASEED_SOURCE_ROOT` (set in tests/unit/CMakeLists.txt) so the
//	test binary works regardless of the CTest working directory.

#include <gtest/gtest.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace
{

namespace fs = std::filesystem;

#ifndef AAASEED_SOURCE_ROOT
#	error "AAASEED_SOURCE_ROOT must be defined by the CMake target."
#endif

//	The 5 v1 sample MEUs. Each entry is the folder name -- the .lua
//	file inside MUST be `<folder>.lua` (Test 2).
constexpr std::array< std::string_view, 5 > kSampleNames = {
	"mire",
	"animator",
	"keyboard",
	"mouse",
	"composer",
};

fs::path samples_root()
{
	return fs::path( AAASEED_SOURCE_ROOT ) / "bundle" / "macos" / "meu" / "Samples";
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

//	Test 1 : `bundle/macos/meu/Samples/` exists + has at least the 5
//	canonical c146 sub-directories (forward-compatible: c147-B added a
//	6th sample `v2_widgets/`, future sessions may add more).
TEST( MeuSamples, SamplesRootExistsWithAtLeastFiveSubdirs )
{
	fs::path const root = samples_root();
	ASSERT_TRUE( fs::exists( root ) ) << root;
	ASSERT_TRUE( fs::is_directory( root ) );

	int subdir_count = 0;
	for( auto const& entry : fs::directory_iterator( root ) )
	{
		if( entry.is_directory() ) ++subdir_count;
	}
	EXPECT_GE( subdir_count, 5 );

	//	Each canonical c146 name MUST still resolve to a directory.
	for( auto const& name : kSampleNames )
	{
		fs::path const p = root / std::string( name );
		EXPECT_TRUE( fs::is_directory( p ) ) << p;
	}
}

//	Test 2 : each sample has `<name>.lua` + `README.md`.
TEST( MeuSamples, EachSampleHasLuaAndReadme )
{
	fs::path const root = samples_root();
	for( auto const& name : kSampleNames )
	{
		fs::path const dir = root / std::string( name );
		fs::path const lua = dir / ( std::string( name ) + ".lua" );
		fs::path const md  = dir / "README.md";
		EXPECT_TRUE( fs::is_regular_file( lua ) ) << lua;
		EXPECT_TRUE( fs::is_regular_file( md  ) ) << md;
	}
}

//	Test 3 : each sample's .lua is non-empty + references at least one
//	`aaa.*` binding. We grep for the literal substring `aaa.` since the
//	bindings live under that namespace.
TEST( MeuSamples, EachLuaReferencesAaaBindings )
{
	fs::path const root = samples_root();
	for( auto const& name : kSampleNames )
	{
		fs::path const lua = root / std::string( name )
		                   / ( std::string( name ) + ".lua" );
		std::string const body = slurp( lua );
		EXPECT_FALSE( body.empty() ) << lua;
		EXPECT_NE( body.find( "aaa." ), std::string::npos )
			<< lua << " : no `aaa.` binding reference found";
	}
}

//	Test 4 : each sample's .lua passes `luac -p` syntax check. Skipped
//	gracefully on hosts where luac is absent (e.g. minimal CI images) ;
//	the homebrew default install on macOS provides it at
//	/opt/homebrew/bin/luac .
TEST( MeuSamples, EachLuaPassesLuacSyntaxCheck )
{
	//	Probe luac availability ; skip the test if not present so this
	//	target doesn't regress hosts without luac. Production CI on the
	//	dev workstation has luac via homebrew lua.
	int const probe = std::system( "luac -v >/dev/null 2>&1" );
	if( probe != 0 )
	{
		GTEST_SKIP() << "luac not on PATH ; skipping syntax-check test";
	}

	fs::path const root = samples_root();
	for( auto const& name : kSampleNames )
	{
		fs::path const lua = root / std::string( name )
		                   / ( std::string( name ) + ".lua" );
		std::string cmd = "luac -p \"";
		cmd += lua.string();
		cmd += "\" >/dev/null 2>&1";
		int const rc = std::system( cmd.c_str() );
		EXPECT_EQ( rc, 0 ) << "luac -p failed for " << lua;
	}
}

//	Test 5 : no sample references Windows-only constructs that would
//	indicate a literal copy from the Win32 kernel won't run on the Mac
//	runner. The Win-only markers are :
//		- `mu_`         (vendor MU-class prefix)
//		- `bu_`         (vendor BU widget class prefix)
//		- `c_layer`     (Layer subsystem, c144-B superseded)
//		- `CLASS.DECLARE` (vendor class-declaration macro)
//		- `add_param_obj_name` / `add_button` / `add_bu_*` (GaBu widgets)
TEST( MeuSamples, NoWindowsOnlyConstructs )
{
	std::array< std::string_view, 7 > const forbidden = {
		"mu_",
		"bu_",
		"c_layer",
		"CLASS.DECLARE",
		"add_param_obj_name",
		"add_button",
		"add_bu_",
	};

	fs::path const root = samples_root();
	for( auto const& name : kSampleNames )
	{
		fs::path const lua = root / std::string( name )
		                   / ( std::string( name ) + ".lua" );
		std::string const body = slurp( lua );
		for( auto const& tok : forbidden )
		{
			EXPECT_EQ( body.find( tok ), std::string::npos )
				<< lua << " contains Windows-only token `" << tok
				<< "` ; would not run on the Mac runner.";
		}
	}
}

//	Test 6 : the authoring guide exists + names every canonical `aaa.*`
//	binding the runner installs. Sourced from
//	src/meu/aaa_meu_runner_mac.mm `install_aaa_bindings()` (the bind()
//	call sites). If the runner gains a new binding, this list should
//	grow alongside the doc edit.
TEST( MeuSamples, AuthoringGuideReferencesAllBindings )
{
	fs::path const guide = guide_path();
	ASSERT_TRUE( fs::is_regular_file( guide ) ) << guide;

	std::string const body = slurp( guide );
	ASSERT_FALSE( body.empty() );

	std::array< std::string_view, 12 > const bindings = {
		"aaa.use_shader",
		"aaa.set_uniform_float",
		"aaa.set_uniform_vec4",
		"aaa.set_uniform_int",
		"aaa.set_bind_texture",
		"aaa.draw_fullscreen_quad",
		"aaa.draw_hud_text",
		"aaa.log",
		"aaa.frame_index",
		"aaa.time",
		"aaa.key_down",
		"aaa.mouse_xy",
	};

	for( auto const& b : bindings )
	{
		EXPECT_NE( body.find( b ), std::string::npos )
			<< "AUTHORING_MEUS_ON_MAC.md missing binding `" << b << "`";
	}
}
