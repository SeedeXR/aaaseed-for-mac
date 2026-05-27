// tests/unit/verify_dmg_script_test.cpp
//
// Phase 8 (c145, 2026-05-27) -- DRY-RUN regression-guards on
// `scripts/verify-dmg.sh`, the inverse of scripts/ship-dmg.sh. Where
// ship-dmg.sh BUILDS the DMG, verify-dmg.sh is shipped alongside
// SHIP_CHECKLIST.md so downstream users can confirm a received DMG is
// structurally valid before installing.
//
// These tests do NOT execute the script. They read it as text and
// assert the contract :
//   1. The script exists in the source tree and is owner-executable.
//   2. It enables bash strict mode (`set -euo pipefail`).
//   3. It references each of the critical structural artifacts it must
//      check : `AAASeed.app`, `Applications`, `hello_world.lua`,
//      `lipo -info`, `hdiutil verify`, `plutil -lint`.
//   4. REGRESSION GUARD -- it does NOT reference `codesign`. Structural
//      verification is the scope ; signature verification is a
//      separate concern (codesign --verify + spctl --assess). Mixing
//      the two in one script muddles the exit-code contract and
//      requires a Developer ID even when you only want to confirm the
//      DMG's bytes survived the download. Per
//      memory/feedback_regression_guard_tests.md (c137 doctrine,
//      "regression guards make deferred behaviour visible in test
//      output").
//
// Doctrine source : memory/feedback_regression_guard_tests.md. Mirrors
// the pattern in tests/unit/ship_script_test.cpp (c143-B) and
// tests/unit/codesign_packaging_test.cpp (c138-B).
//
// Pure C++ ; standard library only : <fstream> + <filesystem> +
// <string>. No engine deps. No `o_str`, no `aaa_str.h`, no `aaa_mem.h`
// (hermetic-mac-sublibs doctrine -- a test that locks a shell script's
// contract has no reason to drag in the engine cone).
//
// Labels (set in tests/unit/CMakeLists.txt) :
//   phase8;unit;distribution;verify-dmg
// `phase8` is FIRST per c121-B doctrine (CTest LABELS first-only
// quirk) so `ctest -L phase8` includes these tests.

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <sys/stat.h>

namespace
{

//	Source-tree root resolver -- walks up looking for the top-level
//	CMakeLists.txt + scripts/verify-dmg.sh marker. Mirrors ship_script_test.cpp.
std::filesystem::path source_tree_root()
{
    namespace fs = std::filesystem;
    fs::path cur = fs::current_path();
    for( int hop = 0; hop < 8; ++hop )
    {
        if( fs::exists( cur / "CMakeLists.txt" )
            && fs::exists( cur / "scripts" / "verify-dmg.sh" ) )
        {
            return cur;
        }
        if( cur.has_parent_path() && cur.parent_path() != cur )
            cur = cur.parent_path();
        else
            break;
    }
    //	Fallback : tests run from out/macos-arm64-debug ; source is
    //	two levels up from there.
    return fs::current_path().parent_path().parent_path();
}

std::filesystem::path verify_script_path()
{
    return source_tree_root() / "scripts" / "verify-dmg.sh";
}

std::string read_file( std::filesystem::path const& p )
{
    std::ifstream f( p );
    if( ! f )
        return {};
    std::string content( ( std::istreambuf_iterator< char >( f ) ),
                         std::istreambuf_iterator< char >() );
    return content;
}

//	Helper : strip comment lines (lines whose first non-whitespace char
//	is `#`) from a shell script body. Lets test 4 search ONLY runtime
//	code for `codesign` without false positives from explanatory header
//	comments.
std::string strip_shell_comments( std::string const& body )
{
    std::string out;
    out.reserve( body.size() );
    std::size_t pos = 0;
    while( pos < body.size() )
    {
        std::size_t const eol = body.find( '\n', pos );
        std::size_t const line_end = ( eol == std::string::npos )
                                         ? body.size()
                                         : eol;
        //	Find first non-whitespace char on this line.
        std::size_t first_non_ws = pos;
        while( first_non_ws < line_end
               && ( body[ first_non_ws ] == ' '
                    || body[ first_non_ws ] == '\t' ) )
            ++first_non_ws;
        bool const is_comment = ( first_non_ws < line_end
                                  && body[ first_non_ws ] == '#' );
        if( ! is_comment )
        {
            out.append( body, pos, line_end - pos );
        }
        out.push_back( '\n' );
        if( eol == std::string::npos )
            break;
        pos = eol + 1;
    }
    return out;
}

}   //	namespace

