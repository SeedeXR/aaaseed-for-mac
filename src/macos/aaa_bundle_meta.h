// src/macos/aaa_bundle_meta.h
//
// Phase 8 (todo.md L365-L383) : runtime accessor for .app bundle
// metadata declared in bundle/macos/Info.plist. Exposes the canonical
// Info.plist keys (CFBundleIdentifier / CFBundleName / CFBundleVersion
// / CFBundleShortVersionString / CFBundleExecutable / LSMinimumSystem-
// Version) as pure C++ strings plus a generic key lookup pair.
//
// Reads via `[[NSBundle mainBundle] infoDictionary]` +
// `objectForInfoDictionaryKey:` at impl side (aaa_bundle_meta.mm).
//
// Hermetic Mac sub-lib doctrine (c104 / feedback_hermetic_mac_sublibs.md) :
//   - Pure CPU. No Metal touch.
//   - std::string / std::optional only in the header.
//   - No `o_str`, no `aaa_mem.h`, no `aaa_str.h`, no `aaa_type.h`.
//   - No vendor edits. No link to `aaaseed_code_utils`.
//   - Foundation only at the impl side.
//
// Note on `NSBundle mainBundle` semantics under unit tests : when
// invoked from a gtest binary that is NOT inside an .app bundle, the
// main bundle is the test executable's bundle, and the Info.plist
// keys we ship inside AAASeed.app/Contents/Info.plist are NOT
// available. Callers must tolerate empty strings / nullopt in that
// case ; the API contract is "return a sensible default, never
// crash", not "guarantee a specific value".
//
// Wiring lives in CMakeLists.txt (top-level add_subdirectory) as a
// new static lib `aaaseed_bundle_meta`. Tests in
// `tests/unit/macos_bundle_meta_test.cpp`.

#pragma once

#include <optional>
#include <string>

namespace aaa
{
namespace macos
{
namespace bundle
{

//	CFBundleIdentifier. Returns "com.seedexr.aaaseed" when running inside
//	AAASeed.app ; returns the test binary's bundle id when running
//	from gtest (typically a UUID-suffixed temp id, or empty).
std::string bundle_identifier();

//	CFBundleName. Returns "AAASeed" inside the .app, may be empty or
//	the test binary name under unit tests.
std::string bundle_name();

//	CFBundleVersion. Returns "0.1.0" inside the .app ; may be empty
//	under unit tests.
std::string bundle_version();

//	CFBundleShortVersionString. Mirrors CFBundleVersion for v1
//	(no separate marketing version yet).
std::string bundle_short_version_string();

//	CFBundleExecutable. Returns "aaaseed_app" inside the .app ;
//	under unit tests, returns the gtest binary's filename (or empty
//	if NSBundle has no executable info -- e.g. raw command-line tool).
std::string executable_name();

//	LSMinimumSystemVersion. Returns "13.0" inside the .app ;
//	may be empty under unit tests.
std::string minimum_system_version();

//	Generic Info.plist string lookup. Returns nullopt when the key
//	is absent OR when the value is not an NSString (e.g. boolean,
//	array, dict, number). Empty-string values DO return an engaged
//	optional holding "".
std::optional< std::string > info_plist_string_value( std::string const& key );

//	Generic Info.plist boolean lookup. Returns the underlying value
//	when the key holds an NSNumber whose objCType is BOOL ; returns
//	`default_value` when the key is absent or holds a non-boolean
//	type. <true/> in plist XML maps to true ; <false/> to false.
bool info_plist_bool_value( std::string const& key, bool default_value = false );

}   //	namespace bundle
}   //	namespace macos
}   //	namespace aaa
