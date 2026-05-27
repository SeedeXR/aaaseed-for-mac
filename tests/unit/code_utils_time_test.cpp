// Tests the macOS port of two code_utils modules :
//   Src/code_utils/time/aaa_time.{cpp,h}    -- monotonic clock wrapper
//   Src/code_utils/os_version.{cpp,h}       -- OS version / hostname / cpu
//
// On macOS the time path is backed by clock_gettime( CLOCK_MONOTONIC_RAW )
// and the os_version path is backed by sysctlbyname / gethostname /
// getlogin_r. The tests exercise both the underlying POSIX APIs directly
// (to catch toolchain / SDK issues early) and the engine wrappers.

#include <gtest/gtest.h>

#include <sys/sysctl.h>
#include <time.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "aaa_type.h"
#include "os_version.h"
#include "time/aaa_time.h"

// ---- Raw POSIX-API sanity ---------------------------------------------------

TEST( CodeUtilsTime, ClockMonotonicRawIncreasesAcrossSleep )
{
    struct timespec t0 {};
    struct timespec t1 {};

    ASSERT_EQ( 0, clock_gettime( CLOCK_MONOTONIC_RAW, &t0 ) );
    ASSERT_EQ( 0, usleep( 1000 ) );     // sleep 1 ms
    ASSERT_EQ( 0, clock_gettime( CLOCK_MONOTONIC_RAW, &t1 ) );

    // Convert to a single nanosecond count and check strict monotonicity.
    INT64 const ns0 = INT64( t0.tv_sec ) * 1000000000LL + INT64( t0.tv_nsec );
    INT64 const ns1 = INT64( t1.tv_sec ) * 1000000000LL + INT64( t1.tv_nsec );
    EXPECT_GT( ns1, ns0 );

    // The gap should be at least 500 us (we asked for 1000 us). Stay loose
    // enough to survive a noisy scheduler.
    EXPECT_GE( ns1 - ns0, 500LL * 1000LL );
}

TEST( CodeUtilsTime, OsProductVersionLooksLikeMacOs13OrMore )
{
    char    buf[64] = { 0 };
    size_t  size    = sizeof(buf);

    ASSERT_EQ( 0, sysctlbyname( "kern.osproductversion", buf, &size, nullptr, 0 ) );
    ASSERT_GT( size, 0u );
    buf[ sizeof(buf) - 1 ] = 0;

    int major = 0;
    int minor = 0;
    sscanf( buf, "%d.%d", &major, &minor );
    EXPECT_GE( major, 13 ) << "kern.osproductversion = '" << buf << "'";
}

TEST( CodeUtilsTime, HwMachineIsArm64OnAppleSilicon )
{
    char    buf[64] = { 0 };
    size_t  size    = sizeof(buf);

    ASSERT_EQ( 0, sysctlbyname( "hw.machine", buf, &size, nullptr, 0 ) );
    ASSERT_GT( size, 0u );
    buf[ sizeof(buf) - 1 ] = 0;

    EXPECT_STREQ( "arm64", buf );
}

TEST( CodeUtilsTime, GetHostNameSucceeds )
{
    char buf[256] = { 0 };
    ASSERT_EQ( 0, gethostname( buf, sizeof(buf) - 1 ) );
    buf[ sizeof(buf) - 1 ] = 0;
    EXPECT_GT( strlen( buf ), 0u );
}

// ---- Engine wrappers --------------------------------------------------------

TEST( CodeUtilsTime, EngineStoreReturnsIncreasingTimespecAcrossSleep )
{
    aaa::time::ST_TIME t0 {};
    aaa::time::ST_TIME t1 {};

    aaa::time::store( &t0 );
    usleep( 1000 );
    aaa::time::store( &t1 );

    // tv_sec / tv_nsec on macOS (timespec). Compare via single ns count.
    INT64 const ns0 = INT64( t0.tv_sec ) * 1000000000LL + INT64( t0.tv_nsec );
    INT64 const ns1 = INT64( t1.tv_sec ) * 1000000000LL + INT64( t1.tv_nsec );
    EXPECT_GT( ns1, ns0 );
}

TEST( CodeUtilsTime, EngineGetIntervalMicroSecIsReasonable )
{
    aaa::time::ST_TIME t0 {};
    aaa::time::ST_TIME t1 {};

    aaa::time::store( &t0 );
    usleep( 5000 );     // 5 ms
    aaa::time::store( &t1 );

    UINT32 const us = aaa::time::get_interval_micro_sec( t1, t0 );
    EXPECT_GE( us, 2000u );         // floor : at least 2 ms
    EXPECT_LE( us, 500000u );       // ceiling : not more than 500 ms (noisy CI)
}

TEST( CodeUtilsTime, EngineGetIntervalSecIsReasonable )
{
    aaa::time::ST_TIME t0 {};
    aaa::time::ST_TIME t1 {};

    aaa::time::store( &t0 );
    usleep( 10 * 1000 );    // 10 ms
    aaa::time::store( &t1 );

    DOUBLE const sec = aaa::time::get_interval_sec( t1, t0 );
    EXPECT_GT( sec, 0.001 );
    EXPECT_LT( sec, 1.0 );
}

TEST( CodeUtilsTime, EngineTestRealTimePossibleSucceeds )
{
    // On macOS 13+ Apple Silicon, CLOCK_MONOTONIC_RAW is always available.
    EXPECT_TRUE( aaa::time::test_real_time_possible() );
}

// ---- os_version engine wrappers --------------------------------------------

TEST( CodeUtilsOsVersion, GetInfoPopulatesMacFields )
{
    n_os_version::get_info();

    EXPECT_TRUE(  n_os_version::b_macintosh );
    EXPECT_FALSE( n_os_version::b_windows   );
    EXPECT_FALSE( n_os_version::b_linux     );

    EXPECT_TRUE( n_os_version::b_arm64 );
    EXPECT_TRUE( n_os_version::b_x64   );      // arm64 macOS is 64-bit

    EXPECT_GE( n_os_version::version_major, 13 );
    EXPECT_GE( n_os_version::cpu_logical,   1  );
    EXPECT_GE( n_os_version::cpu_physical,  1  );
    EXPECT_GT( n_os_version::memory_size_bytes, INT64(0) );
}
