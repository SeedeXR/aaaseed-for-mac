// tests/unit/aaa_event_bridge_gesture_test.cpp
//
// c133 / Phase 4 (todo L291) : GestureBridge drain / post / NSEvent
// mapping coverage. Exercises ScrollEvent / MagnifyEvent / RotateEvent
// / SwipeEvent queue shape via the `post_*` synthetic path.
//
// Why a plain C++ TU (not .mm) :
//   - NSEventTypeMagnify / Rotate / Swipe / ScrollWheel cannot be
//     synthesized via AppKit's `+mouseEventWithType:` (it throws ;
//     c119-B's test file documents the same caveat).
//   - The GestureBridge public API is pure C++ (post_*, drain_*,
//     degrees_to_radians) ; the ObjC++ `handle_ns_event` entry point
//     is covered by the static `degrees_to_radians` regression and
//     by inspection.
//
// Hermetic doctrine reminder : this TU includes only the GestureBridge
// header + gtest. No engine, no AAASeedInputView, no o_str.

#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <vector>

#include "src/ui/macos/aaa_event_bridge_gesture.h"

TEST( GestureBridge, ScrollDrainsInOrder )
{
    aaa::input::GestureBridge bridge;
    bridge.post_scroll_event(  1.0,  2.0, /*momentum=*/false, /*ts=*/100 );
    bridge.post_scroll_event(  3.0,  4.0, /*momentum=*/false, /*ts=*/200 );
    bridge.post_scroll_event( -5.0,  6.5, /*momentum=*/true,  /*ts=*/300 );

    std::vector< aaa::input::ScrollEvent > const events =
        bridge.drain_scroll_events();
    ASSERT_EQ( events.size(), 3u );

    EXPECT_DOUBLE_EQ( events[ 0 ].dx, 1.0 );
    EXPECT_DOUBLE_EQ( events[ 0 ].dy, 2.0 );
    EXPECT_FALSE   ( events[ 0 ].momentum_phase );
    EXPECT_EQ      ( events[ 0 ].timestamp_ns, 100u );

    EXPECT_DOUBLE_EQ( events[ 1 ].dx, 3.0 );
    EXPECT_DOUBLE_EQ( events[ 1 ].dy, 4.0 );
    EXPECT_EQ      ( events[ 1 ].timestamp_ns, 200u );

    EXPECT_DOUBLE_EQ( events[ 2 ].dx, -5.0 );
    EXPECT_DOUBLE_EQ( events[ 2 ].dy,  6.5 );
    EXPECT_TRUE    ( events[ 2 ].momentum_phase );
    EXPECT_EQ      ( events[ 2 ].timestamp_ns, 300u );
}

TEST( GestureBridge, MagnifyAccumulatesCumulative )
{
    aaa::input::GestureBridge bridge;
    bridge.post_magnify_event( 0.10 );
    bridge.post_magnify_event( 0.05 );
    bridge.post_magnify_event( -0.02 );

    std::vector< aaa::input::MagnifyEvent > const events =
        bridge.drain_magnify_events();
    ASSERT_EQ( events.size(), 3u );

    //	Each event carries the running cumulative-at-emit-time.
    EXPECT_DOUBLE_EQ( events[ 0 ].delta,      0.10 );
    EXPECT_DOUBLE_EQ( events[ 0 ].cumulative, 0.10 );

    EXPECT_DOUBLE_EQ( events[ 1 ].delta,      0.05 );
    EXPECT_DOUBLE_EQ( events[ 1 ].cumulative, 0.15 );

    EXPECT_DOUBLE_EQ( events[ 2 ].delta,      -0.02 );
    EXPECT_DOUBLE_EQ( events[ 2 ].cumulative,  0.13 );
}

