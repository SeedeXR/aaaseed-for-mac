// tests/unit/syphon_round_trip_perf_test.mm
//
// c127-A : Mac IPC perf baseline -- Syphon publish -> DN -> consume
// round-trip. The reciprocity stack (c124-A send + c125-A ClientMac +
// c126-A receive Lua-glue) is symmetric end-to-end on Mac as of c126 ;
// what's missing is a perf number for the hot loop : how many
// microseconds does ONE publish + DN-deliver + consume_iosurface cycle
// cost on Apple Silicon ? This is the Mac equivalent of the Windows
// Spout shared-texture round-trip baseline ; combined with the
// c121-B Phase 3 per-frame perf test it characterizes the full
// "render frame + push to peer" budget the reciprocity wiring imposes.
//
// Same-process scope :
//   ServerMac + ClientMac live in the SAME process. Delivery flows
//   through CFNotificationCenterGetLocalCenter() (c124-C extension)
//   which is in-process + synchronous : the trampoline fires INLINE
//   on publish_iosurface, so the measured round-trip is the pure
//   in-process IPC cost (CF dispatch + IOSurfaceLookup + CFRetain).
//   Cross-process numbers can be characterized separately once a
//   second-process driver lands ; that path adds distnoted daemon
//   round-trip cost which is fundamentally a different envelope.
//
// Three-artifact perf doctrine (philosophy.md 2.95 ; c121-B precedent) :
//   1. os_signpost interval per round-trip iteration + outer-loop
//      interval. Subsystem ai.bsa.aaaseed groups with the other perf
//      tests in Instruments.
//   2. std::chrono::steady_clock wall-clock per iteration ; mean /
//      median / p95 reported.
//   3. Budget envelope [1us, 50000us] : 1us floor catches a no-op
//      (e.g. trampoline never fired, has_frame stuck true from prior
//      iter, consume returned nullptr instantly) ; 50ms cap is the
//      reciprocity-stack budget bar -- well above what we expect
//      (microseconds) but bounded enough to catch a regression that
//      pushes per-frame IPC into the millisecond range.
//
// RESOURCE_LOCK "aaaseed_syphon_dn_bus" serializes with c122 / c124-C
// / c124-D tests that also post on the DN bus. distnoted drops
// deliveries under tight parallel post-load ; the lock guarantees
// rock-solid behaviour under ctest -j.

#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <IOSurface/IOSurface.h>
#import <gtest/gtest.h>

#import "src/syphon/syphon_client_mac.h"
#import "src/syphon/syphon_mac.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <os/log.h>
#include <os/signpost.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace
{
    //  Per-process signpost log handle. Subsystem ai.bsa.aaaseed
    //  groups with c121-B + other perf tests under one Instruments
    //  filter.
    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "ai.bsa.aaaseed", "performance" );
        return log;
    }

    //  Build a 1280x720 BGRA8 IOSurface. Returns +1 retained. The
    //  resolution mirrors c121-B's per-frame perf -- the resolution
    //  the .app actually presents -- so this perf number combines
    //  cleanly with that one to size the "render + push to peer"
    //  budget end-to-end.
    IOSurfaceRef MakeBGRA8Surface_1280x720()
    {
        NSDictionary * props = @{
            (NSString *) kIOSurfaceWidth          : @( 1280 ),
            (NSString *) kIOSurfaceHeight         : @( 720  ),
            (NSString *) kIOSurfaceBytesPerElement: @4,
            (NSString *) kIOSurfacePixelFormat    : @( (UInt32) 'BGRA' ),
        };
        return IOSurfaceCreate( (CFDictionaryRef) props );
    }
}   //  anonymous namespace

