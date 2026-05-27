// src/ui/macos/aaa_event_adapter_mac.h
//
// Phase 4 / v1 ship-gate (todo.md L29) : engine-side `c_event_mouse` +
// `c_event_keyboard` adapter. Closes the LAST open Phase 4 bullet.
//
// The Mac event bridges already exist :
//   - c119-B  : aaa::input::EventBridge        (keyboard + mouse drain)
//   - c132-B  : aaa::input::GestureBridge      (trackpad / scroll / pinch)
//   - c133-A  : AAASeedInputView wiring        (NSEvent -> bridges)
//
// What this file adds : the glue that converts the Mac-side
// `EngineEvent` POD (kept in the c119-B FIFO) into the engine-shaped
// POD that the vendor `c_event_mouse` / `c_event_keyboard` factories
// consume. We deliberately do NOT touch the vendor factory itself --
// the vendor c_event_* types inherit from c_event with virtual dispatch
// + a global event queue ; porting that requires the full event-queue
// subsystem to land on Mac (deferred). This shim is a value-shape
// adapter : pure POD in, pure POD out, no engine link.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - std::vector / std::uint64_t only. NO `o_str`, NO `aaa_str.h`,
//     NO `aaa_mem.h`. NO link to `aaaseed_code_utils`.
//   - Single-threaded model. Drain happens on the main thread per frame.
//   - Pure C++ API. NO ObjC types in the namespace surface (c134-A
//     bridge doctrine -- use `void*` if a future variant needs an
//     NSEvent direct path).
//
// Wiring : extends the existing `aaaseed_event_bridge` STATIC target
// in src/ui/macos/CMakeLists.txt with this TU + its companion .mm
// (kept .mm for future NSEvent direct-path flexibility, even though
// the current body is pure C++). Test target
// `aaaseed_event_adapter_mac_tests` in tests/unit/CMakeLists.txt.

#pragma once

#include <cstdint>
#include <vector>

#include "src/ui/macos/aaa_event_bridge.h"

namespace aaa
{
namespace input
{

//	Modifier bitfield. Mirrors the NSEvent modifierFlags shape (one
//	bit per logical key) so the encoding is portable to the eventual
//	vendor adapter (which today uses an INT32 modifier bag too).
//	Constants are inline constexpr so plain-C++ test TUs can use them
//	in static_assert.
constexpr int kModShift = 1 << 0;
constexpr int kModCtrl  = 1 << 1;
constexpr int kModAlt   = 1 << 2;
constexpr int kModCmd   = 1 << 3;

//	Mouse event POD shaped for c_event_mouse consumption. The engine's
//	c_event_mouse::set_event_button takes (x,y, BUTTON, STATE) and
//	set_event_move takes (x,y, b_passive) ; we keep BOTH payloads in a
//	single tagged struct so a consumer can dispatch on `type` :
//	  0=move 1=down 2=up 3=drag 4=scroll
//	`button` matches the vendor mouse::BUTTON convention (0=left,
//	1=right, 2=other). `scroll_dx`/`scroll_dy` are NSEvent
//	scrollingDeltaX/scrollingDeltaY (points / line, AppKit-native).
//	`timestamp_ns` is the NSEvent absolute timestamp in nanoseconds ;
//	0 is reserved for "synthesized / no timestamp" (consumer can
//	detect that by checking `timestamp_ns != 0`).
//	`modifiers` is the `kMod*` bitfield (see above).
struct MouseEventOut
{
    int       type         { 0 };   //	0=move 1=down 2=up 3=drag 4=scroll
    int       button       { 0 };   //	0=left 1=right 2=other
    double    x            { 0.0 };
    double    y            { 0.0 };
    double    scroll_dx    { 0.0 };
    double    scroll_dy    { 0.0 };
    std::uint64_t timestamp_ns { 0 };
    int       modifiers    { 0 };   //	kModShift | kModCtrl | kModAlt | kModCmd
};

//	Keyboard event POD shaped for c_event_keyboard consumption. The
//	vendor c_event_keyboard::set_event takes (key, modifiers, x, y) ;
//	we keep (x,y) implicit at "last mouse location" (consumer fills
//	from its own state) -- our drain stream doesn't have per-key
//	cursor coords. `type` : 0=key_down 1=key_up. `key_code` is the
//	Apple HID virtual-key code as delivered by NSEvent.keyCode.
struct KeyboardEventOut
{
    int       type         { 0 };   //	0=key_down 1=key_up
    int       key_code     { 0 };   //	Apple HID virtual key code
    int       modifiers    { 0 };   //	kMod* bitfield
    std::uint64_t timestamp_ns { 0 };
};

//	Pure conversion -- (x,y, button, type_code, modifiers, timestamp)
//	into a MouseEventOut. No bridge state touched. Used by drain_*
//	and by tests directly.
MouseEventOut mouse_event_to_engine( double x, double y, int button,
                                     int type_code, int modifiers,
                                     std::uint64_t timestamp_ns );

//	Pure conversion -- (key_code, down, modifiers, timestamp) into a
//	KeyboardEventOut. `down=true` -> type=0 (key_down), `down=false`
//	-> type=1 (key_up).
KeyboardEventOut keyboard_event_to_engine( int key_code, bool down,
                                           int modifiers,
                                           std::uint64_t timestamp_ns );

//	Drain an EventBridge of its accumulated EngineEvents and translate
//	the mouse-class entries (MOUSE_DOWN / MOUSE_UP / MOUSE_MOVE /
//	SCROLL) into a vector of MouseEventOut. The bridge is FULLY
//	drained (every queued EngineEvent is popped) -- keyboard-class
//	entries observed in the same pass are forwarded to a companion
//	`drain_to_engine_keyboard`-style sink IF the consumer calls them
//	back-to-back ; otherwise keyboard events are dropped. Per the c119-B
//	doctrine the bridge is single-threaded + single-consumer, so the
//	consumer is expected to call BOTH drains every frame before any
//	new push.
//	Returns an empty vector when the bridge is empty.
std::vector< MouseEventOut > drain_to_engine_mouse( EventBridge& bridge );

//	Companion drain for keyboard. Same semantics as drain_to_engine_mouse
//	but the mouse-class entries are silently dropped. Consumers that
//	care about both should call drain_to_engine_mouse FIRST (to harvest
//	mouse entries before they're lost) and drain_to_engine_keyboard
//	SECOND ; the bridge handles each call independently.
std::vector< KeyboardEventOut > drain_to_engine_keyboard( EventBridge& bridge );

}   //	namespace input
}   //	namespace aaa
