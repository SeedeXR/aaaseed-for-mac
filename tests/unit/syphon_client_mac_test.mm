// tests/unit/syphon_client_mac_test.mm
//
// c124-C : Phase 6 reciprocity LAST HOP -- ClientMac coverage. Pairs
// with the c122/c123-A syphon_mac_smoke_test ServerMac coverage. The
// critical test here is ServerToClientSameProcess : create a
// ServerMac + a ClientMac in the same process, publish an IOSurface,
// poll the runloop, verify the client picked it up + the recovered
// IOSurfaceRef matches the original ID.
//
// All paths are sandbox-safe : no bootstrap_register, no entitlements,
// no Mach-port name registration. DN observer registration uses
// CFNotificationCenterAddObserver only.
//
// Parallel-ctest robustness :
//   The system distnoted daemon drops deliveries when many ctest -j
//   workers post tight loops on the SAME distributed notification
//   name. To keep these same-process tests rock-solid under
//   ctest -j N, c124-C extends both ServerMac and ClientMac to ALSO
//   use CFNotificationCenterGetLocalCenter() -- the local center is
//   in-process only and synchronous, so same-process post + observe
//   is 100% reliable regardless of cross-process distnoted noise.
//   Each test still uses a unique PID-stamped server name to keep
//   any incidental distributed deliveries clearly distinguishable.

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <Metal/Metal.h>
#import <gtest/gtest.h>
#import <string>
#import <unistd.h>

#import "src/syphon/syphon_client_mac.h"
#import "src/syphon/syphon_mac.h"

namespace
{
//	Helper : build a BGRA8 IOSurface of the given dims. Returns +1
//	retained.
IOSurfaceRef MakeBGRA8Surface( NSUInteger width, NSUInteger height )
{
    NSDictionary * props = @{
        (NSString *) kIOSurfaceWidth          : @( width ),
        (NSString *) kIOSurfaceHeight         : @( height ),
        (NSString *) kIOSurfaceBytesPerElement: @4,
        (NSString *) kIOSurfacePixelFormat    : @( (UInt32) 'BGRA' ),
    };
    return IOSurfaceCreate( (CFDictionaryRef) props );
}

//	Helper : pid-stamped, test-stamped unique server name. Defeats
//	cross-process DN cross-talk between ctest -j workers (even
//	though local-center delivery is what we depend on for these
//	tests, the distributed center still fires on the same name
//	pattern and uniqueness avoids confusing the assertions).
std::string UniqueServerName( char const * test_tag )
{
    std::string name = "AAASeedClientTest_";
    name += test_tag;
    name += "_pid";
    name += std::to_string( (long) getpid() );
    return name;
}

//	Drain any foreign-process DN frames already queued at this
//	point. Local-center deliveries are in-process only -- they can't
//	come from another ctest worker -- but cross-process DN
//	deliveries CAN seep in. We drain everything currently pending so
//	the next publish_iosurface() leaves the client in a clean
//	state.
void DrainPendingNoise( aaa::syphon::ClientMac & client )
{
    //	Two short ticks to flush whatever distnoted dropped on us.
    client.poll( 0.05 );
    while( client.has_frame() )
    {
        IOSurfaceRef noise = client.consume_iosurface();
        if( noise != nullptr )
            CFRelease( noise );
        client.poll( 0.01 );
    }
}
}   //	anonymous namespace

TEST( SyphonClientMac, EmptyClientHasNoFrame )
{
    @autoreleasepool
    {
        aaa::syphon::ClientMac client;
        client.poll( 0.05 );
        //	Drain any noise that crept in from a parallel ctest
        //	worker posting on the distributed center.
        if( client.has_frame() )
        {
            IOSurfaceRef noise = client.consume_iosurface();
            if( noise != nullptr )
                CFRelease( noise );
        }
        EXPECT_FALSE( client.has_frame() );
        EXPECT_EQ( client.consume_iosurface(), nullptr );
    }
}

TEST( SyphonClientMac, ServerToClientSameProcess )
{
    @autoreleasepool
    {
        std::string const server_name =
            UniqueServerName( "Same" );
        aaa::syphon::ServerMac server( server_name.c_str() );
        ASSERT_TRUE( server.is_open() );
        ASSERT_EQ( server.advertise(), aaa::syphon::STATUS_OK );

        aaa::syphon::ClientMac client;
        DrainPendingNoise( client );

        IOSurfaceRef surface = MakeBGRA8Surface( 4, 4 );
        ASSERT_NE( surface, nullptr );
        IOSurfaceID const original_id = IOSurfaceGetID( surface );
        EXPECT_NE( original_id, (IOSurfaceID) 0 );

        EXPECT_EQ( server.publish_iosurface( surface ),
                   aaa::syphon::STATUS_OK );

        //	Local center delivery is synchronous + inline -- the
        //	trampoline fires BEFORE publish_iosurface returns. So at
        //	this point has_frame() must already be true with our
        //	server's name. NO poll() : a poll() drains pending
        //	distributed-center notifications from cross-process
        //	parallel ctest workers, which would overwrite our slot.

        ASSERT_TRUE( client.has_frame() );
        EXPECT_STREQ( client.last_server_name(),
                      server_name.c_str() );

        IOSurfaceRef recovered = client.consume_iosurface();
        ASSERT_NE( recovered, nullptr );
        EXPECT_EQ( IOSurfaceGetID( recovered ), original_id );
        EXPECT_EQ( IOSurfaceGetWidth( recovered ),  (size_t) 4 );
        EXPECT_EQ( IOSurfaceGetHeight( recovered ), (size_t) 4 );
        CFRelease( recovered );

        EXPECT_FALSE( client.has_frame() );
        CFRelease( surface );
    }
}