TEST( SyphonRoundTripPerf, PublishConsume_1280x720_BgraStaysUnderBudget )
{
    @autoreleasepool
    {
        constexpr int    kIterations = 100;
        constexpr double kMinMeanUs  = 1.0;       //  no-op floor
        constexpr double kMaxMeanUs  = 50000.0;   //  50ms budget cap

        //  Pid-stamped name so any incidental cross-process DN
        //  deliveries from parallel ctest workers don't collide on
        //  our slot.
        std::string const server_name =
            std::string( "AAASeedSyphonRoundTripPerf_pid" ) +
            std::to_string( (long) getpid() );

        aaa::syphon::ServerMac server( server_name.c_str() );
        ASSERT_TRUE( server.is_open() );
        ASSERT_EQ( server.advertise(), aaa::syphon::STATUS_OK );

        aaa::syphon::ClientMac client;

        //  Drain any noise queued from parallel workers (defense-
        //  in-depth even with the RESOURCE_LOCK -- distributed
        //  center can still seep frames from other processes).
        client.poll( 0.05 );
        while( client.has_frame() )
        {
            IOSurfaceRef noise = client.consume_iosurface();
            if( noise != nullptr )
                CFRelease( noise );
            client.poll( 0.01 );
        }

        IOSurfaceRef surface = MakeBGRA8Surface_1280x720();
        ASSERT_NE( surface, nullptr );

        //  --- Warm-up : publish+consume once. Lets the local
        //  notification center initialize its observer table /
        //  CFMessagePort dispatch. The first iteration's chrono
        //  number is uncharacteristic ; excluding it eliminates a
        //  high-variance outlier from the mean / p95.
        EXPECT_EQ( server.publish_iosurface( surface ),
                   aaa::syphon::STATUS_OK );
        ASSERT_TRUE( client.has_frame() );
        IOSurfaceRef warmup_recv = client.consume_iosurface();
        ASSERT_NE( warmup_recv, nullptr );
        CFRelease( warmup_recv );

        //  --- Measured loop --------------------------------------
        os_signpost_id_t loop_id = os_signpost_id_generate( perf_log() );
        os_signpost_interval_begin( perf_log(), loop_id,
                                    "syphon.round_trip.loop",
                                    "iterations=%d", kIterations );

        std::vector< double > iter_us;
        iter_us.reserve( kIterations );

        for( int i = 0; i < kIterations; ++i )
        {
            os_signpost_id_t iid = os_signpost_id_generate( perf_log() );
            os_signpost_interval_begin( perf_log(), iid,
                                        "syphon.round_trip.iter",
                                        "i=%d", i );

            auto const t0 = std::chrono::steady_clock::now();

            ASSERT_EQ( server.publish_iosurface( surface ),
                       aaa::syphon::STATUS_OK );
            //  Local center delivery is synchronous + inline -- the
            //  trampoline fires BEFORE publish_iosurface returns.
            //  has_frame() is true at this point with our slot
            //  freshly populated.
            ASSERT_TRUE( client.has_frame() );
            IOSurfaceRef recovered = client.consume_iosurface();
            ASSERT_NE( recovered, nullptr );

            auto const t1 = std::chrono::steady_clock::now();

            os_signpost_interval_end( perf_log(), iid,
                                      "syphon.round_trip.iter",
                                      "i=%d", i );

            CFRelease( recovered );

            double const us = std::chrono::duration< double, std::micro >(
                                  t1 - t0 ).count();
            iter_us.push_back( us );
        }

        os_signpost_interval_end( perf_log(), loop_id,
                                  "syphon.round_trip.loop",
                                  "iterations=%d", kIterations );

        //  --- Stats : mean / median / p95 ------------------------
        ASSERT_EQ( int( iter_us.size() ), kIterations );

        double sum = 0.0;
        for( double v : iter_us ) sum += v;
        double const mean_us = sum / double( kIterations );

        std::vector< double > sorted = iter_us;
        std::sort( sorted.begin(), sorted.end() );
        double const median_us = sorted[ kIterations / 2 ];
        //  p95 : index 95 for N=100 (0-based) ; the 5 slowest
        //  iterations sit above this value.
        double const p95_us    = sorted[ ( kIterations * 95 ) / 100 ];

        std::printf( "[ syphon round-trip ] mean=%.1fus median=%.1fus "
                     "p95=%.1fus N=%d (1280x720 BGRA8, local-center, "
                     "same-process)\n",
                     mean_us, median_us, p95_us, kIterations );

        //  --- Budget assertion -----------------------------------
        EXPECT_GT( mean_us, kMinMeanUs )
            << "Mean round-trip " << mean_us << "us is implausibly low "
            << "(< " << kMinMeanUs << "us). The publish/consume cycle "
            << "likely skipped work -- trampoline didn't fire, has_frame "
            << "was stuck true, or consume returned nullptr instantly.";
        EXPECT_LT( mean_us, kMaxMeanUs )
            << "Mean round-trip " << mean_us << "us exceeds the Phase 6 "
            << "reciprocity-stack budget bar of " << kMaxMeanUs << "us. "
            << "The IPC path has regressed into the millisecond range.";

        CFRelease( surface );
    }
}
