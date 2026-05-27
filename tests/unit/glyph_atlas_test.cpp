// tests/unit/glyph_atlas_test.cpp
//
// Unit test for the printable-ASCII glyph atlas helper (continuation
// 58). First building block toward the .app debug HUD : verifies the
// atlas builds correctly from SourceCodePro-Medium.ttf at 16 px into
// a 256x256 alpha-8 bitmap, every printable ASCII has a non-degenerate
// packed rect, and the atlas is non-empty.
//
// Saves the built atlas as a PNG fixture under the build dir so a
// developer can visually inspect it (`open out/.../bin/glyph_atlas.png`)
// to catch packing regressions that a numeric assertion would miss.
//
// CTest label : unit;glyph_atlas.

#include <gtest/gtest.h>

#include "src/text/glyph_atlas.h"

#include "stb_image_write.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <vector>

#ifndef AAA_STB_TT_FONT_PATH
#error "AAA_STB_TT_FONT_PATH must be defined by CMake to point at SourceCodePro-Medium.ttf"
#endif

#ifndef AAA_GLYPH_ATLAS_OUT_DIR
#error "AAA_GLYPH_ATLAS_OUT_DIR must be defined by CMake -- where to write the debug PNG"
#endif

namespace
{
    std::vector< std::uint8_t > load_file_bytes( char const* path )
    {
        std::ifstream in( path, std::ios::binary | std::ios::ate );
        if( !in ) return {};
        auto const size = in.tellg();
        in.seekg( 0, std::ios::beg );
        std::vector< std::uint8_t > out( static_cast< std::size_t >( size ) );
        if( !in.read( reinterpret_cast< char* >( out.data() ), size ) )
            return {};
        return out;
    }
}

TEST( GlyphAtlas, BuildSourceCodeProAt16px )
{
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    aaa::text::GlyphAtlas atlas;
    bool const ok = aaa::text::build_printable_ascii_atlas(
        font_bytes.data(),
        font_bytes.size(),
        /*pixel_height*/ 16.0f,
        /*atlas_width */ 256,
        /*atlas_height*/ 256,
        atlas );
    ASSERT_TRUE( ok ) << "atlas build failed";

    EXPECT_EQ( atlas.width, 256 );
    EXPECT_EQ( atlas.height, 256 );
    EXPECT_EQ( atlas.alpha8.size(), size_t( 256 * 256 ) );

    //	Font metrics : Source Code Pro Medium at 16 px should have
    //	ascent in the 11-15 range, descent in [-5, -2], line_advance
    //	in [16, 24] (em-scaled). Loose checks -- they validate sign +
    //	rough magnitude.
    EXPECT_GT( atlas.ascent, 5.0f );
    EXPECT_LT( atlas.ascent, 20.0f );
    EXPECT_LT( atlas.descent, 0.0f );
    EXPECT_GT( atlas.descent, -10.0f );
    EXPECT_GT( atlas.line_advance, 10.0f );
    EXPECT_LT( atlas.line_advance, 30.0f );

    //	Every printable ASCII should have a non-degenerate UV rect AND
    //	positive xadvance. Space (32) may have zero coverage (u0==u1) but
    //	still has positive advance ; treat it specially.
    int with_pixels = 0;
    for( int i = 0; i < aaa::text::kNumAsciiChars; ++i )
    {
        aaa::text::GlyphMetric const& g = atlas.glyphs[ i ];
        int const codepoint = aaa::text::kFirstAsciiChar + i;
        SCOPED_TRACE( "codepoint = " + std::to_string( codepoint ) );

        //	Advance must be positive for every printable char.
        EXPECT_GT( g.xadvance, 0.0f );
        //	UVs must be in [0, 1].
        EXPECT_GE( g.u0, 0.0f );  EXPECT_LE( g.u1, 1.0f );
        EXPECT_GE( g.v0, 0.0f );  EXPECT_LE( g.v1, 1.0f );

        if( g.u1 > g.u0 && g.v1 > g.v0 )
            ++with_pixels;
    }
    //	Space has no pixels ; every other printable does. Expect at
    //	least 90 of 95 glyphs to have pixel coverage.
    EXPECT_GE( with_pixels, 90 );

    //	Atlas must be non-empty -- sum of all alpha values is well above
    //	a stray-pixel noise floor.
    long total_alpha = 0;
    for( std::uint8_t v : atlas.alpha8 )
        total_alpha += v;
    EXPECT_GT( total_alpha, 10000 )
        << "atlas alpha-sum " << total_alpha << " suspiciously low";

    std::printf( "[GlyphAtlas] %dx%d, %d glyphs with pixels, ascent=%.2f "
                 "descent=%.2f line_adv=%.2f total_alpha=%ld\n",
                 atlas.width, atlas.height, with_pixels,
                 atlas.ascent, atlas.descent, atlas.line_advance, total_alpha );

    //	Save the atlas as a PNG under the build dir for visual inspection.
    //	One-channel grayscale PNG -- stb_image_write supports comp=1.
    std::string const out_path = std::string( AAA_GLYPH_ATLAS_OUT_DIR ) + "/glyph_atlas.png";
    int const write_ok = stbi_write_png(
        out_path.c_str(),
        atlas.width,
        atlas.height,
        /*channels*/ 1,
        atlas.alpha8.data(),
        /*stride*/  atlas.width );
    EXPECT_NE( write_ok, 0 )
        << "Failed to write atlas PNG to " << out_path;
    if( write_ok )
        std::printf( "[GlyphAtlas] wrote debug atlas PNG to %s\n", out_path.c_str() );
}

