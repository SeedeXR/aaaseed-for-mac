// tests/native/midi_mac_test.mm
//
// GoogleTest coverage for the native macOS CoreMIDI sub-library
// (src/midi/midi_mac.{h,mm}).
//
// CI-safe : NO physical MIDI hardware required. The deterministic core is
// the pure encoding/framing helpers (encode_short, short_message_length,
// is_valid_sysex), which are tested exhaustively. The CoreMIDI-touching
// surface (init/deinit, device counts, name getters) is tested for
// crash-freedom and idempotency only -- those calls succeed headless
// because MIDIClientCreate and the enumeration APIs work without any
// connected endpoint.
//
// Loopback note : MIDISourceCreate makes a VIRTUAL SOURCE -- an endpoint
// that OTHER apps read from, fed by MIDIReceived. To observe those bytes
// from within this same process you must open that virtual source as an
// input via MIDIPortConnectSource and pump the run loop. CoreMIDI routes
// such in-process virtual traffic through the system midiserver, whose
// delivery is asynchronous and run-loop dependent ; under a headless
// ctest worker (no CFRunLoop spin, possibly sandboxed) it is NOT reliably
// observable within a bounded test. We therefore do the deterministic
// pure-helper tests for the encoding contract and GTEST_SKIP() the live
// loopback with an explicit log line rather than asserting on a flaky
// delivery.

#import <CoreMIDI/CoreMIDI.h>
#import <CoreFoundation/CoreFoundation.h>
#import <gtest/gtest.h>

#import "src/midi/midi_mac.h"

#include <array>
#include <cstddef>
#include <string>
#include <vector>

//	==================================================================
//	Pure helper : encode_short
//	==================================================================

TEST( MidiMacEncode, NoteOnBytesPassThrough )
{
    //	Note On, channel 0, key 60, velocity 100.
    auto const m = aaa::midi::encode_short( 0x90, 60, 100 );
    EXPECT_EQ( m[ 0 ], 0x90 );
    EXPECT_EQ( m[ 1 ], 60 );
    EXPECT_EQ( m[ 2 ], 100 );
}

TEST( MidiMacEncode, DataBytesMaskedTo7Bits )
{
    //	Data bytes above 0x7F must be masked ; status byte left intact.
    auto const m = aaa::midi::encode_short( 0xB0, 0xFF, 0x80 );
    EXPECT_EQ( m[ 0 ], 0xB0 );        //	status untouched
    EXPECT_EQ( m[ 1 ], 0x7F );        //	0xFF & 0x7F
    EXPECT_EQ( m[ 2 ], 0x00 );        //	0x80 & 0x7F
}

TEST( MidiMacEncode, StatusByteNotMasked )
{
    //	A full system-realtime status (0xF8 clock) survives verbatim.
    auto const m = aaa::midi::encode_short( 0xF8, 0x00, 0x00 );
    EXPECT_EQ( m[ 0 ], 0xF8 );
}

//	==================================================================
//	Pure helper : short_message_length
//	==================================================================

TEST( MidiMacLength, TwoDataByteMessages )
{
    EXPECT_EQ( aaa::midi::short_message_length( 0x80 ), 3u );  //	note off
    EXPECT_EQ( aaa::midi::short_message_length( 0x90 ), 3u );  //	note on
    EXPECT_EQ( aaa::midi::short_message_length( 0xA0 ), 3u );  //	poly AT
    EXPECT_EQ( aaa::midi::short_message_length( 0xB0 ), 3u );  //	CC
    EXPECT_EQ( aaa::midi::short_message_length( 0xE0 ), 3u );  //	pitch bend
}

TEST( MidiMacLength, OneDataByteMessages )
{
    EXPECT_EQ( aaa::midi::short_message_length( 0xC0 ), 2u );  //	program
    EXPECT_EQ( aaa::midi::short_message_length( 0xD0 ), 2u );  //	channel AT
}

