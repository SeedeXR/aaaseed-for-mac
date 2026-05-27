// src/syphon/syphon_mac.h
//
// Phase 6 reciprocity beachhead (c122 + c123-A) : Mac-side Syphon
// C-binding header. The Win-side counterpart is `aaa.spout.send(name, tex)` ;
// the Lua-side shim in `src/lua/meu_compat.lua` (c115) already maps
// `aaa.spout.send` -> `aaa.syphon.send` on Mac. What's missing is the
// C-side that backs `aaa.syphon.*` ; this header is the first concrete
// step.
//
// Scope :
//   c122 froze the call shape : ctor + publish_iosurface +
//   publish_metal_texture + is_open().
//
//   c123-A extends ServerMac to actually advertise a published frame via
//   Mach-port-free primitives :
//     - CFMessagePortCreateLocal       : Mach-port-free local IPC. No
//                                        bootstrap_register (would need
//                                        an entitlement).
//     - CFNotificationCenterPost...    : Distributed Notifications --
//                                        broadcasts the server-name +
//                                        IOSurface xpc_object payload
//                                        to listening apps.
//     - IOSurfaceCreateXPCObject       : wraps an IOSurfaceRef into an
//                                        xpc_object_t for cross-process
//                                        passing.
//   The Mach-port-free path is sandbox-safe : the unit-test runner has
//   no entitlements and CFMessagePortCreateLocal + Distributed
//   Notifications both work from inside the default sandbox.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++. No engine link, no o_str, no aaa_mem.
//   - std:: + CoreFoundation + Metal + IOSurface + xpc only. No
//     third-party Syphon framework dependency at this stage.
//   - Single-threaded model ; publish_*() called from the render thread.
//   - No vendor edits. No err.h / aaa_type.h touch.
//
// Wiring lives in `src/syphon/CMakeLists.txt` (mirrors src/aaa pattern).
// Smoke test : `tests/unit/syphon_mac_smoke_test.mm`.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

//	IOSurface forward-decl : the type is an opaque CF object. Including
//	<IOSurface/IOSurface.h> from a plain C++ TU works (IOSurfaceRef is
//	just an Obj-C runtime pointer), but the header pulls in CoreVideo
//	which is unnecessary for callers that only need the type name.
typedef struct __IOSurface * IOSurfaceRef;

//	Forward-decl the CoreFoundation CFMessagePortRef so the header stays
//	C++-friendly. The .mm impl casts back to the real opaque type.
typedef struct __CFMessagePort * CFMessagePortRef;

#ifdef __OBJC__
@protocol MTLTexture;
#else
//	C++-only TUs see `id<MTLTexture>` as an opaque pointer-sized handle.
//	The .mm impl casts back to the protocol type at the boundary.
typedef void * MTLTextureHandle;
#endif

namespace aaa
{
namespace syphon
{

//	Result codes for publish_*() / advertise(). Kept as a plain enum
//	(not enum class) so the Lua binding layer can return them as INT32
//	without a cast.
enum Status : std::int32_t
{
    STATUS_OK                = 0,
    STATUS_NIL_TEXTURE       = 1,   //	id<MTLTexture> was nil
    STATUS_NIL_IOSURFACE     = 2,   //	IOSurfaceRef was NULL
    STATUS_NO_IOSURFACE_BACK = 3,   //	MTLTexture not IOSurface-backed
    STATUS_NOT_OPEN          = 4,   //	ServerMac ctor failed / not advertising
    STATUS_ADVERTISE_FAILED  = 5,   //	CFMessagePortCreateLocal returned null
};

//	ServerMac -- the Mac-side advertiser of a named texture stream. One
//	instance per published stream. Construction records the name + flips
//	`is_open_` to true ; advertise() then creates a local CFMessagePort
//	so other in-process / cross-process consumers can discover the
//	server. publish_metal_texture() additionally posts a Distributed
//	Notification carrying the IOSurface xpc_object payload.
//
//	Ownership : caller owns the MTLTexture / IOSurfaceRef passed to
//	publish_*() ; ServerMac does NOT retain those. ServerMac DOES own
//	the CFMessagePortRef once advertise() succeeds and releases it in
//	the dtor.
class ServerMac
{
public:
    //	Construct with a server name. Empty name leaves the server in a
    //	not-open state (is_open() returns false). Non-empty name "opens"
    //	the server (records name + sets flag). advertise() must be
    //	called separately to publish the CFMessagePort.
    explicit ServerMac( char const * server_name );

    ~ServerMac();

    ServerMac( ServerMac const & )             = delete;
    ServerMac & operator=( ServerMac const & ) = delete;

    //	True once the ctor accepted the name. False on empty-name ctor
    //	or after destruction.
    bool is_open() const { return is_open_; }

    //	The server's advertised name (UTF-8). Returns "" pre-open.
    char const * name() const { return name_.c_str(); }

    //	Create the CFMessagePort under the server name so consumers
    //	can discover the server. Idempotent : calling advertise()
    //	twice returns STATUS_OK without recreating the port. Returns
    //	STATUS_NOT_OPEN if the server was constructed with an empty
    //	name, STATUS_ADVERTISE_FAILED if CFMessagePortCreateLocal
    //	returned null (e.g. name collision with another live server).
    Status advertise();

    //	True once advertise() has successfully created the
    //	CFMessagePortRef.
    bool is_advertised() const { return message_port_ != nullptr; }

    //	Distributed Notification name used by publish_*() to broadcast
    //	a new frame. Consumers register on the distributed center for
    //	this exact string.
    static char const * distributed_notification_name()
    {
        return "aaa.syphon.frame";
    }

    //	Publish a CVPixelBuffer-style IOSurface to all connected clients.
    //	Returns STATUS_OK on success, STATUS_NIL_IOSURFACE on null input,
    //	STATUS_NOT_OPEN if the server failed to advertise. If advertise()
    //	has been called, additionally posts a Distributed Notification
    //	carrying the IOSurface xpc_object payload so listening apps can
    //	pick up the frame.
    Status publish_iosurface( IOSurfaceRef surface );

#ifdef __OBJC__
    //	Publish an IOSurface-backed MTLTexture. Internally calls
    //	[texture iosurface] (Apple Silicon : MTLTextures created via
    //	MTLDevice newTextureWithDescriptor:iosurface:plane: are
    //	IOSurface-backed by construction). Returns STATUS_OK on a valid
    //	IOSurface-backed texture, STATUS_NO_IOSURFACE_BACK if the
    //	texture has no underlying IOSurface, STATUS_NIL_TEXTURE on nil.
    Status publish_metal_texture( id<MTLTexture> texture );
#else
    Status publish_metal_texture( MTLTextureHandle texture );
#endif

private:
    std::string      name_;
    bool             is_open_       { false };
    CFMessagePortRef message_port_  { nullptr };

    //	c124+ : server-info JSON dictionary, connected-client list,
    //	frame counter, ... Deliberately absent so the API surface
    //	stays minimal until consumer-side code exists to need them.
};

}   //	namespace syphon
}   //	namespace aaa
