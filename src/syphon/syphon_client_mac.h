// src/syphon/syphon_client_mac.h
//
// Phase 6 reciprocity LAST HOP : Mac-side Syphon CLIENT. ServerMac
// (c122 + c123-A) is the advertiser ; ClientMac is the receiver. The
// two complete the Mac<->Mac reciprocity loop : a separate Mac process
// (or another component in the same process) registers for the same
// Distributed Notification name `"aaa.syphon.frame"`, picks up the
// IOSurfaceID payload, and reconstructs the IOSurfaceRef + wraps it
// back into an MTLTexture.
//
// Scope :
//   - ctor : register a DN observer on the distributed center for
//     `ServerMac::distributed_notification_name()`. The `context`
//     pointer is `this` ; the static C trampoline recovers it and
//     dispatches into a member function under a mutex.
//   - dtor : remove the observer cleanly.
//   - poll(timeout_sec) : pump CFRunLoopRunInMode so queued DN
//     deliveries fire on the current thread. Same-process DN with
//     kCFNotificationDeliverImmediately usually fires inline on Post
//     so the timeout can be 0 ; we still take an explicit timeout so
//     CROSS-process DN can be drained.
//   - has_frame() : true if a DN arrived since the last consume_*().
//   - last_server_name() : UTF-8 name of the most recent server.
//   - consume_iosurface() : returns the latest IOSurfaceRef (caller
//     receives a +1 retain via CFRetain ; ClientMac drops its own
//     reference). Clears has_frame.
//   - consume_metal_texture(device) : wraps the IOSurface via
//     `-[MTLDevice newTextureWithDescriptor:iosurface:plane:]` and
//     returns the texture. Clears has_frame.
//
// Sandbox-safety :
//   - DN observer registration uses CFNotificationCenterAddObserver on
//     CFNotificationCenterGetDistributedCenter(). No bootstrap_register,
//     no Mach-port name registration, no entitlements required. The
//     server-side advertise() needed those (and we sidestepped them
//     with CFMessagePortCreateLocal) ; the CLIENT side is even more
//     restricted-friendly.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++. No engine link, no o_str, no aaa_mem.
//   - std::mutex + std::string + CoreFoundation + Metal + IOSurface
//     only.
//   - Single ClientMac instance per consumer process is the expected
//     usage ; multiple instances ARE supported but each maintains an
//     independent observer slot.
//   - No vendor edits. No err.h / aaa_type.h touch.

#pragma once

#include <cstdint>
#include <mutex>
#include <string>

//	IOSurface forward-decl (mirror syphon_mac.h).
typedef struct __IOSurface * IOSurfaceRef;

#ifdef __OBJC__
@protocol MTLTexture;
@protocol MTLDevice;
#endif

namespace aaa
{
namespace syphon
{

//	ClientMac -- the Mac-side receiver. Registers a Distributed
//	Notification observer at construction time and stashes the most
//	recent frame for the consumer to pick up via consume_*().
//
//	Ownership : ClientMac retains the latest IOSurfaceRef. On
//	consume_iosurface() the retained reference is HANDED to the
//	caller (caller owns the +1) ; ClientMac then clears its slot.
//	On dtor any unconsumed frame is released.
//
//	Threading : the DN trampoline can fire on any thread that pumps
//	the runloop ; the latest-frame slot is guarded by a std::mutex.
//	has_frame() / last_server_name() / consume_*() are safe to call
//	from any thread.
class ClientMac
{
public:
    ClientMac();
    ~ClientMac();

    ClientMac( ClientMac const & )             = delete;
    ClientMac & operator=( ClientMac const & ) = delete;

    //	Pump the runloop for `timeout_sec` so queued DN deliveries
    //	fire on the current thread. Same-process deliveries with
    //	kCFNotificationDeliverImmediately usually arrive inline on
    //	Post, but cross-process or queued deliveries need an explicit
    //	runloop tick.
    void poll( double timeout_sec );

    //	True if at least one DN has arrived since the last consume_*().
    bool has_frame() const;

    //	UTF-8 name of the most recent server that posted a frame.
    //	Returns "" if no frame has arrived. Pointer is stable until
    //	the next DN or consume_*().
    char const * last_server_name() const;

    //	Hand the latest IOSurfaceRef to the caller. The returned ref
    //	is +1-retained (caller releases via CFRelease). Returns
    //	nullptr if no frame is currently held. Clears the has_frame
    //	flag.
    IOSurfaceRef consume_iosurface();

#ifdef __OBJC__
    //	Wrap the latest IOSurface into an MTLTexture via
    //	`-[device newTextureWithDescriptor:iosurface:plane:]`. The
    //	descriptor is built from the IOSurface's own width/height/
    //	pixel-format. Returns nil if no frame is held or if texture
    //	creation fails. Clears the has_frame flag.
    id<MTLTexture> consume_metal_texture( id<MTLDevice> device );
#endif

    //	-- internal use ; called by the C trampoline only. Public so
    //	   the file-scope trampoline (an extern "C" free function) can
    //	   reach it without `friend` gymnastics.
    void on_distributed_notification(
        char const * server_name,
        std::uint32_t surface_id );

private:
    mutable std::mutex   mutex_;
    IOSurfaceRef         latest_surface_ { nullptr };
    std::string          latest_server_;
    bool                 has_frame_      { false };
};

}   //	namespace syphon
}   //	namespace aaa
