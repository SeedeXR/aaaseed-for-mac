// src/ui/macos/aaa_dpi.mm
//
// Phase 4 (todo.md L298) : DPI adapter impl. Wraps
// `[NSScreen mainScreen].backingScaleFactor` + `[NSScreen screens]`
// behind the pure-C++ API declared in aaa_dpi.h.
//
// Hermetic Mac sub-lib (c104 / feedback_hermetic_mac_sublibs.md) :
// Foundation + AppKit only. No `o_str`, no `aaa_mem`, no vendor
// link. ASCII-only source.
//
// Gating : the whole TU is `#if defined(__APPLE__)` so a future
// cross-platform .mm/.cpp split can drop a stub or call into a
// shared header without breaking Windows builds.

#include "src/ui/macos/aaa_dpi.h"

#if defined(__APPLE__)

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

namespace aaa
{
namespace ui
{
namespace dpi
{

namespace
{

//	Convert an NSString to std::string (UTF-8). nil-safe : returns
//	an empty string when the input is nil or has no UTF-8 rep.
std::string ns_to_string( NSString* s )
{
    if( s == nil )
        return std::string {};
    char const* c = [s UTF8String];
    if( c == nullptr )
        return std::string {};
    return std::string { c };
}

//	Fill a ScreenInfo from an NSScreen* + its index. Caller has
//	already null-checked `screen`.
ScreenInfo info_from_screen( NSScreen* screen, int idx )
{
    ScreenInfo si;
    si.index           = idx;
    si.scale_factor    = static_cast< double >( [screen backingScaleFactor] );

    NSRect const frame_pts = [screen frame];
    si.width_points    = static_cast< int >( frame_pts.size.width );
    si.height_points   = static_cast< int >( frame_pts.size.height );

    //	convertRectToBacking gives the same rect in backing-store
    //	(pixel) coordinates. This is the canonical AppKit way to
    //	turn points into pixels for a specific screen ; preferable
    //	to manually multiplying because Apple has flirted with
    //	non-integer scale factors on engineering builds.
    NSRect const frame_px = [screen convertRectToBacking:frame_pts];
    si.width_pixels    = static_cast< int >( frame_px.size.width );
    si.height_pixels   = static_cast< int >( frame_px.size.height );

    //	NSScreen.localizedName is macOS 10.15+. The Mac port pins
    //	macOS 13 so it's always available, but the @available gate
    //	keeps the file forward-portable.
    if( @available( macOS 10.15, * ) )
    {
        si.localized_name = ns_to_string( [screen localizedName] );
    }
    else
    {
        si.localized_name = std::string {};
    }
    return si;
}

//	Degenerate fallback when no screen is attached (headless / SSH).
ScreenInfo make_fallback()
{
    ScreenInfo si;
    si.index           = 0;
    si.scale_factor    = 1.0;
    si.width_points    = 0;
    si.height_points   = 0;
    si.width_pixels    = 0;
    si.height_pixels   = 0;
    si.localized_name  = std::string {};
    return si;
}

}   //	anonymous namespace

ScreenInfo main_screen()
{
    @autoreleasepool
    {
        NSScreen* m = [NSScreen mainScreen];
        if( m == nil )
            return make_fallback();
        //	main_screen() always reports index 0 -- this is the
        //	contract the header documents. The screen's actual
        //	position in [NSScreen screens] may differ ; all_screens()
        //	is the right call when the caller cares about that.
        return info_from_screen( m, /*idx*/ 0 );
    }
}

std::vector< ScreenInfo > all_screens()
{
    @autoreleasepool
    {
        NSArray< NSScreen* >* screens = [NSScreen screens];
        std::vector< ScreenInfo > out;
        if( screens == nil )
            return out;
        out.reserve( [screens count] );
        int idx = 0;
        for( NSScreen* s in screens )
        {
            if( s == nil )
            {
                ++idx;
                continue;
            }
            out.push_back( info_from_screen( s, idx ) );
            ++idx;
        }
        return out;
    }
}

double main_scale_factor()
{
    @autoreleasepool
    {
        NSScreen* m = [NSScreen mainScreen];
        if( m == nil )
            return 1.0;
        return static_cast< double >( [m backingScaleFactor] );
    }
}

double points_to_pixels( double points )
{
    //	Short-circuit the zero case so the test contract holds
    //	exactly (no IEEE 0.0 * x = NaN edge for x that might be
    //	infinite -- not expected, but cheap insurance).
    if( points == 0.0 )
        return 0.0;
    return points * main_scale_factor();
}

double pixels_to_points( double pixels )
{
    if( pixels == 0.0 )
        return 0.0;
    double const s = main_scale_factor();
    if( s == 0.0 )
        return pixels;   //	pathological ; preserve input
    return pixels / s;
}

double scale_factor_for_window( void* window )
{
    @autoreleasepool
    {
        if( window == nullptr )
            return main_scale_factor();
        //	The header signature is `void*` for ABI parity between
        //	.mm and .cpp callers (see header comment). Internally
        //	cast back to NSWindow* -- safe because ObjC++ callers
        //	pass an NSWindow* directly (implicit conversion to void*)
        //	and there is no other use site.
        NSWindow* w = (__bridge NSWindow*) window;
        //	-[NSWindow screen] returns nil when the window hasn't
        //	been placed on a screen yet (pre-orderFront, off-screen,
        //	or minimised). Fall back to the main screen so callers
        //	always get a usable number. -backingScaleFactor on the
        //	NSWindow itself reads the current screen's scale and is
        //	the canonical lookup on modern macOS ; we prefer it when
        //	the window has a screen.
        NSScreen* s = [w screen];
        if( s == nil )
            return main_scale_factor();
        return static_cast< double >( [w backingScaleFactor] );
    }
}

}   //	namespace dpi
}   //	namespace ui
}   //	namespace aaa

#endif  //	defined(__APPLE__)