TEST( GlyphAtlas, RejectsZeroSizeAtlas )
{
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    aaa::text::GlyphAtlas atlas;
    EXPECT_FALSE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        16.0f, 0, 256, atlas ) );
    EXPECT_FALSE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        16.0f, 256, 0, atlas ) );
    EXPECT_FALSE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        0.0f, 256, 256, atlas ) );
    EXPECT_FALSE( aaa::text::build_printable_ascii_atlas(
        nullptr, 0, 16.0f, 256, 256, atlas ) );
}

TEST( GlyphAtlas, LayoutTextQuadsAdvancesCursor )
{
    //	Build a reusable atlas for the layout tests below.
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    aaa::text::GlyphAtlas atlas;
    ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        16.0f, 256, 256, atlas ) );

    //	"AB" -- both printable, both have pixel coverage.
    std::vector< aaa::text::Vertex2D > verts;
    int const quads = aaa::text::layout_text_quads(
        atlas, "AB", /*cursor_x*/ 10.0f, /*cursor_y*/ 20.0f, verts );

    EXPECT_EQ( quads, 2 );
    EXPECT_EQ( verts.size(), size_t( 12 ) );  //  2 quads * 6 vertices

    //	First quad's top-left corner = cursor + first glyph's xoff/yoff.
    aaa::text::GlyphMetric const& A = atlas.glyphs[ 'A' - aaa::text::kFirstAsciiChar ];
    EXPECT_FLOAT_EQ( verts[ 0 ].x, 10.0f + A.xoff );
    EXPECT_FLOAT_EQ( verts[ 0 ].y, 20.0f + A.yoff );
    EXPECT_FLOAT_EQ( verts[ 0 ].u, A.u0 );
    EXPECT_FLOAT_EQ( verts[ 0 ].v, A.v0 );

    //	Second quad's top-left x should reflect 'A's xadvance.
    aaa::text::GlyphMetric const& B = atlas.glyphs[ 'B' - aaa::text::kFirstAsciiChar ];
    float const expected_b_x = 10.0f + A.xadvance + B.xoff;
    EXPECT_FLOAT_EQ( verts[ 6 ].x, expected_b_x );
}

TEST( GlyphAtlas, LayoutSpaceAdvancesWithoutGeometry )
{
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    aaa::text::GlyphAtlas atlas;
    ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        16.0f, 256, 256, atlas ) );

    std::vector< aaa::text::Vertex2D > verts;
    int const quads = aaa::text::layout_text_quads(
        atlas, "A B", /*cursor_x*/ 0.0f, /*cursor_y*/ 0.0f, verts );

    //	"A B" : A and B emit quads ; the space between only advances.
    EXPECT_EQ( quads, 2 );

    //	The B quad should be offset from A by (xadvance_A + xadvance_space + B.xoff).
    aaa::text::GlyphMetric const& A     = atlas.glyphs[ 'A' - aaa::text::kFirstAsciiChar ];
    aaa::text::GlyphMetric const& space = atlas.glyphs[ ' ' - aaa::text::kFirstAsciiChar ];
    aaa::text::GlyphMetric const& B     = atlas.glyphs[ 'B' - aaa::text::kFirstAsciiChar ];
    float const expected_b_x = A.xadvance + space.xadvance + B.xoff;
    EXPECT_FLOAT_EQ( verts[ 6 ].x, expected_b_x );
}

