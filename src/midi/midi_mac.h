// src/midi/midi_mac.h
//
// Native macOS CoreMIDI sub-library for the AAASeed Mac port.
//
// Scope :
//   The Win-side engine exposes a MIDI in/out surface backed by Win32
//   midiInOpen / midiOutShortMsg. On Mac the equivalent native stack is
//   Apple's CoreMIDI. This sub-library is the hermetic Mac-side backing
//   for that surface :
//     - init()/deinit()      : manage a single shared MIDIClientRef plus
//                              one input port and one output port. Both
//                              calls are idempotent.
//     - device enumeration   : MIDIGetNumberOfSources/Destinations +
//                              MIDIGetSource/Destination, with display
//                              names via MIDIObjectGetStringProperty +
//                              kMIDIPropertyDisplayName.
//     - open_in(idx, cb)     : MIDIPortConnectSource on the input port ;
//                              incoming MIDIPacketLists are decoded and
//                              the raw bytes are forwarded to the caller
//                              callback under a mutex.
//     - send_short / send_sysex : build a MIDIPacketList (PacketListInit
//                              + PacketListAdd) and MIDISend it to the
//                              destination at the given index.
//
// CoreMIDI note : the MIDI 1.0 byte-stream API used here
// (MIDIPacketList, MIDIReadProc, MIDISend, MIDIReceived, MIDISourceCreate)
// is marked deprecated in current SDKs in favour of the MIDIEventList /
// MIDIProtocol UMP stack, but the legacy symbols are still exported and
// fully functional. They are deliberately chosen here to mirror the
// Win-side byte-oriented short-message / sysex model 1:1.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). No engine link, no o_str, no aaa_mem, no vendor
//     includes. std:: + CoreMIDI + CoreFoundation only.
//   - Manual reference counting (compiled with -fno-objc-arc) ; this
//     header carries no ObjC objects, so callers stay C++-clean.
//   - All shared state lives in a file-scope struct in the .mm and is
//     never exposed here. A static C MIDIReadProc trampoline recovers
//     that context and dispatches to the stored callback under a mutex.
//
// Mac-only code in its own directory : no #ifdef platform guards.

#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <string>