TEST( GestureBridge, RotateConvertsDegToRad )
{
    //	The static utility used by handle_ns_event. NSEvent reports
    //	rotation in DEGREES ; we convert at ingest. 90 deg = pi/2 rad.
    double const rad_90 = aaa::input::GestureBridge::degrees_to_radians( 90.0 );
    EXPECT_NEAR( rad_90, 1.5707963267948966, 1.0e-12 );

    //	Spot-check 180 / 360 too -- catches a sign or factor-of-2 bug.
    EXPECT_NEAR( aaa::input::GestureBridge::degrees_to_radians( 180.0 ),
                 3.14159265358979323846, 1.0e-12 );
    EXPECT_NEAR( aaa::input::GestureBridge::degrees_to_radians( 360.0 ),
                 6.28318530717958647692, 1.0e-12 );

    //	And the cumulative path : feed a 90-deg rotate via the rad-side
    //	API so the test also covers the queue-shape carry-through.
    aaa::input::GestureBridge bridge;
    bridge.post_rotate_event( rad_90 );
    std::vector< aaa::input::RotateEvent > const events =
        bridge.drain_rotate_events();
    ASSERT_EQ( events.size(), 1u );
    EXPECT_NEAR( events[ 0 ].delta_rad,      1.5707963267948966, 1.0e-12 );
    EXPECT_NEAR( events[ 0 ].cumulative_rad, 1.5707963267948966, 1.0e-12 );
}

TEST( GestureBridge, SwipeDirectionMapping )
{
    aaa::input::GestureBridge bridge;
    bridge.post_swipe_event( -1 );  //	left
    bridge.post_swipe_event( +1 );  //	right
    bridge.post_swipe_event( +2 );  //	up
    bridge.post_swipe_event( -2 );  //	down

    std::vector< aaa::input::SwipeEvent > const events =
        bridge.drain_swipe_events();
    ASSERT_EQ( events.size(), 4u );
    EXPECT_EQ( events[ 0 ].direction, -1 );
    EXPECT_EQ( events[ 1 ].direction, +1 );
    EXPECT_EQ( events[ 2 ].direction, +2 );
    EXPECT_EQ( events[ 3 ].direction, -2 );
}

TEST( GestureBridge, DrainEmptiesQueue )
{
    aaa::input::GestureBridge bridge;
    bridge.post_scroll_event ( 1.0, 1.0, false );
    bridge.post_magnify_event( 0.5 );
    bridge.post_rotate_event ( 0.25 );
    bridge.post_swipe_event  ( -1 );

    EXPECT_EQ( bridge.pending_scroll(),  1u );
    EXPECT_EQ( bridge.pending_magnify(), 1u );
    EXPECT_EQ( bridge.pending_rotate(),  1u );
    EXPECT_EQ( bridge.pending_swipe(),   1u );

    //	First drains return the queued events.
    EXPECT_EQ( bridge.drain_scroll_events ().size(), 1u );
    EXPECT_EQ( bridge.drain_magnify_events().size(), 1u );
    EXPECT_EQ( bridge.drain_rotate_events ().size(), 1u );
    EXPECT_EQ( bridge.drain_swipe_events  ().size(), 1u );

    //	Pending counters all back to zero after drain.
    EXPECT_EQ( bridge.pending_scroll(),  0u );
    EXPECT_EQ( bridge.pending_magnify(), 0u );
    EXPECT_EQ( bridge.pending_rotate(),  0u );
    EXPECT_EQ( bridge.pending_swipe(),   0u );

    //	Second drains return empty -- the contract every consumer relies on.
    EXPECT_TRUE( bridge.drain_scroll_events ().empty() );
    EXPECT_TRUE( bridge.drain_magnify_events().empty() );
    EXPECT_TRUE( bridge.drain_rotate_events ().empty() );
    EXPECT_TRUE( bridge.drain_swipe_events  ().empty() );
}

TEST( GestureBridge, MomentumPhaseFlagged )
{
    aaa::input::GestureBridge bridge;
    bridge.post_scroll_event( 0.0,  10.0, /*momentum=*/false );
    bridge.post_scroll_event( 0.0,   5.0, /*momentum=*/true  );
    bridge.post_scroll_event( 0.0,   2.5, /*momentum=*/true  );
    bridge.post_scroll_event( 0.0,  -1.0, /*momentum=*/false );

    std::vector< aaa::input::ScrollEvent > const events =
        bridge.drain_scroll_events();
    ASSERT_EQ( events.size(), 4u );
    EXPECT_FALSE( events[ 0 ].momentum_phase );
    EXPECT_TRUE ( events[ 1 ].momentum_phase );
    EXPECT_TRUE ( events[ 2 ].momentum_phase );
    EXPECT_FALSE( events[ 3 ].momentum_phase );
}
