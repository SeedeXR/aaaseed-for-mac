// Stage 2 of the engine port: first engine .cpp (Src/math/aaa_rand.cpp)
// compiled and linked from the Mac side, exercised by unit tests against
// known invariants.
//
// rand() output is libc-implementation-dependent, so we test the structural
// invariants (range, determinism) rather than exact bit-equal values.

#include <gtest/gtest.h>

#include <cstdlib>

#include "aaa_type.h"
#include "math/aaa_rand.h"

class MathRandTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        std::srand( 42 );
        rnd_init();
    }
};

TEST_F( MathRandTest, ConstantReturnsExactlyTenth )
{
    EXPECT_EQ( rnd_constant(), REAL( 0.1 ) );
    EXPECT_EQ( rnd_constant(), REAL( 0.1 ) );
    EXPECT_EQ( rnd_constant(), REAL( 0.1 ) );
}

TEST_F( MathRandTest, FlipAlternatesBetweenTenthAndNegativeTenth )
{
    REAL const a = rnd_flip();
    REAL const b = rnd_flip();
    REAL const c = rnd_flip();
    REAL const d = rnd_flip();

    EXPECT_NE( a, b );
    EXPECT_EQ( a, c );
    EXPECT_EQ( b, d );

    EXPECT_TRUE( a == REAL( 0.1 ) || a == REAL( -0.1 ) );
    EXPECT_TRUE( b == REAL( 0.1 ) || b == REAL( -0.1 ) );
}

TEST_F( MathRandTest, RndMaaStaysInHalfOpenRange )
{
    for( int i = 0; i < 10000; ++i )
    {
        REAL const x = rnd_maa();
        EXPECT_GE( x, REAL( -0.5 ) );
        EXPECT_LT( x, REAL( 0.5 ) );
    }
}

TEST_F( MathRandTest, RndMaaIsDeterministicForFixedSeed )
{
    std::srand( 42 );
    REAL const a1 = rnd_maa();
    REAL const a2 = rnd_maa();
    REAL const a3 = rnd_maa();

    std::srand( 42 );
    REAL const b1 = rnd_maa();
    REAL const b2 = rnd_maa();
    REAL const b3 = rnd_maa();

    EXPECT_EQ( a1, b1 );
    EXPECT_EQ( a2, b2 );
    EXPECT_EQ( a3, b3 );
}

TEST_F( MathRandTest, RndGaussSpreadsAroundCenter )
{
    REAL sum = 0;
    int const n = 5000;
    for( int i = 0; i < n; ++i )
        sum += rnd_gauss();

    REAL const mean = sum / REAL( n );
    EXPECT_GT( mean, REAL( -0.1 ) );
    EXPECT_LT( mean, REAL(  0.1 ) );
}
