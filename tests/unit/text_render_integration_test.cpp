// tests/unit/text_render_integration_test.cpp
//
// End-to-end text rendering through MetalBackend (continuation 60).
// Final C-side step before the .app debug HUD wiring : proves the
// glyph-atlas + layout-helper pipeline produces actual pixels through
// the same `GOL::Backend` interface the .app uses.
//
// Pipeline under test :
//   1. Build a 256x256 alpha-8 atlas from Source Code Pro 16 px
//      (aaa::text::build_printable_ascii_atlas, c58).
//   2. Upload `atlas.alpha8` to an R8 MTLTexture
//      (MetalBackend::gen_texture_2d + texture_data_2d).
//   3. Lay out "AB" at cursor (10, 30)
//      (aaa::text::layout_text_quads, c59).
//   4. Build a vertex buffer from the 12 vertices.
//   5. Compile a minimal text MSL shader (vertex transforms pixel-space
//      xy to NDC via a screen-size uniform ; fragment samples the R8
//      atlas, outputs white-with-coverage-alpha).
//   6. Render to a 128x64 RGBA8 offscreen target with alpha-blend.
//   7. Readback pixels.
//   8. Assert : (a) the "AB" region has bright pixels, (b) regions
//      far from the text are still the clear color.
//
// This is the LAST C-side proof before AAASeedMTKView debug HUD : if
// this test passes, wiring the HUD is just "do the same thing inside
// drawInMTKView, replacing the offscreen target with the drawable".
//
// CTest label : integration;text_render.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"
#include "src/text/glyph_atlas.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#ifndef AAA_STB_TT_FONT_PATH
#error "AAA_STB_TT_FONT_PATH must be defined by CMake"
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

    //	Minimal text MSL shader. Vertex stage : pixel-space (x,y) -> NDC
    //	using a screen-size uniform at fragment-buffer slot 0 (Metal
    //	allows fragment buffers to feed vertex too via the bind helper ;
    //	for cleanness this test binds the uniform at a vertex buffer
    //	slot instead). Fragment : sample R8 atlas with bilinear ;
    //	output (1,1,1, sampled_coverage).
    constexpr char const* kTextMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VIn {
    float2 pos    [[attribute(0)]];
    float2 uv     [[attribute(1)]];
};

struct VOut {
    float4 position [[position]];
    float2 uv;
};

struct ScreenSize {
    float2 size;   // (width, height) in pixels
    float2 pad;    // 16-byte alignment safety
};

vertex VOut vs_main(
    VIn in [[stage_in]],
    constant ScreenSize& screen [[buffer(1)]] )
{
    //	Pixel-space (y-down, origin top-left) -> NDC (-1..1, y-up).
    float2 ndc;
    ndc.x =  (in.pos.x / screen.size.x) * 2.0 - 1.0;
    ndc.y = -((in.pos.y / screen.size.y) * 2.0 - 1.0);
    VOut o;
    o.position = float4( ndc, 0.0, 1.0 );
    o.uv       = in.uv;
    return o;
}

fragment float4 fs_main(
    VOut in                       [[stage_in]],
    texture2d<float> atlas        [[texture(0)]],
    sampler          atlas_sampler [[sampler(0)]] )
{
    float coverage = atlas.sample( atlas_sampler, in.uv ).r;
    return float4( 1.0, 1.0, 1.0, coverage );
}
)MSL";

    struct ScreenSize
    {
        float w, h, pad0, pad1;
    };
    static_assert( sizeof( ScreenSize ) == 16, "uniform alignment" );
}