TEST( MidiMacLength, SystemAndRealtimeMessages )
{
    EXPECT_EQ( aaa::midi::short_message_length( 0xF8 ), 1u );  //	clock
    EXPECT_EQ( aaa::midi::short_message_length( 0xFA ), 1u );  //	start
    EXPECT_EQ( aaa::midi::short_message_length( 0xFF ), 1u );  //	reset
}

TEST( MidiMacLength, ChannelNibbleIgnored )
{
    //	Length depends only on the high nibble, not the channel.
    EXPECT_EQ( aaa::midi::short_message_length( 0x9F ), 3u );  //	note on ch15
    EXPECT_EQ( aaa::midi::short_message_length( 0xC7 ), 2u );  //	program ch7
}

//	==================================================================
//	Pure helper : is_valid_sysex
//	==================================================================

TEST( MidiMacSysex, WellFormedAccepted )
{
    unsigned char const buf[] = { 0xF0, 0x7E, 0x00, 0x06, 0x01, 0xF7 };
    EXPECT_TRUE( aaa::midi::is_valid_sysex( buf, sizeof( buf ) ) );
}

TEST( MidiMacSysex, MinimalTwoByteAccepted )
{
    unsigned char const buf[] = { 0xF0, 0xF7 };
    EXPECT_TRUE( aaa::midi::is_valid_sysex( buf, sizeof( buf ) ) );
}

TEST( MidiMacSysex, NullOrTooShortRejected )
{
    EXPECT_FALSE( aaa::midi::is_valid_sysex( nullptr, 4 ) );
    unsigned char const one[] = { 0xF0 };
    EXPECT_FALSE( aaa::midi::is_valid_sysex( one, 1 ) );
    EXPECT_FALSE( aaa::midi::is_valid_sysex( one, 0 ) );
}

TEST( MidiMacSysex, MissingSoxOrEoxRejected )
{
    unsigned char const no_sox[] = { 0x00, 0x01, 0xF7 };
    EXPECT_FALSE( aaa::midi::is_valid_sysex( no_sox, sizeof( no_sox ) ) );
    unsigned char const no_eox[] = { 0xF0, 0x01, 0x02 };
    EXPECT_FALSE( aaa::midi::is_valid_sysex( no_eox, sizeof( no_eox ) ) );
}

TEST( MidiMacSysex, EmbeddedStatusByteRejected )
{
    //	A stray status byte (>= 0x80) inside the payload is illegal.
    unsigned char const buf[] = { 0xF0, 0x01, 0x90, 0x02, 0xF7 };
    EXPECT_FALSE( aaa::midi::is_valid_sysex( buf, sizeof( buf ) ) );
}

//	==================================================================
//	Lifecycle : init / deinit idempotency (CoreMIDI, headless-safe)
//	==================================================================

TEST( MidiMacLifecycle, InitDeinitIdempotent )
{
    //	Double init then double deinit must not crash and must leave the
    //	module in a clean, not-initialized state.
    ASSERT_TRUE( aaa::midi::init() );
    EXPECT_TRUE( aaa::midi::is_initialized() );

    EXPECT_TRUE( aaa::midi::init() );          //	second init : no-op
    EXPECT_TRUE( aaa::midi::is_initialized() );

    aaa::midi::deinit();
    EXPECT_FALSE( aaa::midi::is_initialized() );

    aaa::midi::deinit();                       //	second deinit : no-op
    EXPECT_FALSE( aaa::midi::is_initialized() );
}

TEST( MidiMacLifecycle, DeinitWithoutInitIsSafe )
{
    //	deinit() before any init() must be a harmless no-op.
    aaa::midi::deinit();
    EXPECT_FALSE( aaa::midi::is_initialized() );
}

TEST( MidiMacLifecycle, CloseInWithoutOpenIsSafe )
{
    //	close_in() with nothing open must not crash, regardless of init.
    aaa::midi::close_in();
    ASSERT_TRUE( aaa::midi::init() );
    aaa::midi::close_in();
    aaa::midi::deinit();
    SUCCEED();
}

//	==================================================================
//	Device enumeration : counts and name getters, 0-device-safe
//	==================================================================

