// src/syphon/syphon_directory_mac.h
//
// second_todo.md S4 (continuation 153) : Syphon server DIRECTORY for the
// Mac receive side. The wave-1 stack (c122-c124) shipped ServerMac
// (advertise + publish) and ClientMac (receive a frame from a KNOWN server
// name). What was missing is DISCOVERY : enumerating which named servers are
// currently publishing without knowing their names in advance. The upstream
// engine author's mac-port branch exposes this as
// `aaa_syphon_directory_list()` (uuid + name). This is the native equivalent.
//
// Mechanism : ServerMac::publish_iosurface() posts a Distributed Notification
// `"aaa.syphon.frame"` carrying { "server" : NSString, "surface_id" : NSNumber }
// on BOTH the distributed and local notification centers (see
// syphon_client_mac.mm for the payload contract). DirectoryMac registers the
// SAME observer the client uses, but instead of latest-frame-wins it
// accumulates the set of distinct server names it has seen, with a per-server
// frame counter and the most recent surface id. poll() drains queued
// deliveries; the discovered set is then read back via count() / server_name().
//
// This is purely passive : it never publishes, so two DirectoryMac instances
// (or a Directory + a Client) coexist without contention. It reuses the exact
// payload keys ClientMac parses, so it stays in lock-step if the contract
// changes.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++. std:: + CoreFoundation only. No engine link, no o_str,
//     no aaa_mem. Manual reference counting (-fno-objc-arc).
//   - distnoted dual-center caveat (feedback_distnoted_dual_center.md) :
//     observe BOTH GetDistributedCenter() and GetLocalCenter() so in-process
//     posts are caught reliably and cross-process posts are caught
//     best-effort.
//
// Tests : tests/native/syphon_directory_mac_test.mm (publish from two named
// servers, poll, assert both names discovered + frame counts advance).

#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace aaa
{
namespace syphon
{

//	DirectoryMac -- passive enumerator of currently-advertising Syphon
//	servers, discovered via the shared "aaa.syphon.frame" Distributed
//	Notification. Thread-safe : the DN trampoline can fire on any thread
//	that pumps the runloop ; the discovered-set is guarded by a mutex.
class DirectoryMac
{
public:
    DirectoryMac();
    ~DirectoryMac();

    DirectoryMac( DirectoryMac const & )             = delete;
    DirectoryMac & operator=( DirectoryMac const & ) = delete;

    //	Pump the runloop for `timeout_sec` so queued DN deliveries fire on
    //	the current thread. Same-process posts (local center,
    //	DeliverImmediately) usually arrive inline ; cross-process posts need
    //	an explicit tick. A timeout of 0 still flushes inline deliveries.
    void poll( double timeout_sec );

    //	Number of distinct server names discovered so far.
    std::size_t count() const;

    //	Sorted list of discovered server names (sorted for deterministic
    //	indexing across runs).
    std::vector<std::string> server_names() const;

    //	Frames seen from `name` since construction / last clear(). 0 if the
    //	name was never seen.
    std::uint64_t frame_count( std::string const & name ) const;

    //	Most recent IOSurfaceID published by `name`, or 0 if never seen.
    std::uint32_t latest_surface_id( std::string const & name ) const;

    //	Forget all discovered servers.
    void clear();

    //	-- internal ; called by the file-scope C trampoline only. Public so
    //	   the trampoline (an extern free function) can reach it without
    //	   friend gymnastics. Mirrors ClientMac's contract.
    void on_distributed_notification(
        char const *  server_name,
        std::uint32_t surface_id );

private:
    struct Entry
    {
        std::uint64_t frames        { 0 };
        std::uint32_t last_surface  { 0 };
    };

    mutable std::mutex                                    mutex_;
    //	server name -> accumulated stats. std::map keeps iteration sorted,
    //	which server_names() relies on for deterministic ordering.
    std::vector<std::pair<std::string, Entry>>            servers_;

    //	Find-or-insert under the held lock. Returns a reference into
    //	servers_ ; callers must hold mutex_.
    Entry & entry_for_locked( std::string const & name );
};

}   //	namespace syphon
}   //	namespace aaa
