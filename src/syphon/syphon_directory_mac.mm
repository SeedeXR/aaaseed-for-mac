// src/syphon/syphon_directory_mac.mm
//
// Implementation of the passive Syphon server directory. See
// syphon_directory_mac.h for full scope + doctrine. Manual reference
// counting (-fno-objc-arc).

#include "src/syphon/syphon_directory_mac.h"
#include "src/syphon/syphon_mac.h"      //	ServerMac::distributed_notification_name()

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>

#include <algorithm>

namespace aaa
{
namespace syphon
{

namespace
{

//	C trampoline for CFNotificationCenterAddObserver. `observer` is the
//	DirectoryMac `this`. We parse the SAME userInfo keys ClientMac parses
//	("server" : NSString, "surface_id" : NSNumber) so the directory stays in
//	lock-step with the publish contract.
void DirectoryMacDNTrampoline(
    CFNotificationCenterRef /* center */,
    void *                  observer,
    CFNotificationName      /* name   */,
    void const *            /* object */,
    CFDictionaryRef         user_info )
{
    if( observer == nullptr || user_info == nullptr )
        return;
    auto * self = static_cast<DirectoryMac *>( observer );

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
        surface_id = (std::uint32_t) [(NSNumber *) sid_obj unsignedIntValue];

    self->on_distributed_notification( server_cstr, surface_id );
}

}   //	anonymous namespace

DirectoryMac::DirectoryMac()
{
    CFStringRef notif_name = CFStringCreateWithCString(
        kCFAllocatorDefault,
        ServerMac::distributed_notification_name(),
        kCFStringEncodingUTF8 );
    if( notif_name == nullptr )
        return;

    //	Dual-center observation (feedback_distnoted_dual_center.md) : local
    //	for reliable in-process delivery, distributed for best-effort
    //	cross-process discovery.
    CFNotificationCenterAddObserver(
        CFNotificationCenterGetDistributedCenter(),
        this,
        &DirectoryMacDNTrampoline,
        notif_name,
        nullptr,
        CFNotificationSuspensionBehaviorDeliverImmediately );

    CFNotificationCenterAddObserver(
        CFNotificationCenterGetLocalCenter(),
        this,
        &DirectoryMacDNTrampoline,
        notif_name,
        nullptr,
        CFNotificationSuspensionBehaviorDeliverImmediately );

    CFRelease( notif_name );
}

DirectoryMac::~DirectoryMac()
{
    CFNotificationCenterRemoveEveryObserver(
        CFNotificationCenterGetDistributedCenter(), this );
    CFNotificationCenterRemoveEveryObserver(
        CFNotificationCenterGetLocalCenter(), this );
}

void DirectoryMac::poll( double timeout_sec )
{
    //	Drain queued deliveries. Inline (local-center, DeliverImmediately)
    //	posts have usually already fired by the time we get here ; the
    //	runloop tick flushes cross-process / queued deliveries.
    if( timeout_sec < 0.0 )
        timeout_sec = 0.0;
    CFRunLoopRunInMode( kCFRunLoopDefaultMode, timeout_sec, true );
}

DirectoryMac::Entry & DirectoryMac::entry_for_locked( std::string const & name )
{
    for( auto & kv : servers_ )
        if( kv.first == name )
            return kv.second;
    servers_.emplace_back( name, Entry{} );
    return servers_.back().second;
}

void DirectoryMac::on_distributed_notification(
    char const *  server_name,
    std::uint32_t surface_id )
{
    if( server_name == nullptr || server_name[0] == '\0' )
        return;
    std::lock_guard<std::mutex> lock( mutex_ );
    Entry & e      = entry_for_locked( std::string( server_name ) );
    e.frames      += 1;
    e.last_surface = surface_id;
}

std::size_t DirectoryMac::count() const
{
    std::lock_guard<std::mutex> lock( mutex_ );
    return servers_.size();
}

std::vector<std::string> DirectoryMac::server_names() const
{
    std::lock_guard<std::mutex> lock( mutex_ );
    std::vector<std::string> out;
    out.reserve( servers_.size() );
    for( auto const & kv : servers_ )
        out.push_back( kv.first );
    std::sort( out.begin(), out.end() );
    return out;
}

std::uint64_t DirectoryMac::frame_count( std::string const & name ) const
{
    std::lock_guard<std::mutex> lock( mutex_ );
    for( auto const & kv : servers_ )
        if( kv.first == name )
            return kv.second.frames;
    return 0;
}

std::uint32_t DirectoryMac::latest_surface_id( std::string const & name ) const
{
    std::lock_guard<std::mutex> lock( mutex_ );
    for( auto const & kv : servers_ )
        if( kv.first == name )
            return kv.second.last_surface;
    return 0;
}

void DirectoryMac::clear()
{
    std::lock_guard<std::mutex> lock( mutex_ );
    servers_.clear();
}

}   //	namespace syphon
}   //	namespace aaa
