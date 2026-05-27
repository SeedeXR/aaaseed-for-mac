// tests/unit/ship_script_test.cpp
//
// Phase 8 (todo.md L379-L387, c143-B, 2026-05-27) -- DRY-RUN
// regression-guards on `scripts/ship-dmg.sh`, the universal-binary
// ship pipeline.
//
// These tests do NOT execute the script. They read it as text and
// assert the contract : strict-mode flags, lipo invocation, three
// compression-tier references (ULMO / ULFO / UDBZ), env-gated
// codesign + notarize, no hardcoded Developer ID identity, default
// ARCHES list covers BOTH arm64 + x86_64. Locks the behavioural
// surface so a future session can't silently drift the pipeline.
//
// Doctrine source : memory/feedback_regression_guard_tests.md (c137
// doctrine, "regression guards make deferred behaviour visible in
// test output"). Mirrors the pattern used by
// tests/unit/codesign_packaging_test.cpp (c138-B) and
// tests/unit/dmg_packaging_test.cpp (c137-B).
//
// Pure C++ ; standard library only : <fstream> + <filesystem> +
// <string>. No engine deps. No `o_str`, no `aaa_str.h`, no `aaa_mem.h`
// (per hermetic-mac-sublibs doctrine -- a test that locks a shell
// script's contract has no reason to drag in the engine cone).
//
// Labels (set in tests/unit/CMakeLists.txt) :
//   phase8;unit;distribution;ship-script
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
//	CMakeLists.txt + scripts/ship-dmg.sh marker. Mirrors c138-B
//	codesign_packaging_test.cpp + c137-B dmg_packaging_test.cpp.
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
            cur = cur.parent_path();
        else
            break;
    }
    //	Fallback : tests run from out/macos-arm64-debug ; source is
    //	two levels up from there.
    return fs::current_path().parent_path().parent_path();
}

std::filesystem::path ship_script_path()
{
    return source_tree_root() / "scripts" / "ship-dmg.sh";
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

}   //	namespace

//	Test 1 : scripts/ship-dmg.sh exists in the source tree AND has the
//	executable bit set on the owner permission. The script will be
//	invoked directly (./scripts/ship-dmg.sh) by operators ; without the
//	+x bit they would have to remember `bash scripts/...`.
TEST( ShipScriptTest, ExistsAndExecutable )
{
    auto const p = ship_script_path();
    ASSERT_TRUE( std::filesystem::exists( p ) )
        << "Expected " << p.string() << " to exist.";

    struct stat st = {};
    ASSERT_EQ( ::stat( p.c_str(), &st ), 0 )
        << "stat() on ship-dmg.sh failed.";
    EXPECT_NE( st.st_mode & S_IXUSR, 0u )
        << "scripts/ship-dmg.sh is missing the owner-executable bit. "
           "Run `chmod +x scripts/ship-dmg.sh` to restore.";
}

//	Test 2 : script enables bash strict mode. We probe for both the
//	canonical `set -euo pipefail` one-liner AND each individual option,
//	since some maintainers split them across multiple lines.
TEST( ShipScriptTest, StrictModeEnabled )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );

    bool const has_eu_pipefail = body.find( "set -euo pipefail" ) != std::string::npos;
    bool const has_separate =
        body.find( "set -e" ) != std::string::npos &&
        body.find( "set -u" ) != std::string::npos &&
        body.find( "pipefail" ) != std::string::npos;

    EXPECT_TRUE( has_eu_pipefail || has_separate )
        << "ship-dmg.sh should enable strict mode "
           "(`set -euo pipefail` or equivalent). Without -e a failed "
           "cmake or hdiutil call would not abort the pipeline.";
}

//	Test 3 : script references `lipo`. The universal-binary build hinges
//	on `lipo -create` ; a script that silently skipped it would ship a
//	single-arch DMG masquerading as universal.
TEST( ShipScriptTest, ReferencesLipo )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "lipo" ), std::string::npos )
        << "ship-dmg.sh should invoke `lipo` to fuse per-arch executables.";
}

//	Test 4 : script references all three compression formats in the
//	cascade. ULMO best-compression, ULFO best-portability,
//	UDBZ broadest-legacy-compat -- losing any one of the three weakens
//	the "best working compression for portability" mandate.
TEST( ShipScriptTest, ReferencesAllThreeCompressionFormats )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "ULMO" ), std::string::npos )
        << "ship-dmg.sh should reference ULMO (LZMA, smallest).";
    EXPECT_NE( body.find( "ULFO" ), std::string::npos )
        << "ship-dmg.sh should reference ULFO (LZFSE, Apple-native).";
    EXPECT_NE( body.find( "UDBZ" ), std::string::npos )
        << "ship-dmg.sh should reference UDBZ (bzip2, legacy compat).";
}

