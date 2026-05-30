// src/net/net_json_lua.mm
//
// Implementation of the JSON-string -> Lua-table binding declared in
// net_json_lua.h. Pure ObjC++ over NSJSONSerialization + Foundation,
// manual reference counting (-fno-objc-arc). Links aaaseed_lua.
//
// Verified Foundation / CoreFoundation symbols (MacOSX SDK) :
//   +[NSJSONSerialization JSONObjectWithData:options:error:]
//   NSJSONReadingFragmentsAllowed   (allow top-level scalars/arrays)
//   CFGetTypeID / CFBooleanGetTypeID (distinguish JSON true/false from
//                                     numbers, since both arrive as
//                                     NSNumber)
//   -[NSNumber doubleValue], -[NSNumber boolValue]
//   -[NSString UTF8String]
//   NSNull (singleton sentinel for JSON null)

#import "net_json_lua.h"

#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>

#include <string>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
}

namespace aaa
{
namespace net
{

namespace
{
//	Forward declaration : the recursive value pusher.
void push_object( lua_State * L, id obj );

//	Push an NSDictionary as a Lua table keyed by string keys. NSNull
//	values are skipped (Lua has no concept of a stored nil ; setting a
//	key to nil would be a no-op / removal). Non-string keys are ignored
//	-- JSON object keys are always strings, but we guard defensively.
void push_dictionary( lua_State * L, NSDictionary * dict )
{
    lua_newtable( L );
    for( id key in dict )
    {
        if( ![key isKindOfClass:[NSString class]] )
            continue;

        id value = [dict objectForKey:key];
        if( value == nil || value == [NSNull null] )
            continue;   //	JSON null -> leave the key unset (nil)

        //	Push the value first, then set t[key] = value. Using
        //	lua_setfield keeps the key as a Lua string.
        push_object( L, value );
        lua_setfield( L, -2, [(NSString *) key UTF8String] );
    }
}

//	Push an NSArray as a 1-indexed Lua table. NSNull entries become a
//	stored... nothing : we cannot store nil at an index without leaving a
//	hole, so an NSNull element is pushed as Lua nil via lua_pushnil and
//	assigned, which removes/leaves the slot empty. To preserve positions
//	we still advance the index. This matches the JSON-array semantics
//	where `[1, null, 3]` yields t[1]=1, t[2]=nil, t[3]=3.
void push_array( lua_State * L, NSArray * arr )
{
    lua_newtable( L );
    int index = 1;
    for( id value in arr )
    {
        if( value == nil || value == [NSNull null] )
            lua_pushnil( L );
        else
            push_object( L, value );

        lua_rawseti( L, -2, index );
        ++index;
    }
}

//	Push a single NSNumber. JSON true/false also arrive as NSNumber, so
//	we first test whether the underlying CFTypeID is CFBoolean ; if so
//	push a Lua boolean, otherwise push a Lua number (double).
void push_number( lua_State * L, NSNumber * num )
{
    if( CFGetTypeID( (CFTypeRef) num ) == CFBooleanGetTypeID() )
        lua_pushboolean( L, [num boolValue] ? 1 : 0 );
    else
        lua_pushnumber( L,
            static_cast<lua_Number>( [num doubleValue] ) );
}

//	Recursive dispatch : push whatever Foundation object the JSON parse
//	produced as the matching Lua value. Unknown / NSNull objects push
//	Lua nil.
void push_object( lua_State * L, id obj )
{
    if( obj == nil || obj == [NSNull null] )
    {
        lua_pushnil( L );
        return;
    }

    if( [obj isKindOfClass:[NSDictionary class]] )
        push_dictionary( L, (NSDictionary *) obj );
    else if( [obj isKindOfClass:[NSArray class]] )
        push_array( L, (NSArray *) obj );
    else if( [obj isKindOfClass:[NSString class]] )
        lua_pushstring( L, [(NSString *) obj UTF8String] );
    else if( [obj isKindOfClass:[NSNumber class]] )
        push_number( L, (NSNumber *) obj );
    else
        lua_pushnil( L );   //	unexpected leaf type
}
}   //	anonymous namespace

int parse_json_to_lua( lua_State * L, std::string const & json )
{
    @autoreleasepool
    {
        NSData * data = [NSData dataWithBytes:json.data()
                                       length:json.size()];

        NSError * error = nil;
        //	FragmentsAllowed so top-level scalars/arrays parse too, not
        //	just objects -- the conversion handles every JSON shape.
        id obj = [NSJSONSerialization
                    JSONObjectWithData:data
                               options:NSJSONReadingFragmentsAllowed
                                 error:&error];

        if( obj == nil )
        {
            //	Parse error : push nil + an error string (2 values).
            lua_pushnil( L );
            if( error != nil )
                lua_pushstring( L,
                    [[error localizedDescription] UTF8String] );
            else
                lua_pushstring( L, "JSON parse error" );
            return 2;
        }

        push_object( L, obj );
        return 1;
    }
}

namespace
{
//	Lua-callable thunk : aaa.net.parse_json( jsonString ).
//	Returns the parsed table / scalar (1 value) on success, or nil +
//	error string (2 values) on parse failure.
int l_parse_json( lua_State * L )
{
    if( lua_gettop( L ) < 1 || !lua_isstring( L, 1 ) )
        return luaL_error( L,
            "aaa.net.parse_json : expected a JSON string argument" );

    size_t       len = 0;
    char const * str = lua_tolstring( L, 1, &len );
    std::string  json( str ? str : "", str ? len : 0 );

    return parse_json_to_lua( L, json );
}
}   //	anonymous namespace

void register_lua_bindings( lua_State * L )
{
    if( L == nullptr )
        return;

    //	Ensure `aaa` is a table.
    lua_getglobal( L, "aaa" );
    if( !lua_istable( L, -1 ) )
    {
        lua_pop( L, 1 );
        lua_newtable( L );
        lua_setglobal( L, "aaa" );
        lua_getglobal( L, "aaa" );
    }

    //	Ensure `aaa.net` is a table.
    lua_getfield( L, -1, "net" );
    if( !lua_istable( L, -1 ) )
    {
        lua_pop( L, 1 );
        lua_newtable( L );
        lua_setfield( L, -2, "net" );
        lua_getfield( L, -1, "net" );
    }

    //	Install `aaa.net.parse_json`.
    lua_pushcfunction( L, &l_parse_json );
    lua_setfield( L, -2, "parse_json" );

    //	Pop `aaa.net` and `aaa`.
    lua_pop( L, 2 );
}

}   //	namespace net
}   //	namespace aaa
