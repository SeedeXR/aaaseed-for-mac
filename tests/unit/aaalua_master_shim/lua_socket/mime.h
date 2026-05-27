// tests/unit/aaalua_master_shim/lua_socket/mime.h
//
// c112 (2026-05-26) -- shim shadow for vendor/.../lua_socket/mime.h.
// Same linkage-mismatch fix as the luasocket.h shim : adds `extern "C"`
// so C++ TUs see the same C-mangled name the lib exports.

#ifndef MIME_H
#define MIME_H 1

#include "luasocket.h"   //	resolves to our shim's luasocket.h above

extern "C" int luaopen_mime_core( lua_State* L );

#endif // MIME_H
