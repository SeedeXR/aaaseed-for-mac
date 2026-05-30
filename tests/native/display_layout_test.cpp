// tests/native/display_layout_test.cpp
//
// second_todo.md S8 (c153) : PURE unit coverage for the multi-display
// geometry core (virtual bounding canvas, normalized sub-rects, primary
// selection). Framework-free / no display hardware.

#include <gtest/gtest.h>

#include "src/display/display_layout.h"

#include <vector>

using aaa::display::Rect;

namespace
{
bool near( double a, double b ) { return ( a - b < 1e-9 ) && ( b - a < 1e-9 ); }
}

TEST( DisplayLayout, VirtualBoundsTwoSideBySide )
{
    //	1920x1080 primary at origin + a second to its right.
    std::vector<Rect> screens = {
        { 0.0,    0.0, 1920.0, 1080.0 },
        { 1920.0, 0.0, 1920.0, 1080.0 },
    };
    Rect b = aaa::display::virtual_bounds( screens );
    EXPECT_TRUE( near( b.x, 0.0 ) );
    EXPECT_TRUE( near( b.y, 0.0 ) );
    EXPECT_TRUE( near( b.w, 3840.0 ) );
    EXPECT_TRUE( near( b.h, 1080.0 ) );
}

TEST( DisplayLayout, VirtualBoundsNegativeOrigin )
{
    //	A display to the LEFT of and BELOW the primary (negative coords).
    std::vector<Rect> screens = {
        { 0.0,     0.0, 1000.0, 800.0 },
        { -500.0, -200.0, 500.0, 400.0 },
    };
    Rect b = aaa::display::virtual_bounds( screens );
    EXPECT_TRUE( near( b.x, -500.0 ) );
    EXPECT_TRUE( near( b.y, -200.0 ) );
    EXPECT_TRUE( near( b.w, 1500.0 ) );
    EXPECT_TRUE( near( b.h, 1000.0 ) );
}

TEST( DisplayLayout, NormalizedSubrect )
{
    Rect bounds = { 0.0, 0.0, 3840.0, 1080.0 };
    Rect right  = { 1920.0, 0.0, 1920.0, 1080.0 };
    Rect n = aaa::display::normalized_subrect( right, bounds );
    EXPECT_TRUE( near( n.x, 0.5 ) );
    EXPECT_TRUE( near( n.y, 0.0 ) );
    EXPECT_TRUE( near( n.w, 0.5 ) );
    EXPECT_TRUE( near( n.h, 1.0 ) );
}

TEST( DisplayLayout, NormalizedSubrectDegenerateBounds )
{
    Rect n = aaa::display::normalized_subrect( { 0, 0, 10, 10 }, { 0, 0, 0, 0 } );
    EXPECT_TRUE( near( n.x, 0.0 ) );
    EXPECT_TRUE( near( n.w, 0.0 ) );
}

TEST( DisplayLayout, PrimaryIsClosestToOrigin )
{
    std::vector<Rect> screens = {
        { 1920.0, 0.0, 1920.0, 1080.0 },   //	aux, right
        { 0.0,    0.0, 1920.0, 1080.0 },   //	primary, at origin
    };
    EXPECT_EQ( aaa::display::primary_index( screens ), (std::size_t)1 );
}

TEST( DisplayLayout, EmptyInputs )
{
    std::vector<Rect> none;
    Rect b = aaa::display::virtual_bounds( none );
    EXPECT_TRUE( near( b.w, 0.0 ) && near( b.h, 0.0 ) );
    EXPECT_EQ( aaa::display::primary_index( none ), (std::size_t)0 );
}
