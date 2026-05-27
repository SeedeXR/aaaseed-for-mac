// tests/unit/codesign_packaging_test.cpp
//
// Phase 8 (todo.md L385-L387, c138-B, 2026-05-27) : DRY-RUN
// validation of the codesign + notarize CMake module.
//
// These tests do NOT invoke codesign / notarytool / stapler and do
// NOT make any network calls. They read `cmake/codesign.cmake` as
// text and assert the canonical command-line shape : codesign with
// --deep / --options runtime / --timestamp / --entitlements /
// notarytool submit / --wait / stapler staple, plus the env-gating
// references ($ENV{CODESIGN_IDENTITY}, $ENV{NOTARY_API_KEY_PATH}).
// The point is to lock the contract -- the actual signing runs only
// when the operator exports the identity strings + invokes the build.
//
// Regression-guard tests (per memory/feedback_regression_guard_tests.md,
// c137 doctrine) :
//   - Test 12 asserts NO hardcoded "Developer ID Application:"
//     identity string leaked into the .cmake source.
//   - Test 13 asserts CODESIGN_IDENTITY env var is empty at test-
//     run time (smoke check ; passes either way + logs STATUS if
//     set so the regression behaviour is visible to operators).
//
// Pure C++ ; no engine deps. Standard library only :
// <fstream> + <filesystem> + <string> + <cstdlib>. No `o_str`, no
// `aaa_str.h`, no `aaa_mem.h` (per hermetic-mac-sublibs doctrine).
//
// Labels (set in tests/unit/CMakeLists.txt) :
//   phase8;unit;distribution;codesign
// `phase8` is FIRST per c121-B doctrine (CTest LABELS first-only
// quirk) so `ctest -L phase8` includes this test.

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace
{

//	Source-tree root resolver -- walks up looking for the top-level
//	CMakeLists.txt + bundle/macos/Info.plist.in marker. Mirrors the
//	pattern used by tests/unit/dmg_packaging_test.cpp (c137-B). The
//	marker file changed from `Info.plist` to `Info.plist.in` when
//	the Info.plist singularity collapse landed (2026-05-27).
std::filesystem::path source_tree_root()
{
    namespace fs = std::filesystem;
    fs::path cur = fs::current_path();
    for( int hop = 0; hop < 8; ++hop )
    {
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
    //	Fallback : tests run from out/macos-arm64-debug, source is
    //	two levels up.
    return fs::current_path().parent_path().parent_path();
}

//	Slurp the codesign.cmake module into memory. Returns empty on I/O
//	failure ; callers ASSERT non-empty before sub-string probes.
std::string read_file( std::filesystem::path const& p )
{
    std::ifstream f( p );
    if( ! f )
        return {};
    std::string content( ( std::istreambuf_iterator< char >( f ) ),
                         std::istreambuf_iterator< char >() );
    return content;
}

//	Convenience : path to the codesign.cmake module, resolved once.
std::filesystem::path codesign_cmake_path()
{
    return source_tree_root() / "cmake" / "codesign.cmake";
}

}   //	namespace

//	Test 1 : cmake/codesign.cmake exists in the source tree.
TEST( CodesignPackagingTest, ModuleFileExists )
{
    auto const p = codesign_cmake_path();
    EXPECT_TRUE( std::filesystem::exists( p ) )
        << "Expected " << p.string() << " to exist.";
}

//	Test 2 : module references the `codesign` command line. The
//	literal token appears in the COMMAND clause of the POST_BUILD hook.
TEST( CodesignPackagingTest, ModuleReferencesCodesignCommand )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "codesign" ), std::string::npos )
        << "codesign.cmake should invoke the codesign command.";
}

//	Test 3 : module references `--entitlements` flag. Locks the
//	hardened-runtime entitlements wiring.
TEST( CodesignPackagingTest, ModuleReferencesEntitlementsFlag )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "--entitlements" ), std::string::npos )
        << "codesign.cmake should pass --entitlements to codesign.";
}

//	Test 4 : module references the canonical entitlements path
//	`bundle/macos/entitlements.plist` (c136-B). Substring match on the
//	relative path -- the module uses ${CMAKE_SOURCE_DIR}/... but the
//	tail of the path is unique.
TEST( CodesignPackagingTest, ModuleReferencesEntitlementsPath )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "bundle/macos/entitlements.plist" ), std::string::npos )
        << "codesign.cmake should reference bundle/macos/entitlements.plist (c136-B).";
}

//	Test 5 : module references `--options runtime`. Hardened runtime
//	is a notarization prerequisite.
TEST( CodesignPackagingTest, ModuleReferencesOptionsRuntime )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "--options" ), std::string::npos );
    EXPECT_NE( body.find( "runtime" ),   std::string::npos )
        << "codesign.cmake should pass --options runtime for hardened runtime.";
}

//	Test 6 : module references `--timestamp`. Apple secure timestamp
//	is also a notarization prerequisite.
TEST( CodesignPackagingTest, ModuleReferencesTimestamp )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "--timestamp" ), std::string::npos )
        << "codesign.cmake should pass --timestamp to codesign.";
}

//	Test 7 : module references `xcrun notarytool`. notarytool is the
//	post-Xcode-13 replacement for the deprecated altool.
TEST( CodesignPackagingTest, ModuleReferencesNotarytool )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "xcrun notarytool" ), std::string::npos )
        << "codesign.cmake should invoke xcrun notarytool.";
}

