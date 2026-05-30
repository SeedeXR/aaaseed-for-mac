// tests/native/audio_mac_test.mm
//
// second_todo.md S1 (c153) : device-enumeration + Player crash-safety. CI-safe
// (no audio hardware required). Live playback / mic-input paths are gated
// behind AAA_AUDIO_LIVE=1 and GTEST_SKIP'd otherwise (never silent).

#import <gtest/gtest.h>

#include "src/audio/audio_mac.h"

#include <cstdlib>
#include <string>

TEST( AudioMacDevices, CountsDoNotCrash )
{
    //	std::size_t is unsigned ; the contract is "no crash, returns a count".
    std::size_t out = aaa::audio::get_output_device_count();
    std::size_t in  = aaa::audio::get_input_device_count();
    SUCCEED() << "outputs=" << out << " inputs=" << in;
}

TEST( AudioMacDevices, OutOfRangeNamesAreEmpty )
{
    EXPECT_EQ( aaa::audio::get_output_device_name( 100000 ), std::string() );
    EXPECT_EQ( aaa::audio::get_input_device_name( 100000 ),  std::string() );
}

TEST( AudioMacDevices, EnumeratedNamesAreNonEmptyWhenPresent )
{
    std::size_t n = aaa::audio::get_output_device_count();
    if( n == 0 )
        GTEST_SKIP() << "no output devices on this runner";
    //	At least the first enumerated device should report a name.
    EXPECT_FALSE( aaa::audio::get_output_device_name( 0 ).empty() );
}

TEST( AudioMacPlayer, OpenNonexistentReturnsFalse )
{
    aaa::audio::Player p;
    EXPECT_FALSE( p.open( "/no/such/file/really.wav" ) );
    EXPECT_FALSE( p.is_playing() );
    EXPECT_EQ( p.duration_seconds(), 0.0 );
    EXPECT_EQ( p.position_seconds(), 0.0 );
}

TEST( AudioMacPlayer, VolumePanRateClamp )
{
    aaa::audio::Player p;
    p.set_volume( 5.0f );  EXPECT_FLOAT_EQ( p.get_volume(), 1.0f );
    p.set_volume( -1.0f ); EXPECT_FLOAT_EQ( p.get_volume(), 0.0f );
    p.set_pan( 5.0f );     EXPECT_FLOAT_EQ( p.get_pan(), 1.0f );
    p.set_pan( -5.0f );    EXPECT_FLOAT_EQ( p.get_pan(), -1.0f );
    p.set_rate( 100.0f );  EXPECT_LE( p.get_rate(), 4.0f );
    p.set_rate( 0.0f );    EXPECT_GE( p.get_rate(), 0.25f );
}

TEST( AudioMacInput, LiveCaptureGated )
{
    if( std::getenv( "AAA_AUDIO_LIVE" ) == nullptr )
        GTEST_SKIP() << "live audio input requires a mic + AAA_AUDIO_LIVE=1";

    aaa::audio::Input in;
    bool started = in.start( []( float const *, std::size_t, double ){} );
    if( !started )
        GTEST_SKIP() << "no usable input device";
    EXPECT_TRUE( in.is_running() );
    in.stop();
    EXPECT_FALSE( in.is_running() );
}
