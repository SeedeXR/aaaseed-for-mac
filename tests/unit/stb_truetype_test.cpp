// tests/unit/stb_truetype_test.cpp
//
// Smoke test for the vendored stb_truetype.h (continuation 57).
// Phase 5 third beachhead, alongside stb_image (c44) + Lua 5.1 (c45).
// Establishes that the Mac port can rasterize TTF glyphs to a CPU
// bitmap with no host font library -- everything ships in
// `third_party/stb/`. Future .app debug HUD or any text-rendering
// MEU consumes the same pipeline : load TTF -> rasterize glyph ->
// upload to MTLTexture -> sample in a fragment shader.
//
// Test fixture : SourceCodePro-Medium.ttf from
// vendor/aaaseed-runtime/AAAKernel/Fonts/Google/Source_Code_Pro/static/.
// Google Fonts ships SourceCodePro under SIL Open Font License (OFL),
// permissive enough to bundle. It's a monospace font -- ideal for a
// debug HUD where columns matter.
//
// Strategy : read the TTF into memory, init a stbtt_fontinfo, ask for
// the 'A' glyph at 32 px, rasterize to a CPU bitmap, verify the bitmap
// is non-zero somewhere (a properly initialized font + a 32-px 'A'
// produces a glyph with at least dozens of opaque pixels).
//
// Doctrine reminder : Mac-primary, Windows-reciprocal. stb_truetype is
// pure portable C ; Windows engine continues to use freetype. The two
// can coexist if a MEU author ever wants TrueType on both sides.
//
// CTest label : unit;stb_truetype.

#include <gtest/gtest.h>

#include "stb_truetype.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

#ifndef AAA_STB_TT_FONT_PATH
#error "AAA_STB_TT_FONT_PATH must be defined by CMake to point at SourceCodePro-Medium.ttf"
#endif

namespace
{
    std::vector< std::uint8_t > load_file_bytes( char const* path )
    {
        std::ifstream in( path, std::ios::binary | std::ios::ate );
        if( !in )
            return {};
        auto const size = in.tellg();
        in.seekg( 0, std::ios::beg );
        std::vector< std::uint8_t > out( static_cast< std::size_t >( size ) );
        if( !in.read( reinterpret_cast< char* >( out.data() ), size ) )
            return {};
        return out;
    }
}

TEST( StbTruetype, LoadSourceCodeProAndRasterizeLetterA )
{
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) )
        << "Failed to read font file at " << AAA_STB_TT_FONT_PATH
        << " (got " << font_bytes.size() << " bytes)";

    //	TTF magic check before we trust the parser. The file starts with
    //	either 0x00010000 (TrueType) or 'OTTO' / 'true' / 'typ1'. Source
    //	Code Pro is TrueType so byte 1 must be 0x01.
    ASSERT_GE( font_bytes.size(), size_t( 4 ) );
    EXPECT_EQ( font_bytes[ 0 ], 0x00 );
    EXPECT_EQ( font_bytes[ 1 ], 0x01 );

    //	stbtt_GetFontOffsetForIndex returns -1 on failure ; 0 for a
    //	single-font TTF (vs a TTC collection).
    int const offset = stbtt_GetFontOffsetForIndex( font_bytes.data(), 0 );
    ASSERT_GE( offset, 0 ) << "stbtt_GetFontOffsetForIndex rejected the file";

    stbtt_fontinfo font;
    ASSERT_NE( stbtt_InitFont( &font, font_bytes.data(), offset ), 0 )
        << "stbtt_InitFont failed";

    //	32-pixel pixel size. stbtt_ScaleForPixelHeight maps user height
    //	to internal em-units scale factor.
    float const scale = stbtt_ScaleForPixelHeight( &font, 32.0f );
    EXPECT_GT( scale, 0.0f );

    //	Rasterize 'A' to a CPU bitmap. stbtt_GetCodepointBitmap mallocs
    //	the bitmap -- we MUST stbtt_FreeBitmap it.
    int w = 0, h = 0, xoff = 0, yoff = 0;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        &font, 0.0f, scale, 'A', &w, &h, &xoff, &yoff );
    ASSERT_NE( bitmap, nullptr ) << "stbtt_GetCodepointBitmap returned null";

    //	A 32-px 'A' should produce a bitmap of reasonable size. The exact
    //	dimensions depend on the font's metrics, but for Source Code Pro
    //	Medium @ 32px we expect roughly 15-25 px wide and ~20-25 px tall.
    EXPECT_GE( w, 5 );
    EXPECT_LE( w, 64 );
    EXPECT_GE( h, 5 );
    EXPECT_LE( h, 64 );

    //	Count non-zero pixels. A legitimate 'A' rasterization at 32 px
    //	should have at least dozens of pixels with non-zero alpha
    //	(the strokes of the letter).
    int non_zero_pixels = 0;
    int max_alpha = 0;
    for( int i = 0; i < w * h; ++i )
    {
        if( bitmap[ i ] != 0 ) ++non_zero_pixels;
        if( bitmap[ i ] > max_alpha ) max_alpha = bitmap[ i ];
    }
    EXPECT_GT( non_zero_pixels, 20 )
        << "Bitmap " << w << "x" << h << " has only " << non_zero_pixels
        << " non-zero pixels -- suspicious for letter 'A'";
    EXPECT_GE( max_alpha, 200 )
        << "Bitmap max alpha " << max_alpha
        << " too low ; expected near-opaque stroke center";

    std::printf( "[StbTruetype] 'A' rasterized at 32px : %dx%d, %d non-zero "
                 "pixels, max alpha %d, xoff %d, yoff %d\n",
                 w, h, non_zero_pixels, max_alpha, xoff, yoff );

    stbtt_FreeBitmap( bitmap, nullptr );
}

TEST( StbTruetype, FontMetricsAreReasonable )
{
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    stbtt_fontinfo font;
    ASSERT_NE( stbtt_InitFont( &font, font_bytes.data(), 0 ), 0 );

    int ascent = 0, descent = 0, line_gap = 0;
    stbtt_GetFontVMetrics( &font, &ascent, &descent, &line_gap );
    //	Ascent > 0, descent < 0, line_gap >= 0. Em units -- Source Code
    //	Pro uses a 1000-unit em.
    EXPECT_GT( ascent, 0 );
    EXPECT_LT( descent, 0 );
    EXPECT_GE( line_gap, 0 );
    //	Sanity-check ranges so a future font swap that uses a different
    //	em size doesn't silently regress.
    EXPECT_LT( ascent, 5000 );
    EXPECT_GT( descent, -2500 );
}
