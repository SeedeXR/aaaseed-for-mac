// tests/unit/tinyexr_test.cpp
//
// Phase 5 fifth beachhead smoke test (continuation 67) -- closes the
// EXR asset-format gap noted in Phase 5. Mirrors the c44 stb_image
// round-trip pattern : build a known 4x4 RGBA float pattern, encode
// to EXR in memory via SaveEXRToMemory (FP32, ZIP-compressed), decode
// via LoadEXRFromMemory, verify pixels round-trip exactly.
//
// EXR is lossless for full-precision float -- bit-exact equality
// expected when `save_as_fp16 = 0` (FLOAT pixel format). When
// `save_as_fp16 = 1` the result would be HALF precision and round
// to nearest representable half ; we test the lossless path here.
//
// CTest label : unit;tinyexr.

#include <gtest/gtest.h>

#include "tinyexr.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace
{
    //	4x4 RGBA float pattern. Distinct values per pixel so any
    //	mismatch is unambiguous.
    std::vector< float > make_pattern_4x4()
    {
        constexpr int W = 4;
        constexpr int H = 4;
        std::vector< float > p( W * H * 4, 0.0f );
        for( int y = 0; y < H; ++y )
        {
            for( int x = 0; x < W; ++x )
            {
                std::size_t const i = ( y * W + x ) * 4;
                p[ i + 0 ] = float( x + 1 ) * 0.1f;   // R
                p[ i + 1 ] = float( y + 1 ) * 0.2f;   // G
                p[ i + 2 ] = float( x * y ) * 0.05f;  // B
                p[ i + 3 ] = 1.0f;                    // A
            }
        }
        return p;
    }
}

TEST( TinyExr, RoundTripFp32Rgba4x4Pattern )
{
    constexpr int W = 4;
    constexpr int H = 4;

    std::vector< float > const original = make_pattern_4x4();
    ASSERT_EQ( original.size(), std::size_t( W * H * 4 ) );

    //	Encode to EXR in memory.
    unsigned char* exr_bytes = nullptr;
    char const*    save_err  = nullptr;
    int const      bytes_written = SaveEXRToMemory(
        original.data(),
        W, H,
        /*components*/  4,
        /*save_as_fp16*/ 0,   // FP32 = lossless
        &exr_bytes,
        &save_err );

    ASSERT_GT( bytes_written, 0 )
        << "SaveEXRToMemory failed : "
        << ( save_err ? save_err : "(no err string)" );
    ASSERT_NE( exr_bytes, nullptr );

    //	EXR magic : 0x76, 0x2F, 0x31, 0x01 (4 bytes header).
    ASSERT_GE( bytes_written, 4 );
    EXPECT_EQ( exr_bytes[ 0 ], 0x76 );
    EXPECT_EQ( exr_bytes[ 1 ], 0x2F );
    EXPECT_EQ( exr_bytes[ 2 ], 0x31 );
    EXPECT_EQ( exr_bytes[ 3 ], 0x01 );

    //	Decode back.
    float* decoded_rgba = nullptr;
    int    dec_w = 0, dec_h = 0;
    char const* load_err = nullptr;
    int const   load_ret = LoadEXRFromMemory(
        &decoded_rgba, &dec_w, &dec_h,
        exr_bytes, std::size_t( bytes_written ),
        &load_err );

    ASSERT_EQ( load_ret, TINYEXR_SUCCESS )
        << "LoadEXRFromMemory failed : "
        << ( load_err ? load_err : "(no err string)" );
    ASSERT_NE( decoded_rgba, nullptr );
    EXPECT_EQ( dec_w, W );
    EXPECT_EQ( dec_h, H );

    //	Pixel-by-pixel comparison. FP32 round-trip should be exact.
    for( int i = 0; i < W * H * 4; ++i )
    {
        EXPECT_FLOAT_EQ( decoded_rgba[ i ], original[ i ] )
            << "Pixel " << i << " mismatch (decoded=" << decoded_rgba[ i ]
            << ", original=" << original[ i ] << ")";
    }

    std::free( decoded_rgba );
    std::free( exr_bytes );
}

TEST( TinyExr, FailsCleanlyOnGarbageInput )
{
    //	Feed obvious garbage. LoadEXRFromMemory must return a negative
    //	error code (not TINYEXR_SUCCESS) and may populate err.
    unsigned char const garbage[ 16 ] = { 0, 1, 2, 3, 4, 5, 6, 7,
                                          8, 9, 10, 11, 12, 13, 14, 15 };
    float* decoded = nullptr;
    int    w = 0, h = 0;
    char const* err = nullptr;
    int const   ret = LoadEXRFromMemory( &decoded, &w, &h,
                                         garbage, sizeof( garbage ),
                                         &err );
    EXPECT_NE( ret, TINYEXR_SUCCESS );
    EXPECT_EQ( decoded, nullptr );
    //	err may or may not be set ; not strictly required by API.
    if( decoded ) std::free( decoded );
}
