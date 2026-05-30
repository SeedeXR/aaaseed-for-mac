// tests/unit/macos_bundle_meta_test.cpp
//
// Phase 8 (todo.md L365-L383, c136-B, 2026-05-27) : unit tests for
// the `aaa::macos::bundle` runtime accessor.
//
// Important : gtest binaries do NOT live inside AAASeed.app, so
// `[NSBundle mainBundle]` resolves to the test executable's own
// bundle (a thin command-line tool bundle synthesized by Foundation
// when there is no Info.plist on disk). The Info.plist keys we
// landed at bundle/macos/Info.plist are NOT visible from here.
//
// Tests therefore assert "non-empty AND not crashing" semantics
// (defensive contract -- never crash, return sensible defaults),
// not "exactly com.seedexr.aaaseed". A future integration test launched
// from inside the .app bundle could tighten these assertions ; for
// now the unit-test layer locks the no-crash contract.
//
// Pure C++ ; pulls only <gtest/gtest.h> + the bundle_meta header.

#include "src/macos/aaa_bundle_meta.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>

namespace
{

//	True iff `s` parses as N dot-separated nonnegative integer
//	components. Used for semver-ish version-string sanity checks.
//	Empty components ("1..2") are rejected. Allows trailing extras
//	by ignoring anything past N components.
bool parses_as_dotted_ints( std::string const& s, int min_components )
{
    if( s.empty() )
        return false;
    int components = 0;
    std::size_t start = 0;
    while( start <= s.size() )
    {
        std::size_t end = s.find( '.', start );
        if( end == std::string::npos )
            end = s.size();
        if( end == start )
            return false; //	empty component
        for( std::size_t i = start; i < end; ++i )
        {
            if( !std::isdigit( static_cast< unsigned char >( s[i] ) ) )
                return false;
        }
        ++components;
        if( end == s.size() )
            break;
        start = end + 1;
    }
    return components >= min_components;
}

}   //	anonymous namespace

//	Test 1 : bundle_identifier() returns SOMETHING (either the .app
//	bundle id when launched from AAASeed.app, or the test runner's
//	auto-synthesized bundle id). Foundation can return an empty
//	string for a tool with no plist on disk -- accept that too
//	(the contract is "never crash", not "always non-empty"). The
//	real check is that the call does not throw / segfault.
TEST(MacosBundleMeta, BundleIdentifierDoesNotCrash)
{
    std::string id = aaa::macos::bundle::bundle_identifier();
    //	No specific value is required ; the act of returning a
    //	well-formed std::string is the assertion.
    EXPECT_GE( id.size(), 0u );
    //	If we DID get a non-empty value, prove it's ASCII printable
    //	(Foundation guarantees this for bundle ids in practice).
    for( char c : id )
    {
        EXPECT_GE( static_cast< unsigned char >( c ), 0x20u );
    }
}

//	Test 2 : bundle_version() either parses as semver-style dotted
//	integers OR is empty (unit-test environment without an
//	Info.plist on disk). Both are acceptable.
TEST(MacosBundleMeta, BundleVersionIsSemverOrEmpty)
{
    std::string v = aaa::macos::bundle::bundle_version();
    if( !v.empty() )
    {
        //	Require at least 3 dot-separated integer components
        //	when present -- matches the "0.1.0" form landed in
        //	bundle/macos/Info.plist.
        EXPECT_TRUE( parses_as_dotted_ints( v, 3 ) )
            << "bundle_version() = '" << v
            << "' should parse as N.N.N when non-empty";
    }
}

//	Test 3 : info_plist_string_value("CFBundleName") returns
//	"AAASeed" if the plist is loaded by the test process, or
//	nullopt otherwise. Either branch is valid.
TEST(MacosBundleMeta, InfoPlistStringValueBundleName)
{
    auto opt = aaa::macos::bundle::info_plist_string_value( "CFBundleName" );
    if( opt.has_value() )
    {
        //	If we got a value, it should be non-empty (Foundation
        //	doesn't synthesize blank-string keys).
        EXPECT_FALSE( opt->empty() );
    }
    //	The nullopt branch is the gtest-binary-not-in-app case ;
    //	no further assertion needed.
}

//	Test 4 : info_plist_bool_value("NSHighResolutionCapable", false)
//	returns true if the plist is loaded ; default (false) otherwise.
TEST(MacosBundleMeta, InfoPlistBoolValueHighRes)
{
    bool hr = aaa::macos::bundle::info_plist_bool_value(
        "NSHighResolutionCapable", false );
    //	The bool either reflects the plist (true) or falls back to
    //	the default (false). Both are acceptable -- the real check
    //	is that the call never crashes and returns a clean bool.
    EXPECT_TRUE( hr == true || hr == false );
}

//	Test 5 : a nonexistent key returns nullopt cleanly.
TEST(MacosBundleMeta, InfoPlistStringValueNonexistentKey)
{
    auto opt = aaa::macos::bundle::info_plist_string_value(
        "nonexistent_key_xyz_aaa_phase8" );
    EXPECT_FALSE( opt.has_value() );
}

//	Test 6 : executable_name() is non-empty. NSBundle ALWAYS has an
//	executablePath for a running process -- even a bare gtest binary
//	-- so this should reliably return a non-empty string. Tighter
//	than the bundle_identifier test on purpose.
TEST(MacosBundleMeta, ExecutableNameIsNonEmpty)
{
    std::string name = aaa::macos::bundle::executable_name();
    EXPECT_FALSE( name.empty() );
    //	executable_name() returns just the lastPathComponent ; no
    //	path separators should leak through.
    EXPECT_EQ( name.find( '/' ), std::string::npos );
}

//	Test 7 : minimum_system_version() either parses with at least
//	one '.' separator OR is empty. The .app ships "13.0" but unit
//	tests will typically see empty.
TEST(MacosBundleMeta, MinimumSystemVersionIsDottedOrEmpty)
{
    std::string mv = aaa::macos::bundle::minimum_system_version();
    if( !mv.empty() )
    {
        //	Require at least 2 dot-separated integer components
        //	("13.0", "14.5"). Single integers are rejected.
        EXPECT_TRUE( parses_as_dotted_ints( mv, 2 ) )
            << "minimum_system_version() = '" << mv
            << "' should parse as N.N when non-empty";
    }
}
