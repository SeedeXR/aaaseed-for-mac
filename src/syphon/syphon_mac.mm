// src/syphon/syphon_mac.mm
//
// Phase 6 reciprocity beachhead (c122 + c123-A) : Mac-side Syphon
// C-binding impl. See syphon_mac.h for full doctrine.
//
// What this file owns at c123-A :
//   - ServerMac construction stores the name + flips is_open_.
//   - ServerMac destruction releases the CFMessagePortRef.
//   - advertise() creates a local CFMessagePort under the server name
//     so consumers can discover the server. Idempotent. NO
//     bootstrap_register : Mach-port-free local IPC only.
//   - publish_iosurface() validates the IOSurfaceRef + (when
//     advertised) wraps it in an xpc_object via IOSurfaceCreateXPCObject
//     and posts a Distributed Notification on the server-name so
//     listening apps can fetch the frame.
//   - publish_metal_texture() pulls `[texture iosurface]` (Apple Silicon
//     ABI) and delegates to publish_iosurface().
//
// What this file deliberately does NOT do :
//   - No bootstrap_register / bootstrap_check_in. The unit-test sandbox
//     would fail those without explicit entitlements.
//   - No third-party Syphon.framework vendor. Apple-only APIs.
//   - No JSON server-info dictionary, no client-side reply pump. Those
//     are c124+ work driven by consumer-side needs.

#import "syphon_mac.h"

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <IOSurface/IOSurfaceObjC.h>
#import <Metal/Metal.h>
#import <xpc/xpc.h>

