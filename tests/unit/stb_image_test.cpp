// tests/unit/stb_image_test.cpp
//
// Round-trip test for the vendored stb_image / stb_image_write headers.
// Phase 5 asset-library beachhead (continuation 44) -- first verification
// that the Mac port can encode + decode PNG / JPG / similar without a
// host image library.
//
// Strategy : build a known 4x4 RGBA pattern in memory, encode it to a
// PNG byte buffer via stbi_write_png_to_func, decode it back via
// stbi_load_from_memory, verify every pixel survives the round trip.
//
// Why round-trip rather than decoding a vendored test PNG : avoids the
// fragility of a hand-crafted byte-array fixture AND simultaneously
// proves both the encode and decode paths work.
//
// Doctrine reminder : stb_image is the Mac asset-library lever. The
// Windows engine continues to use FreeImage ; we don't replace it
// there. This lib is Mac-only by virtue of being unused on Windows.
//
// CTest label : unit;stb_image.

#include <gtest/gtest.h>

#include "stb_image.h"
#include "stb_image_write.h"

#include <cstdint>
#include <cstring>
#include <vector>

namespace
{
    //	Captures the bytes that stbi_write_png_to_func emits. The
    //	callback receives chunks ; we append each to the vector.
    void png_write_callback( void* user, void* data, int size )
    {
        auto* out = static_cast< std::vector< std::uint8_t >* >( user );
        std::uint8_t const* p = static_cast< std::uint8_t const* >( data );
        out->insert( out->end(), p, p + size );
    }

    //	4x4 RGBA8 pattern with distinct per-pixel values so any pixel
    //	mismatch is unambiguous after the round trip.
    std::vector< std::uint8_t > make_pattern_4x4()
    {
        constexpr int W = 4;
        constexpr int H = 4;
        std::vector< std::uint8_t > p( W * H * 4, 0 );
        for( int y = 0; y < H; ++y )
        {
            for( int x = 0; x < W; ++x )
            {
                std::size_t const i = ( y * W + x ) * 4;
                p[ i + 0 ] = std::uint8_t( 16 * x + y );      // R varies with x
                p[ i + 1 ] = std::uint8_t( 16 * y + x );      // G varies with y
                p[ i + 2 ] = std::uint8_t( 200 - 10 * x );    // B descending
                p[ i + 3 ] = 255;                              // opaque
            }
        }
        return p;
    }
}

TEST( StbImage, RoundTripPngPreserves4x4Rgba8Pattern )
{
    constexpr int W = 4;
    constexpr int H = 4;

    std::vector< std::uint8_t > original = make_pattern_4x4();
    ASSERT_EQ( original.size(), size_t( W * H * 4 ) );

    //	Encode to PNG in memory.
    std::vector< std::uint8_t > png_bytes;
    int const enc_ok = stbi_write_png_to_func(
        &png_write_callback, &png_bytes,
        W, H, /*channels*/ 4,
        original.data(),
        /*stride_in_bytes*/ W * 4 );

    ASSERT_NE( enc_ok, 0 ) << "stbi_write_png_to_func failed";
    ASSERT_GT( png_bytes.size(), size_t( 30 ) )
        << "PNG output suspiciously small : " << png_bytes.size() << " bytes";

    //	PNG byte 0..7 should be the magic : 89 50 4E 47 0D 0A 1A 0A.
    EXPECT_EQ( png_bytes[ 0 ], 0x89 );
    EXPECT_EQ( png_bytes[ 1 ], 'P' );
    EXPECT_EQ( png_bytes[ 2 ], 'N' );
    EXPECT_EQ( png_bytes[ 3 ], 'G' );

    //	Decode back.
    int dec_w = 0, dec_h = 0, dec_channels = 0;
    std::uint8_t* decoded = stbi_load_from_memory(
        png_bytes.data(), int( png_bytes.size() ),
        &dec_w, &dec_h, &dec_channels,
        /*desired_channels*/ 4 );

    ASSERT_NE( decoded, nullptr ) << "stbi_load_from_memory failed : "
                                  << stbi_failure_reason();
    EXPECT_EQ( dec_w, W );
    EXPECT_EQ( dec_h, H );
    //	`dec_channels` is the source channel count ; the actual buffer
    //	is in `desired_channels` (4) format. Either 3 or 4 is fine
    //	depending on whether stb embedded alpha.

    //	Pixel-by-pixel comparison. PNG is lossless ; expect exact match.
    for( int i = 0; i < W * H * 4; ++i )
    {
        ASSERT_EQ( decoded[ i ], original[ i ] )
            << "Pixel byte " << i << " differs : "
            << "decoded=" << int( decoded[ i ] )
            << " original=" << int( original[ i ] );
    }

    stbi_image_free( decoded );
}

TEST( StbImage, FailureReasonOnGarbageInput )
{
    //	Feed obvious garbage. stbi_load_from_memory must return null
    //	AND stbi_failure_reason() must return something non-empty.
    std::uint8_t const garbage[ 16 ] = { 0, 1, 2, 3, 4, 5, 6, 7,
                                         8, 9, 10, 11, 12, 13, 14, 15 };
    int w = 0, h = 0, c = 0;
    std::uint8_t* decoded = stbi_load_from_memory(
        garbage, sizeof( garbage ), &w, &h, &c, 4 );

    EXPECT_EQ( decoded, nullptr );
    char const* reason = stbi_failure_reason();
    EXPECT_NE( reason, nullptr );
    if( reason )
        EXPECT_GT( std::strlen( reason ), size_t( 0 ) );
}
