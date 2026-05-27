// src/syphon/syphon_client_mac.mm
//
// Phase 6 reciprocity LAST HOP : Mac-side Syphon CLIENT impl. See
// syphon_client_mac.h for full doctrine.
//
// Wire shape (mirror of ServerMac::publish_iosurface c123-A) :
//   userInfo NSDictionary payload :
//     "server"     : NSString  -- the server name
//     "surface_id" : NSNumber  -- IOSurfaceID (uint32) kernel handle
//
//   ClientMac resolves the IOSurfaceID back to an IOSurfaceRef via
//   IOSurfaceLookup(). The xpc_object_t / IOSurfaceLookupFromXPCObject
//   path is NOT used at the userInfo layer -- Distributed Notification
//   plumbing already round-trips an IOSurfaceID safely cross-process,
//   and IOSurfaceLookup is the simpler primitive. (The xpc handle is
//   the canonical Syphon-style transport for shared-memory IPC, but
//   Distributed Notifications carry their own NSDictionary plumbing
//   which subsumes the same shared-memory exchange under the hood.)
//
// What this file deliberately does NOT do :
//   - No CFMessagePort handshake. The server-side advertise() creates
//     the port for discovery ; this client doesn't need to send any
//     reply traffic at c124-C. Future bidirectional flow control
//     (back-pressure, NACK on dropped frames) would add a
//     CFMessagePortCreateRemote here.
//   - No queue of frames. Latest-frame-wins is the Syphon model :
//     consumers that fall behind drop the stale frame. A future ring
//     buffer is c125+ work.

#import "syphon_client_mac.h"
#import "syphon_mac.h"           // for distributed_notification_name()

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <IOSurface/IOSurfaceObjC.h>
#import <Metal/Metal.h>

