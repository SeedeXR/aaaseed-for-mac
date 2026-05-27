// tests/unit/dmg_packaging_test.cpp
//
// Phase 8 (todo.md L369-L387, c137-B, 2026-05-27) : DRY-RUN validation
// of the DMG packaging CMake target.
//
// These tests do NOT invoke hdiutil ; they validate that the
// `cmake/dmg.cmake` module is internally consistent, that the
// staging contract (Applications symlink, ULMO format, volname,
// version-stamped output name) is wired correctly, and that the
// placeholder assets in `cmake/dmg_assets/` exist with the expected
// drag-to-Applications copy. Building the `aaaseed_dmg` target is
// what actually invokes hdiutil ; these tests just lock the spec.
//
// Pure C++ ; no engine deps. Uses only the standard library :
// <fstream> + <filesystem> + <string> + <cstdlib>. No `o_str`,
// no `aaa_str.h`, no `aaa_mem.h` (per c137-B scope guard).
//
// Labels (set in tests/unit/CMakeLists.txt) :
//   phase8;unit;distribution;dmg
// `phase8` is FIRST per c121-B doctrine (CTest LABELS first-only
// quirk) so `ctest -L phase8` includes this test.

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace
{

//	Source-tree root resolved from the test binary's CWD via the
//	convention that ctest is invoked from CMAKE_BINARY_DIR. We walk
//	up looking for the top-level CMakeLists.txt as a marker.
std::filesystem::path source_tree_root()
{
    namespace fs = std::filesystem;
    fs::path cur = fs::current_path();
    for( int hop = 0; hop < 8; ++hop )
    {
        //	Marker pair : top-level CMakeLists.txt + the canonical
        //	Info.plist template. The marker was bundle/macos/Info.plist
        //	(literal) before the Info.plist singularity collapse
        //	(2026-05-27) ; it is now bundle/macos/Info.plist.in (the
        //	${...}-substituted template -- the single source of truth).
        if( fs::exists( cur / "CMakeLists.txt" )
            && fs::exists( cur / "bundle" / "macos" / "Info.plist.in" ) )
        {
            return cur;
        }
        if( cur.has_parent_path() && cur.parent_path() != cur )
            cur = cur.parent_path();
        else
            break;
    }
    //	Fallback : tests are typically run from out/macos-arm64-debug ;
    //	source tree is two levels up. Hard-coded fallback only.
    return fs::current_path().parent_path().parent_path();
}

//	Slurp a small text file into a std::string. Returns empty on
//	failure ; callers check non-emptiness.
std::string read_file( std::filesystem::path const& p )
{
    std::ifstream f( p );
    if( ! f )
        return {};
    std::string content( ( std::istreambuf_iterator< char >( f ) ),
                         std::istreambuf_iterator< char >() );
    return content;
}

}   //	namespace

//	Test 1 : cmake/dmg.cmake exists in the source tree.
TEST( DmgPackagingTest, ModuleFileExists )
{
    auto const root = source_tree_root();
    auto const dmg_cmake = root / "cmake" / "dmg.cmake";
    EXPECT_TRUE( std::filesystem::exists( dmg_cmake ) )
        << "Expected " << dmg_cmake.string() << " to exist.";
}

//	Test 2 : cmake/dmg_assets/README.txt exists and carries the
//	canonical drag-to-Applications copy.
TEST( DmgPackagingTest, PlaceholderReadmeHasDragCopy )
{
    auto const root = source_tree_root();
    auto const readme = root / "cmake" / "dmg_assets" / "README.txt";
    ASSERT_TRUE( std::filesystem::exists( readme ) )
        << "Expected " << readme.string() << " to exist.";

    auto const body = read_file( readme );
    EXPECT_FALSE( body.empty() );
    EXPECT_NE( body.find( "Drag" ), std::string::npos )
        << "README.txt should mention 'Drag'.";
    EXPECT_NE( body.find( "Applications" ), std::string::npos )
        << "README.txt should mention 'Applications'.";
}

//	Test 3 : cmake/dmg.cmake references the core hdiutil invocation
//	pieces. Locks the todo.md L379 contract in a single assertion
//	block -- if a future refactor drops any of these, the test fires.
TEST( DmgPackagingTest, ModuleReferencesHdiutilContract )
{
    auto const root = source_tree_root();
    auto const dmg_cmake = root / "cmake" / "dmg.cmake";
    auto const body = read_file( dmg_cmake );

    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "hdiutil create" ), std::string::npos );
    EXPECT_NE( body.find( "-format ULMO" ),   std::string::npos );
    EXPECT_NE( body.find( "-srcfolder" ),     std::string::npos );
    EXPECT_NE( body.find( "-volname" ),       std::string::npos );
}

//	Test 4 : the Applications symlink (drag-to-install affordance)
//	is wired. Substring match on the literal -- the create_symlink
//	command in dmg.cmake names it explicitly.
TEST( DmgPackagingTest, ModuleReferencesApplicationsSymlink )
{
    auto const root = source_tree_root();
    auto const dmg_cmake = root / "cmake" / "dmg.cmake";
    auto const body = read_file( dmg_cmake );

    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "Applications" ), std::string::npos )
        << "dmg.cmake should reference the /Applications symlink target.";
}

//	Test 5 : DMG output name is version-stamped via ${PROJECT_VERSION}.
//	Guards against accidental hard-coding of "0.1.0" or similar.
TEST( DmgPackagingTest, ModuleVersionStampsOutputName )
{
    auto const root = source_tree_root();
    auto const dmg_cmake = root / "cmake" / "dmg.cmake";
    auto const body = read_file( dmg_cmake );

    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "${PROJECT_VERSION}" ), std::string::npos )
        << "dmg.cmake should derive the DMG name from PROJECT_VERSION.";
}

//	Test 6 : `hdiutil` is on PATH on this macOS host. Smoke check
//	only -- we never INVOKE it, just confirm presence. This catches
//	the absurd case of a stripped CI image where /usr/bin/hdiutil
//	is missing.
TEST( DmgPackagingTest, HdiutilAvailable )
{
    int const rc = std::system( "which hdiutil > /dev/null 2>&1" );
    EXPECT_EQ( rc, 0 ) << "hdiutil not found on PATH -- DMG build target "
                          "will fail. Reinstall Xcode command line tools.";
}

//	Test 7 : signing is OUT OF SCOPE for c137-B. dmg.cmake must NOT
//	reference codesign / notarytool ; those live in the runbook at
//	bundle/macos/README.md and require Apple Developer ID (deferred).
//	This test preserves the c136-B "ready but not executed" stance.
TEST( DmgPackagingTest, ModuleDoesNotInvokeCodesign )
{
    auto const root = source_tree_root();
    auto const dmg_cmake = root / "cmake" / "dmg.cmake";
    auto const body = read_file( dmg_cmake );

    ASSERT_FALSE( body.empty() );
    EXPECT_EQ( body.find( "codesign" ), std::string::npos )
        << "dmg.cmake must not invoke codesign ; signing is "
           "out of scope for c137-B. See bundle/macos/README.md.";
    EXPECT_EQ( body.find( "notarytool" ), std::string::npos )
        << "dmg.cmake must not invoke notarytool ; notarization is "
           "out of scope for c137-B.";
}