TEST( MidiMacDevices, CountsAreNonNegativeAndStable )
{
    //	std::size_t is unsigned so ">= 0" is trivially true ; the real
    //	assertion is that the calls do not crash and are stable across
    //	repeated invocation on a headless box (typically 0 endpoints).
    std::size_t const in_a  = aaa::midi::get_in_device_count();
    std::size_t const out_a = aaa::midi::get_out_device_count();
    std::size_t const in_b  = aaa::midi::get_in_device_count();
    std::size_t const out_b = aaa::midi::get_out_device_count();
    EXPECT_EQ( in_a,  in_b );
    EXPECT_EQ( out_a, out_b );
}

TEST( MidiMacDevices, NameGettersHandleEnumeratedAndOutOfRange )
{
    std::size_t const in_count  = aaa::midi::get_in_device_count();
    std::size_t const out_count = aaa::midi::get_out_device_count();

    //	Every enumerated device must yield a getter call that does not
    //	crash (name may legitimately be empty if the property is unset).
    for ( std::size_t i = 0; i < in_count; ++i )
    {
        std::string const n = aaa::midi::get_in_device_name( i );
        (void)n;
    }
    for ( std::size_t i = 0; i < out_count; ++i )
    {
        std::string const n = aaa::midi::get_out_device_name( i );
        (void)n;
    }

    //	Out-of-range index -> empty string, never a crash. This is the
    //	key 0-device-safe path on headless CI.
    EXPECT_TRUE( aaa::midi::get_in_device_name( in_count ).empty() );
    EXPECT_TRUE( aaa::midi::get_in_device_name( in_count + 1000 ).empty() );
    EXPECT_TRUE( aaa::midi::get_out_device_name( out_count ).empty() );
    EXPECT_TRUE( aaa::midi::get_out_device_name( out_count + 1000 ).empty() );
}

//	==================================================================
//	Output / input on absent hardware : must fail gracefully, not crash
//	==================================================================

TEST( MidiMacOutput, SendToOutOfRangeDestinationFailsCleanly )
{
    ASSERT_TRUE( aaa::midi::init() );

    std::size_t const out_count = aaa::midi::get_out_device_count();

    //	Sending to a non-existent destination index returns false, never
    //	crashes -- the central headless guarantee for the send path.
    EXPECT_FALSE( aaa::midi::send_short( out_count + 100, 0x90, 60, 100 ) );

    unsigned char const sx[] = { 0xF0, 0x7E, 0x7F, 0x06, 0x01, 0xF7 };
    EXPECT_FALSE( aaa::midi::send_sysex( out_count + 100, sx, sizeof( sx ) ) );

    //	Null / empty sysex is rejected up front.
    EXPECT_FALSE( aaa::midi::send_sysex( 0, nullptr, 4 ) );
    EXPECT_FALSE( aaa::midi::send_sysex( 0, sx, 0 ) );

    aaa::midi::deinit();
}

TEST( MidiMacInput, OpenOutOfRangeSourceFailsCleanly )
{
    ASSERT_TRUE( aaa::midi::init() );

    std::size_t const in_count = aaa::midi::get_in_device_count();
    bool const opened = aaa::midi::open_in(
        in_count + 100,
        []( unsigned char const *, std::size_t ) {} );
    EXPECT_FALSE( opened );

    aaa::midi::close_in();
    aaa::midi::deinit();
}

//	==================================================================
//	Live loopback : hardware/run-loop dependent -> explicit skip
//	==================================================================

TEST( MidiMacLoopback, VirtualSourceRoundTrip )
{
    //	A genuine round-trip (MIDISourceCreate -> MIDIReceived -> observe
    //	via an input port + InCallback) depends on the system midiserver
    //	delivering through a spun CFRunLoop, which a headless ctest worker
    //	does not provide deterministically. Skip loudly rather than flake.
    GTEST_SKIP() << "MidiMacLoopback: live CoreMIDI virtual-endpoint "
                    "round-trip requires a spun CFRunLoop / midiserver "
                    "delivery not available headlessly; covered "
                    "deterministically by the encode/length/sysex pure "
                    "helper tests instead.";
}
