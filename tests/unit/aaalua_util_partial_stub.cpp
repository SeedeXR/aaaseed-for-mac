// aaalua_util_partial_stub.cpp
//
// Minimum out-of-line members of `c_lua_state` + free functions in the
// `aaalua` namespace that the wrapper-port test targets need at link
// time. Continuation 51 adds this when aaalua_array.cpp uses
// `c_lua_state::add_fn_to_table` (out-of-line in aaalua_util.cpp:725)
// and `aaalua::new_metatable` (out-of-line in aaalua_util.cpp:741).
//
// Definitions are LIFTED VERBATIM from aaalua_util.cpp -- the inline
// helpers they call (`push_string`, `set_table`, `set_field`,
// `get_table_always`, `pop`) are FINLINE in aaalua_util.h so the
// linkage resolves entirely from headers + this TU.
//
// Once aaalua_util.cpp is fully Mac-ported (or its out-of-line methods
// are migrated into the header as FINLINE), delete this file and link
// the real aaalua_util.cpp.

#include <cstdarg>
#include <cstdio>

#include "aaa_type.h"

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
}

#include "language/lua/aaalua_util.h"

C_PCHAR_C c_lua_state::get_type_str( INT32 type )
{
    switch( type )
    {
    case LUA_TNIL:              return "Nil";
    case LUA_TBOOLEAN:          return "Boolean";
    case LUA_TLIGHTUSERDATA:    return "LightUserData";
    case LUA_TNUMBER:           return "Number";
    case LUA_TSTRING:           return "String";
    case LUA_TTABLE:            return "Table";
    case LUA_TFUNCTION:         return "Function";
    case LUA_TUSERDATA:         return "UserData";
    case LUA_TTHREAD:           return "Thread";
    default:                    return "None";
    }
}

void c_lua_state::error_and_escape( C_PCHAR_C fmt, ... ) CONST
{
    va_list args;
    va_start( args, fmt );
    char mess[ 256 ];
    std::vsnprintf( mess, 255, fmt, args );
    va_end( args );

    luaL_error( _L, "%s", mess );
}

void c_lua_state::add_fn_to_table( C_PCHAR_C name, lua_CFunction CONST fn, C_PCHAR_C str_help ) CONST
{
    push_string( name );
    lua_pushcfunction( _L, fn );
    set_table( -3 );

    if( str_help )
    {
        get_table_always( "doc" );
        set_field( name, str_help );
        pop( 1 );
    }
}

namespace aaalua
{
    int new_metatable( lua_State* CONST L, C_PCHAR tname )
    {
        lua_getfield( L, LUA_REGISTRYINDEX, tname );  // get registry.name
        if( !lua_isnil( L, -1 ) )                     // name already in use ?
            return 0;                                 // leave previous value on top, but return 0
        lua_pop( L, 1 );
        lua_newtable( L );                            // create metatable
        lua_pushvalue( L, -1 );
        lua_setfield( L, LUA_REGISTRYINDEX, tname );  // registry.name = metatable
        return 1;
    }
}
