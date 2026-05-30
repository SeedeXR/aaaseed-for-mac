// tests/native/syphon_directory_mac_test.mm
//
// second_todo.md S4 (c153) : coverage for the Syphon server DIRECTORY
// (discovery) + the pure vertical-flip helper.
//
// Directory tests publish frames from two distinctly-named ServerMac
// instances and assert the DirectoryMac discovers BOTH, with advancing
// frame counts and the correct latest surface id. To stay 100% reliable in
// a parallel ctest worker (distnoted drops cross-process posts under load),
// we drive discovery through the LOCAL notification center path : ServerMac
// posts on both centers, DirectoryMac observes both, and the local-center
// delivery is in-process + synchronous (feedback_distnoted_dual_center.md).
//
// The flip helper tests are pure (no GPU / IOSurface) per the project's
// "test the deterministic logic" rule.

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <gtest/gtest.h>

#include "src/syphon/syphon_directory_mac.h"
#include "src/syphon/syphon_mac.h"
#include "src/syphon/syphon_pixel_util.h"

#include <cstdint>
#include <vector>

namespace
{

//	Create a tiny IOSurface so publish_iosurface() has a real surface id to
//	broadcast. BGRA8, 16x16. Caller releases via CFRelease.
IOSurfaceRef make_test_surface()
{
    NSDictionary * props = @{
        (id)kIOSurfaceWidth          : @16,
        (id)kIOSurfaceHeight         : @16,
        (id)kIOSurfaceBytesPerElement: @4,
        (id)kIOSurfacePixelFormat    : @( (unsigned)'BGRA' ),
    };
    return IOSurfaceCreate( (__bridge CFDictionaryRef) props );
}

}   //	anonymous namespace

//	-----------------------------------------------------------------------
//	Directory discovery
//	-----------------------------------------------------------------------
TEST( SyphonDirectory, DiscoversTwoNamedServers )
{
    aaa::syphon::DirectoryMac dir;

    aaa::syphon::ServerMac a( "aaa.test.dir.alpha" );
    aaa::syphon::ServerMac b( "aaa.test.dir.beta" );
    ASSERT_TRUE( a.is_open() );
    ASSERT_TRUE( b.is_open() );
    ASSERT_EQ( a.advertise(), aaa::syphon::STATUS_OK );
    ASSERT_EQ( b.advertise(), aaa::syphon::STATUS_OK );

    IOSurfaceRef sa = make_test_surface();
    IOSurfaceRef sb = make_test_surface();
    ASSERT_NE( sa, nullptr );
    ASSERT_NE( sb, nullptr );

    //	Publish a few frames from each ; local-center delivery is inline.
    ASSERT_EQ( a.publish_iosurface( sa ), aaa::syphon::STATUS_OK );
    ASSERT_EQ( b.publish_iosurface( sb ), aaa::syphon::STATUS_OK );
    ASSERT_EQ( a.publish_iosurface( sa ), aaa::syphon::STATUS_OK );
    dir.poll( 0.1 );

    EXPECT_GE( dir.count(), (std::size_t)2 );

    auto names = dir.server_names();
    bool saw_alpha = false, saw_beta = false;
    for( auto const & n : names )
    {
        if( n == "aaa.test.dir.alpha" ) saw_alpha = true;
        if( n == "aaa.test.dir.beta" )  saw_beta  = true;
    }
    EXPECT_TRUE( saw_alpha );
    EXPECT_TRUE( saw_beta );

    //	alpha published twice, beta once.
    EXPECT_GE( dir.frame_count( "aaa.test.dir.alpha" ), (std::uint64_t)2 );
    EXPECT_GE( dir.frame_count( "aaa.test.dir.beta" ),  (std::uint64_t)1 );

    //	latest surface id matches sa's kernel id.
    EXPECT_EQ( dir.latest_surface_id( "aaa.test.dir.alpha" ),
               (std::uint32_t) IOSurfaceGetID( sa ) );

    CFRelease( sa );
    CFRelease( sb );
}