TEST( GlyphAtlas, LayoutSkipsNonPrintableBytes )
{
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    aaa::text::GlyphAtlas atlas;
    ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        16.0f, 256, 256, atlas ) );

    //	Embed a newline byte (10) -- outside [32,126]. Layout should
    //	silently skip it.
    char const noisy[] = { 'A', '\n', 'B', '\0' };
    std::vector< aaa::text::Vertex2D > verts;
    int const quads = aaa::text::layout_text_quads(
        atlas, noisy, /*cursor_x*/ 0.0f, /*cursor_y*/ 0.0f, verts );
    EXPECT_EQ( quads, 2 );
}

TEST( GlyphAtlas, LayoutEmptyStringEmitsNoGeometry )
{
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    aaa::text::GlyphAtlas atlas;
    ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        16.0f, 256, 256, atlas ) );

    std::vector< aaa::text::Vertex2D > verts;
    EXPECT_EQ( aaa::text::layout_text_quads( atlas, "", 0.0f, 0.0f, verts ), 0 );
    EXPECT_EQ( verts.size(), size_t( 0 ) );

    EXPECT_EQ( aaa::text::layout_text_quads( atlas, nullptr, 0.0f, 0.0f, verts ), 0 );
    EXPECT_EQ( verts.size(), size_t( 0 ) );
}

TEST( GlyphAtlas, MeasureTextWidthMatchesLayoutAdvances )
{
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    aaa::text::GlyphAtlas atlas;
    ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        16.0f, 256, 256, atlas ) );

    //	Empty / null returns 0.
    EXPECT_FLOAT_EQ( aaa::text::measure_text_width( atlas, "" ),      0.0f );
    EXPECT_FLOAT_EQ( aaa::text::measure_text_width( atlas, nullptr ), 0.0f );

    //	Non-printable bytes contribute 0 ; \n + 'A' == 'A' alone.
    char const just_a[]  = "A";
    char const newline_a[] = "\nA";
    EXPECT_FLOAT_EQ(
        aaa::text::measure_text_width( atlas, just_a ),
        aaa::text::measure_text_width( atlas, newline_a ) );

    //	measure_text_width should equal sum of xadvances == cursor
    //	position after layout consumes all glyphs.
    char const* const sample = "FPS=99.9 frame=42";
    float const measured = aaa::text::measure_text_width( atlas, sample );

    std::vector< aaa::text::Vertex2D > verts;
    aaa::text::layout_text_quads( atlas, sample, 0.0f, 0.0f, verts );
    //	The last quad's right-edge x is approximately the cursor
    //	position after the last glyph EXCEPT the trailing xadvance has
    //	already been applied. Sum xadvance directly for cross-check.
    float sum_xadv = 0.0f;
    for( char const* p = sample; *p != '\0'; ++p )
    {
        unsigned char const c = static_cast< unsigned char >( *p );
        if( c < aaa::text::kFirstAsciiChar ) continue;
        if( c > aaa::text::kLastAsciiChar  ) continue;
        sum_xadv += atlas.glyphs[ c - aaa::text::kFirstAsciiChar ].xadvance;
    }
    EXPECT_FLOAT_EQ( measured, sum_xadv );
    EXPECT_GT( measured, 0.0f );

    //	Sanity-check : the visible bbox of the LAST emitted quad ends
    //	at less than (but close to) measured width. The trailing glyph
    //	rasterizes inside the cursor cell ; its right edge is at
    //	cursor_x + xoff + glyph_width <= measured.
    if( !verts.empty() )
    {
        float max_x = 0.0f;
        for( auto const& v : verts )
            if( v.x > max_x ) max_x = v.x;
        EXPECT_LE( max_x, measured + 1.0f );  //  +1px slack for rasteriser bbox padding
    }
}
