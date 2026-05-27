// AAASeedInputView.h
//
// MTKView subclass that accepts keyboard focus and dispatches keyDown:
// / keyUp: events. First Phase 4 input step (continuation 36) -- gives
// the AAASeed Mac binary something to respond to. The state surface
// here is intentionally small : just the last-pressed key code + a
// pressed-key set. The eventual engine c_event_keyboard plumbing will
// consume this surface or replace it.
//
// Stock MTKView does NOT capture key events because it doesn't return
// YES from `acceptsFirstResponder`. Subclassing is the canonical fix.
// We keep MTKView's rendering machinery intact ; this subclass only
// adds input handling on the side.
//
// Doctrine reminder : Mac-primary, Windows-reciprocal. The engine's
// c_event_keyboard system runs on Windows via the Win32 message pump.
// On Mac, NSEvent -> NSResponder -> this class -> c_event_keyboard
// (when wired). Today the wiring stops at this class.
//
// c133 / Phase 4 view-side wiring (todo L290/L291 follow-up) : the
// view now OWNS one `aaa::input::EventBridge` (keyboard + mouse FIFO,
// c119-B) and one `aaa::input::GestureBridge` (scroll/magnify/rotate/
// swipe, c132-B). Each NSResponder override pushes the NSEvent into
// the matching bridge BEFORE the existing local-buffer side-effects.
// Tests/consumers reach the bridges via the `eventBridge` /
// `gestureBridge` accessors. Per-view instances ; no global state
// (feedback_hermetic_mac_sublibs.md).

#pragma once

#import <MetalKit/MetalKit.h>

#ifdef __cplusplus
namespace aaa { namespace input { class EventBridge; class GestureBridge; } }
#endif

//	c150-A v4 : AAASeedInputView conforms to NSTextInputClient so the
//	macOS Text Input System routes IME composition + commit events
//	through this view to the WidgetSystem's text_input + text_area
//	widgets. All 11 protocol methods are implemented in the .mm.
@interface AAASeedInputView : MTKView< NSTextInputClient >

//	The most recent key code received via keyDown:. -1 when no key has
//	been pressed since startup. Read by the render delegate to see if
//	user input has changed render state. Reset to -1 by setting it back.
@property( nonatomic, assign ) NSInteger lastKeyCode;

//	Set of keys currently held down. Each entry is the NSEvent.keyCode
//	as @( NSUInteger ). Updated by keyDown: / keyUp:. Engine event
//	systems can poll this for held-key semantics if needed.
@property( nonatomic, readonly, copy ) NSSet< NSNumber* >* pressedKeys;

//	Total key events received since the view was created. Used by tests
//	to confirm event dispatch is actually firing.
@property( nonatomic, readonly, assign ) NSInteger keyEventCount;

//	--- Mouse / scroll surface (continuation 37) -----------------------
//	Last reported pointer location in the view's local coordinates.
//	Origin = bottom-left (NSView convention). NSZeroPoint until the
//	first mouse event.
@property( nonatomic, readonly, assign ) NSPoint lastMouseLocation;

//	Currently-held mouse buttons. Entries are @( NSEventType ) values
//	for the down-events seen (NSEventTypeLeftMouseDown,
//	NSEventTypeRightMouseDown, NSEventTypeOtherMouseDown). Lifted on
//	the matching up-event.
@property( nonatomic, readonly, copy ) NSSet< NSNumber* >* mouseButtonsPressed;

//	Cumulative mouse / scroll event count for test observability.
@property( nonatomic, readonly, assign ) NSInteger mouseEventCount;

//	Most recent scrollWheel delta. (deltaX, deltaY) ; pixels on
//	high-precision devices, lines otherwise. Tests can reset by
//	consuming and reading back.
@property( nonatomic, assign ) NSPoint lastScrollDelta;

//	--- Bridge accessors (c133 view-side wiring) -----------------------
//	Raw pointers into per-view owned bridge instances. Lifetime is tied
//	to the view (created in init, destroyed in dealloc). Returning raw
//	avoids exposing the std::unique_ptr template across the ObjC
//	property boundary. Tests reach bridges through these.
#ifdef __cplusplus
- (aaa::input::EventBridge*)  eventBridge;
- (aaa::input::GestureBridge*)gestureBridge;
#endif

@end
