// tests/unit/info_plist_singularity_test.cpp
//
// Phase 8 (todo.md L365-L383, Info.plist singularity collapse, 2026-05-27) :
// REGRESSION GUARD that there is EXACTLY ONE Info.plist source-of-truth
// in the AAASeed Mac tree.
//
// History : c136-B opened the Phase 8 distribution beachhead with TWO
// Info.plist files -- a ${...}-substituted template at
// `src/ui/macos/Info.plist.in` (used by MACOSX_BUNDLE_INFO_PLIST for
// the dev .app) AND a fully-resolved canonical at
// `bundle/macos/Info.plist` (the runbook artefact for codesign /
// notarytool / hdiutil). They drifted independently. The 2026-05-27
// collapse merged them into one canonical template at
// `bundle/macos/Info.plist.in`, configure_file()'d into
// `${CMAKE_BINARY_DIR}/Info.plist` at CMake-time, and that resolved
// plist is what both the dev .app + future packaging steps consume.
//
// These tests enforce the singularity stays singular ; if a future
// session reintroduces a second `Info.plist*` under src/ or top-level,
// the regression-guard test fires. Per feedback_regression_guard_tests.md
// (c137 codification) : name the test for the deferred / forbidden
// thing, point the failure message at the rationale.
//
// Pure C++ ; no engine deps. Uses only the standard library :
// <fstream> + <filesystem> + <string> + <cstdlib> (system() for plutil).
// No `o_str`, no `aaa_str.h`, no `aaa_mem.h` (hermetic doctrine).
//
// Labels (set in tests/unit/CMakeLists.txt) :
//   phase8;unit;distribution;plist
// `phase8` is FIRST per c121-B doctrine (CTest LABELS first-only quirk)
// so `ctest -L phase8` includes this test.

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{

//	Source-tree root resolver -- walks up looking for the top-level
//	CMakeLists.txt + bundle/macos/Info.plist.in marker. After the
//	Info.plist singularity collapse the marker file is the .in
//	template (the literal .plist has been deleted from the source
//	tree). Mirrors dmg_packaging_test.cpp + codesign_packaging_test.cpp.
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
    //	Fallback : tests are typically run from out/macos-arm64-debug ;
    //	source tree is two levels up.
    return fs::current_path().parent_path().parent_path();
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

//	Scan the source tree (bundle/, src/, plus top-level) for any file
//	matching `Info.plist*` (catches Info.plist, Info.plist.in,
//	Info.plist.template, etc.). Returns absolute paths.
std::vector< std::filesystem::path >
find_all_info_plists( std::filesystem::path const& root )
{
    namespace fs = std::filesystem;
    std::vector< fs::path > hits;
    //	Directories to scan. Skip out/ + build/ + third_party/ + vendor/
    //	+ tests/ (test fixtures may legitimately ship sample plists in
    //	the future) ; we ONLY care about the production source tree.
    std::vector< fs::path > const roots = {
        root / "bundle",
        root / "src",
    };
    for( auto const& base : roots )
    {
        if( ! fs::exists( base ) )
            continue;
        for( auto& entry : fs::recursive_directory_iterator( base ) )
        {
            if( ! entry.is_regular_file() )
                continue;
            auto const name = entry.path().filename().string();
            if( name.rfind( "Info.plist", 0 ) == 0 )
                hits.push_back( entry.path() );
        }
    }
    //	Also scan top-level (non-recursive) -- the canonical lives at
    //	bundle/macos/, but a stray top-level Info.plist would also count.
    for( auto& entry : fs::directory_iterator( root ) )
    {
        if( ! entry.is_regular_file() )
            continue;
        auto const name = entry.path().filename().string();
        if( name.rfind( "Info.plist", 0 ) == 0 )
            hits.push_back( entry.path() );
    }
    return hits;
}

}   //	namespace

