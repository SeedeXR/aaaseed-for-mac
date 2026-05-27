// src/ui/macos/aaa_event_bridge_gesture.h
//
// Phase 4 trackpad / gesture adapter -- the c119-B EventBridge sibling
// for NSEventTypeScrollWheel / NSEventTypeMagnify / NSEventTypeRotate /
// NSEventTypeSwipe. Companion to `aaa_event_bridge.{h,mm}` (keyboard /
// mouse drain) ; lives in a separate TU because the gesture surface is
// stream-shaped per gesture family rather than single-FIFO state-shaped,
// and the per-family drain API does not fit c119-B's tagged-union queue.
//
// Doctrine reminder (feedback_hermetic_mac_sublibs.md) :
//   - Pure CPU. No Metal touch. No GPU resources.
//   - std::vector / std::uint64_t only. NO o_str, NO aaa_str.h,
//     NO aaa_mem.h. NO link to aaaseed_code_utils.
//   - Single-threaded model. Drain happens on the main thread per frame.
//   - ObjC++ entry points gated behind `#if defined(__APPLE__) && __OBJC__`
//     so plain C++ TUs can include the POD payloads.
//
// API shape : one queue per gesture family. Each `drain_*` returns a
// std::vector< T > of the events accumulated since the previous drain ;
// the queue is reset on every drain (FIFO semantics, two drains in a row
// return empty for the second). `post_*` lets a test harness synthesize
// events without manufacturing NSEvents. The `handle_ns_event` entry
// point (ObjC++-only) dispatches a real NSEvent into the matching queue
// via the NSEventType mapping documented in the .mm.
//
// When the engine's gesture event subsystem lands on Mac, an adapter
// will translate these POD events into the vendor types.

#pragma once

#include <cstdint>
#include <vector>

#if defined(__APPLE__) && defined(__OBJC__)
@class NSEvent;
#endif

namespace aaa
{
namespace input
{

//	One ScrollEvent per NSEventTypeScrollWheel dispatch. `dx` / `dy` are
//	the NSEvent `scrollingDeltaX` / `scrollingDeltaY` values directly.
//	`momentum_phase` is true when AppKit reports a non-None momentum
//	phase (i.e. the OS-driven inertial scroll tail) ; consumers can use
//	this to distinguish "real" finger motion from inertia.
struct ScrollEvent
{
    double        dx              { 0.0 };
    double        dy              { 0.0 };
    bool          momentum_phase  { false };
    std::uint64_t timestamp_ns    { 0 };
};

//	One MagnifyEvent per NSEventTypeMagnify dispatch. `delta` mirrors
//	NSEvent `magnification` (a delta, not absolute scale). `cumulative`
//	is the running sum across all magnify events posted into this bridge
//	since construction or the last `reset_cumulative*` call.
struct MagnifyEvent
{
    double        delta           { 0.0 };
    double        cumulative      { 0.0 };
    std::uint64_t timestamp_ns    { 0 };
};

//	One RotateEvent per NSEventTypeRotate dispatch. NSEvent reports
//	rotation in DEGREES ; we convert to radians at ingest so the engine
//	side never sees the unit mismatch. `cumulative_rad` is the running
//	sum since construction / reset.
struct RotateEvent
{
    double        delta_rad       { 0.0 };
    double        cumulative_rad  { 0.0 };
    std::uint64_t timestamp_ns    { 0 };
};

//	One SwipeEvent per NSEventTypeSwipe dispatch. Direction encoding :
//	  -1 = left      (deltaX > 0 in AppKit convention)
//	  +1 = right     (deltaX < 0)
//	  +2 = up        (deltaY > 0)
//	  -2 = down      (deltaY < 0)
//	(AppKit's swipe-deltaX is +1 when the user swipes LEFT because the
//	content moves right ; we invert at ingest to make consumer intent
//	obvious.)
struct SwipeEvent
{
    int           direction       { 0 };
    std::uint64_t timestamp_ns    { 0 };
};

//	GestureBridge -- four parallel FIFO queues, one per gesture family.
//
//	Lifecycle :
//	  - Construct once at app boot (or per test).
//	  - On NSEvent arrival : either `handle_ns_event(evt)` (real path)
//	    or `post_*(...)` (test harness / synthetic path).
//	  - Per frame : `drain_*_events()` returns a vector of accumulated
//	    events and clears the corresponding queue.
//
//	The four queues are independent : `drain_scroll_events` does NOT
//	touch the magnify / rotate / swipe queues. This matches how engine
//	consumers will typically subscribe to one gesture family at a time.
class GestureBridge
{
public:
    GestureBridge() = default;
    ~GestureBridge() = default;

