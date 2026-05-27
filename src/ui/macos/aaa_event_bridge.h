// src/ui/macos/aaa_event_bridge.h
//
// Phase 4 input plumbing : engine-side `c_event_keyboard` + `c_event_mouse`
// adapters never landed on Mac (todo.md L214/217). `AAASeedInputView.{h,mm}`
// collects NSEvents into a side-buffer but nothing drains it.
//
// This sub-lib is the smallest unblock (c113 strategic audit). It owns a
// pure-CPU, Mac-only `aaa::input::EventBridge` that :
//
//   1. Drains the NSEvent side-buffer of an `AAASeedInputView*` into a
//      private std::vector< EngineEvent > FIFO.
//   2. Lets a consumer pop one EngineEvent at a time via `next_event(out)`.
//
// `EngineEvent` is a Mac-side POD struct -- NOT the vendor `c_event_keyboard`
// C++ type. The vendor type inherits from `c_event` (virtual methods +
// dispatch via `process_low()` + global event queue) and porting that
// requires the full engine event-queue subsystem to land on Mac. That work
// is deferred ; this POD shape gives Phase 3 golden MEU bring-up something
// to feed off NOW.
//
// Hermetic Mac sub-lib doctrine (c104 / feedback_hermetic_mac_sublibs.md) :
//   - Pure CPU. No Metal touch. No GPU resources.
//   - std::vector / std::string only. No `o_str`, no `aaa_mem`, no `c_cpu`.
//   - Single-threaded model. Drain happens on the main thread per frame.
//   - No link to the vendor engine. No vendor edits. No err.h / aaa_type.h
//     touch.
//
// Wiring lives in `src/ui/macos/CMakeLists.txt` as a new static sub-lib
// `aaaseed_event_bridge`. Test target `aaaseed_event_bridge_tests` lives
// in `tests/unit/aaa_event_bridge_test.mm`.

#pragma once

#include <cstdint>
#include <vector>

#ifdef __OBJC__
@class AAASeedInputView;
#else
//	C++-only TUs can include this header for the EngineEvent POD ; the
//	drain entry point is gated behind ObjC++ via forward decl.
struct AAASeedInputView;
#endif

namespace aaa
{
namespace input
{

//	The Mac-side engine-event POD. Single union'd payload keeps the type
//	cache-friendly + trivially copyable (vector resize is a memcpy).
//	When the vendor `c_event_keyboard`/`c_event_mouse` port lands, an
//	adapter will translate EngineEvent -> vendor type.
struct EngineEvent
{
    enum class Kind : std::uint8_t
    {
        KEY_DOWN,
        KEY_UP,
        MOUSE_DOWN,
        MOUSE_UP,
        MOUSE_MOVE,
        SCROLL,
    };

    Kind kind { Kind::KEY_DOWN };

    //	Payload : disjoint use depending on `kind`. Not a real C++ union
    //	to keep aggregate-init + designated-init friendly for tests. The
    //	~24 bytes overhead is irrelevant at the event volumes we see
    //	(a few dozen per frame at most).
    std::int32_t key_code { 0 };    //	KEY_DOWN / KEY_UP
    std::int32_t button   { 0 };    //	MOUSE_DOWN / MOUSE_UP : 0=left 1=right 2=other
    double       x        { 0.0 };  //	MOUSE_DOWN / MOUSE_UP / MOUSE_MOVE : view-local coords
    double       y        { 0.0 };
    double       dx       { 0.0 };  //	SCROLL : delta X
    double       dy       { 0.0 };  //	SCROLL : delta Y
};

//	EventBridge — drain + consume FIFO.
//
//	Lifecycle :
//	  - Construct once at app boot (or per test).
//	  - Per frame on main thread : `drain_from_view(view)` to harvest
//	    accumulated NSEvents into the internal queue, then loop
//	    `while( next_event(ev) ) { ... }` to feed consumers.
//
//	Drain semantics :
//	  - The current AAASeedInputView surface is state-based (pressed-keys
//	    set, last code, counters) not event-stream. This first drain
//	    iteration emits one synthetic EngineEvent per key currently in
//	    `pressedKeys` (KEY_DOWN) + the last mouse location (MOUSE_MOVE
//	    if non-zero) + one event per held mouse button (MOUSE_DOWN).
//	    After draining, the view's accumulator counters are reset so the
//	    next frame's drain sees only new activity.
//	  - This is a stepping-stone semantic. When AAASeedInputView grows a
//	    proper event-stream buffer, EventBridge::drain_from_view simply
//	    pops from that buffer instead of synthesizing from state.
class EventBridge
{
public:
    EventBridge() = default;
    ~EventBridge() = default;

    EventBridge( EventBridge const& )            = delete;
    EventBridge& operator=( EventBridge const& ) = delete;

#ifdef __OBJC__
    //	Drain the view's NSEvent side-buffer into the internal FIFO.
    //	No-op (and safe) when `view` is nil. Returns the number of
    //	events appended.
    std::size_t drain_from_view( AAASeedInputView* view );
#endif

    //	Consumer API : pops the oldest queued EngineEvent into `out` and
    //	returns true ; returns false (and leaves `out` untouched) when
    //	the queue is empty.
    bool next_event( EngineEvent& out );

    //	Direct push for tests + future internal use. Not part of the
    //	hot-path consumer API.
    void push( EngineEvent const& ev );

    //	Number of EngineEvents currently waiting to be consumed.
    std::size_t pending() const { return events_.size() - head_; }

    //	Drop every queued event. Mainly for test setup/teardown.
    void clear();

private:
    //	std::vector + head-index FIFO. Cheap : drain typically yields
    //	< 32 events per frame, consumer drains immediately. No need for
    //	a deque or a ring buffer at this scale.
    std::vector< EngineEvent > events_ {};
    std::size_t                head_   { 0 };
};

}   //	namespace input
}   //	namespace aaa
