// src/display/display_mac.h
//
// second_todo.md S8 (c153) : live macOS multi-display support. Two pieces :
//
//   1. enumerate_screens() -- the connected NSScreens as display_layout Rects
//      (Cocoa bottom-left points). CI-safe : returns >=1 on any machine with
//      a display, [] on a truly headless runner.
//
//   2. class MultiDisplay -- creates one borderless NSWindow per NON-primary
//      screen, each backed by a CAMetalLayer, so the engine can present each
//      display's sub-rect of a shared render (the fullscreen-span feature
//      from the upstream MultiWindow_mac). The windows join all Spaces and
//      are screen-saver-level so they cover the whole display.
//
// Scope honesty : this sub-lib provides the native building blocks (screen
// geometry + aux borderless windows + per-display CAMetalLayers). WIRING the
// engine's present loop to draw each aux layer's sub-rect is host
// integration that touches src/ui/macos and is tracked separately in
// second_todo.md S8 -- it is NOT claimed done here.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). std:: + AppKit + QuartzCore (CAMetalLayer) only.
//     No engine link, no o_str, no aaa_mem. Manual reference counting.
//   - C++-clean header (PIMPL + display_layout Rect) ; no ObjC type crosses
//     the boundary. The CAMetalLayer is handed back as an opaque void*.
//
// Testability : the geometry is in display_layout.h (pure, fully tested).
// enumerate_screens() is tested for non-crash + >=1 when a display exists.
// MultiDisplay window creation needs a WindowServer connection, so its test
// is gated (>=2 screens AND a usable GUI session) and GTEST_SKIP'd in CI.

#pragma once

#include "src/display/display_layout.h"

#include <cstddef>
#include <vector>

namespace aaa
{
namespace display
{

//	Connected screens as Cocoa-frame Rects (bottom-left origin, points). The
//	primary screen (menu-bar / global origin) can be found with
//	primary_index() from display_layout.h. Empty on a headless runner.
std::vector<Rect> enumerate_screens();

//	MultiDisplay -- borderless aux windows for every non-primary screen.
class MultiDisplay
{
public:
    MultiDisplay();
    ~MultiDisplay();

    MultiDisplay( MultiDisplay const & )             = delete;
    MultiDisplay & operator=( MultiDisplay const & ) = delete;

    //	Create a borderless, all-Spaces, screen-saver-level NSWindow backed
    //	by a CAMetalLayer for each NON-primary screen. Returns the number of
    //	aux windows created (0 if there is only one screen). Must be called
    //	on the main thread. Idempotent : a second enable() is a no-op that
    //	returns the current aux count.
    std::size_t enable();

    //	Tear down all aux windows. Safe to call when not enabled.
    void disable();

    bool        is_enabled() const;
    std::size_t aux_count() const;

    //	The CAMetalLayer for aux window `index` (0-based), as an opaque
    //	void* (cast to CAMetalLayer* / id<MTLDrawable> source on the ObjC
    //	side). nullptr if index is out of range.
    void * aux_metal_layer( std::size_t index ) const;

    //	The screen rect (Cocoa frame) backing aux window `index`. Zero rect
    //	if out of range.
    Rect aux_rect( std::size_t index ) const;

private:
    struct Impl;
    Impl * impl_;
};

}   //	namespace display
}   //	namespace aaa