namespace aaa
{
namespace syphon
{

namespace
{
//	C trampoline for CFNotificationCenterAddObserver. The `observer`
//	pointer is the ClientMac `this` we passed in at registration time.
//	We extract `server` + `surface_id` from the userInfo dictionary
//	and forward into the C++ member function.
void ClientMacDNTrampoline(
    CFNotificationCenterRef /* center */,
    void *                  observer,
    CFNotificationName      /* name   */,
    void const *            /* object */,
    CFDictionaryRef         user_info )
{
    if( observer == nullptr || user_info == nullptr )
        return;
    auto * self = static_cast<ClientMac *>( observer );

    NSDictionary * info = (__bridge NSDictionary *) user_info;

    char const *  server_cstr = "";
    std::uint32_t surface_id  = 0;

    id server_obj = [info objectForKey: @"server"];
    if( [server_obj isKindOfClass: [NSString class]] )
    {
        char const * utf8 = [(NSString *) server_obj UTF8String];
        if( utf8 != nullptr )
            server_cstr = utf8;
    }

    id sid_obj = [info objectForKey: @"surface_id"];
    if( [sid_obj isKindOfClass: [NSNumber class]] )
    {
        surface_id = (std::uint32_t)
            [(NSNumber *) sid_obj unsignedIntValue];
    }

    self->on_distributed_notification( server_cstr, surface_id );
}
}   //	anonymous namespace

ClientMac::ClientMac()
{
    //	Register the DN observer. The name string is the same one
    //	ServerMac::publish_iosurface() posts on -- they MUST match
    //	exactly or the observer never fires.
    CFStringRef notif_name = CFStringCreateWithCString(
        kCFAllocatorDefault,
        ServerMac::distributed_notification_name(),
        kCFStringEncodingUTF8 );
    if( notif_name == nullptr )
        return;

    //	`object` = nullptr means "fire for ANY server with this notif
    //	name", not just one specific server-name object. The
    //	suspension behavior `DeliverImmediately` matches the c123-A
    //	server-side post flag.
    //
    //	Register on BOTH the distributed and local centers :
    //	  - Distributed : cross-process delivery via distnoted. The
    //	    primary path for the Mac-to-Mac reciprocity loop ; matches
    //	    the spout/Syphon model where a separate consumer process
    //	    picks up the frame.
    //	  - Local : same-process synchronous delivery. distnoted can
    //	    drop messages under heavy parallel post-load (multiple
    //	    ctest workers posting tight loops on the same notif name).
    //	    The local center path is in-process only and 100% reliable.
    //	    Servers post on both centers (see syphon_mac.mm c124-C).
    CFNotificationCenterAddObserver(
        CFNotificationCenterGetDistributedCenter(),
        this,
        &ClientMacDNTrampoline,
        notif_name,
        nullptr,
        CFNotificationSuspensionBehaviorDeliverImmediately );

    CFNotificationCenterAddObserver(
        CFNotificationCenterGetLocalCenter(),
        this,
        &ClientMacDNTrampoline,
        notif_name,
        nullptr,
        CFNotificationSuspensionBehaviorDeliverImmediately );

    CFRelease( notif_name );
}

ClientMac::~ClientMac()
{
    //	Remove from BOTH centers FIRST so a pending DN can't fire
    //	after we start tearing down `this`. The ctor registered on
    //	both ; the dtor mirrors.
    CFNotificationCenterRemoveEveryObserver(
        CFNotificationCenterGetDistributedCenter(),
        this );
    CFNotificationCenterRemoveEveryObserver(
        CFNotificationCenterGetLocalCenter(),
        this );

    //	Release any unconsumed frame.
    if( latest_surface_ != nullptr )
    {
        CFRelease( latest_surface_ );
        latest_surface_ = nullptr;
    }
}

void ClientMac::poll( double timeout_sec )
{
    //	Tick the current thread's runloop so any queued DN deliveries
    //	fire. For same-process / kCFNotificationDeliverImmediately,
    //	the trampoline often runs inline on Post and this call is a
    //	no-op ; for cross-process the runloop is the delivery pump.
    if( timeout_sec < 0.0 )
        timeout_sec = 0.0;
    CFRunLoopRunInMode( kCFRunLoopDefaultMode, timeout_sec, false );
}

bool ClientMac::has_frame() const
{
    std::lock_guard<std::mutex> guard( mutex_ );
    return has_frame_;
}

char const * ClientMac::last_server_name() const
{
    std::lock_guard<std::mutex> guard( mutex_ );
    //	c_str() pointer is stable for the lifetime of latest_server_
    //	UNTIL the next mutating call. Callers that race
    //	consume_*()/poll() against last_server_name() must copy the
    //	string if they need to outlive a single critical section.
    return latest_server_.c_str();
}

IOSurfaceRef ClientMac::consume_iosurface()
{
    std::lock_guard<std::mutex> guard( mutex_ );
    if( latest_surface_ == nullptr )
    {
        has_frame_ = false;
        return nullptr;
    }
    IOSurfaceRef out = latest_surface_;
    //	Hand our +1 retain to the caller -- clear the slot WITHOUT
    //	releasing. The caller is responsible for CFRelease.
    latest_surface_ = nullptr;
    has_frame_      = false;
    return out;
}

id<MTLTexture> ClientMac::consume_metal_texture( id<MTLDevice> device )
{
    if( device == nil )
        return nil;

    IOSurfaceRef surface = consume_iosurface();
    if( surface == nullptr )
        return nil;

    //	Build a texture descriptor matching the IOSurface's geometry.
    //	The IOSurface pixel format we publish on the server side is
    //	'BGRA' (kIOSurfacePixelFormat fourcc) ; on the texture side
    //	that's MTLPixelFormatBGRA8Unorm. For now we assume that
    //	format -- a future variant can read the IOSurface props back
    //	via IOSurfaceGetPixelFormat() and select the MTL format.
    NSUInteger const width  = (NSUInteger) IOSurfaceGetWidth( surface );
    NSUInteger const height = (NSUInteger) IOSurfaceGetHeight( surface );

    MTLTextureDescriptor * desc = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat: MTLPixelFormatBGRA8Unorm
                                     width: width
                                    height: height
                                 mipmapped: NO];
    desc.usage       = MTLTextureUsageShaderRead;
    desc.storageMode = MTLStorageModeShared;

    id<MTLTexture> texture = [device newTextureWithDescriptor: desc
                                                    iosurface: surface
                                                        plane: 0];
    //	consume_iosurface() handed the +1 retain to us ; release it now
    //	that the MTLTexture has its own strong reference to the
    //	IOSurface internally.
    CFRelease( surface );

    return texture;
}

void ClientMac::on_distributed_notification(
    char const *  server_name,
    std::uint32_t surface_id )
{
    //	IOSurfaceLookup returns a +1 retained IOSurfaceRef ; an unknown
    //	or stale ID returns NULL.
    IOSurfaceRef surface =
        IOSurfaceLookup( (IOSurfaceID) surface_id );

    std::lock_guard<std::mutex> guard( mutex_ );

    //	Drop the previously held frame if the consumer never picked
    //	it up. Latest-wins.
    if( latest_surface_ != nullptr )
    {
        CFRelease( latest_surface_ );
        latest_surface_ = nullptr;
    }

    if( surface != nullptr )
    {
        latest_surface_ = surface;
        has_frame_      = true;
    }
    //	If the lookup failed we still update the bookkeeping fields so
    //	tests can see the server name even when the surface failed to
    //	resolve cross-process.
    if( server_name != nullptr )
        latest_server_.assign( server_name );
    else
        latest_server_.clear();

    //	Even on lookup failure flip has_frame_ true so the consumer
    //	knows a DN arrived ; consume_iosurface() returning nullptr is
    //	the failure signal.
    has_frame_ = true;
}

}   //	namespace syphon
}   //	namespace aaa
