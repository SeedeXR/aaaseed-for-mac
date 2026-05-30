// src/net/net_json_lua.h
//
// Lua-side C-binding bridge for the native macOS HTTP + JSON sub-lib.
// This file owns the JSON-string -> Lua-table conversion installed as
// `aaa.net.parse_json`. The HTTP transport itself lives in net_mac.h.
//
// Lua surface installed by register_lua_bindings() :
//
//   aaa.net.parse_json( json : string ) -> table          (on success)
//   aaa.net.parse_json( json : string ) -> nil, errmsg     (on parse error)
//
// Conversion rules (NSJSONSerialization -> Lua), applied recursively :
//   NSDictionary -> Lua table keyed by the dictionary's string keys
//   NSArray      -> Lua table, 1-indexed (Lua array convention)
//   NSNumber     -> Lua boolean if it wraps a CFBoolean (true/false),
//                   otherwise Lua number (lua_pushnumber)
//   NSString     -> Lua string
//   NSNull       -> nil (the key/index is simply left unset)
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++. Links aaaseed_lua + the net sub-lib only. No engine
//     link, no o_str, no aaa_mem cascade. No vendor includes.
//   - Manual reference counting (-fno-objc-arc).
//   - The Lua VM is single-threaded ; no shared mutable state here.
//
// Mirrors the convention in src/syphon/syphon_lua_mac.h :
//   - `struct lua_State;` forward-declared, `#include <string>`.
//   - A void register_lua_bindings(lua_State*) that defensively creates
//     the `aaa` and `aaa.net` tables before installing the closure.
//   - A test-only accessor exposed for unit tests, NOT for Lua scripts.

#pragma once

#include <string>

struct lua_State;

namespace aaa
{
namespace net
{

//	Install `aaa.net.parse_json` as a Lua callable. Creates the `aaa`
//	and `aaa.net` tables if they do not yet exist (defensive : standalone
//	tests open a bare lua_State). Idempotent : calling twice overwrites
//	the closure with itself, no leak.
void register_lua_bindings( lua_State * L );

//	Test-only helper : parse `json` and push the resulting Lua value(s)
//	onto L's stack exactly as `aaa.net.parse_json` would. Returns the
//	number of values pushed : 1 on success (the table / scalar), or 2 on
//	parse error (nil + error string). Exposed so unit tests can drive the
//	conversion without round-tripping through luaL_dostring.
int parse_json_to_lua( lua_State * L, std::string const & json );

}   //	namespace net
}   //	namespace aaa
