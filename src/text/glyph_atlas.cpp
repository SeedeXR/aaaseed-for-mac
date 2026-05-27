// src/text/glyph_atlas.cpp
//
// Implementation : wraps stbtt_PackBegin / PackFontRange / PackEnd to
// produce a printable-ASCII atlas. See header for the why.

#include "src/text/glyph_atlas.h"

#include "stb_truetype.h"

namespace aaa { namespace text {

bool build_printable_ascii_atlas(
    std::uint8_t const* font_bytes,
    std::size_t         font_size_bytes,
    float               pixel_height,
    int                 atlas_width,
    int                 atlas_height,
    GlyphAtlas&         out )
{
    if( !font_bytes || font_size_bytes == 0 )
        return false;
    if( pixel_height <= 0.0f )
        return false;
    if( atlas_width <= 0 || atlas_height <= 0 )
        return false;

    out.width  = atlas_width;
    out.height = atlas_height;
    out.alpha8.assign( static_cast< std::size_t >( atlas_width ) *
                       static_cast< std::size_t >( atlas_height ),
                       std::uint8_t{ 0 } );

    //	Pack into a 1-channel bitmap. Padding=1 keeps neighbours from
    //	bleeding into each other under bilinear filtering at draw time.
    stbtt_pack_context spc{};
    if( !stbtt_PackBegin( &spc,
                          out.alpha8.data(),
                          atlas_width,
                          atlas_height,
                          /*stride*/ 0,
                          /*padding*/ 1,
                          /*alloc_context*/ nullptr ) )
    {
        out = {};
        return false;
    }

    stbtt_packedchar packed[ kNumAsciiChars ] = {};
    int const ok = stbtt_PackFontRange(
        &spc,
        font_bytes,
        /*font_index*/ 0,
        pixel_height,
        kFirstAsciiChar,
        kNumAsciiChars,
        packed );
    stbtt_PackEnd( &spc );

    if( !ok )
    {
        out = {};
        return false;
    }

    //	Convert packedchar (atlas-pixel rect + pixel xoff/yoff/xadvance)
    //	to normalized UV + same pixel metrics, ready for the HUD shader.
    float const inv_w = 1.0f / float( atlas_width );
    float const inv_h = 1.0f / float( atlas_height );
    for( int i = 0; i < kNumAsciiChars; ++i )
    {
        stbtt_packedchar const& p = packed[ i ];
        GlyphMetric&            g = out.glyphs[ i ];
        g.u0       = float( p.x0 ) * inv_w;
        g.v0       = float( p.y0 ) * inv_h;
        g.u1       = float( p.x1 ) * inv_w;
        g.v1       = float( p.y1 ) * inv_h;
        g.xoff     = p.xoff;
        g.yoff     = p.yoff;
        g.xadvance = p.xadvance;
    }

    //	Font-wide metrics. We re-init a stbtt_fontinfo here just for
    //	the v-metrics call ; the pack API doesn't expose them. Cheap.
    stbtt_fontinfo info{};
    if( stbtt_InitFont( &info, font_bytes,
                        stbtt_GetFontOffsetForIndex( font_bytes, 0 ) ) )
    {
        int ascent = 0, descent = 0, line_gap = 0;
        stbtt_GetFontVMetrics( &info, &ascent, &descent, &line_gap );
        float const scale = stbtt_ScaleForPixelHeight( &info, pixel_height );
        out.ascent       = float( ascent )   * scale;
        out.descent      = float( descent )  * scale;
        out.line_advance = ( float( ascent ) - float( descent ) + float( line_gap ) ) * scale;
    }

    return true;
}

int layout_text_quads(
    GlyphAtlas const&        atlas,
    char const*              text,
    float                    cursor_x,
    float                    cursor_y,
    std::vector< Vertex2D >& out_vertices )
{
    out_vertices.clear();
    if( !text ) return 0;

    int quads_emitted = 0;
    for( char const* p = text; *p != '\0'; ++p )
    {
        unsigned char const c = static_cast< unsigned char >( *p );
        if( c < kFirstAsciiChar || c > kLastAsciiChar )
            continue;
        int const idx = c - kFirstAsciiChar;
        GlyphMetric const& g = atlas.glyphs[ idx ];

        //	Space (32) has empty UV rect (u0==u1). Advance only, no geom.
        bool const has_pixels = ( g.u1 > g.u0 ) && ( g.v1 > g.v0 );
        if( has_pixels )
        {
            //	Compute the quad corners in screen-space pixels.
            //	xoff / yoff are stb_truetype's offsets from the cursor
            //	baseline origin to the glyph bbox's top-left.
            float const x0 = cursor_x + g.xoff;
            float const y0 = cursor_y + g.yoff;
            //	Quad width / height = UV-rect-times-atlas-size since
            //	the packed rect is the glyph bitmap in atlas pixels.
            float const w  = ( g.u1 - g.u0 ) * static_cast< float >( atlas.width );
            float const h  = ( g.v1 - g.v0 ) * static_cast< float >( atlas.height );
            float const x1 = x0 + w;
            float const y1 = y0 + h;

            //	Two triangles, 6 vertices, winding consistent with
            //	the MetalBackend default (counter-clockwise as seen
            //	from the screen-space convention y-down).
            //	(0,0)----(1,0)
            //	  |  \      |
            //	  |    \    |
            //	(0,1)----(1,1)
            out_vertices.push_back( Vertex2D{ x0, y0, g.u0, g.v0 } );
            out_vertices.push_back( Vertex2D{ x0, y1, g.u0, g.v1 } );
            out_vertices.push_back( Vertex2D{ x1, y0, g.u1, g.v0 } );

            out_vertices.push_back( Vertex2D{ x1, y0, g.u1, g.v0 } );
            out_vertices.push_back( Vertex2D{ x0, y1, g.u0, g.v1 } );
            out_vertices.push_back( Vertex2D{ x1, y1, g.u1, g.v1 } );

            ++quads_emitted;
        }

        cursor_x += g.xadvance;
    }

    return quads_emitted;
}

float measure_text_width( GlyphAtlas const& atlas, char const* text )
{
    if( !text ) return 0.0f;
    float w = 0.0f;
    for( char const* p = text; *p != '\0'; ++p )
    {
        unsigned char const c = static_cast< unsigned char >( *p );
        if( c < kFirstAsciiChar || c > kLastAsciiChar )
            continue;
        w += atlas.glyphs[ c - kFirstAsciiChar ].xadvance;
    }
    return w;
}

}}  //  namespace aaa::text
