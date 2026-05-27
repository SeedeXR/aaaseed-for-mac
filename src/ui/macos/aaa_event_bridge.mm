// src/ui/macos/aaa_event_bridge.mm
//
// Phase 4 input plumbing : EventBridge implementation. See header for the
// design narrative. This TU is ObjC++ purely so it can talk to
// `AAASeedInputView` -- the rest of the EventBridge surface is pure C++.
//
// Hermetic doctrine reminder : no o_str, no aaa_mem, no engine link.
// std::vector + Objective-C runtime only.

#import "aaa_event_bridge.h"
#import "AAASeedInputView.h"

#import <Cocoa/Cocoa.h>

namespace aaa
{
namespace input
{

bool EventBridge::next_event( EngineEvent& out )
{
    if( head_ >= events_.size() )
    {
        //	Reset head when the queue drains so subsequent pushes start
        //	from index 0 again (prevents unbounded vector growth).
        if( head_ != 0 )
        {
            events_.clear();
            head_ = 0;
        }
        return false;
    }
    out = events_[ head_++ ];
    return true;
}

void EventBridge::push( EngineEvent const& ev )
{
    events_.push_back( ev );
}

void EventBridge::clear()
{
    events_.clear();
    head_ = 0;
}

//	Drain pass.
//
//	The view's current side-buffer is state-only (pressed-keys NSSet,
//	last-known mouse location, button NSSet, counters). It does NOT
//	keep a per-event stream. Our drain therefore reconstructs an
//	event stream from state changes :
//
//	  - For every NSNumber in `pressedKeys`, emit one KEY_DOWN. The
//	    set is stable across drains, so the next frame's drain still
//	    sees them. That's fine for the consumer pattern : the engine
//	    wants to know "key X is down THIS frame" -- exactly what we
//	    deliver.
//	  - Mouse buttons same shape : one MOUSE_DOWN per held button.
//	  - The view's `lastMouseLocation` becomes one MOUSE_MOVE when
//	    non-zero. (NSZeroPoint = no mouse event seen yet.)
//	  - Scroll : `lastScrollDelta` becomes one SCROLL event when non-zero,
//	    then is reset to zero so we don't replay it next frame.
//	  - `lastKeyCode` becomes the head of the KEY_DOWN stream (consumers
//	    that only want "most recent" can take the last event). Reset to -1
//	    after drain so the next frame sees fresh input.
//
//	When AAASeedInputView grows a real event-stream buffer (separate
//	work item ; will piggyback on the proper c_event_keyboard adapter
//	port), this body collapses to a queue splice.
std::size_t EventBridge::drain_from_view( AAASeedInputView* view )
{
    if( view == nil )
    {
        return 0;
    }

    std::size_t const before = events_.size();

    //	Keys : one KEY_DOWN per held key. Order is NSSet iteration order
    //	(unspecified), which is fine -- consumers don't rely on order
    //	for held-key semantics.
    NSSet< NSNumber* >* const keys = view.pressedKeys;
    for( NSNumber* k in keys )
    {
        EngineEvent ev;
        ev.kind     = EngineEvent::Kind::KEY_DOWN;
        ev.key_code = (std::int32_t) [k integerValue];
        events_.push_back( ev );
    }

    //	Most-recent keyDown that happened since last drain. lastKeyCode = -1
    //	means "nothing new". We only emit when it's not already represented
    //	by the pressedKeys pass above (rare race-free case : key pressed
    //	and released between drains -- pressedKeys empty, lastKeyCode set).
    NSInteger const last = view.lastKeyCode;
    if( last >= 0 )
    {
        bool already = false;
        for( NSNumber* k in keys )
        {
            if( [k integerValue] == last ) { already = true; break; }
        }
        if( ! already )
        {
            EngineEvent ev;
            ev.kind     = EngineEvent::Kind::KEY_DOWN;
            ev.key_code = (std::int32_t) last;
            events_.push_back( ev );
        }
        view.lastKeyCode = -1;
    }

    //	Mouse buttons : one MOUSE_DOWN per held button.
    NSSet< NSNumber* >* const buttons = view.mouseButtonsPressed;
    NSPoint const             loc     = view.lastMouseLocation;
    for( NSNumber* b in buttons )
    {
        EngineEvent ev;
        ev.kind = EngineEvent::Kind::MOUSE_DOWN;
        //	Map AppKit event type → button index : left=0 right=1 other=2.
        NSEventType const t = (NSEventType) [b unsignedIntegerValue];
        if(      t == NSEventTypeLeftMouseDown  ) { ev.button = 0; }
        else if( t == NSEventTypeRightMouseDown ) { ev.button = 1; }
        else                                      { ev.button = 2; }
        ev.x = loc.x;
        ev.y = loc.y;
        events_.push_back( ev );
    }

    //	Mouse move : emit when last location is non-origin. Cheap to
    //	always emit when buttons-pressed is non-empty too, but the
    //	non-zero gate keeps the no-input case truly empty.
    if( loc.x != 0.0 || loc.y != 0.0 )
    {
        EngineEvent ev;
        ev.kind = EngineEvent::Kind::MOUSE_MOVE;
        ev.x    = loc.x;
        ev.y    = loc.y;
        events_.push_back( ev );
    }

    //	Scroll : emit + reset when non-zero.
    NSPoint const scroll = view.lastScrollDelta;
    if( scroll.x != 0.0 || scroll.y != 0.0 )
    {
        EngineEvent ev;
        ev.kind = EngineEvent::Kind::SCROLL;
        ev.dx   = scroll.x;
        ev.dy   = scroll.y;
        events_.push_back( ev );
        view.lastScrollDelta = NSZeroPoint;
    }

    return events_.size() - before;
}

}   //	namespace input
}   //	namespace aaa
