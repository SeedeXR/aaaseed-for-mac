// tests/unit/ui_dpi_test.cpp
//
// Phase 4 (todo.md L298) : aaa::ui::dpi smoke + invariants. Verifies
// the hermetic Mac sub-lib `aaaseed_ui_dpi_mac` (NSScreen-backed
// DPI adapter) exposes a usable surface :
//
//   1. main_scale_factor returns > 0 ("is it sensible" check ; under
//      ctest on the macOS test host there IS a main screen).
//   2. all_screens is non-empty.
//   3. points <-> pixels round-trip is exact within 1e-9.
//   4. main_screen index == 0.
//   5. points_to_pixels(0.0) == 0.0 (edge case ; exact).
//   6. scale_factor_for_window(nullptr) returns main scale (fallback
//      when window has no screen yet).
//
// Hermetic test : links aaaseed_ui_dpi_mac only. NO link to
// aaaseed_code_utils -- preserves the c104 hermetic doctrine. Pure
// C++ TU (no .mm) so the header's no-ObjC-in-header contract is
// verified at compile time : if aaa_dpi.h ever sprouted an
// unconditional Objective-C symbol the build of this file would
// break.

#include "src/ui/macos/aaa_dpi.h"

#include <gtest/gtest.h>

#include <cmath>
#include <string>
#include <vector>

namespace
{

using aaa::ui::dpi::all_screens;
using aaa::ui::dpi::main_scale_factor;
using aaa::ui::dpi::main_screen;
using aaa::ui::dpi::pixels_to_points;
using aaa::ui::dpi::points_to_pixels;
using aaa::ui::dpi::scale_factor_for_window;
using aaa::ui::dpi::ScreenInfo;

}   //	anonymous namespace

//	-----------------------------------------------------------------
//	Test 1 : main_scale_factor returns a sensible positive number.
//
//	Real macOS hosts always have a main screen under ctest (the
//	WindowServer is up). The expected values are 1.0 (non-Retina)
//	or 2.0 (Retina) ; we accept anything strictly > 0 to stay
//	future-proof against Apple shipping fractional scale factors.
//	-----------------------------------------------------------------
TEST( UIDpiTest, MainScaleFactorIsPositive )
{
    double const s = main_scale_factor();
    EXPECT_GT( s, 0.0 );
    //	Sanity ceiling : > 10x would mean we read the wrong field.
    EXPECT_LT( s, 10.0 );
}

//	-----------------------------------------------------------------
//	Test 2 : all_screens returns at least one entry on a real host.
//	-----------------------------------------------------------------
TEST( UIDpiTest, AllScreensIsNonEmpty )
{
    std::vector< ScreenInfo > const screens = all_screens();
    EXPECT_FALSE( screens.empty() );
    //	Index parity : first entry should be 0, sequential thereafter.
    for( std::size_t i = 0 ; i < screens.size() ; ++i )
    {
        EXPECT_EQ( screens[ i ].index, static_cast< int >( i ) );
        EXPECT_GT( screens[ i ].scale_factor, 0.0 );
    }
}

//	-----------------------------------------------------------------
//	Test 3 : points <-> pixels round-trip is exact within 1e-9.
//
//	Tries a handful of plausible UI sizes. Tolerance is well below
//	any reasonable display dimension ; the underlying math is just
//	multiply-then-divide by the same scale, so the only source of
//	drift is IEEE rounding.
//	-----------------------------------------------------------------
TEST( UIDpiTest, PointsPixelsRoundTripIsInverse )
{
    double const samples[] = { 1.0, 16.0, 100.0, 1024.5, 1920.0, 3.14159265 };
    for( double const x : samples )
    {
        double const round_trip = pixels_to_points( points_to_pixels( x ) );
        EXPECT_NEAR( round_trip, x, 1e-9 ) << "x=" << x;
    }
}

//	-----------------------------------------------------------------
//	Test 4 : main_screen reports index 0 (header contract).
//	-----------------------------------------------------------------
TEST( UIDpiTest, MainScreenIndexIsZero )
{
    ScreenInfo const m = main_screen();
    EXPECT_EQ( m.index, 0 );
    EXPECT_GT( m.scale_factor, 0.0 );
}

//	-----------------------------------------------------------------
//	Test 5 : points_to_pixels(0.0) is exactly 0.0 (edge case).
//	-----------------------------------------------------------------
TEST( UIDpiTest, PointsToPixelsZeroIsZero )
{
    EXPECT_EQ( points_to_pixels( 0.0 ), 0.0 );
    EXPECT_EQ( pixels_to_points( 0.0 ), 0.0 );
}

//	-----------------------------------------------------------------
//	Test 6 : scale_factor_for_window(nullptr) returns main scale.
//
//	The header documents this fallback so callers that don't yet
//	have a window (boot path, pre-orderFront) get a usable number.
//	Passing nullptr in a pure-C++ TU works because the header's
//	non-__OBJC__ branch typedefs NSWindow as void.
//	-----------------------------------------------------------------
TEST( UIDpiTest, ScaleFactorForNullWindowFallsBackToMain )
{
    double const got = scale_factor_for_window( nullptr );
    EXPECT_EQ( got, main_scale_factor() );
}