//	Test 5 : script references CODESIGN_IDENTITY env-gating. Signing
//	must be optional + opt-in by env (c138-B doctrine).
TEST( ShipScriptTest, ReferencesCodesignIdentityEnv )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "CODESIGN_IDENTITY" ), std::string::npos )
        << "ship-dmg.sh should env-gate signing on CODESIGN_IDENTITY.";
}

//	Test 6 : script references xcrun notarytool for the optional
//	notarization step. notarytool is the post-Xcode-13 replacement for
//	altool and the canonical notarization entrypoint.
TEST( ShipScriptTest, ReferencesXcrunNotarytool )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "xcrun notarytool" ), std::string::npos )
        << "ship-dmg.sh should invoke xcrun notarytool for optional notarize.";
}

//	Test 7 -- REGRESSION GUARD (memory/feedback_regression_guard_tests.md).
//	The literal "Developer ID Application:" must not appear except in
//	comments. Mirrors codesign_packaging_test.cpp test 12 (c138-B).
TEST( ShipScriptTest, NoHardcodedDeveloperIdIdentity )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );

    std::string const needle = "Developer ID Application:";
    std::size_t pos = 0;
    while( ( pos = body.find( needle, pos ) ) != std::string::npos )
    {
        //	Find the start of the current line. If it begins with `#`,
        //	the match is inside a shell comment -- harmless.
        std::size_t line_start = body.rfind( '\n', pos );
        if( line_start == std::string::npos )
            line_start = 0;
        else
            ++line_start;
        bool const in_comment = ( line_start < body.size()
                                  && body[ line_start ] == '#' );

        EXPECT_TRUE( in_comment )
            << "ship-dmg.sh at offset " << pos << " contains literal "
               "'Developer ID Application:' OUTSIDE a comment line. "
               "REGRESSION : a hardcoded Developer ID identity has leaked. "
               "Read it from $CODESIGN_IDENTITY instead. See "
               "memory/feedback_regression_guard_tests.md (c137 doctrine).";
        pos += needle.size();
    }
}

//	Test 8 : script references `hdiutil verify`. Without the verify
//	step a corrupt DMG would silently ship.
TEST( ShipScriptTest, ReferencesHdiutilVerify )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "hdiutil verify" ), std::string::npos )
        << "ship-dmg.sh should run `hdiutil verify` on the produced DMG.";
}

//	Test 9 : script references `lipo -info` to verify the bundled
//	executable's arch list at the very end (sanity-check that the
//	universal-binary intent actually landed in the DMG).
TEST( ShipScriptTest, ReferencesLipoInfoForArchVerification )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "lipo -info" ), std::string::npos )
        << "ship-dmg.sh should run `lipo -info` to verify the bundled exe's arch list.";
}

//	Test 10 : default ARCHES list covers BOTH arm64 + x86_64. Locks the
//	universal-binary default the user mandated -- a future drift to
//	`ARCHES="arm64"` (single-arch) would silently break the
//	cross-platform-Intel half of the mandate.
TEST( ShipScriptTest, DefaultArchesCoversBothArm64AndX86_64 )
{
    auto const body = read_file( ship_script_path() );
    ASSERT_FALSE( body.empty() );

    //	Look for the bash-default expansion pattern `${ARCHES:-...}` or
    //	`ARCHES=...` with both arch tokens in the default value.
    std::size_t const eq_pos = body.find( "ARCHES:-" );
    if( eq_pos == std::string::npos )
    {
        //	Fall back to a plain ARCHES= assignment in the configuration
        //	header. Either way the default string must mention both arches.
        ADD_FAILURE() << "ship-dmg.sh should set a default ARCHES via "
                         "`${ARCHES:-...}` parameter expansion.";
        return;
    }
    //	Grab the next 64 chars after `ARCHES:-` and confirm both tokens
    //	appear inside (i.e., in the default value).
    std::string const window = body.substr( eq_pos, 64 );
    EXPECT_NE( window.find( "arm64" ), std::string::npos )
        << "ship-dmg.sh default ARCHES must include arm64. Window=[" << window << "]";
    EXPECT_NE( window.find( "x86_64" ), std::string::npos )
        << "ship-dmg.sh default ARCHES must include x86_64 (Intel half "
           "of the universal-binary mandate). Window=[" << window << "]";
}
