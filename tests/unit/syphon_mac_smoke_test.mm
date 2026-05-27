// tests/unit/syphon_mac_smoke_test.mm
//
// c122 : Smoke coverage for the Phase 6 reciprocity beachhead. Verifies
// the API signature of aaa::syphon::ServerMac compiles + the ctor /
// publish_*() / dtor cycle doesn't crash.
//
// c123-A : Additional coverage for the Mach-port-free advertisement
// path :
//   - advertise() creates a local CFMessagePort + flips is_advertised().
//   - advertise() is idempotent.
//   - publish_metal_texture() posts a Distributed Notification once
//     advertised ; an in-process observer registered on the distributed
//     center receives it.
//   - IOSurfaceCreateXPCObject + IOSurfaceLookupFromXPCObject round-trip
//     preserves the IOSurfaceID.
//
// All paths are sandbox-safe : no bootstrap_register, no entitlements.

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <Metal/Metal.h>
#import <xpc/xpc.h>
#import <gtest/gtest.h>

#import "src/syphon/syphon_mac.h"

TEST( SyphonMacSmoke, ConstructionWithNonEmptyNameOpensServer )
{
    @autoreleasepool
    {
        aaa::syphon::ServerMac server( "AAASeedTestServer" );
        EXPECT_TRUE( server.is_open() );
        EXPECT_STREQ( server.name(), "AAASeedTestServer" );
    }
}

TEST( SyphonMacSmoke, ConstructionWithEmptyNameDoesNotOpen )
{
    aaa::syphon::ServerMac server( "" );
    EXPECT_FALSE( server.is_open() );
    EXPECT_STREQ( server.name(), "" );

    //	publish_*() on a not-open server must return STATUS_NOT_OPEN
    //	rather than crash.
    EXPECT_EQ( server.publish_iosurface( nullptr ),
               aaa::syphon::STATUS_NOT_OPEN );
}

TEST( SyphonMacSmoke, PublishMetalTextureIOSurfaceBackedReturnsOk )
{
    @autoreleasepool
    {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if( device == nil )
        {
            GTEST_SKIP() << "Metal device unavailable on this runner ; "
                            "skipping IOSurface-backed publish test.";
        }

        //	Build an IOSurface-backed MTLTexture. The IOSurface must be
        //	allocated explicitly (Apple Silicon : plain
        //	newTextureWithDescriptor: yields a private-storage texture
        //	with no IOSurface backing, which we want to verify the
        //	server correctly rejects with STATUS_NO_IOSURFACE_BACK).
        NSDictionary * props = @{
            (NSString *) kIOSurfaceWidth          : @64,
            (NSString *) kIOSurfaceHeight         : @64,
            (NSString *) kIOSurfaceBytesPerElement: @4,
            (NSString *) kIOSurfacePixelFormat    : @( (UInt32) 'BGRA' ),
        };
        IOSurfaceRef surface = IOSurfaceCreate( (CFDictionaryRef) props );
        ASSERT_NE( surface, nullptr );

        MTLTextureDescriptor * desc = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                         width: 64
                                        height: 64
                                     mipmapped: NO];
        desc.usage       = MTLTextureUsageShaderRead;
        desc.storageMode = MTLStorageModeShared;

        id<MTLTexture> texture = [device newTextureWithDescriptor: desc
                                                        iosurface: surface
                                                            plane: 0];
        ASSERT_NE( texture, nil );

        aaa::syphon::ServerMac server( "AAASeedTestServerPublish" );
        ASSERT_TRUE( server.is_open() );

        EXPECT_EQ( server.publish_metal_texture( texture ),
                   aaa::syphon::STATUS_OK );

        //	Non-IOSurface-backed texture (plain newTextureWithDescriptor:)
        //	must surface STATUS_NO_IOSURFACE_BACK rather than crash.
        id<MTLTexture> private_tex =
            [device newTextureWithDescriptor: desc];
        ASSERT_NE( private_tex, nil );
        EXPECT_EQ( server.publish_metal_texture( private_tex ),
                   aaa::syphon::STATUS_NO_IOSURFACE_BACK );

        //	nil texture path.
        EXPECT_EQ( server.publish_metal_texture( nil ),
                   aaa::syphon::STATUS_NIL_TEXTURE );

        CFRelease( surface );
    }
}

