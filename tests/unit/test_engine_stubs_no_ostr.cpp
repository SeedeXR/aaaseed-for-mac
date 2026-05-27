// test_engine_stubs_no_ostr.cpp
//
// Same as test_engine_stubs.cpp but WITHOUT the stub `o_str` class --
// targets that link against a real-layout `o_str` stub (see
// aaalua_ostr_stub.cpp) need this split to avoid duplicate-symbol
// errors. Continuation 49 introduces this split for the aaalua_debug.cpp
// port -- once aaa_str.cpp is fully Mac-ported and shipped via
// aaaseed_code_utils, BOTH this file and aaalua_ostr_stub.cpp can be
// deleted in favour of the real lib.

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <malloc/malloc.h>

#include "aaa_type.h"

namespace mem
{
    //  c113 : added `__attribute__((weak))` to all mem:: stubs so the
    //  real aaa_mem.cpp symbols from libaaaseed_code_utils.a win at link
    //  time when the audit target's lazy-link pulls them transitively
    //  (via luasocket bridge that landed c113). Other test targets that
    //  don't pull aaa_mem.cpp keep using these stubs unchanged.
    __attribute__((weak))
    void memcpy( void* dst, void const* src, unsigned long n, char const* /*tag*/ )
    {
        std::memcpy( dst, src, n );
    }

    __attribute__((weak))
    void memclear( void* dst, unsigned long n )
    {
        std::memset( dst, 0, n );
    }

    //  Added continuation 51 for aaalua_array.cpp's `MALLOC_ALIGNED` macro.
    //  posix_memalign requires alignment >= sizeof(void*) AND power-of-two.
    //  The engine's MALLOC_ALIGNED passes `align = 0` as "use the default" ;
    //  we use 16 (SSE/AVX-friendly, same convention as realloc_aligned).
    __attribute__((weak))
    void* malloc_aligned( unsigned long size, unsigned int alignment, char const* /*tag*/ )
    {
        if( size == 0 )
            return nullptr;
        if( alignment == 0 )
            alignment = 16;
        void* ptr = nullptr;
        if( posix_memalign( &ptr, alignment, size ) != 0 )
            return nullptr;
        return ptr;
    }

    __attribute__((weak))
    void* realloc_aligned( void* ptr, unsigned long size, unsigned int alignment, char const* /*tag*/ )
    {
        if( size == 0 )
        {
            std::free( ptr );
            return nullptr;
        }
        if( alignment == 0 )
            alignment = 16;
        void* new_ptr = nullptr;
        if( posix_memalign( &new_ptr, alignment, size ) != 0 )
        {
            std::free( ptr );
            return nullptr;
        }
        if( ptr != nullptr )
        {
            size_t const old_size = malloc_size( ptr );
            size_t const copy_size = ( old_size < size ) ? old_size : size;
            std::memcpy( new_ptr, ptr, copy_size );
            std::free( ptr );
        }
        return new_ptr;
    }

    __attribute__((weak))
    void free_aligned( void* ptr, char const* /*tag*/ )
    {
        std::free( ptr );
    }
}

void ERR_PRINT_STRING( char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::vfprintf( stderr, fmt, ap );
    std::fputc( '\n', stderr );
    va_end( ap );
}

void debug_break( char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::fputs( "debug_break: ", stderr );
    std::vfprintf( stderr, fmt, ap );
    std::fputc( '\n', stderr );
    va_end( ap );
}

void DBG_PRINT_STRING( char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::vfprintf( stderr, fmt, ap );
    std::fputc( '\n', stderr );
    va_end( ap );
}

void PRINT_STRING( char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::vfprintf( stdout, fmt, ap );
    std::fputc( '\n', stdout );
    va_end( ap );
}

void debug_break_if( bool b_break, char const* fmt, ... )
{
    if( !b_break )
        return;
    std::va_list ap;
    va_start( ap, fmt );
    std::fputs( "debug_break_if: ", stderr );
    std::vfprintf( stderr, fmt, ap );
    std::fputc( '\n', stderr );
    va_end( ap );
}

void GOOD_PRINT_STRING( char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::vfprintf( stdout, fmt, ap );
    std::fputc( '\n', stdout );
    va_end( ap );
}

void SWITCH_PRINT_STATE( char const* name, bool state )
{
    std::fprintf( stdout, "[switch] %s = %s\n", name ? name : "?", state ? "on" : "off" );
}

void BELL() {}

void BOX_ERR( char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::fputs( "BOX_ERR: ", stderr );
    std::vfprintf( stderr, fmt, ap );
    std::fputc( '\n', stderr );
    va_end( ap );
}

//  BOX_TITLE_ERR_VA stub from c66 reverted alongside the mem dispatch
//  flag stubs. Same reason : unused now that the o_str_real test
//  target is deferred.

//  Added continuation 63 for aaalua_util.cpp port. aaalua_util.cpp's
//  pop_str_and_print + aaalua_stack_dump paths funnel through these
//  PRINT macros (declared in err.h). Tests don't need to see the
//  formatted output ; route to stderr.
void HEADER_PRINT_STRING( char const* header, char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    if( header && *header ) std::fputs( header, stderr );
    std::vfprintf( stderr, fmt, ap );
    std::fputc( '\n', stderr );
    va_end( ap );
}

void VERBOSE_PRINT_STRING( char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::vfprintf( stderr, fmt, ap );
    std::fputc( '\n', stderr );
    va_end( ap );
}

class c_net;
c_net* net = nullptr;

//  Continuation 65 -- cascade exposed when aaa_str.cpp was added to
//  aaaseed_code_utils (c64). aaa_str.cpp's MEMMOVE pulls mem::memmove
//  out of aaa_mem.cpp which then references the dispatch-flag globals
//  + c_cpu::one (the singleton). The real flags / singleton live in
//  CPU.cpp which we haven't ported (621 LOC, pulls infrastructure/param
//  + aaaseed.h + obj/root.h -- multi-session).
//
//  Stub strategy : null-safe defaults. c_cpu::get_align_def() already
//  contains `if(one)` -- a null `c_cpu::one` flows through the
//  fallback `CACHE_LINE_SIZE_DEFAULT = 64` branch. mem::update()
//  references the dispatch flags only to AND them with the runtime
//  capability check, so default-zero gives "no special dispatch" --
//  the engine then falls back to libc memcpy/memmove. Correct for
//  tests ; only the actual production .app would prefer real dispatch
//  decisions, which is what porting CPU.cpp would deliver later.

//  c65/c66 stubs (mem dispatch flags + memmove + spy::sleep + BOX_TITLE_
//  ERR_VA) REVERTED -- they collide with the libaaaseed_code_utils.a
//  versions (aaa_mem.cpp / memcpy.cpp) once a test target also pulls
//  the real aaa_str.cpp via the lib. Multi-session port problem
//  documented in task #97. The 3 c65/c66 stub files
//  (aaalua_cpu_stub.cpp + aaalua_file_virtual_stub.cpp +
//  aaalua_dir_pool_stub.cpp) survive on disk for that future session.

//  c_cpu::one singleton lives in aaalua_cpu_stub.cpp (its own TU).
//  Splitting it avoids a name-lookup collision : this TU uses
//  `namespace mem { ... }` (decades-old stub convention) while the
//  engine's aaa_mem.h declares `class mem final`. Including CPU.h
//  here would pull aaa_mem.h transitively and break the REALLOC
//  inline macro's resolution of `mem::realloc` (class-static vs
//  namespace-member ambiguity).