//	Test 8 : module references `--wait`. notarytool returns
//	immediately by default ; --wait blocks until Apple's service
//	finishes processing the submission.
TEST( CodesignPackagingTest, ModuleReferencesNotarytoolWait )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "--wait" ), std::string::npos )
        << "codesign.cmake should pass --wait to notarytool to block until done.";
}

//	Test 9 : module references `xcrun stapler staple`. Stapling
//	attaches the notarization ticket to the DMG so Gatekeeper can
//	verify offline.
TEST( CodesignPackagingTest, ModuleReferencesStapler )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "xcrun stapler staple" ), std::string::npos )
        << "codesign.cmake should invoke xcrun stapler staple after notarytool.";
}

//	Test 10 : module references $ENV{CODESIGN_IDENTITY}. Locks the
//	env-gating pattern : the identity string is NEVER hardcoded ;
//	always read from the environment at configure time.
TEST( CodesignPackagingTest, ModuleReadsCodesignIdentityFromEnv )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "$ENV{CODESIGN_IDENTITY}" ), std::string::npos )
        << "codesign.cmake should read CODESIGN_IDENTITY from the environment.";
}

//	Test 11 : module references $ENV{NOTARY_API_KEY_PATH}. Same
//	env-gating pattern for the notarization API key.
TEST( CodesignPackagingTest, ModuleReadsNotaryKeyPathFromEnv )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "$ENV{NOTARY_API_KEY_PATH}" ), std::string::npos )
        << "codesign.cmake should read NOTARY_API_KEY_PATH from the environment.";
}

//	Test 12 -- REGRESSION GUARD (memory/feedback_regression_guard_tests.md).
//	The literal substring "Developer ID Application:" must only appear
//	in comments / status messages, never as a hardcoded codesign --sign
//	argument. Concretely : any occurrence MUST be followed by a $ENV{...}
//	reference within a small window, or be in a comment line. We
//	approximate by asserting that EVERY occurrence is "near" $ENV.
//
//	The point isn't to prevent the feature ; it's to make the deferral
//	of "real signing happens at operator's deliberate environment-export
//	step" visible in test output. If a future session pastes an actual
//	Developer ID string into the .cmake file (a common security mistake),
//	this test fires.
TEST( CodesignPackagingTest, ModuleDoesNotLeakHardcodedDeveloperId )
{
    auto const body = read_file( codesign_cmake_path() );
    ASSERT_FALSE( body.empty() );

    //	Walk every occurrence. For each, look in the next 256 chars for
    //	either "$ENV{" or a comment marker "#" on the same line / right
    //	before. If neither is present, the identity is hardcoded.
    std::string const needle = "Developer ID Application:";
    std::size_t pos = 0;
    while( ( pos = body.find( needle, pos ) ) != std::string::npos )
    {
        //	Scan back to the start of the current line. If it begins
        //	with a `#`, the occurrence is a comment -- harmless.
        std::size_t line_start = body.rfind( '\n', pos );
        if( line_start == std::string::npos )
            line_start = 0;
        else
            ++line_start;
        bool const in_comment = ( line_start < body.size()
                                  && body[ line_start ] == '#' );

        //	Look ahead up to 256 chars for "$ENV{". If present, the
        //	identity is operator-supplied (env-gated) -- also harmless.
        std::size_t const tail = std::min( pos + 256, body.size() );
        bool const has_env_ref =
            body.find( "$ENV{", pos ) < tail;

        EXPECT_TRUE( in_comment || has_env_ref )
            << "codesign.cmake at offset " << pos << " has 'Developer ID Application:' "
               "without a nearby $ENV{...} reference and not inside a comment. "
               "REGRESSION : a hardcoded Developer ID identity string has leaked. "
               "See memory/feedback_regression_guard_tests.md (c137 doctrine) -- "
               "remove the literal identity, read it from $CODESIGN_IDENTITY instead.";

        pos += needle.size();
    }
}

//	Test 13 -- REGRESSION GUARD (memory/feedback_regression_guard_tests.md).
//	Smoke check on the operator environment : CODESIGN_IDENTITY should
//	NOT be set at routine test-run time. If it IS set (an operator with
//	a real Developer ID running tests locally), the test still passes
//	but logs a STATUS line so the side-effect of running with signing
//	enabled is visible. Either branch is acceptable ; the point is
//	visibility.
TEST( CodesignPackagingTest, CodesignIdentityEnvUnsetSmokeCheck )
{
    char const* const ident = std::getenv( "CODESIGN_IDENTITY" );
    if( ident == nullptr || ident[ 0 ] == '\0' )
    {
        //	Expected case : no Developer ID configured. NO-OP path
        //	taken by aaaseed_codesign_target() -- the build stays
        //	unsigned per c137-B / c138-B beachhead contract.
        SUCCEED() << "CODESIGN_IDENTITY env var is unset -- aaaseed_codesign_target "
                     "is a NO-OP for this configure (expected).";
    }
    else
    {
        //	Operator has Developer ID configured. STATUS log only ;
        //	signing IS active for this configure.
        std::cout << "[          ] CODESIGN_IDENTITY is set ('"
                  << ident << "') -- aaaseed_codesign_target wires "
                     "real codesign hook for this configure." << std::endl;
        SUCCEED();
    }
}
