// aaalua_cpu_stub.cpp
//
// Continuation 65 -- one-symbol stub for `c_cpu::one`. Lives in its
// own TU so this file can `#include "CPU.h"` cleanly (pulls in
// `class mem final` from aaa_mem.h) without colliding with the
// `namespace mem { ... }` stub blocks in test_engine_stubs_no_ostr.cpp.
//
// `c_cpu::one = nullptr` is SAFE because `c_cpu::get_align_def()`
// in CPU.h:22-28 has an explicit null check that falls back to
// `CACHE_LINE_SIZE_DEFAULT = 64`. mem::update() reads c_cpu::one
// via get_align_def() ; both paths handle null cleanly.
//
// When CPU.cpp is fully Mac-ported (multi-session, brings in
// infrastructure/param + aaaseed.h + obj/root.h), delete this stub.

#include "CPU.h"

c_cpu* c_cpu::one = nullptr;
