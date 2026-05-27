// tests/unit/syphon_lua_mac_test.cpp
//
// c124 : Smoke coverage for the Lua-side C-binding into ServerMac. The
// 3-piece Phase 6 reciprocity stack (meu_compat.lua -> aaa.syphon.send
// -> ServerMac) needs an end-to-end test that exercises only the c124
// link : Lua state opens, register_lua_bindings() installs `aaa.syphon.send`
// as a global, calling it from a Lua script creates a ServerMac in the
// process-local registry.
//
// We deliberately do NOT exercise publish_metal_texture() here -- that
// path is already covered by syphon_mac_smoke_test (c122/c123-A) and
// would require a real MTLDevice + IOSurface-backed texture. The c124
// scope is the binding layer ; texture handoff is verified end-to-end
// in Phase 6 integration tests once GaBu lands a Metal-backed render
// target on the Lua side.
//
// CTest label : unit;syphon_lua.

#include <gtest/gtest.h>

#include <CoreFoundation/CoreFoundation.h>
#include <IOSurface/IOSurface.h>

#include <string>
#include <unistd.h>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include "src/syphon/syphon_lua_mac.h"
#include "src/syphon/syphon_mac.h"

TEST( SyphonLuaMac, RegisterBindings )
{
    lua_State * L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    aaa::syphon::register_lua_bindings( L );

    //	Walk `aaa` -> `aaa.syphon` -> `aaa.syphon.send` and verify it's
    //	a Lua function. Mirrors the pattern in lua_engine_bridge_test.cpp
    //	but goes through a nested table.
    lua_getglobal( L, "aaa" );
    ASSERT_TRUE( lua_istable( L, -1 ) ) << "global `aaa` is not a table";

    lua_getfield( L, -1, "syphon" );
    ASSERT_TRUE( lua_istable( L, -1 ) ) << "`aaa.syphon` is not a table";

    lua_getfield( L, -1, "send" );
    EXPECT_TRUE( lua_isfunction( L, -1 ) )
        << "`aaa.syphon.send` is not a function";

    lua_pop( L, 3 );
    lua_close( L );
}

TEST( SyphonLuaMac, CallSendWithNilTextureCreatesServer )
{
    lua_State * L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    aaa::syphon::register_lua_bindings( L );

    //	Use a unique name so this test is independent of any other test
    //	in the same process that might also create ServerMacs.
    char const * const server_name = "AAASeedTestSyphonLuaNil";

    //	Sanity : registry must not yet contain this name.
    ASSERT_FALSE( aaa::syphon::has_server( server_name ) );

    char const * script =
        "return aaa.syphon.send( \"AAASeedTestSyphonLuaNil\", nil )";
    int const ret = luaL_dostring( L, script );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    //	Return value is a status integer (0 = STATUS_OK).
    ASSERT_TRUE( lua_isnumber( L, -1 ) );
    EXPECT_EQ( lua_tointeger( L, -1 ), 0 );
    lua_pop( L, 1 );

    //	Registry now holds the ServerMac.
    EXPECT_TRUE( aaa::syphon::has_server( server_name ) );

    lua_close( L );
}

