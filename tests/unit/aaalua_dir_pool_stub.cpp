// aaalua_dir_pool_stub.cpp
//
// Continuation 66 -- stubs for c_dir_pool statics + instance methods
// referenced from aaa_str.cpp's set_fname_pure / expand_fname /
// compact_fname paths. The real c_dir_pool.cpp pulls infrastructure/
// layer machinery ; multi-session port. Stub strategy : `cur = nullptr`
// and the instance bodies are no-ops -- tests that don't call
// o_str::expand_fname / compact_fname never hit them.
//
// Lives in its own TU (no `namespace mem` here) so that dir_pool.h's
// transitive include of aaa_mem.h (which declares `class mem final`)
// stays unambiguous.

#include "file/dir_pool.h"

c_dir_pool* c_dir_pool::cur = nullptr;

void c_dir_pool::expand_fname( o_str& /*dst*/, C_PCHAR_C /*in*/ )
{
}

void c_dir_pool::expand_fname( o_str& /*dst*/, o_str CONST & /*src*/ )
{
}

void c_dir_pool::compact_fname( char*& /*dst*/, o_str CONST & /*src*/ )
{
}

void c_dir_pool::compact_fname( o_str& /*dst*/, o_str CONST & /*src*/ )
{
}
