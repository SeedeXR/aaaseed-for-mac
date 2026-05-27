// test_engine_stubs.cpp
//
// Minimal definitions of engine-level symbols that the code_utils Batch-1
// translation units reference but which are normally provided by other
// engine source we have NOT yet ported (aaa_mem.cpp, err.cpp).
//
// This file is linked ONLY into test executables, never into the shipping
// engine. It exists so we can run code_utils unit tests in isolation while
// the rest of the engine source is still being brought across.
//
// When aaa_mem.cpp and err.cpp are properly ported to Mac (with POSIX
// substitutes for GetProcessMemoryInfo, sysctl etc), delete this file and
// link the real symbols instead.

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <malloc/malloc.h>      // macOS : malloc_size(ptr)

#include "aaa_type.h"

namespace mem
{
    //  c115 (2026-05-26) : mem::memcpy stub removed -- now provided by
    //  src/code_utils/memcpy_mac.cpp inside aaaseed_code_utils. Test
    //  targets that link the lib (all current users of this file) pick
    //  up the real Mac forwarder. Targets that DON'T link the lib never
    //  used the stub anyway.
    //
    //  mem::memclear kept here : not yet ported into the Mac code_utils
    //  lib (aaa_mem.cpp's memclear path pulls scheduler etc.). Whenever
    //  the full mem:: surface lands, this stub goes too.
    void memclear( void* dst, unsigned long n )
    {
        std::memset( dst, 0, n );
    }

    //  Aligned allocation : on macOS posix_memalign requires alignment to be
    //  a power of two and a multiple of sizeof(void*) ; the engine asks for
    //  16 / 32 / 64 byte alignment which all satisfy that.
    //
    //  malloc_size() on macOS returns the actual allocated size for any
    //  malloc-family pointer (incl. posix_memalign), so the realloc-style
    //  copy preserves the smaller of (old, new) bytes as the standard
    //  realloc() contract requires.
    void* realloc_aligned( void* ptr, unsigned long size, unsigned int alignment, char const* /*tag*/ )
    {
        if( size == 0 )
        {
            std::free( ptr );
            return nullptr;
        }
        //  REALLOC_ALIGNED(p, s) passes alignment=0 as "use the engine default".
        //  The engine resolves that via c_cpu::get_align_def() ; in tests we use
        //  a hard-coded 16 (matches the SSE/AVX-friendly alignment AAASeed math
        //  expects everywhere on x86-64). posix_memalign rejects 0.
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
    // Tests don't actually halt on debug_break; the engine in production may.
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
    //  In tests we never actually trap — fall through.
}

//  Stubs added 2026-05-26 (continuation 24) for the POSIX time/os_version
//  port. os_version.cpp uses o_str for vendor / kernel-version / hostname
//  strings ; aaa_time.cpp pulls in `_net` (a global from obj_ui/com/net.h)
//  for the time-sync update path. None of these are exercised by the
//  current tests ; minimal definitions silence the linker without porting
//  the full string + net subsystems.

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

void BELL()
{
    //  Engine plays a system beep here ; tests don't need audio.
}

void BOX_ERR( char const* fmt, ... )
{
    std::va_list ap;
    va_start( ap, fmt );
    std::fputs( "BOX_ERR: ", stderr );
    std::vfprintf( stderr, fmt, ap );
    std::fputc( '\n', stderr );
    va_end( ap );
}

//  Minimal o_str just enough to satisfy os_version.cpp's static-init writes
//  (set + ctor + dtor). The real string class lives in aaa_str.cpp which
//  has not been Mac-ported yet. We define an empty class body matching the
//  declarations referenced by the linker.
//  NOTE : if the test ever inspects o_str contents this stub will lie ;
//  the time/os tests don't, they only verify POSIX-API return values and
//  numeric fields populated by sysctlbyname.
class o_str
{
public:
    o_str();
    ~o_str();
    void set( char const* );
private:
    char const* _payload = nullptr;
};
o_str::o_str()  = default;
o_str::~o_str() = default;
void o_str::set( char const* s ) { _payload = s; }

//  `net` is the global pointer of type `c_net*` declared in
//  `obj_ui/com/net.h` (line 517: `extern c_net* net;`). aaa_time.cpp's
//  update() path calls `net->is_time_slave()`, `net->is_time_received()`,
//  `net->get_time_received()` BUT first checks `if( net && ...)`, so
//  `net == nullptr` makes the whole branch a no-op. We define `net` as
//  a null pointer with the right TYPE TAG (forward-declare c_net) so the
//  linker is happy AND aaa_time::update() skips the net-sync branch at
//  runtime. The c_net type itself is never instantiated.
class c_net;
c_net* net = nullptr;