TEST( SyphonDirectory, ClearForgetsServers )
{
    aaa::syphon::DirectoryMac dir;
    aaa::syphon::ServerMac s( "aaa.test.dir.clearme" );
    ASSERT_EQ( s.advertise(), aaa::syphon::STATUS_OK );
    IOSurfaceRef surf = make_test_surface();
    ASSERT_NE( surf, nullptr );
    ASSERT_EQ( s.publish_iosurface( surf ), aaa::syphon::STATUS_OK );
    dir.poll( 0.1 );
    EXPECT_GE( dir.count(), (std::size_t)1 );
    dir.clear();
    EXPECT_EQ( dir.count(), (std::size_t)0 );
    EXPECT_EQ( dir.frame_count( "aaa.test.dir.clearme" ), (std::uint64_t)0 );
    CFRelease( surf );
}

TEST( SyphonDirectory, IgnoresEmptyServerName )
{
    aaa::syphon::DirectoryMac dir;
    dir.on_distributed_notification( "", 123 );
    dir.on_distributed_notification( nullptr, 456 );
    EXPECT_EQ( dir.count(), (std::size_t)0 );
}

//	-----------------------------------------------------------------------
//	Vertical-flip helper (pure)
//	-----------------------------------------------------------------------
TEST( SyphonFlip, FlipsRowsInPlace )
{
    //	3 rows x stride 4 ; values encode the row index for easy assert.
    std::vector<std::uint8_t> buf = {
        0, 0, 0, 0,     //	row 0
        1, 1, 1, 1,     //	row 1
        2, 2, 2, 2,     //	row 2
    };
    ASSERT_TRUE( aaa::syphon::flip_rows_in_place( buf.data(), 3, 4 ) );
    EXPECT_EQ( buf[ 0 ], 2 );   //	row 0 now holds old row 2
    EXPECT_EQ( buf[ 4 ], 1 );   //	middle row unchanged
    EXPECT_EQ( buf[ 8 ], 0 );   //	row 2 now holds old row 0
}

TEST( SyphonFlip, EvenRowCountFullSwap )
{
    std::vector<std::uint8_t> buf = {
        10, 10,
        20, 20,
        30, 30,
        40, 40,
    };
    ASSERT_TRUE( aaa::syphon::flip_rows_in_place( buf.data(), 4, 2 ) );
    EXPECT_EQ( buf[ 0 ], 40 );
    EXPECT_EQ( buf[ 2 ], 30 );
    EXPECT_EQ( buf[ 4 ], 20 );
    EXPECT_EQ( buf[ 6 ], 10 );
}

TEST( SyphonFlip, HonoursPaddedStride )
{
    //	stride 4 but only 2 meaningful bytes/row ; padding travels with row.
    std::vector<std::uint8_t> buf = {
        1, 1, 9, 9,
        2, 2, 8, 8,
    };
    ASSERT_TRUE( aaa::syphon::flip_rows_in_place( buf.data(), 2, 4 ) );
    EXPECT_EQ( buf[ 0 ], 2 ); EXPECT_EQ( buf[ 2 ], 8 );
    EXPECT_EQ( buf[ 4 ], 1 ); EXPECT_EQ( buf[ 6 ], 9 );
}

TEST( SyphonFlip, NoOpOnDegenerateInput )
{
    std::uint8_t one[ 4 ] = { 5, 5, 5, 5 };
    EXPECT_FALSE( aaa::syphon::flip_rows_in_place( one, 1, 4 ) );   //	height 1
    EXPECT_FALSE( aaa::syphon::flip_rows_in_place( nullptr, 4, 4 ) );
    EXPECT_FALSE( aaa::syphon::flip_rows_in_place( one, 4, 0 ) );
    EXPECT_EQ( one[ 0 ], 5 );   //	untouched
}
