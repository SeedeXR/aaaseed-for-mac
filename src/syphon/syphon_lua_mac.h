// src/syphon/syphon_lua_mac.h
//
// c124 : Phase 6 reciprocity stack -- Lua-side C-binding bridge into
// the Mac-side Syphon ServerMac (c122 + c123-A). This is the final
// piece of the 3-piece stack :
//
//   meu_compat.lua  (c115)  -- Lua dispatch shim, routes `aaa.spout.send`
//                              to `aaa.syphon.send` on Mac.
//   aaa.syphon.send (c124)  -- THIS file. C-binding that opens / advertises
//                              a ServerMac per name and forwards the
//                              MTLTexture handle.
//   ServerMac       (c122,  -- Hermetic Mac sub-lib doing the actual
//                    c123A)   CFMessagePort advertise + Distributed
//                              Notification publish + IOSurface xpc handoff.
//
// Lua surface installed by `register_lua_bindings()` :
//
//   aaa.syphon.send( server_name : string, texture : integer | nil )
//   aaa.syphon.receive( server_name : string ) -> integer
//
// `server_name` is a UTF-8 Lua string. `texture` is an MTLTexture handle
// encoded as a Lua integer (engine convention : id<MTLTexture> reinterpreted
// as uintptr_t and pushed via `lua_pushinteger`). A nil / 0 texture is
// treated as the "register only" path : the named ServerMac is lazily
// created and advertise()'d, but no frame is published.
//
// `aaa.syphon.receive` (c124-D) mirrors `send` from the consumer side :
// lazy-creates a ClientMac in a process-local registry keyed by server
// name (DN deliveries are filtered to that name by the trampoline's
// `last_server_name()` check at call time), drains any inline DN, and if
// a frame is available wraps the IOSurface into an MTLTexture via the
// system-default MTLDevice. Returns the texture handle as a `lua_Integer`
// (uintptr_t cast), or 0 if no frame is available.
//
// Lifecycle / ownership :
//   - One ServerMac per distinct `server_name`, kept in a process-local
//     std::unordered_map<std::string, std::unique_ptr<ServerMac>>. First
//     call with a given name creates + advertise()'s ; subsequent calls
//     reuse the same instance (idempotent at the binding layer ; ServerMac
//     itself is also idempotent on advertise()).
//   - The registry is leaked on process exit. The CFMessagePortRef each
//     ServerMac holds is invalidated + released in `~ServerMac` ; since
//     we don't tear the registry down explicitly, the OS reclaims the
//     ports at process exit -- acceptable for a long-running engine.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++. Links aaaseed_lua + aaaseed_syphon only. No engine
//     link, no o_str, no aaa_mem cascade.
//   - No vendor edits. No err.h / aaa_type.h touch.
//   - Single-threaded model. The Lua VM is single-threaded ; the
//     registry's unordered_map is touched only from the Lua callback.
//
// Test-only accessor : `has_server(name)` returns true iff the registry
// holds a ServerMac under that name. Exposed so the smoke test can verify
// the lazy-create + idempotent paths without poking the registry through
// the `aaa.syphon` Lua table (which would force the accessor into the
// public API surface).

#pragma once

#include <string>

struct lua_State;

namespace aaa
{
namespace syphon
{

//	Register the `aaa.syphon.send` global as a Lua callable. Creates the
//	`aaa` and `aaa.syphon` tables if they do not yet exist (defensive :
//	the engine populates them in real boots, but standalone tests open
//	a bare lua_State). Idempotent : calling twice on the same state
//	overwrites the closure with itself, no leak.
void register_lua_bindings( lua_State * L );

//	Test-only accessor : true iff a ServerMac with the given name has
//	been created by a prior `aaa.syphon.send` call on this process. Not
//	exposed to Lua scripts (would pollute `aaa.syphon` with a test hook).
bool has_server( std::string const & name );

//	Test-only accessor : true iff a ClientMac with the given name has
//	been created by a prior `aaa.syphon.receive` call on this process.
//	Mirror of `has_server` for the receive side. Not exposed to Lua.
bool has_client( std::string const & name );

}   //	namespace syphon
}   //	namespace aaa