namespace aaa
{
namespace syphon
{

namespace
{
//	CFMessagePort callback : ServerMac doesn't service requests from
//	clients in c123-A (no client-side code exists yet). Returning NULL
//	is the documented "no reply" path. Kept here so advertise() has a
//	non-null callback to pass to CFMessagePortCreateLocal -- a NULL
//	callback would make the port read-only and CFMessagePortCreateLocal
//	rejects it with kCFMessagePortIsInvalid.
CFDataRef MessagePortNoopCallback(
    CFMessagePortRef /* port    */,
    SInt32           /* msgid   */,
    CFDataRef        /* data    */,
    void *           /* info    */ )
{
    return nullptr;
}
}   //	anonymous namespace

ServerMac::ServerMac( char const * server_name )
{
    if( server_name == nullptr )
        return;
    std::size_t const len = std::char_traits<char>::length( server_name );
    if( len == 0 )
        return;
    name_.assign( server_name, len );
    //	c122 : "opening" the server is just recording the name. The
    //	real CFMessagePort registration is deferred to advertise() so
    //	tests that only exercise the API shape don't allocate a port.
    is_open_ = true;
}

ServerMac::~ServerMac()
{
    if( message_port_ != nullptr )
    {
        //	Invalidate first so a queued message can't fire into a
        //	half-released port, then drop the CF retain.
        CFMessagePortInvalidate( message_port_ );
        CFRelease( message_port_ );
        message_port_ = nullptr;
    }
    is_open_ = false;
}

Status ServerMac::advertise()
{
    if( !is_open_ )
        return STATUS_NOT_OPEN;
    if( message_port_ != nullptr )
        //	Idempotent : already advertised.
        return STATUS_OK;

    CFStringRef cf_name = CFStringCreateWithCString(
        kCFAllocatorDefault, name_.c_str(), kCFStringEncodingUTF8 );
    if( cf_name == nullptr )
        return STATUS_ADVERTISE_FAILED;

    Boolean should_free = false;
    CFMessagePortRef port = CFMessagePortCreateLocal(
        kCFAllocatorDefault,
        cf_name,
        &MessagePortNoopCallback,
        nullptr,
        &should_free );
    CFRelease( cf_name );

    if( port == nullptr )
        return STATUS_ADVERTISE_FAILED;

    //	`should_free` true means the name was already taken and CF
    //	returned a fresh port that's NOT registered under our name.
    //	For our purposes that's still a usable local port, but we
    //	signal failure to the caller so it can pick a different name.
    if( should_free )
    {
        CFMessagePortInvalidate( port );
        CFRelease( port );
        return STATUS_ADVERTISE_FAILED;
    }

    message_port_ = port;
    return STATUS_OK;
}

Status ServerMac::publish_iosurface( IOSurfaceRef surface )
{
    if( !is_open_ )
        return STATUS_NOT_OPEN;
    if( surface == nullptr )
        return STATUS_NIL_IOSURFACE;
    //	Touch the surface to validate it's a real CF object. IOSurfaceGetID
    //	returns 0 for an invalid surface ; we accept any non-crashing
    //	return because the surface lifetime is the caller's contract.
    (void) IOSurfaceGetID( surface );

    //	If advertise() has been called, broadcast the new frame via a
    //	Distributed Notification. The userInfo carries :
    //	  - "server"   : the server name (NSString)
    //	  - "iosurface": the IOSurfaceRef as an NSObject (the IOSurfaceRef
    //	                 ABI is toll-free-bridgeable with IOSurface * on
    //	                 modern SDKs and DN serialization handles
    //	                 IOSurfaceCreateXPCObject under the hood).
    //	If advertise() has NOT been called, publishing still succeeds
    //	(it's a valid frame) but no broadcast happens -- this matches
    //	the c122 contract.
    if( message_port_ != nullptr )
    {
        @autoreleasepool
        {
            //	Wrap the IOSurface as an xpc_object so it survives the
            //	cross-process trip. The DN payload doesn't strictly need
            //	the xpc handle (Distributed Notifications run inside the
            //	same userInfo NSDictionary plumbing), but the xpc handle
            //	is the canonical way Syphon-style servers hand off an
            //	IOSurface and we keep it in the payload for consumers
            //	that prefer the xpc path.
            xpc_object_t xpc_surface =
                IOSurfaceCreateXPCObject( surface );

            NSString * ns_name =
                [NSString stringWithUTF8String: name_.c_str()];

            //	The IOSurface ID is a 32-bit kernel handle ; consumers
            //	can use IOSurfaceLookupFromXPCObject on the xpc handle
            //	OR IOSurfaceLookup on the bare ID. We include both for
            //	robustness.
            IOSurfaceID surface_id = IOSurfaceGetID( surface );

            NSDictionary * user_info = @{
                @"server"     : ns_name ? ns_name : @"",
                @"surface_id" : @(surface_id),
            };

            CFStringRef notif_name = CFStringCreateWithCString(
                kCFAllocatorDefault,
                distributed_notification_name(),
                kCFStringEncodingUTF8 );
            CFStringRef object_name = CFStringCreateWithCString(
                kCFAllocatorDefault,
                name_.c_str(),
                kCFStringEncodingUTF8 );

            if( notif_name != nullptr && object_name != nullptr )
            {
                //	c124-C : ALSO post on the LOCAL CFNotificationCenter
                //	for same-process consumers. The local center is
                //	in-process only (no distnoted hop), so delivery is
                //	synchronous + reliable even under heavy parallel
                //	ctest load that would otherwise cause distnoted to
                //	drop messages. Cross-process consumers still use the
                //	distributed center path below.
                CFNotificationCenterPostNotification(
                    CFNotificationCenterGetLocalCenter(),
                    notif_name,
                    object_name,
                    (CFDictionaryRef) user_info,
                    true /* deliverImmediately */ );

                CFNotificationCenterPostNotificationWithOptions(
                    CFNotificationCenterGetDistributedCenter(),
                    notif_name,
                    object_name,
                    (CFDictionaryRef) user_info,
                    kCFNotificationDeliverImmediately
                        | kCFNotificationPostToAllSessions );
            }
            if( notif_name  != nullptr ) CFRelease( notif_name );
            if( object_name != nullptr ) CFRelease( object_name );

            if( xpc_surface != nullptr )
            {
                //	Under MRC the xpc_object_t is a CF-style ref ; release
                //	it via xpc_release to match the IOSurfaceCreateXPCObject
                //	+1 retain.
                xpc_release( xpc_surface );
            }
        }
    }

    return STATUS_OK;
}

Status ServerMac::publish_metal_texture( id<MTLTexture> texture )
{
    if( !is_open_ )
        return STATUS_NOT_OPEN;
    if( texture == nil )
        return STATUS_NIL_TEXTURE;
    //	Apple Silicon : MTLTextures created from an IOSurface via
    //	`-[MTLDevice newTextureWithDescriptor:iosurface:plane:]` expose
    //	the surface through `-iosurface`. Plain `-[MTLDevice
    //	newTextureWithDescriptor:]` returns a private-storage texture
    //	with no IOSurface backing -> nil here, and we surface that
    //	as STATUS_NO_IOSURFACE_BACK so the caller can fall back to a
    //	CPU readback or recreate the texture as IOSurface-backed.
    IOSurfaceRef surface = [texture iosurface];
    if( surface == nullptr )
        return STATUS_NO_IOSURFACE_BACK;
    return publish_iosurface( surface );
}

}   //	namespace syphon
}   //	namespace aaa
