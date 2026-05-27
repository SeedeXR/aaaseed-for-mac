// src/ui/macos/aaa_event_bridge_gesture.mm
//
// Phase 4 trackpad / gesture adapter implementation. See header for the
// design narrative. This TU is ObjC++ purely so it can talk to NSEvent ;
// the rest of the GestureBridge surface is pure C++.
//
// Hermetic doctrine reminder : no o_str, no aaa_mem.h, no aaa_str.h, no
// engine link. std::vector + Foundation + AppKit only.
//
// NSEvent -> queue mapping table (real-path ingest in handle_ns_event) :
//
//   NSEvent type             |  queue    |  field extraction
//   -------------------------+-----------+----------------------------------
//   NSEventTypeScrollWheel   |  scroll   |  dx = scrollingDeltaX
//                            |           |  dy = scrollingDeltaY
//                            |           |  momentum = (momentumPhase != 0)
//   NSEventTypeMagnify       |  magnify  |  delta = magnification (NSEvent
//                            |           |          reports scale delta)
//   NSEventTypeRotate        |  rotate   |  delta_rad = rotation * pi / 180
//                            |           |  (NSEvent reports DEGREES)
//   NSEventTypeSwipe         |  swipe    |  direction encoded from deltaX /
//                            |           |  deltaY signs (AppKit content-
//                            |           |  direction is inverted vs finger
//                            |           |  direction ; we invert)

#if defined(__APPLE__)

#import "aaa_event_bridge_gesture.h"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include <cmath>

namespace aaa
{
namespace input
{

namespace
{
    //	Local pi to avoid pulling <numbers> (C++20 std::numbers::pi) and
    //	M_PI from <cmath> on platforms where it lives behind a feature
    //	gate. Same value to ULP precision either way.
    constexpr double kPi = 3.14159265358979323846;
}

double GestureBridge::degrees_to_radians( double deg )
{
    return deg * ( kPi / 180.0 );
}

std::vector< ScrollEvent > GestureBridge::drain_scroll_events()
{
    std::vector< ScrollEvent > out;
    out.swap( scroll_ );
    return out;
}

std::vector< MagnifyEvent > GestureBridge::drain_magnify_events()
{
    std::vector< MagnifyEvent > out;
    out.swap( magnify_ );
    return out;
}

std::vector< RotateEvent > GestureBridge::drain_rotate_events()
{
    std::vector< RotateEvent > out;
    out.swap( rotate_ );
    return out;
}

std::vector< SwipeEvent > GestureBridge::drain_swipe_events()
{
    std::vector< SwipeEvent > out;
    out.swap( swipe_ );
    return out;
}

void GestureBridge::post_scroll_event( double dx, double dy, bool momentum,
                                       std::uint64_t timestamp_ns )
{
    ScrollEvent ev;
    ev.dx              = dx;
    ev.dy              = dy;
    ev.momentum_phase  = momentum;
    ev.timestamp_ns    = timestamp_ns;
    scroll_.push_back( ev );
}

void GestureBridge::post_magnify_event( double delta,
                                        std::uint64_t timestamp_ns )
{
    cumulative_magnify_ += delta;
    MagnifyEvent ev;
    ev.delta         = delta;
    ev.cumulative    = cumulative_magnify_;
    ev.timestamp_ns  = timestamp_ns;
    magnify_.push_back( ev );
}

void GestureBridge::post_rotate_event( double delta_rad,
                                       std::uint64_t timestamp_ns )
{
    cumulative_rotate_rad_ += delta_rad;
    RotateEvent ev;
    ev.delta_rad        = delta_rad;
    ev.cumulative_rad   = cumulative_rotate_rad_;
    ev.timestamp_ns     = timestamp_ns;
    rotate_.push_back( ev );
}

void GestureBridge::post_swipe_event( int direction,
                                      std::uint64_t timestamp_ns )
{
    SwipeEvent ev;
    ev.direction     = direction;
    ev.timestamp_ns  = timestamp_ns;
    swipe_.push_back( ev );
}

bool GestureBridge::handle_ns_event( NSEvent* event )
{
    if( event == nil )
    {
        return false;
    }

    //	NSEvent.timestamp is in seconds since boot (NSTimeInterval =
    //	double). Convert to nanoseconds for the POD payload ; we cap at
    //	uint64 max implicitly via the multiplication overflow window
    //	(seconds * 1e9 fits in uint64 for ~584 years of uptime).
    NSTimeInterval const ts_sec = [event timestamp];
    std::uint64_t  const ts_ns  =
        (std::uint64_t) ( ts_sec * 1.0e9 );

    NSEventType const type = [event type];

    switch( type )
    {
        case NSEventTypeScrollWheel:
        {
            double const dx       = [event scrollingDeltaX];
            double const dy       = [event scrollingDeltaY];
            bool   const momentum =
                ( [event momentumPhase] != NSEventPhaseNone );
            post_scroll_event( dx, dy, momentum, ts_ns );
            return true;
        }

        case NSEventTypeMagnify:
        {
            double const delta = [event magnification];
            post_magnify_event( delta, ts_ns );
            return true;
        }

        case NSEventTypeRotate:
        {
            //	NSEvent.rotation is in DEGREES (CGFloat). Convert at
            //	ingest so consumers always see radians.
            double const deg = (double) [event rotation];
            post_rotate_event( degrees_to_radians( deg ), ts_ns );
            return true;
        }

        case NSEventTypeSwipe:
        {
            //	Swipe events encode direction in deltaX / deltaY signs.
            //	AppKit's deltas are content-motion (i.e. when the finger
            //	swipes left, deltaX is +1 because the content moves
            //	right). Invert so direction matches finger intent.
            double const dx = [event deltaX];
            double const dy = [event deltaY];
            int direction = 0;
            if(      dx > 0.0 ) { direction = -1; }  //	finger left
            else if( dx < 0.0 ) { direction = +1; }  //	finger right
            else if( dy > 0.0 ) { direction = +2; }  //	finger up
            else if( dy < 0.0 ) { direction = -2; }  //	finger down
            post_swipe_event( direction, ts_ns );
            return true;
        }

        default:
            return false;
    }
}

void GestureBridge::clear()
{
    scroll_  .clear();
    magnify_ .clear();
    rotate_  .clear();
    swipe_   .clear();
}

}   //	namespace input
}   //	namespace aaa

#endif  //	__APPLE__