    GestureBridge( GestureBridge const& )            = delete;
    GestureBridge& operator=( GestureBridge const& ) = delete;

    //	Drain APIs -- return all queued events of the named family and
    //	clear that queue. Returning by value gives the consumer a move-
    //	from'd vector ; cheap at the gesture-rate volumes we see
    //	(< 60 per second per family in practice).
    std::vector< ScrollEvent  > drain_scroll_events();
    std::vector< MagnifyEvent > drain_magnify_events();
    std::vector< RotateEvent  > drain_rotate_events();
    std::vector< SwipeEvent   > drain_swipe_events();

    //	Synthetic posts -- used by the test harness and any future
    //	non-AppKit synth path. The `timestamp_ns` arg defaults to 0
    //	which matches the test fixtures ; production paths fill it.
    void post_scroll_event ( double dx, double dy, bool momentum,
                             std::uint64_t timestamp_ns = 0 );
    void post_magnify_event( double delta,
                             std::uint64_t timestamp_ns = 0 );
    void post_rotate_event ( double delta_rad,
                             std::uint64_t timestamp_ns = 0 );
    void post_swipe_event  ( int direction,
                             std::uint64_t timestamp_ns = 0 );

    //	Real-path ingest. Dispatches an NSEvent into the matching queue
    //	based on `[event type]`. Returns true if the event was a
    //	recognised gesture (and queued) ; false otherwise (caller is
    //	free to forward unhandled events to the next responder). ObjC++
    //	gating so plain C++ TUs can include this header.
#if defined(__APPLE__) && defined(__OBJC__)
    bool handle_ns_event( NSEvent* event );
#endif

    //	Observability for tests / engine-side diagnostics.
    std::size_t pending_scroll()  const { return scroll_.size();  }
    std::size_t pending_magnify() const { return magnify_.size(); }
    std::size_t pending_rotate()  const { return rotate_.size();  }
    std::size_t pending_swipe()   const { return swipe_.size();   }

    //	Cumulative-state resets -- mainly for tests that want to start
    //	from a known baseline.
    void reset_cumulative_magnify() { cumulative_magnify_     = 0.0; }
    void reset_cumulative_rotate () { cumulative_rotate_rad_  = 0.0; }

    //	Drop every queued event in every family. Mainly for test setup
    //	/ teardown ; does not touch cumulative state.
    void clear();

    //	Degree -> radian conversion used by `handle_ns_event` when
    //	ingesting NSEventTypeRotate (NSEvent reports DEGREES). Exposed
    //	as a static utility so the regression test for the conversion
    //	does not need to synthesize an NSEventTypeRotate event (AppKit
    //	throws when `+mouseEventWithType:` is called with rotate /
    //	magnify / scroll types ; same caveat as c119-B's scroll test).
    static double degrees_to_radians( double deg );

private:
    std::vector< ScrollEvent  > scroll_                 {};
    std::vector< MagnifyEvent > magnify_                {};
    std::vector< RotateEvent  > rotate_                 {};
    std::vector< SwipeEvent   > swipe_                  {};

    //	Running totals -- updated on every post / handle ; copied into
    //	the corresponding event's `cumulative*` field at queue time.
    double                      cumulative_magnify_     { 0.0 };
    double                      cumulative_rotate_rad_  { 0.0 };
};

}   //	namespace input
}   //	namespace aaa