//	Test 1 : scripts/verify-dmg.sh exists AND has the owner-executable
//	bit set. The script is shipped to downstream users who will run
//	`./scripts/verify-dmg.sh path/to/AAASeed.dmg` directly ; without
//	+x they have to remember `bash scripts/...`, breaking the
//	"one-command verification" UX promised in SHIP_CHECKLIST.md.
TEST( VerifyDmgScriptTest, ExistsAndExecutable )
{
    auto const p = verify_script_path();
    ASSERT_TRUE( std::filesystem::exists( p ) )
        << "Expected " << p.string() << " to exist.";

    struct stat st = {};
    ASSERT_EQ( ::stat( p.c_str(), &st ), 0 )
        << "stat() on verify-dmg.sh failed.";
    EXPECT_NE( st.st_mode & S_IXUSR, 0u )
        << "scripts/verify-dmg.sh is missing the owner-executable bit. "
           "Run `chmod +x scripts/verify-dmg.sh` to restore.";
}

//	Test 2 : script enables bash strict mode. The DMG-verification
//	pipeline calls hdiutil + lipo + plutil + find ; without -e a silent
//	failure mid-pipeline would still report success.
TEST( VerifyDmgScriptTest, StrictModeEnabled )
{
    auto const body = read_file( verify_script_path() );
    ASSERT_FALSE( body.empty() );

    bool const has_eu_pipefail = body.find( "set -euo pipefail" ) != std::string::npos;
    bool const has_separate =
        body.find( "set -e" ) != std::string::npos &&
        body.find( "set -u" ) != std::string::npos &&
        body.find( "pipefail" ) != std::string::npos;

    EXPECT_TRUE( has_eu_pipefail || has_separate )
        << "verify-dmg.sh should enable strict mode "
           "(`set -euo pipefail` or equivalent). Without -e a failed "
           "hdiutil or lipo call would not abort the pipeline and the "
           "script could report success on a broken DMG.";
}

//	Test 3 : script references all the critical structural artifacts.
//	If any of these grep tokens vanishes, the corresponding verification
//	step has been silently dropped from the pipeline and a broken DMG
//	might pass the script unnoticed.
TEST( VerifyDmgScriptTest, ReferencesAllCriticalArtifacts )
{
    auto const body = read_file( verify_script_path() );
    ASSERT_FALSE( body.empty() );

    EXPECT_NE( body.find( "AAASeed.app" ), std::string::npos )
        << "verify-dmg.sh should reference AAASeed.app (the bundle the DMG ships).";
    EXPECT_NE( body.find( "Applications" ), std::string::npos )
        << "verify-dmg.sh should reference the Applications symlink "
           "(required for the documented drag-and-drop install).";
    EXPECT_NE( body.find( "hello_world.lua" ), std::string::npos )
        << "verify-dmg.sh should verify Resources/meu/hello_world.lua "
           "(the MEU runner's bundled script ; absence indicates a "
           "broken bundle wiring).";
    EXPECT_NE( body.find( "lipo -info" ), std::string::npos )
        << "verify-dmg.sh should run `lipo -info` to confirm BOTH arm64 "
           "AND x86_64 slices are present (universal-binary mandate).";
    EXPECT_NE( body.find( "hdiutil verify" ), std::string::npos )
        << "verify-dmg.sh should run `hdiutil verify` on the DMG "
           "(catches a corrupt-during-download payload).";
    EXPECT_NE( body.find( "plutil -lint" ), std::string::npos )
        << "verify-dmg.sh should run `plutil -lint` on the bundle "
           "Info.plist (catches a malformed plist that Gatekeeper "
           "would later reject).";
}

//	Test 4 -- REGRESSION GUARD (memory/feedback_regression_guard_tests.md).
//	verify-dmg.sh must NOT reference `codesign`. Structural verification
//	is the scope ; signature verification is a SEPARATE concern handled
//	by codesign --verify + spctl --assess (and requires a Developer ID,
//	which is intentionally absent from the v1 unsigned ship). Mixing the
//	two in one script muddles the exit-code contract per the script's
//	header documentation. If a future session adds a codesign call
//	here, this test fires with a pointer back to the doctrine.
TEST( VerifyDmgScriptTest, DoesNotReferenceCodesign )
{
    auto const body = read_file( verify_script_path() );
    ASSERT_FALSE( body.empty() );

    //	Strip comments first -- the explanatory header is allowed to
    //	MENTION codesign as a non-scope concern. The test guards
    //	against runtime invocations only.
    std::string const code = strip_shell_comments( body );

    EXPECT_EQ( code.find( "codesign" ), std::string::npos )
        << "verify-dmg.sh contains a non-comment reference to "
           "`codesign`. REGRESSION : verify-dmg.sh is the STRUCTURAL "
           "half of post-receipt verification ; signature verification "
           "is a separate concern (codesign --verify + spctl --assess) "
           "and requires a Developer ID. Move signature checks to a "
           "different script. See memory/feedback_regression_guard_tests.md "
           "(c137 doctrine) and the script's own header comments.";
}