//	Test 1 : the canonical template exists. If it's gone, the .app
//	bundle won't get an Info.plist at build time -- catastrophic.
TEST( InfoPlistSingularityTest, CanonicalTemplateExists )
{
    auto const root = source_tree_root();
    auto const canonical = root / "bundle" / "macos" / "Info.plist.in";
    EXPECT_TRUE( std::filesystem::exists( canonical ) )
        << "Canonical Info.plist template missing at "
        << canonical.string()
        << " . The .app bundle has no metadata source. Restore from git "
           "or recreate per bundle/macos/README.md.";
}

//	Test 2 : EXACTLY ONE `Info.plist*` exists in the source tree
//	(bundle/, src/, top-level). Regression guard -- if a future
//	session adds a second one, the test names every path found
//	so the human can see the duplication immediately.
TEST( InfoPlistSingularityTest, ExactlyOnePlistInSourceTree )
{
    auto const root = source_tree_root();
    auto const hits = find_all_info_plists( root );

    std::ostringstream listing;
    for( auto const& h : hits )
        listing << "  " << h.string() << "\n";

    EXPECT_EQ( hits.size(), 1u )
        << "Expected exactly ONE Info.plist* in the source tree (the "
           "canonical template bundle/macos/Info.plist.in). Found "
        << hits.size() << " :\n"
        << listing.str()
        << "If you intentionally added a second plist, collapse them "
           "into the canonical OR remove this regression guard with a "
           "documented justification (see "
           "memory/feedback_regression_guard_tests.md).";
}

//	Test 3 : `src/ui/macos/Info.plist.in` does NOT exist. This is the
//	REGRESSION GUARD -- it was the second plist before the collapse.
//	If someone re-adds it (likely while "fixing" the dev .app), we
//	want the test to fail loudly with the rationale.
TEST( InfoPlistSingularityTest, OldPerUiPlistGoneAndStaysGone )
{
    auto const root = source_tree_root();
    auto const old_path = root / "src" / "ui" / "macos" / "Info.plist.in";
    EXPECT_FALSE( std::filesystem::exists( old_path ) )
        << "Resurrected old per-UI Info.plist.in at "
        << old_path.string()
        << " . The canonical lives at bundle/macos/Info.plist.in ; "
           "configure_file() in src/ui/macos/CMakeLists.txt produces "
           "the resolved plist. Remove the duplicate or remove this "
           "regression guard with a documented justification (see "
           "memory/feedback_regression_guard_tests.md).";
}

//	Test 4 : the canonical template preserves ${PROJECT_VERSION}-style
//	substitution. Strictly we substitute MACOSX_BUNDLE_BUNDLE_VERSION
//	(which is itself derived from ${PROJECT_VERSION} in
//	src/ui/macos/CMakeLists.txt) ; both substitution sites must remain.
TEST( InfoPlistSingularityTest, TemplatePreservesSubstitutionSites )
{
    auto const root = source_tree_root();
    auto const canonical = root / "bundle" / "macos" / "Info.plist.in";
    auto const body = read_file( canonical );

    ASSERT_FALSE( body.empty() )
        << "Cannot read " << canonical.string();
    EXPECT_NE( body.find( "${MACOSX_BUNDLE_BUNDLE_VERSION}" ),
               std::string::npos )
        << "Template must keep ${MACOSX_BUNDLE_BUNDLE_VERSION} so the "
           "version-stamping (downstream of PROJECT_VERSION) survives.";
    EXPECT_NE( body.find( "${MACOSX_BUNDLE_SHORT_VERSION_STRING}" ),
               std::string::npos );
    EXPECT_NE( body.find( "${CMAKE_OSX_DEPLOYMENT_TARGET}" ),
               std::string::npos );
}