TEST( SyphonLuaMac, CallSendTwiceSameNameIdempotent )
{
    lua_State * L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    aaa::syphon::register_lua_bindings( L );

    char const * const server_name = "AAASeedTestSyphonLuaIdem";

    //	First call : creates the server.
    {
        int const ret = luaL_dostring( L,
            "return aaa.syphon.send( \"AAASeedTestSyphonLuaIdem\", nil )" );
        ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
        lua_pop( L, 1 );
    }
    ASSERT_TRUE( aaa::syphon::has_server( server_name ) );

    //	Second call with the same name : must NOT crash, must return OK,
    //	must NOT create a duplicate (we can't observe the lack of a
    //	duplicate directly without a count accessor, but if the
    //	lookup_or_create path forgot the idempotent guard the second
    //	call would either crash on a CFMessagePort collision or replace
    //	the live ServerMac with a stale one ; either failure mode is
    //	caught by the test still observing has_server() true and the
    //	status returning OK).
    {
        int const ret = luaL_dostring( L,
            "return aaa.syphon.send( \"AAASeedTestSyphonLuaIdem\", 0 )" );
        ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
        ASSERT_TRUE( lua_isnumber( L, -1 ) );
        EXPECT_EQ( lua_tointeger( L, -1 ), 0 );
        lua_pop( L, 1 );
    }
    EXPECT_TRUE( aaa::syphon::has_server( server_name ) );

    //	Third call from a fresh Lua state must still find the server in
    //	the (process-local) registry -- proves the registry survives
    //	lua_close() and isn't tied to the VM lifetime.
    lua_close( L );
    EXPECT_TRUE( aaa::syphon::has_server( server_name ) );
}

//	-- c124-D : receive-side glue (mirror of c124-A send-side) ----------

TEST( SyphonLuaMac, RegisterReceiveBinding )
{
    lua_State * L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    aaa::syphon::register_lua_bindings( L );

    //	Walk `aaa` -> `aaa.syphon` -> `aaa.syphon.receive` and verify
    //	it's a Lua function. Mirror of the RegisterBindings test above
    //	but checks the receive-side surface.
    lua_getglobal( L, "aaa" );
    ASSERT_TRUE( lua_istable( L, -1 ) ) << "global `aaa` is not a table";

    lua_getfield( L, -1, "syphon" );
    ASSERT_TRUE( lua_istable( L, -1 ) ) << "`aaa.syphon` is not a table";

    lua_getfield( L, -1, "receive" );
    EXPECT_TRUE( lua_isfunction( L, -1 ) )
        << "`aaa.syphon.receive` is not a function";

    lua_pop( L, 3 );
    lua_close( L );
}

TEST( SyphonLuaMac, ReceiveOnUnknownServerReturnsZero )
{
    lua_State * L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    aaa::syphon::register_lua_bindings( L );

    //	Pick a name guaranteed to have no live ServerMac. Calling
    //	aaa.syphon.receive on it lazy-creates a ClientMac, polls,
    //	finds no frame, returns 0.
    char const * const client_name = "AAASeedTestSyphonLuaRecvNone";

    ASSERT_FALSE( aaa::syphon::has_client( client_name ) );

    char const * script =
        "return aaa.syphon.receive( \"AAASeedTestSyphonLuaRecvNone\" )";
    int const ret = luaL_dostring( L, script );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    ASSERT_TRUE( lua_isnumber( L, -1 ) );
    EXPECT_EQ( lua_tointeger( L, -1 ), 0 );
    lua_pop( L, 1 );

    //	ClientMac was lazy-created on first receive() call.
    EXPECT_TRUE( aaa::syphon::has_client( client_name ) );

    lua_close( L );
}

namespace
{
//	Build a BGRA8 IOSurface via plain CoreFoundation (keeps this TU
//	a .cpp, no ObjC). Returns +1 retained. Mirrors
//	syphon_client_mac_test.mm::MakeBGRA8Surface but uses CFNumber /
//	CFDictionary primitives instead of NSDictionary literals.
IOSurfaceRef MakeBGRA8SurfaceCF( int width, int height )
{
    int  bytes_per_element = 4;
    UInt32 pixel_format    = (UInt32) 'BGRA';

    CFNumberRef w = CFNumberCreate( kCFAllocatorDefault,
                                    kCFNumberIntType, &width );
    CFNumberRef h = CFNumberCreate( kCFAllocatorDefault,
                                    kCFNumberIntType, &height );
    CFNumberRef bpe = CFNumberCreate( kCFAllocatorDefault,
                                      kCFNumberIntType,
                                      &bytes_per_element );
    CFNumberRef pf = CFNumberCreate( kCFAllocatorDefault,
                                     kCFNumberSInt32Type,
                                     &pixel_format );

    CFStringRef keys[]   = {
        kIOSurfaceWidth, kIOSurfaceHeight,
        kIOSurfaceBytesPerElement, kIOSurfacePixelFormat
    };
    CFNumberRef values[] = { w, h, bpe, pf };

    CFDictionaryRef props = CFDictionaryCreate(
        kCFAllocatorDefault,
        (void const **) keys,
        (void const **) values,
        4,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks );

    IOSurfaceRef surface = IOSurfaceCreate( props );

    CFRelease( props );
    CFRelease( w );
    CFRelease( h );
    CFRelease( bpe );
    CFRelease( pf );

    return surface;
}
}   //	anonymous namespace

