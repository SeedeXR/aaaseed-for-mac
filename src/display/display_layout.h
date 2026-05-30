// src/display/display_layout.h
//
// second_todo.md S8 (c153) : PURE multi-display geometry core for the
// fullscreen-span feature. The upstream engine author's mac-port
// (MultiWindow_mac) renders once to a shared FBO and blits each display's
// sub-rect ; this header is the framework-free math that decides, for a set
// of screen rects, (a) the virtual bounding canvas that spans them all and
// (b) each screen's normalized sub-rect within that canvas. The live NSScreen
// enumeration + borderless-window manager that CONSUME this live in
// display_mac.{h,mm}.
//
// Header-only + std:: only (mirrors syphon_pixel_util.h) so it is unit-tested
// deterministically without any display hardware.

#pragma once

#include <cstddef>
#include <vector>

namespace aaa
{
namespace display
{

//	A screen / canvas rectangle in a top-left-or-bottom-left agnostic space.
//	The math here is origin-convention-neutral : it only unions and
//	normalizes, so callers may pass Cocoa (bottom-left) frames directly.
struct Rect
{
    double x = 0.0;
    double y = 0.0;
    double w = 0.0;
    double h = 0.0;
};

//	Smallest axis-aligned rectangle containing every screen. Empty input ->
//	all-zero rect. Zero-size screens still contribute their origin.
inline Rect virtual_bounds( std::vector<Rect> const & screens )
{
    if( screens.empty() )
        return Rect{};

    double min_x = screens[ 0 ].x;
    double min_y = screens[ 0 ].y;
    double max_x = screens[ 0 ].x + screens[ 0 ].w;
    double max_y = screens[ 0 ].y + screens[ 0 ].h;

    for( std::size_t i = 1; i < screens.size(); ++i )
    {
        Rect const & s = screens[ i ];
        if( s.x < min_x )         min_x = s.x;
        if( s.y < min_y )         min_y = s.y;
        if( s.x + s.w > max_x )   max_x = s.x + s.w;
        if( s.y + s.h > max_y )   max_y = s.y + s.h;
    }
    return Rect{ min_x, min_y, max_x - min_x, max_y - min_y };
}

//	`screen` expressed as a 0..1 sub-rect of `bounds` (e.g. for sampling the
//	right slice of a shared render target). Degenerate bounds (zero w/h) ->
//	all-zero rect.
inline Rect normalized_subrect( Rect const & screen, Rect const & bounds )
{
    if( bounds.w <= 0.0 || bounds.h <= 0.0 )
        return Rect{};
    return Rect{
        ( screen.x - bounds.x ) / bounds.w,
        ( screen.y - bounds.y ) / bounds.h,
        screen.w / bounds.w,
        screen.h / bounds.h
    };
}

//	Index of the primary screen : the one whose origin is closest to (0,0)
//	(macOS places the menu-bar / primary display at the global origin).
//	Returns 0 for empty input.
inline std::size_t primary_index( std::vector<Rect> const & screens )
{
    if( screens.empty() )
        return 0;
    std::size_t best = 0;
    double      best_d = 1.0e300;
    for( std::size_t i = 0; i < screens.size(); ++i )
    {
        double d = screens[ i ].x * screens[ i ].x
                 + screens[ i ].y * screens[ i ].y;
        if( d < best_d )
        {
            best_d = d;
            best   = i;
        }
    }
    return best;
}

}   //	namespace display
}   //	namespace aaa