TEST( SyphonMacSmoke, DestructionIsCleanAndIdempotent )
{
    //	Construct + destruct in a tight loop ; if the ctor leaked a
    //	Mach port or held a CF retain, the process would exhaust ports
    //	quickly. 256 iterations is well within the per-process port
    //	cap (default ~64k) but enough to flush an obvious leak.
    for( int i = 0; i < 256; ++i )
    {
        aaa::syphon::ServerMac server( "AAASeedTestServerLeak" );
        EXPECT_TRUE( server.is_open() );
    }
    SUCCEED();
}

//  -------------------------------------------------------------------
//  c123-A additions : Mach-port-free advertisement coverage.
//  -------------------------------------------------------------------

TEST( SyphonMacSmoke, AdvertiseCreatesMessagePort )
{
    @autoreleasepool
    {
        //	Unique-per-test name to avoid colliding with a leftover port
        //	from a previous run of the suite in the same process.
        aaa::syphon::ServerMac server( "AAASeedTestServerAdvertise" );
        ASSERT_TRUE( server.is_open() );
        EXPECT_FALSE( server.is_advertised() );

        EXPECT_EQ( server.advertise(), aaa::syphon::STATUS_OK );
        EXPECT_TRUE( server.is_advertised() );
    }
}

TEST( SyphonMacSmoke, AdvertiseIsIdempotent )
{
    @autoreleasepool
    {
        aaa::syphon::ServerMac server( "AAASeedTestServerAdvertiseIdem" );
        ASSERT_TRUE( server.is_open() );

        EXPECT_EQ( server.advertise(), aaa::syphon::STATUS_OK );
        EXPECT_TRUE( server.is_advertised() );

        //	Second call must NOT crash, must NOT recreate the port,
        //	and must return OK.
        EXPECT_EQ( server.advertise(), aaa::syphon::STATUS_OK );
        EXPECT_TRUE( server.is_advertised() );

        //	A third call for good measure.
        EXPECT_EQ( server.advertise(), aaa::syphon::STATUS_OK );
    }
}

TEST( SyphonMacSmoke, AdvertiseOnNotOpenReturnsNotOpen )
{
    aaa::syphon::ServerMac server( "" );
    ASSERT_FALSE( server.is_open() );
    EXPECT_EQ( server.advertise(), aaa::syphon::STATUS_NOT_OPEN );
    EXPECT_FALSE( server.is_advertised() );
}

namespace
{
//	DN observer state for the PublishPostsDistributedNotification test.
//	The C-style callback signature CFNotificationCenter expects forces
//	a file-scope flag rather than a captured lambda.
//
//	c124-C : `expected_name` was added to filter out cross-process
//	noise. When ctest -j N runs parallel workers that ALSO post on
//	the same "aaa.syphon.frame" distributed notification name
//	(SyphonClientMac suite), the observer can receive notifications
//	from those workers BEFORE our own publish lands. Without the
//	filter the EXPECT_STREQ at the assertion site reads the wrong
//	`last_name`. Behavior preserved on the original solo-test case :
//	if expected_name is empty (the legacy default) the observer
//	captures whatever name arrives, identical to pre-c124-C.
struct DNObserverState
{
    bool         received      = false;
    NSString *   last_name     = nil;
    char const * expected_name = "";    // filter target ; "" means "any"
};

void DistributedNotificationObserver(
    CFNotificationCenterRef /* center   */,
    void *                  observer,
    CFNotificationName      /* name     */,
    void const *            /* object   */,
    CFDictionaryRef         user_info )
{
    auto * state = static_cast<DNObserverState *>( observer );
    if( user_info == nullptr )
        return;
    NSDictionary * info = (__bridge NSDictionary *) user_info;
    NSString *     this_name = [info objectForKey: @"server"];

    //	Filter to expected server name iff one was configured.
    if( state->expected_name != nullptr &&
        state->expected_name[0] != '\0' )
    {
        if( this_name == nil )
            return;
        if( strcmp( [this_name UTF8String], state->expected_name ) != 0 )
            return;
    }

    state->received  = true;
    state->last_name = [this_name copy];
}
}   //	anonymous namespace