namespace aaa
{
namespace midi
{

//	Incoming-data callback. data points to the raw MIDI bytes of a single
//	decoded packet (status + data bytes for short messages, or the sysex
//	byte run) ; len is the byte count. The pointer is only valid for the
//	duration of the call. Invoked from CoreMIDI's high-priority receive
//	thread, serialized by an internal mutex.
using InCallback = std::function<void( unsigned char const * data, std::size_t len )>;

//	-----------------------------------------------------------------
//	Pure encoding helpers (no CoreMIDI, no global state).
//
//	These are deterministic byte-layout helpers, factored out so the
//	short-message and sysex framing can be unit-tested without any MIDI
//	hardware or live MIDIClientRef.
//	-----------------------------------------------------------------

//	encode_short -- pack a 3-byte MIDI 1.0 channel-voice / system message.
//	Returns {status, d1, d2} verbatim ; data bytes are masked to 7 bits
//	(0x00..0x7F) as the MIDI spec requires, while the status byte is left
//	untouched (callers pass a full 0x80..0xFF status). The returned array
//	is always length 3 ; messages that logically use fewer data bytes
//	(e.g. program change) simply ignore the trailing element when sent
//	with the appropriate length.
inline std::array<unsigned char, 3>
encode_short( unsigned char status, unsigned char d1, unsigned char d2 )
{
    return { status,
             static_cast<unsigned char>( d1 & 0x7F ),
             static_cast<unsigned char>( d2 & 0x7F ) };
}

//	short_message_length -- number of meaningful bytes for a given status
//	byte. Two-data-byte messages (note on/off 0x80/0x90, poly aftertouch
//	0xA0, control change 0xB0, pitch bend 0xE0) -> 3. One-data-byte
//	messages (program change 0xC0, channel aftertouch 0xD0) -> 2. Anything
//	else (system / realtime) -> 1. Used to size a short-message packet.
inline std::size_t
short_message_length( unsigned char status )
{
    unsigned char const high = static_cast<unsigned char>( status & 0xF0 );
    switch ( high )
    {
        case 0x80:  //	note off
        case 0x90:  //	note on
        case 0xA0:  //	poly key pressure
        case 0xB0:  //	control change
        case 0xE0:  //	pitch bend
            return 3;
        case 0xC0:  //	program change
        case 0xD0:  //	channel pressure
            return 2;
        default:
            return 1;
    }
}

//	is_valid_sysex -- true if data is a well-formed System Exclusive run :
//	at least 2 bytes, starts with 0xF0 (SOX) and ends with 0xF7 (EOX),
//	with no embedded status bytes (>= 0x80) in between. CoreMIDI's
//	MIDIPacketListAdd accepts arbitrary byte runs, so this is a caller-side
//	sanity gate, kept pure so it can be tested directly.
inline bool
is_valid_sysex( unsigned char const * data, std::size_t len )
{
    if ( data == nullptr || len < 2 )
        return false;
    if ( data[ 0 ] != 0xF0 || data[ len - 1 ] != 0xF7 )
        return false;
    for ( std::size_t i = 1; i + 1 < len; ++i )
    {
        if ( data[ i ] >= 0x80 )
            return false;
    }
    return true;
}

//	-----------------------------------------------------------------
//	Lifecycle.
//	-----------------------------------------------------------------

//	init -- create (once) the shared MIDIClientRef plus one input and one
//	output port. Idempotent : a second call while already initialized is a
//	no-op that returns true. Returns false only if CoreMIDI failed to
//	create the client or a port.
bool init();

//	deinit -- disconnect any open input source, dispose the ports and the
//	client, and clear all state. Idempotent : safe to call when not
//	initialized and safe to call twice.
void deinit();

//	is_initialized -- true between a successful init() and a deinit().
bool is_initialized();

//	-----------------------------------------------------------------
//	Device enumeration. Counts are always >= 0 ; safe to call before
//	init() (CoreMIDI enumeration does not require a client). Out-of-range
//	indices yield an empty string from the name getters.
//	-----------------------------------------------------------------

std::size_t get_in_device_count();
std::size_t get_out_device_count();

//	Display name (kMIDIPropertyDisplayName, UTF-8) of the source /
//	destination at index. Empty string if index is out of range or the
//	property is unavailable.
std::string get_in_device_name( std::size_t index );
std::string get_out_device_name( std::size_t index );

//	-----------------------------------------------------------------
//	Input.
//	-----------------------------------------------------------------

//	open_in -- connect the source at index to the shared input port and
//	route incoming packets to cb. Requires init() first. Only one input
//	connection is tracked at a time : opening a new index implicitly
//	closes the previous one. Returns false if not initialized, index is
//	out of range, or MIDIPortConnectSource failed.
bool open_in( std::size_t index, InCallback cb );

//	close_in -- disconnect the currently open input source (if any) and
//	clear the stored callback. Idempotent.
void close_in();

//	-----------------------------------------------------------------
//	Output.
//	-----------------------------------------------------------------

//	send_short -- send a short channel/system message to the destination
//	at out_index via the shared output port. The byte count actually sent
//	is derived from the status byte (see short_message_length). Requires
//	init(). Returns false if not initialized, index out of range, or
//	MIDISend failed.
bool send_short( std::size_t out_index,
                 unsigned char status, unsigned char d1, unsigned char d2 );

//	send_sysex -- send an arbitrary byte run (typically a 0xF0..0xF7
//	System Exclusive message) to the destination at out_index. Requires
//	init(). Returns false if not initialized, index out of range, data is
//	null/empty, or MIDISend failed. The bytes are sent verbatim ; callers
//	may pre-validate with is_valid_sysex().
bool send_sysex( std::size_t out_index,
                 unsigned char const * data, std::size_t len );

}   //	namespace midi
}   //	namespace aaa
