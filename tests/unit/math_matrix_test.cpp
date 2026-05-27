// Exercises aaa_matrix.cpp builders (TRS composition via GLM) on the Mac side.
// Together with math_rand_test.cpp this is the regression baseline for the
// math subsystem.

#include <gtest/gtest.h>

#include <cmath>

#include "aaa_type.h"
#include "math/aaa_matrix.h"

using glm::mat4;
using aaa::matrix::make_matrix_tra_rotyzx_rad_sca;
using aaa::matrix::make_matrix_tra_rotyxz_rad_sca;

namespace
{
    constexpr FP32 kEps = 1e-5f;

    bool nearly_equal( FP32 a, FP32 b, FP32 eps = kEps )
    {
        return std::fabs( a - b ) < eps;
    }
}

TEST( MathMatrix, IdentityFromZeroTraRotOneSca )
{
    FP32 const tra[ 3 ] = { 0, 0, 0 };
    FP32 const rot[ 3 ] = { 0, 0, 0 };
    FP32 const sca[ 3 ] = { 1, 1, 1 };

    mat4 m;
    make_matrix_tra_rotyzx_rad_sca( &m, tra, rot, sca );

    for( int col = 0; col < 4; ++col )
        for( int row = 0; row < 4; ++row )
        {
            FP32 const expected = ( col == row ) ? 1.f : 0.f;
            EXPECT_TRUE( nearly_equal( m[ col ][ row ], expected ) )
                << "m[" << col << "][" << row << "] = " << m[ col ][ row ]
                << " (expected " << expected << ")";
        }
}
