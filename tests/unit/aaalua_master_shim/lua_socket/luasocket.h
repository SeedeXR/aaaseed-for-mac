// tests/unit/aaalua_master_shim/lua_socket/luasocket.h
//
// c112 (2026-05-26) -- shim shadow for vendor/.../lua_socket/luasocket.h.
// Vendor decl at line 52 :
//     extern int luaopen_socket_core( lua_State * L );
// is missing `extern "C"` wrapping, so when included from a C++ TU
// (aaalua_util.cpp) the compiler emits a reference to the C++-mangled
// name `_Z18luaopen_socket_coreP9lua_State`. But the lib's `.c` file
// compiles with C linkage and exports `_luaopen_socket_core`. Linkage
// mismatch -- the audit target fails with unresolved-symbol.
//
// This shim adds the `extern "C"` wrapping. Test target's shim include
// dir is BEFORE the engine path, so this header wins. Vendor untouched.
//
// Optional vendor patch (c111 candidate) : add `extern "C"` directly
// to the vendor header. Then this shim is no longer needed.

//	Mirror vendor's include-guard symbol exactly (not the project's
//	AAA_X_H convention) so any other shim/vendor TU that does
//	`#ifndef LUASOCKET_H` skips re-inclusion correctly.
#ifndef LUASOCKET_H
#define LUASOCKET_H 1

struct lua_State;

extern "C" int luaopen_socket_core( lua_State* L );

#endif // LUASOCKET_H
