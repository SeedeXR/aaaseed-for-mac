// src/syphon/syphon_lua_mac.mm
//
// c124 : Lua-side C-binding bridge into ServerMac. See syphon_lua_mac.h
// for full doctrine.
//
// What this file owns :
//   - Process-local registry { name -> unique_ptr<ServerMac> }.
//   - The Lua-callable thunk `l_syphon_send` : pops (name, texture) from
//     the Lua stack, lazily creates + advertises the ServerMac, and
//     calls publish_metal_texture() iff the texture arg is a non-nil
//     non-zero integer.
//   - The `register_lua_bindings(L)` entry point that installs the
//     thunk under `aaa.syphon.send`.
//   - The test-only `has_server(name)` accessor.

#import "syphon_lua_mac.h"
#import "syphon_client_mac.h"
#import "syphon_mac.h"

#import <CoreFoundation/CoreFoundation.h>
#import <Metal/Metal.h>

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
}

namespace aaa
{
namespace syphon
{

namespace
{
//	Process-local ServerMac registry. Lua VMs are single-threaded but
//	the engine has a `c_lua_master` that may hold multiple VMs ; guard
//	the map with a mutex so a hypothetical cross-VM call from a worker
//	thread doesn't corrupt it. The lock is uncontended on the hot path.
std::mutex                                                       g_registry_mutex;
std::unordered_map<std::string, std::unique_ptr<ServerMac>>      g_registry;

//	Process-local ClientMac registry (c124-D, mirror of g_registry).
//	One ClientMac per requested server name. Each ClientMac registers
//	its own DN observer on BOTH the distributed and local centers (see
//	syphon_client_mac.mm c125-A dual-center doctrine) ; the trampoline
//	dispatches to ALL ClientMacs subscribed to "aaa.syphon.frame", but
//	the binding-layer caller filters by name via last_server_name()
//	after each poll.
std::mutex                                                       g_client_registry_mutex;
std::unordered_map<std::string, std::unique_ptr<ClientMac>>      g_client_registry;

//	Lookup-or-create. Returns a raw pointer to the live ServerMac under
//	`name`, or nullptr if the name is empty / ctor refused. Calls
//	advertise() on first creation so the CFMessagePort is registered
//	immediately ; advertise() failure is non-fatal (the server is still
//	in the registry, publish_*() will short-circuit on is_open()).
ServerMac * lookup_or_create( std::string const & name )
{
    if( name.empty() )
        return nullptr;

    std::lock_guard<std::mutex> lock( g_registry_mutex );

    auto it = g_registry.find( name );
    if( it != g_registry.end() )
        return it->second.get();

    auto server = std::make_unique<ServerMac>( name.c_str() );
    if( !server->is_open() )
        return nullptr;
    //	Best-effort advertise(). Failure (e.g. name collision with a
    //	pre-existing CFMessagePort) is reported in the return code but
    //	we still cache the ServerMac so subsequent calls don't churn
    //	the ctor.
    (void) server->advertise();

    ServerMac * raw = server.get();
    g_registry.emplace( name, std::move( server ) );
    return raw;
}

//	Lookup-or-create for the ClientMac registry. Returns a raw pointer
//	to the live ClientMac under `name`, or nullptr on empty name. The
//	ClientMac ctor registers the DN observer on both centers ; from
//	that point on the ClientMac sees ALL frames posted under
//	`aaa.syphon.frame` and the caller filters by name.
ClientMac * lookup_or_create_client( std::string const & name )
{
    if( name.empty() )
        return nullptr;

    std::lock_guard<std::mutex> lock( g_client_registry_mutex );

    auto it = g_client_registry.find( name );
    if( it != g_client_registry.end() )
        return it->second.get();

    auto client = std::make_unique<ClientMac>();
    ClientMac * raw = client.get();
    g_client_registry.emplace( name, std::move( client ) );
    return raw;
}

//	Lua-callable thunk. Signature : aaa.syphon.send( name, texture ).
//	Returns one integer : the aaa::syphon::Status code (0 on success ;
//	non-zero on partial / failed publish). A nil / 0 texture means
//	"register the server only, do not publish", which the smoke test
//	uses to exercise the lazy-create path without a Metal device.
int l_syphon_send( lua_State * L )
{
    //	Arg 1 : server name (string, required).
    if( lua_gettop( L ) < 1 || !lua_isstring( L, 1 ) )
    {
        return luaL_error( L,
            "aaa.syphon.send: arg 1 must be a string (server name)" );
    }
    std::size_t  name_len   = 0;
    char const * name_cstr  = lua_tolstring( L, 1, &name_len );
    std::string  name( name_cstr, name_len );

    ServerMac * server = lookup_or_create( name );
    if( server == nullptr )
    {
        //	Empty name or ctor refused. Surface as STATUS_NOT_OPEN so
        //	Lua can branch on it.
        lua_pushinteger( L, (lua_Integer) STATUS_NOT_OPEN );
        return 1;
    }

    //	Arg 2 : texture handle (integer or nil). nil / absent / 0 means
    //	"no texture, just keep the server alive + advertised" -- useful
    //	for tests and for the eventual GaBu-side warmup path.
    bool has_texture = false;
    std::uintptr_t texture_handle = 0;
    if( lua_gettop( L ) >= 2 && !lua_isnil( L, 2 ) )
    {
        if( lua_isnumber( L, 2 ) )
        {
            lua_Integer raw = lua_tointeger( L, 2 );
            if( raw != 0 )
            {
                texture_handle = (std::uintptr_t) raw;
                has_texture    = true;
            }
        }
        else
        {
            return luaL_error( L,
                "aaa.syphon.send: arg 2 must be integer or nil (texture handle)" );
        }
    }

    if( !has_texture )
    {
        //	Register-only path : advertise() was called by lookup_or_create.
        lua_pushinteger( L, (lua_Integer) STATUS_OK );
        return 1;
    }

    //	Cast the uintptr_t back to id<MTLTexture>. The engine pushes
    //	MTLTexture handles via `lua_pushinteger((lua_Integer)(uintptr_t)tex)` ;
    //	this is the inverse. Under MRC the bridge cast is plain ; no
    //	retain transfer happens because the engine owns the texture.
    id<MTLTexture> texture =
        (__bridge id<MTLTexture>) (void *) texture_handle;

    Status const status = server->publish_metal_texture( texture );
    lua_pushinteger( L, (lua_Integer) status );
    return 1;
}

//	Lua-callable thunk. Signature : aaa.syphon.receive( name ) -> integer.
//	Lazy-creates a ClientMac per `name`, drains pending DN deliveries
//	with a zero-second poll (the local-center inline path delivers
//	synchronously on Post, so for same-process reciprocity no actual
//	wait is needed), and if a frame is available wraps it into an
//	MTLTexture and returns the texture handle as a `lua_Integer`.
//	Returns 0 if no frame is available or the device / texture
//	creation failed. Caller (Lua side) treats 0 as "no frame".
int l_syphon_receive( lua_State * L )
{
    //	Arg 1 : server name (string, required).
    if( lua_gettop( L ) < 1 || !lua_isstring( L, 1 ) )
    {
        return luaL_error( L,
            "aaa.syphon.receive: arg 1 must be a string (server name)" );
    }
    std::size_t  name_len   = 0;
    char const * name_cstr  = lua_tolstring( L, 1, &name_len );
    std::string  name( name_cstr, name_len );

    ClientMac * client = lookup_or_create_client( name );
    if( client == nullptr )
    {
        lua_pushinteger( L, (lua_Integer) 0 );
        return 1;
    }

    //	Zero-timeout poll : drains any queued DN deliveries inline.
    //	For same-process traffic the local-center delivery already
    //	fired synchronously on Post (see syphon_client_mac.mm dual-
    //	center doctrine), so this is a no-op fast path. For
    //	cross-process traffic this still runs the runloop briefly.
    client->poll( 0.0 );

    if( !client->has_frame() )
    {
        lua_pushinteger( L, (lua_Integer) 0 );
        return 1;
    }

    //	Filter by server name : the ClientMac observer fires for ALL
    //	servers posting under "aaa.syphon.frame", but this caller
    //	asked for a SPECIFIC name. If the latest frame's server name
    //	doesn't match, treat it as "no frame for me" -- but consume
    //	it anyway so we don't pin a stale slot forever.
    if( name != client->last_server_name() )
    {
        IOSurfaceRef stale = client->consume_iosurface();
        if( stale != nullptr )
            CFRelease( stale );
        lua_pushinteger( L, (lua_Integer) 0 );
        return 1;
    }

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if( device == nil )
    {
        //	No Metal device (headless / CI without a GPU). Drop the
        //	frame ; the consumer can poll again later but without a
        //	device there's nothing to hand back.
        IOSurfaceRef drop = client->consume_iosurface();
        if( drop != nullptr )
            CFRelease( drop );
        lua_pushinteger( L, (lua_Integer) 0 );
        return 1;
    }

    id<MTLTexture> texture = client->consume_metal_texture( device );
    //	MTLCreateSystemDefaultDevice returns +1 under MRC ; release
    //	now that the texture (if any) has its own strong reference
    //	via newTextureWithDescriptor:iosurface:plane:.
    [device release];

    if( texture == nil )
    {
        lua_pushinteger( L, (lua_Integer) 0 );
        return 1;
    }

    //	Cast id<MTLTexture> -> uintptr_t -> lua_Integer. The Lua side
    //	holds the handle as an integer ; ownership of the +1 retain
    //	from consume_metal_texture remains with the Lua caller (the
    //	engine convention is that texture handles round-trip as
    //	integers and the Lua-side wrapper releases on close).
    std::uintptr_t handle =
        (std::uintptr_t) (__bridge void *) texture;
    lua_pushinteger( L, (lua_Integer) handle );
    return 1;
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

    //	Ensure `aaa.syphon` is a table.
    lua_getfield( L, -1, "syphon" );
    if( !lua_istable( L, -1 ) )
    {
        lua_pop( L, 1 );
        lua_newtable( L );
        lua_setfield( L, -2, "syphon" );
        lua_getfield( L, -1, "syphon" );
    }

    //	Install `aaa.syphon.send`.
    lua_pushcfunction( L, &l_syphon_send );
    lua_setfield( L, -2, "send" );

    //	Install `aaa.syphon.receive` (c124-D : Mac-side receive glue,
    //	mirror of `send`).
    lua_pushcfunction( L, &l_syphon_receive );
    lua_setfield( L, -2, "receive" );

    //	Pop `aaa.syphon` and `aaa`.
    lua_pop( L, 2 );
}

bool has_server( std::string const & name )
{
    std::lock_guard<std::mutex> lock( g_registry_mutex );
    return g_registry.find( name ) != g_registry.end();
}

bool has_client( std::string const & name )
{
    std::lock_guard<std::mutex> lock( g_client_registry_mutex );
    return g_client_registry.find( name ) != g_client_registry.end();
}

}   //	namespace syphon
}   //	namespace aaa