//	Test 5 : the 5 most-critical Info.plist keys are present. If any
//	go missing, hardened-runtime / notarization fails downstream.
TEST( InfoPlistSingularityTest, TemplateReferencesCriticalKeys )
{
    auto const root = source_tree_root();
    auto const canonical = root / "bundle" / "macos" / "Info.plist.in";
    auto const body = read_file( canonical );

    ASSERT_FALSE( body.empty() );
    EXPECT_NE( body.find( "CFBundleIdentifier" ),    std::string::npos );
    EXPECT_NE( body.find( "CFBundleName" ),          std::string::npos );
    EXPECT_NE( body.find( "CFBundleVersion" ),       std::string::npos );
    EXPECT_NE( body.find( "LSMinimumSystemVersion" ), std::string::npos );
    EXPECT_NE( body.find( "NSPrincipalClass" ),      std::string::npos );
}

//	Test 6 : plutil -lint on both the template AND, if present, the
//	configured artefact in the build dir. The template lints clean
//	even with unsubstituted ${...} sites (plutil treats them as opaque
//	strings). The configured output is read from CMakeCache.txt's
//	build dir path ; skipped with STATUS log if unavailable.
TEST( InfoPlistSingularityTest, PlutilLintTemplateAndConfigured )
{
    auto const root = source_tree_root();
    auto const canonical = root / "bundle" / "macos" / "Info.plist.in";

    //	Lint the template -- the ${...} sites are valid <string>
    //	contents at the XML level. plutil should pass.
    std::string cmd = "plutil -lint \"" + canonical.string()
                    + "\" > /dev/null 2>&1";
    int const rc_tmpl = std::system( cmd.c_str() );
    EXPECT_EQ( rc_tmpl, 0 )
        << "plutil -lint failed on " << canonical.string()
        << " -- the canonical template is malformed XML/plist.";

    //	Try the configured output. Locate the CMake binary dir by
    //	scanning ${root}/out/<config>/CMakeCache.txt or fallback to
    //	the test's CWD's parents.
    namespace fs = std::filesystem;
    fs::path configured;
    //	Common path : ctest invokes from CMAKE_BINARY_DIR, so
    //	current_path() / "Info.plist" is the resolved artefact.
    auto const candidate_a = fs::current_path() / "Info.plist";
    if( fs::exists( candidate_a ) )
        configured = candidate_a;

    if( configured.empty() )
    {
        GTEST_LOG_( INFO )
            << "Configured Info.plist not found in CWD ; skipping "
               "configured-artefact plutil lint (cmake reconfigure "
               "would produce it).";
        return;
    }
    cmd = "plutil -lint \"" + configured.string() + "\" > /dev/null 2>&1";
    int const rc_cfg = std::system( cmd.c_str() );
    EXPECT_EQ( rc_cfg, 0 )
        << "plutil -lint failed on " << configured.string()
        << " -- the configured plist is malformed. Substitution may "
           "have introduced bad XML.";
}

//	Test 7 : `src/ui/macos/CMakeLists.txt` wires the canonical
//	bundle/macos/Info.plist.in into MACOSX_BUNDLE_INFO_PLIST via
//	configure_file() . Catches the regression where the property
//	silently reverts to a per-UI template.
TEST( InfoPlistSingularityTest, CmakeWiresCanonicalTemplate )
{
    auto const root = source_tree_root();
    auto const ui_cmake = root / "src" / "ui" / "macos" / "CMakeLists.txt";
    auto const body = read_file( ui_cmake );

    ASSERT_FALSE( body.empty() )
        << "Cannot read " << ui_cmake.string();
    EXPECT_NE( body.find( "bundle/macos/Info.plist.in" ), std::string::npos )
        << "src/ui/macos/CMakeLists.txt must reference "
           "bundle/macos/Info.plist.in (the canonical template) for the "
           "Info.plist singularity collapse to hold.";
    EXPECT_NE( body.find( "configure_file" ), std::string::npos )
        << "src/ui/macos/CMakeLists.txt must invoke configure_file() to "
           "produce the resolved plist consumed by MACOSX_BUNDLE_INFO_PLIST.";
    EXPECT_NE( body.find( "MACOSX_BUNDLE_INFO_PLIST" ), std::string::npos );
}
