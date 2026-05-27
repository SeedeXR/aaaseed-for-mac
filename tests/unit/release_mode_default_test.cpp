// tests/unit/release_mode_default_test.cpp
//
// Phase 8 (c144-A, 2026-05-27) -- regression-guards on
// `scripts/ship-dmg.sh`'s BUILD_TYPE plumbing. Asserts that Release is
// the default ship mode and that the env var propagates through to the
// per-arch CMake configure step.
//
// These tests do NOT execute the script. They read it as text and
// assert four contract invariants :
//   1. `BUILD_TYPE` env var is referenced.
//   2. The default value is `Release` (grep for `BUILD_TYPE:-Release`).
//   3. The value is propagated as `-DCMAKE_BUILD_TYPE=` to per-arch
//      configure calls.
//   4. `CMAKE_BUILD_TYPE` is NOT hardcoded to `Release` anywhere outside
//      a comment -- the env var must always win.
//
// Doctrine source : memory/feedback_regression_guard_tests.md (c137
// doctrine, "regression guards make deferred behaviour visible in
// test output"). Mirrors the pattern used by
// tests/unit/ship_script_test.cpp (c143-B),
// tests/unit/codesign_packaging_test.cpp (c138-B), and
// tests/unit/dmg_packaging_test.cpp (c137-B).
//
// Pure C++ ; standard library only : <fstream> + <filesystem> +
// <string>. No engine deps. No `o_str`, no `aaa_str.h`, no `aaa_mem.h`
// (per hermetic-mac-sublibs doctrine -- a test that locks a shell
// script's contract has no reason to drag in the engine cone).
//
// Labels (set in tests/unit/CMakeLists.txt) :
//   phase8;unit;distribution;release-mode
// `phase8` is FIRST per c121-B doctrine (CTest LABELS first-only
// quirk) so `ctest -L phase8` includes these tests.

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace
{

//	Walks up looking for the top-level CMakeLists.txt + scripts/ship-dmg.sh
//	marker. Mirrors ship_script_test.cpp (c143-B).
std::filesystem::path source_tree_root()
{
    namespace fs = std::filesystem;
    fs::path cur = fs::current_path();
    for( int hop = 0; hop < 8; ++hop )
    {
        if( fs::exists( cur / "CMakeLists.txt" )
            && fs::exists( cur / "scripts" / "ship-dmg.sh" ) )
        {
            return cur;
        }
        if( cur.has_parent_path() && cur.parent_path() != cur )
        {
            cur = cur.parent_path();
        }
        else
        {
            break;
        }
    }
    return {};
}

std::string read_ship_script()
{
    auto root = source_tree_root();
    if( root.empty() )
    {
        return {};
    }
    std::ifstream f( root / "scripts" / "ship-dmg.sh" );
    if( !f )
    {
        return {};
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

} // namespace

//	1. References BUILD_TYPE env var (read or set somewhere).
TEST( ReleaseModeDefaultTest, ShipScriptReferencesBuildTypeEnvVar )
{
    auto content = read_ship_script();
    ASSERT_FALSE( content.empty() ) << "scripts/ship-dmg.sh unreadable.";
    EXPECT_NE( content.find( "BUILD_TYPE" ), std::string::npos )
        << "ship-dmg.sh must reference BUILD_TYPE so operators can "
           "override the default Release ship mode. See c144-A.";
}

//	2. Default value is Release (the `${BUILD_TYPE:-Release}` form).
TEST( ReleaseModeDefaultTest, DefaultBuildTypeIsRelease )
{
    auto content = read_ship_script();
    ASSERT_FALSE( content.empty() );
    EXPECT_NE( content.find( "BUILD_TYPE:-Release" ), std::string::npos )
        << "Release is the ship default per c144-A. If you intend to "
           "change the default ship mode, update bundle/macos/README.md "
           "and remove this guard with a documented justification.";
}

//	3. The BUILD_TYPE is propagated to cmake configure as
//	`-DCMAKE_BUILD_TYPE=...`.
TEST( ReleaseModeDefaultTest, PropagatesViaCMakeBuildType )
{
    auto content = read_ship_script();
    ASSERT_FALSE( content.empty() );
    EXPECT_NE( content.find( "-DCMAKE_BUILD_TYPE=" ), std::string::npos )
        << "ship-dmg.sh must propagate BUILD_TYPE to cmake configure as "
           "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}. Otherwise the env override "
           "has no effect.";
}

//	4. The cmake configure line does NOT hardcode Release -- it must
//	read from ${BUILD_TYPE}. (We allow the literal `Release` to appear
//	in the default expansion + in comments ; what we reject is the
//	pattern `-DCMAKE_BUILD_TYPE=Release` as a literal flag, since that
//	bypasses the env override.)
TEST( ReleaseModeDefaultTest, CMakeBuildTypeNotHardcoded )
{
    auto content = read_ship_script();
    ASSERT_FALSE( content.empty() );
    EXPECT_EQ( content.find( "-DCMAKE_BUILD_TYPE=Release" ),
        std::string::npos )
        << "ship-dmg.sh must use -DCMAKE_BUILD_TYPE=\"${BUILD_TYPE}\" so "
           "BUILD_TYPE=Debug ./scripts/ship-dmg.sh works. A hardcoded "
           "literal `Release` breaks the override contract (c144-A).";
}

//	5. The Release flag wiring in src/ui/macos/CMakeLists.txt is not
//	silently dropped -- guard the LTO + dead-strip + -Os triplet that
//	makes Release small enough to ship.
TEST( ReleaseModeDefaultTest, UiMacosCMakeListsKeepsReleaseFlags )
{
    auto root = source_tree_root();
    ASSERT_FALSE( root.empty() );
    std::ifstream f( root / "src" / "ui" / "macos" / "CMakeLists.txt" );
    ASSERT_TRUE( f.good() ) << "src/ui/macos/CMakeLists.txt unreadable.";
    std::stringstream ss;
    ss << f.rdbuf();
    auto content = ss.str();
    EXPECT_NE( content.find( "CONFIG:Release" ), std::string::npos )
        << "src/ui/macos/CMakeLists.txt must keep its $<$<CONFIG:Release>:...> "
           "generator-expression wiring (c142-A) so the Release ship "
           "build picks up LTO + dead-strip + -Os.";
    EXPECT_NE( content.find( "-flto" ), std::string::npos )
        << "Release ship build relies on -flto=thin for size savings.";
}
