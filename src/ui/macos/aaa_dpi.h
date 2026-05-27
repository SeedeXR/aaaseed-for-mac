// src/ui/macos/aaa_dpi.h
//
// Phase 4 (todo.md L298) : DPI adapter. Replaces the Win32
// `GetDpiForMonitor` path (vendor/aaaseed-engine/Src/platform/win32/
// touch_windows.h) with `[NSScreen backingScaleFactor]` + multi-display
// enumeration + point<->pixel conversion helpers. AppKit's coordinate
// space is point-based (1pt = 1px on non-Retina ; 1pt = 2px on Retina) ;
// the engine traditionally thinks in pixels, so this layer is where
// the units are pinned.
//
// This header exposes a pure-C++ API (no Objective-C types) so C++-only
// TUs can include it freely. The per-window helper is gated under
// `__OBJC__` to keep the `NSWindow*` forward declaration safe : in an
// Objective-C++ TU the real `@class NSWindow` is used ; in a pure C++
// TU the parameter degrades to `void*` so the symbol still links (the
// caller will pass `nullptr` or marshalled through a void* anyway).
//
// Hermetic Mac sub-lib doctrine (c104 / feedback_hermetic_mac_sublibs.md) :
//   - Pure CPU. No Metal touch.
//   - std::string / std::vector only. No `o_str`, no `aaa_mem.h`,
//     no `aaa_str.h`, no `aaa_type.h`.
//   - No vendor edits. No link to `aaaseed_code_utils`.
//   - Foundation + AppKit only (impl in aaa_dpi.mm).
//
// Wiring lives in `src/ui/macos/CMakeLists.txt` as a new static lib
// `aaaseed_ui_dpi_mac`. Tests in `tests/unit/ui_dpi_test.cpp`.

#pragma once

#include <string>
#include <vector>

namespace aaa
{
namespace ui
{
namespace dpi
{

//	One screen's identity + scaling info. `index` is the position in
//	`[NSScreen screens]` (0 = main screen by convention -- main_screen()
//	always returns index 0). `scale_factor` is `backingScaleFactor` :
//	1.0 on non-Retina, 2.0 on Retina. Points = AppKit / Cocoa units ;
//	pixels = backing-store units (what Metal samples / what the engine
//	historically computes against).
struct ScreenInfo
{
    int             index;             //	0 = main screen
    double          scale_factor;      //	backingScaleFactor (1.0 or 2.0)
    int             width_points;
    int             height_points;
    int             width_pixels;      //	= width_points * scale_factor
    int             height_pixels;
    std::string     localized_name;    //	NSScreen.localizedName (macOS 10.15+)
};

//	Main display info. Falls back to a degenerate { 0, 1.0, 0, 0, 0, 0,
//	"" } record when no screen is attached (headless / SSH session).
ScreenInfo main_screen();

//	All attached screens in `[NSScreen screens]` order, indices 0..N-1.
//	Empty vector when no screen is attached. NSScreen.screens[0] is
//	NOT necessarily NSScreen.mainScreen (main = the screen with the
//	key window) ; we report `[NSScreen screens]` order so the indices
//	match the AppKit enumeration callers can already see.
std::vector< ScreenInfo > all_screens();

//	Convenience : just the main screen's backingScaleFactor. Returns
//	1.0 when no screen is attached.
double main_scale_factor();

//	Point<->pixel conversion using the main screen's scale factor.
//	These are simple multipliers ; provided as named helpers so call
//	sites read intent-fully rather than sprinkling `* scale_factor`
//	expressions around. `points_to_pixels(0.0)` returns exactly 0.0.
double points_to_pixels( double points );
double pixels_to_points( double pixels );

//	Per-window scale lookup. The window's backing scale follows the
//	display it's currently sitting on -- different monitors with
//	different DPIs are common on modern Macs (Retina laptop + 1x
//	external monitor, for example). When `window` is nullptr, or the
//	window has not yet been placed on a screen (newly created, pre-
//	orderFront), this falls back to the main screen's scale so
//	callers always get a usable number.
//
//	The parameter is typed `void*` rather than `NSWindow*` so the
//	header stays Objective-C-free (the no-ObjC-in-header contract
//	of c104). Objective-C++ callers may pass an `NSWindow*` directly
//	-- it converts implicitly to `void*`. C++ callers either pass
//	`nullptr` (and get the main scale fallback) or marshal through
//	`void*` from an opaque handle. Same ABI for both paths so there
//	is no name-mangling surprise across .mm / .cpp consumers.
double scale_factor_for_window( void* window );

}   //	namespace dpi
}   //	namespace ui
}   //	namespace aaa
