// Verifies the NSight → os_signpost shim in vendor/aaaseed-engine/Src/draw/
// NSightEvents.h. On Mac (AAA_NSIGHT_USE() == 0), NXPROFILEFUNC issues real
// os_signpost interval markers visible in Instruments / Xcode Time Profiler.
//
// These tests don't assert on signpost content (signposts are observed via
// the OS, not via a return value). What they DO assert is that :
//   - the macros compile cleanly,
//   - constructing/destroying NXProfileFunc doesn't crash,
//   - nsight::init() / deinit() / update() are callable.
//
// To actually see the signposts : Xcode → Instruments → choose "os_signpost"
// template → run the test binary → filter on subsystem "ai.bsa.aaaseed".

#include <gtest/gtest.h>

#include "aaa_type.h"
#include "draw/NSightEvents.h"

TEST( NsightSignpost, ApiCallsDoNotCrash )
{
    nsight::init();
    nsight::update();
    nsight::deinit();
}

TEST( NsightSignpost, MacroOpensAndClosesScope )
{
    NXPROFILEFUNC( "test_scope_basic" );
    // Some "work" so the signpost interval has measurable length when viewed
    // in Instruments. Without this the scope is too short to see.
    volatile int sum = 0;
    for( int i = 0; i < 1000; ++i )
        sum += i;
    EXPECT_EQ( sum, 499500 );
}

TEST( NsightSignpost, MacroWithColor )
{
    NXPROFILEFUNCCOL( "test_scope_with_color", 0xFF0000FF );
    volatile int x = 42;
    EXPECT_EQ( x, 42 );
}

TEST( NsightSignpost, MacroWithColorAndPayload )
{
    NXPROFILEFUNCCOL2( "test_scope_with_payload", 0xFF00FF00, 1234 );
    volatile int x = 7;
    EXPECT_EQ( x, 7 );
}

TEST( NsightSignpost, NestedScopesWork )
{
    NXPROFILEFUNC( "outer_scope" );
    volatile int a = 0;
    for( int i = 0; i < 100; ++i )
    {
        NXPROFILEFUNC( "inner_scope" );
        a += i;
    }
    EXPECT_EQ( a, 4950 );
}