TEST( TextRenderIntegration, AtlasUploadShaderCompileAndDrawAB )
{
    //	-------- Atlas (CPU) ---------------------------------------------
    std::vector< std::uint8_t > const font_bytes = load_file_bytes( AAA_STB_TT_FONT_PATH );
    ASSERT_GT( font_bytes.size(), size_t( 1024 ) );

    aaa::text::GlyphAtlas atlas;
    ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        /*pixel_height*/ 16.0f,
        /*atlas_width */ 256,
        /*atlas_height*/ 256,
        atlas ) );

    //	-------- Layout (CPU) --------------------------------------------
    //	Render at (10, 30) in a 128x64 target. y=30 baseline puts 16-px
    //	glyphs roughly in vertical middle.
    constexpr float kCursorX = 10.0f;
    constexpr float kCursorY = 30.0f;
    constexpr std::uint32_t kTargetW = 128;
    constexpr std::uint32_t kTargetH = 64;

    std::vector< aaa::text::Vertex2D > verts;
    int const quads = aaa::text::layout_text_quads( atlas, "AB", kCursorX, kCursorY, verts );
    ASSERT_EQ( quads, 2 );
    ASSERT_EQ( verts.size(), size_t( 12 ) );

    //	-------- Backend init --------------------------------------------
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    //	-------- Upload atlas to R8 MTLTexture ---------------------------
    GOL::TextureId atlas_tex = backend.gen_texture_2d(
        std::uint32_t( atlas.width ), std::uint32_t( atlas.height ),
        GOL::TextureFormat::R8 );
    ASSERT_NE( atlas_tex, GOL::kInvalidTextureId );
    backend.texture_data_2d( atlas_tex, atlas.alpha8.data(),
                             /*bytes_per_row*/ atlas.width );

    //	-------- Render target -------------------------------------------
    GOL::TextureId rt = backend.gen_texture_2d( kTargetW, kTargetH,
                                                GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	-------- Vertex buffer + uniform buffer --------------------------
    GOL::BufferId vb = backend.gen_buffer();
    backend.buffer_data( vb,
                         verts.size() * sizeof( aaa::text::Vertex2D ),
                         verts.data(),
                         GOL::BufferUsage::Static );

    ScreenSize screen{ float( kTargetW ), float( kTargetH ), 0.0f, 0.0f };
    GOL::BufferId ub = backend.gen_buffer();
    backend.buffer_data( ub, sizeof( screen ), &screen,
                         GOL::BufferUsage::Static );

    //	-------- Program -------------------------------------------------
    GOL::VertexAttribute attrs[ 2 ];
    attrs[ 0 ].shader_location = 0;
    attrs[ 0 ].buffer_slot     = 0;
    attrs[ 0 ].offset          = offsetof( aaa::text::Vertex2D, x );
    attrs[ 0 ].format          = GOL::VertexFormat::Float2;
    attrs[ 1 ].shader_location = 1;
    attrs[ 1 ].buffer_slot     = 0;
    attrs[ 1 ].offset          = offsetof( aaa::text::Vertex2D, u );
    attrs[ 1 ].format          = GOL::VertexFormat::Float2;

    GOL::VertexBufferLayout layouts[ 1 ];
    layouts[ 0 ].slot   = 0;
    layouts[ 0 ].stride = sizeof( aaa::text::Vertex2D );

    GOL::ProgramDesc pdesc;
    pdesc.msl_source     = kTextMsl;
    pdesc.vertex_entry   = "vs_main";
    pdesc.fragment_entry = "fs_main";
    pdesc.attributes     = attrs;
    pdesc.attribute_nb   = 2;
    pdesc.layouts        = layouts;
    pdesc.layout_nb      = 1;
    pdesc.blend_mode     = GOL::BlendMode::AlphaBlend;

    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "text shader build failed : " << backend.get_last_error();

    //	-------- Render pass ---------------------------------------------
    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	Dark blue clear so any text pixel (white-ish) stands out clearly.
    rpd.clear_color[ 0 ] = 0.05f;
    rpd.clear_color[ 1 ] = 0.05f;
    rpd.clear_color[ 2 ] = 0.20f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "text_render_integration.AB";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kTargetW ), float( kTargetH ) );
    backend.bind_program( prog );
    backend.bind_fragment_texture( atlas_tex, 0 );
    backend.bind_vertex_buffer( vb, 0, 0 );
    //	The screen-size uniform : the vertex shader reads it at
    //	buffer(1). bind_vertex_buffer at slot 1 supplies a constant
    //	buffer to the vertex stage.
    backend.bind_vertex_buffer( ub, 1, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0,
                         std::uint32_t( verts.size() ) );
    backend.end_render_pass();
    backend.present();

    //	-------- Readback + assertions -----------------------------------
    //	Sample 3 regions :
    //	1. Inside the 'A' bbox -- approx (10 + A.xoff + 4, 30 + A.yoff + 8)
    //	   should have at least one bright pixel.
    //	2. Inside the 'B' bbox -- offset by A.xadvance from 'A'.
    //	3. Far from the text (corner, e.g. (120, 60)) should still be
    //	   close to the clear color.
    auto const& A = atlas.glyphs[ 'A' - aaa::text::kFirstAsciiChar ];
    auto const& B = atlas.glyphs[ 'B' - aaa::text::kFirstAsciiChar ];

    auto sample_px = [&] ( std::uint32_t x, std::uint32_t y )
    {
        std::array< std::uint8_t, 4 > p{ 0, 0, 0, 0 };
        backend.read_texture_pixels( rt, x, y, 1, 1, p.data(), 4 );
        return p;
    };

    //	Helper : find a bright pixel inside a glyph's bounding box.
    auto find_bright_in_glyph = [&] ( aaa::text::GlyphMetric const& g, float ax, float ay )
    {
        //	Glyph quad in pixel space.
        std::uint32_t const px0 = std::uint32_t( ax + g.xoff );
        std::uint32_t const py0 = std::uint32_t( ay + g.yoff );
        float const w_px = ( g.u1 - g.u0 ) * float( atlas.width );
        float const h_px = ( g.v1 - g.v0 ) * float( atlas.height );
        std::uint32_t const px1 = std::uint32_t( ax + g.xoff + w_px );
        std::uint32_t const py1 = std::uint32_t( ay + g.yoff + h_px );

        int bright_count = 0;
        int max_r = 0;
        for( std::uint32_t y = py0; y < py1 && y < kTargetH; ++y )
        {
            for( std::uint32_t x = px0; x < px1 && x < kTargetW; ++x )
            {
                auto p = sample_px( x, y );
                int const r = p[ 0 ];
                if( r > 150 ) ++bright_count;
                if( r > max_r ) max_r = r;
            }
        }
        return std::pair{ bright_count, max_r };
    };

    auto [a_bright, a_max] = find_bright_in_glyph( A, kCursorX, kCursorY );
    auto [b_bright, b_max] = find_bright_in_glyph( B, kCursorX + A.xadvance, kCursorY );

    EXPECT_GT( a_bright, 5 )
        << "Expected at least 5 bright pixels in 'A' bbox, got " << a_bright
        << " (max R=" << a_max << ")";
    EXPECT_GT( b_bright, 5 )
        << "Expected at least 5 bright pixels in 'B' bbox, got " << b_bright
        << " (max R=" << b_max << ")";

    //	Corner far from text : should retain the clear-color blue tint.
    auto corner = sample_px( kTargetW - 4, kTargetH - 4 );
    EXPECT_LT( corner[ 0 ], 50 )  << "Corner R should be near clear (~13/255) ; got " << int( corner[ 0 ] );
    EXPECT_GT( corner[ 2 ], 20 )  << "Corner B should reflect clear-color blue ; got " << int( corner[ 2 ] );

    std::printf( "[TextRenderIntegration] 'A' bright pixels=%d max_r=%d ; "
                 "'B' bright=%d max_r=%d ; corner (R,G,B,A)=(%d,%d,%d,%d)\n",
                 a_bright, a_max, b_bright, b_max,
                 corner[ 0 ], corner[ 1 ], corner[ 2 ], corner[ 3 ] );

    //	-------- Cleanup -------------------------------------------------
    backend.delete_buffer(  ub        );
    backend.delete_buffer(  vb        );
    backend.delete_program( prog      );
    backend.delete_texture( rt        );
    backend.delete_texture( atlas_tex );
}
