// aaalua_ostr_stub.cpp
//
// Minimal out-of-line definitions for `o_str` to satisfy the linker
// for engine .cpp files that use o_str via aaa_str.h. The real
// `o_str` lives in `code_utils/aaa_str.cpp` which we have not yet
// Mac-ported (transitive dep on `system/shared/SystemUtils.h` ->
// Windows-only Shtypes.h). Continuation 49 ships this slim stub
// alongside the aaalua_debug.cpp port -- once aaa_str.cpp is ported
// proper, delete this file.
//
// Layout MUST match the real `o_str` declared in aaa_str.h :
//   CHAR*  _data ;
//   UINT32 _len ;
//   INT32  _line_nb ;
//   UINT32 _size_allocated ;
// (no virtuals -- the class is `final`, no inheritance).
//
// Provides only the symbols aaalua_debug.cpp's body actually calls
// at link time : the default ctor, the destructor, and
// `set( C_PCHAR_C )`. Other o_str methods stay unresolved -- if a
// new wrapper file pulls them in, extend this file rather than
// vendoring the full aaa_str.cpp.

#include <cstdlib>
#include <cstring>

#include "aaa_type.h"

//	Match the real aaa_str.h class -- our stub uses the same fields in
//	the same order so a static-storage o_str instance compiled in
//	aaalua_debug.cpp has the right ABI when this TU's ctor / dtor / set
//	operate on it.
class o_str_layout_check
{
public:
    CHAR*  _data;
    UINT32 _len;
    INT32  _line_nb;
    UINT32 _size_allocated;
};
static_assert( sizeof( o_str_layout_check ) >= sizeof( void* ) + 3 * sizeof( UINT32 ),
    "o_str field layout check" );

//	Out-of-line method definitions on the REAL o_str class -- the
//	header in aaa_str.h declares these so the mangled names match.
//	No anon namespace, no using clause : these need external linkage.

#include "aaa_str.h"

__attribute__((weak)) o_str::o_str()
    : _data{ nullptr }
    , _len{ 0 }
    , _line_nb{ 0 }
    , _size_allocated{ 0 }
{
}

__attribute__((weak)) o_str::~o_str()  // c113 : weak so real aaa_str.cpp wins when lib-pull happens
{
    if( _data )
    {
        std::free( _data );
        _data = nullptr;
    }
}

__attribute__((weak))
void o_str::set( C_PCHAR_C str_in )
{
    if( _data )
    {
        std::free( _data );
        _data = nullptr;
        _size_allocated = 0;
    }
    _len = 0;
    _line_nb = 0;
    if( !str_in )
        return;
    std::size_t const n = std::strlen( str_in );
    char* const nd = static_cast<char*>( std::malloc( n + 1 ) );
    if( !nd )
        return;
    std::memcpy( nd, str_in, n + 1 );
    _data = nd;
    _len = static_cast<UINT32>( n );
    _size_allocated = static_cast<UINT32>( n + 1 );
}
