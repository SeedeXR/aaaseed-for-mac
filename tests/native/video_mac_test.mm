// tests/native/video_mac_test.mm
//
// second_todo.md S2 (c153) : Movie + Capture crash-safety / enumeration.
// CI-safe : the static capture enumeration never starts a session (so no
// camera-permission prompt), and the real camera / video-file paths are
// gated behind AAA_VIDEO_LIVE=1 and GTEST_SKIP'd otherwise (never silent).

#import <CoreVideo/CoreVideo.h>
#import <gtest/gtest.h>

#include "src/video/movie_mac.h"
#include "src/video/capture_mac.h"

#include <cstdlib>
#include <string>

TEST( VideoMovie, OpenNonexistentReturnsFalse )
{
    aaa::video::Movie m;
    EXPECT_FALSE( m.open( "/no/such/movie/really.mov" ) );
    EXPECT_FALSE( m.is_open() );
    EXPECT_EQ( m.duration_seconds(), 0.0 );
    EXPECT_FALSE( m.is_playing() );
    EXPECT_EQ( m.copy_current_pixel_buffer(), nullptr );
}

TEST( VideoMovie, OpenNullPathReturnsFalse )
{
    aaa::video::Movie m;
    EXPECT_FALSE( m.open( nullptr ) );
}

TEST( VideoCapture, DeviceEnumDoesNotCrashOrPrompt )
{
    //	Enumeration only -- must not start a session / trigger TCC.
    std::size_t n = aaa::video::Capture::get_device_count();
    EXPECT_EQ( aaa::video::Capture::get_device_name( 100000 ), std::string() );
    SUCCEED() << "video devices=" << n;
}

TEST( VideoCapture, OpenOutOfRangeReturnsFalse )
{
    aaa::video::Capture c;
    EXPECT_FALSE( c.open( 100000 ) );
    EXPECT_FALSE( c.is_running() );
    EXPECT_EQ( c.copy_latest_pixel_buffer(), nullptr );
}

TEST( VideoCapture, LiveCaptureGated )
{
    if( std::getenv( "AAA_VIDEO_LIVE" ) == nullptr )
        GTEST_SKIP() << "live camera capture requires a device + AAA_VIDEO_LIVE=1";

    if( aaa::video::Capture::get_device_count() == 0 )
        GTEST_SKIP() << "no capture devices present";

    aaa::video::Capture c;
    ASSERT_TRUE( c.open( 0 ) );
    ASSERT_TRUE( c.start() );
    EXPECT_TRUE( c.is_running() );
    c.stop();
    EXPECT_FALSE( c.is_running() );
}
