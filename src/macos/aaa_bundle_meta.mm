// src/macos/aaa_bundle_meta.mm
//
// Phase 8 (todo.md L365-L383) : impl side of the .app bundle metadata
// accessor. Reads via NSBundle mainBundle.
//
// Hermetic Mac sub-lib doctrine (c104 / feedback_hermetic_mac_sublibs.md) :
// Foundation only. No `o_str`, no `aaa_mem`, no vendor link.
// ASCII-only source.
//
// Gating : the whole TU is `#if defined(__APPLE__)` so a future
// cross-platform header could pair this with a stub on non-Apple
// targets without breaking the build.

#include "src/macos/aaa_bundle_meta.h"

#if defined(__APPLE__)

#import <Foundation/Foundation.h>

#include <cstring>

namespace aaa
{
namespace macos
{
namespace bundle
{

namespace
{

//	Convert an NSString to std::string (UTF-8). nil-safe : returns
//	an empty string when the input is nil or has no UTF-8 rep.
//	Mirrors the helper used in aaa_dpi.mm / aaa_dialog.mm.
std::string ns_to_string( NSString* s )
{
    if( s == nil )
        return std::string {};
    char const* c = [s UTF8String];
    if( c == nullptr )
        return std::string {};
    return std::string { c };
}

//	Fetch an Info.plist entry as id (autoreleased), nil if absent.
//	Wraps `objectForInfoDictionaryKey:` so callers don't have to
//	construct an NSString every call.
id object_for_info_key( std::string const& key )
{
    NSBundle* main_bundle = [NSBundle mainBundle];
    if( main_bundle == nil )
        return nil;
    NSString* ns_key = [NSString stringWithUTF8String: key.c_str()];
    if( ns_key == nil )
        return nil;
    return [main_bundle objectForInfoDictionaryKey: ns_key];
}

//	Convenience : Info.plist string under a known key, returns ""
//	when absent or non-string. Used by the canonical accessors below.
std::string info_plist_string_or_empty( std::string const& key )
{
    id obj = object_for_info_key( key );
    if( obj == nil )
        return std::string {};
    if( ![obj isKindOfClass: [NSString class]] )
        return std::string {};
    return ns_to_string( (NSString*)obj );
}

}   //	anonymous namespace

std::string bundle_identifier()
{
    //	NSBundle has a typed accessor for this one ; prefer it over
    //	the generic info-key lookup so the test runner's bundle id
    //	(which Foundation synthesizes when the plist key is absent)
    //	still flows through cleanly.
    NSBundle* main_bundle = [NSBundle mainBundle];
    if( main_bundle == nil )
        return std::string {};
    NSString* ident = [main_bundle bundleIdentifier];
    return ns_to_string( ident );
}

std::string bundle_name()
{
    //	CFBundleName is the canonical key ; CFBundleDisplayName is a
    //	separate concept (user-visible name overrides). We expose
    //	CFBundleName here so the result matches the executable's
    //	internal identity, not a localized display.
    return info_plist_string_or_empty( "CFBundleName" );
}

std::string bundle_version()
{
    //	CFBundleVersion = build number (monotonic int historically,
    //	now sometimes semver). For AAASeed v1 we mirror the short
    //	string ; they diverge in App Store builds with revisions.
    return info_plist_string_or_empty( "CFBundleVersion" );
}

std::string bundle_short_version_string()
{
    //	CFBundleShortVersionString = marketing version (what
    //	users see in About / Spotlight). Required key per Apple
    //	docs for App Store submission.
    return info_plist_string_or_empty( "CFBundleShortVersionString" );
}

std::string executable_name()
{
    //	NSBundle.executablePath -> the on-disk binary ; its
    //	lastPathComponent is the canonical executable name. Falls
    //	back to the CFBundleExecutable plist key when executablePath
    //	is nil (extremely rare ; would mean a malformed bundle).
    NSBundle* main_bundle = [NSBundle mainBundle];
    if( main_bundle != nil )
    {
        NSString* exec_path = [main_bundle executablePath];
        if( exec_path != nil )
        {
            NSString* last = [exec_path lastPathComponent];
            std::string s = ns_to_string( last );
            if( !s.empty() )
                return s;
        }
    }
    return info_plist_string_or_empty( "CFBundleExecutable" );
}

std::string minimum_system_version()
{
    //	LSMinimumSystemVersion is the gatekeeper macOS deployment
    //	target. For AAASeed v1 : "13.0" (Ventura). Under unit
    //	tests, the gtest binary's own bundle plist (synthesized
    //	by Foundation) may not carry this key, so the result is
    //	often empty in CI.
    return info_plist_string_or_empty( "LSMinimumSystemVersion" );
}

std::optional< std::string > info_plist_string_value( std::string const& key )
{
    id obj = object_for_info_key( key );
    if( obj == nil )
        return std::nullopt;
    if( ![obj isKindOfClass: [NSString class]] )
        return std::nullopt;
    return std::optional< std::string >{ ns_to_string( (NSString*)obj ) };
}

bool info_plist_bool_value( std::string const& key, bool default_value )
{
    id obj = object_for_info_key( key );
    if( obj == nil )
        return default_value;
    if( ![obj isKindOfClass: [NSNumber class]] )
        return default_value;

    NSNumber* num = (NSNumber*)obj;

    //	Distinguish "this NSNumber wraps a BOOL" from "this NSNumber
    //	wraps an int". Foundation collapses BOOL into NSNumber, but
    //	objCType for a BOOL is "c" (char), whereas a plain int is "i".
    //	plist XML <true/>/<false/> always materializes as the char form
    //	per CFPropertyList parsing rules. We accept both forms here :
    //	the caller asked for "bool", and a 0/1 int is unambiguously
    //	convertible.
    char const* type = [num objCType];
    if( type == nullptr )
        return default_value;

    //	Char/bool, int, long, short -- all reduce cleanly to a
    //	boolean via boolValue. Anything else (float/double, etc.)
    //	is not a "real" bool ; fall back to the caller's default.
    if( std::strcmp( type, "c" ) == 0 ||
        std::strcmp( type, "B" ) == 0 ||
        std::strcmp( type, "i" ) == 0 ||
        std::strcmp( type, "l" ) == 0 ||
        std::strcmp( type, "q" ) == 0 ||
        std::strcmp( type, "s" ) == 0 )
    {
        return [num boolValue] ? true : false;
    }
    return default_value;
}

}   //	namespace bundle
}   //	namespace macos
}   //	namespace aaa

#endif  //	__APPLE__