TEST( SyphonMacSmoke, PublishPostsDistributedNotification )
{
    @autoreleasepool
    {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if( device == nil )
        {
            GTEST_SKIP() << "Metal device unavailable on this runner ; "
                            "skipping distributed-notification publish test.";
        }

        //	Build the IOSurface-backed MTLTexture (same shape as the
        //	c122 PublishMetalTextureIOSurfaceBackedReturnsOk test).
        NSDictionary * props = @{
            (NSString *) kIOSurfaceWidth          : @32,
            (NSString *) kIOSurfaceHeight         : @32,
            (NSString *) kIOSurfaceBytesPerElement: @4,
            (NSString *) kIOSurfacePixelFormat    : @( (UInt32) 'BGRA' ),
        };
        IOSurfaceRef surface = IOSurfaceCreate( (CFDictionaryRef) props );
        ASSERT_NE( surface, nullptr );

        MTLTextureDescriptor * desc = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                         width: 32
                                        height: 32
                                     mipmapped: NO];
        desc.usage       = MTLTextureUsageShaderRead;
        desc.storageMode = MTLStorageModeShared;
        id<MTLTexture> texture = [device newTextureWithDescriptor: desc
                                                        iosurface: surface
                                                            plane: 0];
        ASSERT_NE( texture, nil );

        char const * const server_name = "AAASeedTestServerDNPost";

        aaa::syphon::ServerMac server( server_name );
        ASSERT_TRUE( server.is_open() );
        ASSERT_EQ( server.advertise(), aaa::syphon::STATUS_OK );

        //	Register an in-process observer on the distributed center.
        //	Distributed Notifications are delivered to the runloop of
        //	the registering thread ; CFNotificationCenterPostNotification
        //	with kCFNotificationDeliverImmediately bypasses the runloop
        //	queue for SAME-PROCESS observers, so we don't need to spin
        //	the runloop to receive the callback.
        DNObserverState state;
        state.expected_name = server_name;
        CFStringRef notif_name = CFStringCreateWithCString(
            kCFAllocatorDefault,
            aaa::syphon::ServerMac::distributed_notification_name(),
            kCFStringEncodingUTF8 );
        ASSERT_NE( notif_name, nullptr );
        CFNotificationCenterAddObserver(
            CFNotificationCenterGetDistributedCenter(),
            &state,
            &DistributedNotificationObserver,
            notif_name,
            nullptr,
            CFNotificationSuspensionBehaviorDeliverImmediately );

        //	Publish -- this should fire the DN.
        EXPECT_EQ( server.publish_metal_texture( texture ),
                   aaa::syphon::STATUS_OK );

        //	Drain pending DN deliveries on this thread's runloop. With
        //	kCFNotificationDeliverImmediately the callback typically
        //	fires inline on Post, but in case the runtime queues it
        //	we give the runloop a short tick.
        CFRunLoopRunInMode( kCFRunLoopDefaultMode, 0.2, false );

        EXPECT_TRUE( state.received );
        if( state.received && state.last_name != nil )
        {
            EXPECT_STREQ( [state.last_name UTF8String], server_name );
        }

        CFNotificationCenterRemoveObserver(
            CFNotificationCenterGetDistributedCenter(),
            &state,
            notif_name,
            nullptr );
        CFRelease( notif_name );
        CFRelease( surface );
    }
}

TEST( SyphonMacSmoke, XPCObjectRoundTrip )
{
    @autoreleasepool
    {
        NSDictionary * props = @{
            (NSString *) kIOSurfaceWidth          : @16,
            (NSString *) kIOSurfaceHeight         : @16,
            (NSString *) kIOSurfaceBytesPerElement: @4,
            (NSString *) kIOSurfacePixelFormat    : @( (UInt32) 'BGRA' ),
        };
        IOSurfaceRef original = IOSurfaceCreate( (CFDictionaryRef) props );
        ASSERT_NE( original, nullptr );

        IOSurfaceID const original_id = IOSurfaceGetID( original );
        EXPECT_NE( original_id, (IOSurfaceID) 0 );

        xpc_object_t xpc_handle = IOSurfaceCreateXPCObject( original );
        ASSERT_NE( xpc_handle, nullptr );

        IOSurfaceRef recovered = IOSurfaceLookupFromXPCObject( xpc_handle );
        ASSERT_NE( recovered, nullptr );
        EXPECT_EQ( IOSurfaceGetID( recovered ), original_id );

        CFRelease( recovered );
        xpc_release( xpc_handle );
        CFRelease( original );
    }
}