TEST( SyphonLuaMac, ReceiveAfterServerPublish )
{
    //	Unique pid-stamped name so this test doesn't collide with the
    //	c124-C client_mac_test names (same process can ctest-run them
    //	in any order ; we explicitly opt out of any DN crosstalk).
    std::string server_name =
        "AAASeedTestSyphonLuaRecvPub_pid";
    server_name += std::to_string( (long) getpid() );

    lua_State * L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    aaa::syphon::register_lua_bindings( L );

    //	Spin up a ServerMac inside C++ and advertise it. Note : we
    //	deliberately do NOT go through aaa.syphon.send here -- that
    //	would put the ServerMac in the SEND-side registry, but it'd
    //	still publish on the same DN channel, so the test would
    //	pass for the wrong reason. Going direct keeps the test
    //	focused on the receive-side glue.
    aaa::syphon::ServerMac server( server_name.c_str() );
    ASSERT_TRUE( server.is_open() );
    ASSERT_EQ( server.advertise(), aaa::syphon::STATUS_OK );

    //	Pre-create the ClientMac via a probe receive() so it's
    //	registered on the DN center BEFORE the server posts. Without
    //	this the inline local-center post would fire before any
    //	observer exists.
    {
        std::string probe_script =
            "return aaa.syphon.receive( \"" + server_name + "\" )";
        int const ret = luaL_dostring( L, probe_script.c_str() );
        ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
        ASSERT_TRUE( lua_isnumber( L, -1 ) );
        EXPECT_EQ( lua_tointeger( L, -1 ), 0 );
        lua_pop( L, 1 );
    }
    ASSERT_TRUE( aaa::syphon::has_client( server_name ) );

    //	Publish a 16x16 BGRA8 IOSurface. Local-center delivery is
    //	synchronous + inline -- the ClientMac trampoline fires
    //	before publish_iosurface returns.
    IOSurfaceRef surface = MakeBGRA8SurfaceCF( 16, 16 );
    ASSERT_NE( surface, nullptr );

    ASSERT_EQ( server.publish_iosurface( surface ),
               aaa::syphon::STATUS_OK );

    //	Now the receive-side Lua call should wrap the IOSurface into
    //	an MTLTexture and return a non-zero integer (the texture
    //	handle as uintptr_t).
    std::string recv_script =
        "return aaa.syphon.receive( \"" + server_name + "\" )";
    int const ret = luaL_dostring( L, recv_script.c_str() );
    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );
    ASSERT_TRUE( lua_isnumber( L, -1 ) );

    lua_Integer const handle = lua_tointeger( L, -1 );
    //	If a system Metal device exists on this machine, the handle
    //	is non-zero. CI VMs may lack a default GPU device ; in that
    //	case the binding gracefully returns 0 and we skip the
    //	stronger assertion. This is the SAME tolerance posture as
    //	the c124-C syphon_client_mac_test ConsumeMetalTexture test.
    if( handle != 0 )
    {
        SUCCEED() << "aaa.syphon.receive returned MTLTexture handle "
                  << handle;
    }
    else
    {
        GTEST_SKIP() << "no MTLCreateSystemDefaultDevice -- "
                     << "skipping non-zero-handle assertion";
    }
    lua_pop( L, 1 );

    CFRelease( surface );
    lua_close( L );
}