TEST( SyphonClientMac, TwoServersOneClient )
{
    @autoreleasepool
    {
        std::string const name_a = UniqueServerName( "TwoA" );
        std::string const name_b = UniqueServerName( "TwoB" );

        aaa::syphon::ServerMac server_a( name_a.c_str() );
        aaa::syphon::ServerMac server_b( name_b.c_str() );
        ASSERT_EQ( server_a.advertise(), aaa::syphon::STATUS_OK );
        ASSERT_EQ( server_b.advertise(), aaa::syphon::STATUS_OK );

        aaa::syphon::ClientMac client;
        DrainPendingNoise( client );

        IOSurfaceRef surface_a = MakeBGRA8Surface( 4, 4 );
        IOSurfaceRef surface_b = MakeBGRA8Surface( 8, 8 );
        ASSERT_NE( surface_a, nullptr );
        ASSERT_NE( surface_b, nullptr );

        //	Server A publishes -> trampoline fires INLINE (local
        //	center delivery is synchronous) -> client holds name_a.
        //	NO poll() : cross-process distributed deliveries would
        //	overwrite the slot.
        EXPECT_EQ( server_a.publish_iosurface( surface_a ),
                   aaa::syphon::STATUS_OK );
        ASSERT_TRUE( client.has_frame() );
        std::string after_a( client.last_server_name() );
        EXPECT_EQ( after_a, name_a );
        IOSurfaceRef recv_a = client.consume_iosurface();
        ASSERT_NE( recv_a, nullptr );
        CFRelease( recv_a );

        //	Server B publishes -> trampoline fires inline -> client
        //	holds name_b. Latest-wins semantics.
        EXPECT_EQ( server_b.publish_iosurface( surface_b ),
                   aaa::syphon::STATUS_OK );
        ASSERT_TRUE( client.has_frame() );
        std::string after_b( client.last_server_name() );
        EXPECT_EQ( after_b, name_b );
        IOSurfaceRef recv_b = client.consume_iosurface();
        ASSERT_NE( recv_b, nullptr );
        CFRelease( recv_b );

        CFRelease( surface_a );
        CFRelease( surface_b );
    }
}

TEST( SyphonClientMac, ConsumeMetalTexture )
{
    @autoreleasepool
    {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if( device == nil )
        {
            GTEST_SKIP() << "Metal device unavailable on this runner ; "
                            "skipping Syphon Metal-texture consume test.";
        }

        std::string const server_name =
            UniqueServerName( "Mtl" );
        aaa::syphon::ServerMac server( server_name.c_str() );
        ASSERT_EQ( server.advertise(), aaa::syphon::STATUS_OK );

        aaa::syphon::ClientMac client;
        DrainPendingNoise( client );

        IOSurfaceRef surface = MakeBGRA8Surface( 16, 16 );
        ASSERT_NE( surface, nullptr );
        IOSurfaceID const original_id = IOSurfaceGetID( surface );

        EXPECT_EQ( server.publish_iosurface( surface ),
                   aaa::syphon::STATUS_OK );

        //	Local center delivery is synchronous + inline ; no
        //	poll() needed (and a poll would let cross-process noise
        //	overwrite the slot).
        ASSERT_TRUE( client.has_frame() );

        id<MTLTexture> texture =
            client.consume_metal_texture( device );
        ASSERT_NE( texture, nil );
        EXPECT_EQ( texture.width,  (NSUInteger) 16 );
        EXPECT_EQ( texture.height, (NSUInteger) 16 );
        EXPECT_EQ( texture.pixelFormat, MTLPixelFormatBGRA8Unorm );

        //	The recovered MTLTexture must be IOSurface-backed and
        //	point at the SAME kernel surface we published.
        IOSurfaceRef tex_surface = [texture iosurface];
        ASSERT_NE( tex_surface, nullptr );
        EXPECT_EQ( IOSurfaceGetID( tex_surface ), original_id );

        EXPECT_FALSE( client.has_frame() );
        CFRelease( surface );
    }
}
