// src/midi/midi_mac.mm
//
// Implementation of the native macOS CoreMIDI sub-library. See
// midi_mac.h for scope and doctrine. Pure ObjC++ ; CoreMIDI +
// CoreFoundation only ; manual reference counting (-fno-objc-arc).
//
// All shared state lives in the file-scope g_state struct below and is
// never exposed in the header. A static C MIDIReadProc trampoline
// (read_trampoline) recovers that state via its refCon and dispatches
// decoded packet bytes to the stored InCallback under g_state.lock.

#include "midi_mac.h"

#include <CoreMIDI/CoreMIDI.h>
#include <CoreFoundation/CoreFoundation.h>

#include <mutex>
#include <vector>

namespace aaa
{
namespace midi
{

namespace
{

//	Shared CoreMIDI state. One client, one input port, one output port,
//	plus the currently connected input source and its callback. Guarded by
//	`lock` : the callback is invoked from CoreMIDI's receive thread while
//	open_in()/close_in()/deinit() run on the caller thread.
struct State
{
    std::mutex      lock;
    bool            initialized   { false };
    MIDIClientRef   client        { 0 };
    MIDIPortRef     in_port       { 0 };
    MIDIPortRef     out_port      { 0 };
    MIDIEndpointRef open_source   { 0 };   //	0 when no input is connected
    InCallback      callback;
};

State g_state;

//	Build a transient CFStringRef from a C literal. Caller must CFRelease.
CFStringRef make_cfstring( char const * utf8 )
{
    return CFStringCreateWithCString( kCFAllocatorDefault, utf8,
                                      kCFStringEncodingUTF8 );
}

//	Decode one MIDIPacketList and forward each packet's bytes to cb. Called
//	with g_state.lock already held. Each MIDIPacket may itself contain
//	several concatenated short messages or a sysex run ; we hand the whole
//	packet's data blob to the callback in one shot, which matches the
//	byte-stream contract documented in the header.
void dispatch_packet_list( MIDIPacketList const * list, InCallback const & cb )
{
    if ( list == nullptr || !cb )
        return;

    MIDIPacket const * packet = &list->packet[ 0 ];
    for ( UInt32 i = 0; i < list->numPackets; ++i )
    {
        if ( packet->length > 0 )
            cb( packet->data, static_cast<std::size_t>( packet->length ) );
        packet = MIDIPacketNext( packet );
    }
}

//	Static C trampoline matching the MIDIReadProc signature. readProcRefCon
//	is the &g_state pointer passed to MIDIInputPortCreate. We snapshot the
//	callback under the lock so a concurrent close_in() cannot tear it down
//	mid-dispatch.
void read_trampoline( MIDIPacketList const * pktlist,
                      void * readProcRefCon,
                      void * /*srcConnRefCon*/ )
{
    State * st = static_cast<State *>( readProcRefCon );
    if ( st == nullptr )
        return;

    std::lock_guard<std::mutex> guard( st->lock );
    dispatch_packet_list( pktlist, st->callback );
}

//	Fetch the display name of a MIDI endpoint as UTF-8. Empty on failure.
std::string endpoint_display_name( MIDIEndpointRef endpoint )
{
    if ( endpoint == 0 )
        return std::string();

    CFStringRef name = nullptr;
    OSStatus const err = MIDIObjectGetStringProperty(
        endpoint, kMIDIPropertyDisplayName, &name );
    if ( err != noErr || name == nullptr )
    {
        if ( name != nullptr )
            CFRelease( name );
        return std::string();
    }

    //	Size a UTF-8 buffer for the CFString, convert, release.
    CFIndex const utf16_len = CFStringGetLength( name );
    CFIndex const max_bytes =
        CFStringGetMaximumSizeForEncoding( utf16_len, kCFStringEncodingUTF8 ) + 1;

    std::string result;
    std::vector<char> buffer( static_cast<std::size_t>( max_bytes ), '\0' );
    if ( CFStringGetCString( name, buffer.data(), max_bytes,
                             kCFStringEncodingUTF8 ) )
    {
        result.assign( buffer.data() );
    }

    CFRelease( name );
    return result;
}

}   //	anonymous namespace

bool init()
{
    std::lock_guard<std::mutex> guard( g_state.lock );

    if ( g_state.initialized )
        return true;   //	idempotent

    CFStringRef client_name = make_cfstring( "aaa.midi.client" );
    MIDIClientRef client = 0;
    OSStatus err = MIDIClientCreate( client_name, nullptr, nullptr, &client );
    if ( client_name != nullptr )
        CFRelease( client_name );
    if ( err != noErr )
        return false;

    CFStringRef in_name = make_cfstring( "aaa.midi.in" );
    MIDIPortRef in_port = 0;
    err = MIDIInputPortCreate( client, in_name, read_trampoline,
                               &g_state, &in_port );
    if ( in_name != nullptr )
        CFRelease( in_name );
    if ( err != noErr )
    {
        MIDIClientDispose( client );
        return false;
    }

    CFStringRef out_name = make_cfstring( "aaa.midi.out" );
    MIDIPortRef out_port = 0;
    err = MIDIOutputPortCreate( client, out_name, &out_port );
    if ( out_name != nullptr )
        CFRelease( out_name );
    if ( err != noErr )
    {
        MIDIPortDispose( in_port );
        MIDIClientDispose( client );
        return false;
    }

    g_state.client      = client;
    g_state.in_port     = in_port;
    g_state.out_port    = out_port;
    g_state.open_source = 0;
    g_state.initialized = true;
    return true;
}

void deinit()
{
    std::lock_guard<std::mutex> guard( g_state.lock );

    if ( !g_state.initialized )
        return;   //	idempotent

    if ( g_state.open_source != 0 && g_state.in_port != 0 )
        MIDIPortDisconnectSource( g_state.in_port, g_state.open_source );
    g_state.open_source = 0;
    g_state.callback    = nullptr;

    if ( g_state.in_port != 0 )
        MIDIPortDispose( g_state.in_port );
    if ( g_state.out_port != 0 )
        MIDIPortDispose( g_state.out_port );
    if ( g_state.client != 0 )
        MIDIClientDispose( g_state.client );

    g_state.in_port     = 0;
    g_state.out_port    = 0;
    g_state.client      = 0;
    g_state.initialized = false;
}

bool is_initialized()
{
    std::lock_guard<std::mutex> guard( g_state.lock );
    return g_state.initialized;
}

std::size_t get_in_device_count()
{
    return static_cast<std::size_t>( MIDIGetNumberOfSources() );
}

std::size_t get_out_device_count()
{
    return static_cast<std::size_t>( MIDIGetNumberOfDestinations() );
}

std::string get_in_device_name( std::size_t index )
{
    if ( index >= get_in_device_count() )
        return std::string();
    MIDIEndpointRef src = MIDIGetSource( static_cast<ItemCount>( index ) );
    return endpoint_display_name( src );
}

std::string get_out_device_name( std::size_t index )
{
    if ( index >= get_out_device_count() )
        return std::string();
    MIDIEndpointRef dst = MIDIGetDestination( static_cast<ItemCount>( index ) );
    return endpoint_display_name( dst );
}

bool open_in( std::size_t index, InCallback cb )
{
    if ( index >= get_in_device_count() )
        return false;

    MIDIEndpointRef src = MIDIGetSource( static_cast<ItemCount>( index ) );
    if ( src == 0 )
        return false;

    std::lock_guard<std::mutex> guard( g_state.lock );

    if ( !g_state.initialized || g_state.in_port == 0 )
        return false;

    //	Only one input tracked : tear down the previous connection first.
    if ( g_state.open_source != 0 )
    {
        MIDIPortDisconnectSource( g_state.in_port, g_state.open_source );
        g_state.open_source = 0;
    }

    OSStatus const err = MIDIPortConnectSource( g_state.in_port, src, nullptr );
    if ( err != noErr )
    {
        g_state.callback = nullptr;
        return false;
    }

    g_state.open_source = src;
    g_state.callback    = std::move( cb );
    return true;
}

void close_in()
{
    std::lock_guard<std::mutex> guard( g_state.lock );

    if ( g_state.open_source != 0 && g_state.in_port != 0 )
        MIDIPortDisconnectSource( g_state.in_port, g_state.open_source );
    g_state.open_source = 0;
    g_state.callback    = nullptr;
}

bool send_short( std::size_t out_index,
                 unsigned char status, unsigned char d1, unsigned char d2 )
{
    if ( out_index >= get_out_device_count() )
        return false;

    MIDIEndpointRef dst = MIDIGetDestination( static_cast<ItemCount>( out_index ) );
    if ( dst == 0 )
        return false;

    std::array<unsigned char, 3> const msg = encode_short( status, d1, d2 );
    std::size_t const           n   = short_message_length( status );

    //	A short message fits comfortably in the inline packet[1] of a bare
    //	MIDIPacketList ; a 256-byte buffer is ample headroom.
    Byte                  storage[ 256 ];
    MIDIPacketList *      list   = reinterpret_cast<MIDIPacketList *>( storage );
    MIDIPacket *          cur    = MIDIPacketListInit( list );
    cur = MIDIPacketListAdd( list, sizeof( storage ), cur,
                             /*time=*/0,
                             static_cast<ByteCount>( n ),
                             msg.data() );
    if ( cur == nullptr )
        return false;   //	buffer too small (cannot happen for n <= 3)

    std::lock_guard<std::mutex> guard( g_state.lock );
    if ( !g_state.initialized || g_state.out_port == 0 )
        return false;

    return MIDISend( g_state.out_port, dst, list ) == noErr;
}

bool send_sysex( std::size_t out_index,
                 unsigned char const * data, std::size_t len )
{
    if ( data == nullptr || len == 0 )
        return false;
    if ( out_index >= get_out_device_count() )
        return false;

    MIDIEndpointRef dst = MIDIGetDestination( static_cast<ItemCount>( out_index ) );
    if ( dst == 0 )
        return false;

    //	Size the packet-list buffer to hold the full sysex run plus the
    //	MIDIPacketList / MIDIPacket headers. MIDIPacketListAdd internally
    //	splits runs longer than a single MIDIPacket (data[256]) across
    //	multiple packets, so we provision generously.
    std::size_t const   buf_size = sizeof( MIDIPacketList ) + len + 256;
    std::vector<Byte>   storage( buf_size, 0 );
    MIDIPacketList *    list = reinterpret_cast<MIDIPacketList *>( storage.data() );
    MIDIPacket *        cur  = MIDIPacketListInit( list );
    cur = MIDIPacketListAdd( list, static_cast<ByteCount>( buf_size ), cur,
                             /*time=*/0,
                             static_cast<ByteCount>( len ),
                             data );
    if ( cur == nullptr )
        return false;

    std::lock_guard<std::mutex> guard( g_state.lock );
    if ( !g_state.initialized || g_state.out_port == 0 )
        return false;

    return MIDISend( g_state.out_port, dst, list ) == noErr;
}

}   //	namespace midi
}   //	namespace aaa
