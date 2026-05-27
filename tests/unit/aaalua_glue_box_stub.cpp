// tests/unit/aaalua_glue_box_stub.cpp
//
// c110 (2026-05-26) — link-stage stubs for 2 buckets in the aaalua trio
// link audit cascade :
//   (1) aaalua_glue free fns/flags (5 syms) — defined in
//       vendor/.../language/lua/aaalua_glue.cpp lines 968/1100 + 3 flags.
//   (2) console_box error reporting (4 syms) — declared in
//       vendor/.../code_utils/err.h lines 115/121/201/206. Real
//       defs live in err.cpp (Mac-deferred : pulls UI dialog deps).
//
// Scope : audit target only (aaaseed_aaalua_trio_link_audit_tests).
// Weak attrs preserved so future real ports supersede automatically.
// See [[feedback-weak-symbol-stubs]] in auto-memory.
//
// Behavior :
// - aaalua flags default to false : Lua call tracing / store / print
//   logging stay OFF in audit / runtime tests until the real glue ports.
// - register_all_fn / unregister_all_fn : no-ops. The trio link audit
//   does NOT execute Lua at runtime ; if c113+ adds a runtime test it
//   will need real glue OR a minimal manually-registered fn table.
// - BOX_* console reporters : forward to stderr via vfprintf so any
//   error path that fires during a future runtime test stays observable.
// - SET_COLOR_* : no-ops on Mac. Real engine uses Win32 console attribs ;
//   on Mac ANSI escapes via console.cpp's set_text path are the equivalent,
//   but the BOX_* path doesn't need them.

#include <cstdarg>
#include <cstdio>

//	Forward-declare lua_State so the function signatures match the engine's
//	`extern void register_all_fn( lua_State* L );` declarations from
//	aaalua_glue.h. Including lua/lua.hpp here would pull libluajit headers
//	that the audit target's transitive include cone already resolves.
struct lua_State;

namespace aaalua
{
    __attribute__((weak)) bool b_call_debug_ui = false;
    __attribute__((weak)) bool b_call_store_ui = false;
    __attribute__((weak)) bool b_call_print_ui = false;

    __attribute__((weak)) void register_all_fn( lua_State* /* L */ )
    {
        //	No-op on Mac : real impl registers ~100 C functions across
        //	the engine subsystems into the Lua global table. Audit target
        //	has no runtime ; deferred to c113+ when a real runtime test
        //	lands.
    }

    __attribute__((weak)) void unregister_all_fn( lua_State* /* L */ )
    {
        //	Mirror : no-op until real impl ports.
    }
}

//	BOX_* error reporters declared in err.h. The engine's real impls
//	dispatch to UI dialogs on Windows (MessageBox-flavored) and fall
//	back to console output if no dialog runs. Mac stubs send to stderr.
//	Engine signature : `bool BOX_LUA( char const* title, char const* fmt, ... );`
//	(see vendor/.../code_utils/err.h:206 ; returns bool for "user clicked OK").
__attribute__((weak))
bool BOX_LUA( char const* title, char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::fprintf( stderr, "BOX_LUA [%s]: ", title ? title : "" );
    std::vfprintf( stderr, fmt ? fmt : "", ap );
    std::fputc( '\n', stderr );
    va_end( ap );
    return true;
}

__attribute__((weak))
void BOX_TITLE_ERR_VA( char const* title, char const* fmt, char* args )
{
    //	args is a pre-built va_list (passed as char*) in the engine's
    //	BOX_TITLE_ERR_VA convention. Cast it back to va_list and forward.
    //	On Mac arm64, va_list is a __va_list_tag* under the hood ; the
    //	cast-through-char* pattern is the engine's portability dance.
    std::fprintf( stderr, "BOX_TITLE_ERR [%s]: ", title ? title : "" );
    if( fmt )
        std::vfprintf( stderr, fmt, reinterpret_cast< va_list >( args ) );
    std::fputc( '\n', stderr );
}

__attribute__((weak))
void SET_COLOR_ERR()
{
    //	Real impl flips the console foreground to red ; Mac stub is no-op.
    //	Output already goes to stderr so it stands out.
}

__attribute__((weak))
void SET_COLOR_NORMAL()
{
    //	Mirror : no-op.
}
