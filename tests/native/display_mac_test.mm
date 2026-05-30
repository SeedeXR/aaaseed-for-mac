// tests/native/display_mac_test.mm
//
// second_todo.md S8 (c153) : live NSScreen enumeration + MultiDisplay aux
// windows. enumerate_screens() is exercised everywhere (skips only on a truly
// headless runner). The MultiDisplay aux-window path needs a WindowServer
// connection + >=2 screens, so it is gated behind AAA_DISPLAY_LIVE=1 and
// GTEST_SKIP'd otherwise (never silent).

#import <gtest/gtest.h>

#include "src/display/display_mac.h"
#include "src/display/display_layout.h"

#include <cstdlib>
#include <vector>

TEST( DisplayMac, EnumerateScreensConsistent )
{
    auto screens = aaa::display::enumerate_screens();
    if( screens.empty() )
        GTEST_SKIP() << "headless runner : no NSScreens";

    //	Every screen should report a positive size, and the computed virtual
    //	bounds must contain at least the primary's extent.
    for( auto const & s : screens )
    {
        EXPECT_GT( s.w, 0.0 );
        EXPECT_GT( s.h, 0.0 );
    }
    aaa::display::Rect b = aaa::display::virtual_bounds( screens );
    EXPECT_GT( b.w, 0.0 );
    EXPECT_GT( b.h, 0.0 );

    std::size_t p = aaa::display::primary_index( screens );
    EXPECT_LT( p, screens.size() );
}

TEST( DisplayMac, MultiDisplayAuxGated )
{
    if( std::getenv( "AAA_DISPLAY_LIVE" ) == nullptr )
        GTEST_SKIP() << "MultiDisplay aux windows need a GUI session + "
                        "AAA_DISPLAY_LIVE=1";

    auto screens = aaa::display::enumerate_screens();
    if( screens.size() < 2 )
        GTEST_SKIP() << "only one screen : no aux windows to create";

    aaa::display::MultiDisplay md;
    std::size_t aux = md.enable();
    EXPECT_EQ( aux, screens.size() - 1 );
    EXPECT_TRUE( md.is_enabled() );
    if( aux > 0 )
    {
        EXPECT_NE( md.aux_metal_layer( 0 ), nullptr );
        EXPECT_GT( md.aux_rect( 0 ).w, 0.0 );
    }
    md.disable();
    EXPECT_FALSE( md.is_enabled() );
    EXPECT_EQ( md.aux_count(), (std::size_t)0 );
}

TEST( DisplayMac, MultiDisplayOutOfRangeSafe )
{
    aaa::display::MultiDisplay md;
    //	Without enable() there are no aux windows ; accessors stay safe.
    EXPECT_EQ( md.aux_count(), (std::size_t)0 );
    EXPECT_EQ( md.aux_metal_layer( 0 ), nullptr );
    EXPECT_EQ( md.aux_rect( 99 ).w, 0.0 );
    EXPECT_FALSE( md.is_enabled() );
}
