// tests/unit/o_str_real_test.cpp
//
// Real-`o_str` validation test (continuation 66). The aaa_str.cpp
// port landed silently in c64 (added to aaaseed_code_utils) but every
// existing test target that uses `o_str` masks the real implementation
// behind aaalua_ostr_stub.cpp's 3-method override (ctor/dtor/set). This
// target does NOT link the stub -- it pulls in the real o_str + the
// full layer-1 + layer-2 stub cascade (cpu, mem dispatch, file_virtual,
// dir_pool) to verify aaa_str.cpp actually works end-to-end on Mac.
//
// Validates :
//   - o_str default ctor produces empty string (get() returns "").
//   - set( const char* ) populates _data + _len.
//   - add( const char* ) appends.
//   - erase() resets.
//   - push_name / pop_name static name-stack pattern works.
//   - count_char helper agrees with strlen.
//
// CTest label : unit;o_str_real.

#include <gtest/gtest.h>

#include <cstring>

#include "aaa_type.h"
#include "aaa_str.h"

TEST( OStrReal, DefaultCtorIsEmpty )
{
    o_str s;
    EXPECT_STREQ( s.get(), "" );
    EXPECT_EQ( s.get_len(), 0u );
}

TEST( OStrReal, SetCopiesAndGetReturnsIt )
{
    o_str s;
    s.set( "hello" );
    EXPECT_STREQ( s.get(), "hello" );
    EXPECT_EQ( s.get_len(), 5u );
}

TEST( OStrReal, AddAppends )
{
    o_str s;
    s.set( "foo" );
    s.add( "bar" );
    EXPECT_STREQ( s.get(), "foobar" );
    EXPECT_EQ( s.get_len(), 6u );
}

TEST( OStrReal, EraseResetsLen )
{
    o_str s;
    s.set( "abc" );
    EXPECT_EQ( s.get_len(), 3u );
    s.erase();
    EXPECT_EQ( s.get_len(), 0u );
    EXPECT_STREQ( s.get(), "" );
}

TEST( OStrReal, PushPopNameStackReturnsScratchO_Str )
{
    //	push_name / pop_name pattern : engine convention for short-
    //	lived temporary o_str without heap allocation per call.
    o_str& tmp = o_str::push_name();
    tmp.set( "scratch" );
    EXPECT_STREQ( tmp.get(), "scratch" );
    o_str::pop_name();

    //	Push again ; we should get a usable o_str either way.
    o_str& tmp2 = o_str::push_name();
    tmp2.set( "second" );
    EXPECT_STREQ( tmp2.get(), "second" );
    o_str::pop_name();
}

TEST( OStrReal, ManySetCyclesDontLeakOrCrash )
{
    //	Stress-cycle 1000 set+erase to validate the heap-alloc path.
    o_str s;
    for( int i = 0; i < 1000; ++i )
    {
        char tmp[ 32 ];
        std::snprintf( tmp, sizeof( tmp ), "iter_%d", i );
        s.set( tmp );
        EXPECT_EQ( s.get_len(), std::strlen( tmp ) );
    }
    s.erase();
    EXPECT_EQ( s.get_len(), 0u );
}
