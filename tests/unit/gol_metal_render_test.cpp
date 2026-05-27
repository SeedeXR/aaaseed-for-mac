// End-to-end Metal render test exercising the full GOL::Backend surface :
// compile MSL shader, set up an offscreen render target, begin a render
// pass, bind program, draw a triangle, present, read back pixels, verify
// the GPU actually drew what we asked.
//
// This is the proof-of-life test for the Metal backend. If it passes, we
// can render with Metal end-to-end from C++ on Apple Silicon.

#include <gtest/gtest.h>

#define NS_PRIVATE_IMPLEMENTATION_NO  // we DON'T emit; aaaseed_gol_metal has the impl
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <array>

namespace
{
    //	Minimal MSL shader pair : full-screen red triangle. Uses [[vertex_id]]
    //	so no vertex buffer needed (3 hard-coded positions). The triangle's
    //	vertices are outside the [-1,1] NDC range so when clipped to it, the
    //	visible region is fully covered in red.
    constexpr char const* kFullScreenRedMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VertexOut
{
    float4 position [[position]];
};

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[3];
    verts[0] = float2( -1.0, -3.0 );
    verts[1] = float2( -1.0,  1.0 );
    verts[2] = float2(  3.0,  1.0 );
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    return out;
}

fragment float4 fs_main()
{
    return float4( 1.0, 0.0, 0.0, 1.0 );
}
)MSL";

    //	Read a single pixel out of a shared-storage MTL::Texture (RGBA8).
    //	Backend tests use shared storage so the CPU side can read directly
    //	via getBytes without a blit encoder.
    void read_pixel_rgba8( GOL::MetalBackend& backend, GOL::TextureId id,
                            std::uint32_t x, std::uint32_t y, std::uint8_t out[ 4 ] )
    {
        //	Translate the engine-level id back to MTL::Texture for the
        //	getBytes call. We could add a backend method for this but for
        //	test-only readback the friend-like access is justified.
        MTL::Device* dev = backend.get_device();
        ASSERT_NE( dev, nullptr );
        //	The backend stores textures in its own map ; getBytes needs the
        //	MTL::Texture* directly. We'd add a backend accessor in real
        //	code ; for now use the public accessor pattern via re-create...
        //	Actually : we expose the texture pointer through a backend-only
        //	test helper for now. Simpler : just call getBytes on the active
        //	texture by re-resolving. Use the backend's "get last texture"
        //	semantic — see below.
        (void) backend;
        (void) id;
        (void) x;
        (void) y;
        out[ 0 ] = out[ 1 ] = out[ 2 ] = out[ 3 ] = 0;
    }
}

TEST( GolMetalRender, CompileFullScreenRedShader )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    GOL::ProgramId prog = backend.create_program_msl( kFullScreenRedMsl,
                                                      "vs_main",
                                                      "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "MSL compile failed : " << backend.get_last_error();

    backend.delete_program( prog );
}

TEST( GolMetalRender, BadMslReportsCompileError )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    char const* broken_msl = "this is not valid MSL";
    GOL::ProgramId prog = backend.create_program_msl( broken_msl, "vs_main", "fs_main" );
    EXPECT_EQ( prog, GOL::kInvalidProgramId );
    EXPECT_FALSE( backend.get_last_error().empty() );
}

TEST( GolMetalRender, RenderTriangleClearsAndCompletes )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 64;
    constexpr std::uint32_t H = 64;
    GOL::TextureId target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( target, GOL::kInvalidTextureId );

    GOL::ProgramId prog = backend.create_program_msl( kFullScreenRedMsl, "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "shader compile failed : " << backend.get_last_error();

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;   // start green so we can tell clear ran
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	If we reach here without crashing, the GPU executed the command
    //	buffer. Real pixel verification requires backend-internal access
    //	to the MTL::Texture* ; that test lives in the dedicated readback
    //	test below.
    backend.delete_program( prog );
    backend.delete_texture( target );
    SUCCEED();
}

TEST( GolMetalRender, RenderTriangleAndReadPixelsAreRed )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 64;
    constexpr std::uint32_t H = 64;
    GOL::TextureId target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( target, GOL::kInvalidTextureId );

    GOL::ProgramId prog = backend.create_program_msl( kFullScreenRedMsl, "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;   // blue clear ; if the triangle didn't draw, we'd see blue
    rpd.clear_color[ 3 ] = 1.0f;

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	The full-screen-red shader covers the entire [-1,1] NDC box, so
    //	EVERY pixel of the target should be opaque red. Sample at the
    //	four corners + centre to make sure clipping, viewport, and
    //	rasterisation all behaved.
    std::array< std::uint8_t, 4 > pixel{};

    constexpr std::uint32_t sample_x[] = { 0,     W / 2, W - 1, 0,       W - 1 };
    constexpr std::uint32_t sample_y[] = { 0,     H / 2, H - 1, H - 1,   0     };
    for( std::size_t i = 0; i < std::size( sample_x ); ++i )
    {
        backend.read_texture_pixels( target, sample_x[ i ], sample_y[ i ], 1, 1, pixel.data(), 4 );
        EXPECT_EQ( pixel[ 0 ], 255 ) << "R at (" << sample_x[ i ] << "," << sample_y[ i ] << ")";
        EXPECT_EQ( pixel[ 1 ],   0 ) << "G at (" << sample_x[ i ] << "," << sample_y[ i ] << ")";
        EXPECT_EQ( pixel[ 2 ],   0 ) << "B at (" << sample_x[ i ] << "," << sample_y[ i ] << ")";
        EXPECT_EQ( pixel[ 3 ], 255 ) << "A at (" << sample_x[ i ] << "," << sample_y[ i ] << ")";
    }

    backend.delete_program( prog );
    backend.delete_texture( target );
}

TEST( GolMetalRender, ClearOnlyProducesClearColor )
{
    //	Render a pass with no draw call. The texture should hold exactly
    //	the clear color afterwards. Proves clear_color path independently
    //	of any shader work.
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 16;
    constexpr std::uint32_t H = 16;
    GOL::TextureId target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( target, GOL::kInvalidTextureId );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 32.f / 255.f;
    rpd.clear_color[ 1 ] = 64.f / 255.f;
    rpd.clear_color[ 2 ] = 128.f / 255.f;
    rpd.clear_color[ 3 ] = 1.0f;

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
    backend.end_render_pass();
    backend.present();

    std::array< std::uint8_t, 4 > pixel{};
    backend.read_texture_pixels( target, 8, 8, 1, 1, pixel.data(), 4 );
    EXPECT_EQ( pixel[ 0 ],  32 );
    EXPECT_EQ( pixel[ 1 ],  64 );
    EXPECT_EQ( pixel[ 2 ], 128 );
    EXPECT_EQ( pixel[ 3 ], 255 );

    backend.delete_texture( target );
}

TEST( GolMetalRender, DrawElementsIndexedTriangleIsGreen )
{
    //	Indexed full-screen triangle, green output, with REAL vertex
    //	descriptor wired through ProgramDesc.
    constexpr char const* indexed_msl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VertexIn  { float2 pos [[attribute(0)]]; };
struct VertexOut { float4 position [[position]]; };

vertex VertexOut vs_main( VertexIn in [[stage_in]] )
{
    VertexOut out;
    out.position = float4( in.pos, 0.0, 1.0 );
    return out;
}

fragment float4 fs_main()
{
    return float4( 0.0, 1.0, 0.0, 1.0 );
}
)MSL";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 32;
    constexpr std::uint32_t H = 32;
    GOL::TextureId target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );

    float const verts[] = {
        -1.f, -3.f,
        -1.f,  1.f,
         3.f,  1.f,
    };
    std::uint16_t const indices[] = { 0, 1, 2 };

    GOL::BufferId vb = backend.gen_buffer();
    backend.buffer_data( vb, sizeof( verts ), verts, GOL::BufferUsage::Static );

    GOL::BufferId ib = backend.gen_buffer();
    backend.buffer_data( ib, sizeof( indices ), indices, GOL::BufferUsage::Static );

    GOL::VertexAttribute attrs[ 1 ];
    attrs[ 0 ].shader_location = 0;
    attrs[ 0 ].buffer_slot     = 0;
    attrs[ 0 ].offset          = 0;
    attrs[ 0 ].format          = GOL::VertexFormat::Float2;

    GOL::VertexBufferLayout layouts[ 1 ];
    layouts[ 0 ].slot   = 0;
    layouts[ 0 ].stride = sizeof( float ) * 2;

    GOL::ProgramDesc pdesc;
    pdesc.msl_source     = indexed_msl;
    pdesc.vertex_entry   = "vs_main";
    pdesc.fragment_entry = "fs_main";
    pdesc.attributes     = attrs;
    pdesc.attribute_nb   = 1;
    pdesc.layouts        = layouts;
    pdesc.layout_nb      = 1;

    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "indexed-draw shader build failed : " << backend.get_last_error();

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;   // start red ; if the triangle didn't draw, we'd see red
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.f, 0.f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.bind_vertex_buffer( vb, 0, 0 );
    backend.draw_elements( GOL::PrimitiveType::Triangles, 3, GOL::IndexType::UInt16, ib, 0 );
    backend.end_render_pass();
    backend.present();

    std::array< std::uint8_t, 4 > pixel{};
    backend.read_texture_pixels( target, W / 2, H / 2, 1, 1, pixel.data(), 4 );
    EXPECT_EQ( pixel[ 0 ],   0 );
    EXPECT_EQ( pixel[ 1 ], 255 );
    EXPECT_EQ( pixel[ 2 ],   0 );
    EXPECT_EQ( pixel[ 3 ], 255 );

    backend.delete_program( prog );
    backend.delete_buffer( vb );
    backend.delete_buffer( ib );
    backend.delete_texture( target );
}

TEST( GolMetalRender, TexturedQuadSamplesCorrectPixel )
{
    //	Upload a 2-pixel-wide texture : left half red, right half blue.
    //	Render a full-screen quad sampling it via fragment-shader `tex.sample(s, uv)`.
    //	Verify : left-side screen pixel reads red, right-side reads blue.
    //	Proves bind_fragment_texture + default sampler + MSL texture
    //	sampling all work end-to-end through the abstract Backend.
    constexpr char const* textured_msl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VertexOut
{
    float4 position [[position]];
    float2 uv;
};

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    //	Full-screen triangle covering [-1,1] NDC ; UVs in [0,1].
    float2 verts[3];
    verts[0] = float2( -1.0, -3.0 );
    verts[1] = float2( -1.0,  1.0 );
    verts[2] = float2(  3.0,  1.0 );
    float2 uvs[3];
    uvs[0] = float2( 0.0,  2.0 );
    uvs[1] = float2( 0.0,  0.0 );
    uvs[2] = float2( 2.0,  0.0 );
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

fragment float4 fs_main( VertexOut in [[stage_in]],
                         texture2d<float> tex [[texture(0)]],
                         sampler           s   [[sampler(0)]] )
{
    return tex.sample( s, in.uv );
}
)MSL";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    //	Build the 2x1 texture : pixel (0,0) red, pixel (1,0) blue.
    constexpr std::uint32_t TW = 2;
    constexpr std::uint32_t TH = 1;
    GOL::TextureId tex = backend.gen_texture_2d( TW, TH, GOL::TextureFormat::RGBA8 );
    std::uint8_t tex_pixels[] = {
        255,   0,   0, 255,    // (0,0) red
          0,   0, 255, 255,    // (1,0) blue
    };
    backend.texture_data_2d( tex, tex_pixels, TW * 4 );

    GOL::ProgramId prog = backend.create_program_msl( textured_msl, "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "textured-quad shader build failed : " << backend.get_last_error();

    constexpr std::uint32_t W = 64;
    constexpr std::uint32_t H = 64;
    GOL::TextureId target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.f;
    rpd.clear_color[ 1 ] = 1.f;
    rpd.clear_color[ 2 ] = 0.f;   // green = "we didn't sample the texture"
    rpd.clear_color[ 3 ] = 1.f;

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.f, 0.f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.bind_fragment_texture( tex, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	UV(0.25, 0.5) hits the left half of the 2x1 texture → red.
    //	UV(0.75, 0.5) hits the right half → blue.
    //	UV maps : x_uv = screen_x / W. So left-quarter screen pixel
    //	samples u=0.125 → tex pixel (0,0) → red.
    std::array< std::uint8_t, 4 > left{}, right{};
    backend.read_texture_pixels( target, W / 4,     H / 2, 1, 1, left.data(),  4 );
    backend.read_texture_pixels( target, 3 * W / 4, H / 2, 1, 1, right.data(), 4 );

    //	Linear filtering (the backend's default sampler) mixes neighbouring
    //	texels when the UV doesn't land exactly on a texel centre. UV(0.258)
    //	at the left sample picks up ~1.5% of the blue neighbour, so the
    //	pixel is ~ (251, 0, 4). Allow generous tolerance ; the key check is
    //	"red dominant on left, blue dominant on right".
    EXPECT_GT( left[ 0 ],  200 ) << "left sample expected RED-dominant";
    EXPECT_LT( left[ 2 ],   50 ) << "left sample expected low B";
    EXPECT_LT( right[ 0 ],  50 ) << "right sample expected low R";
    EXPECT_GT( right[ 2 ], 200 ) << "right sample expected BLUE-dominant";

    backend.delete_program( prog );
    backend.delete_texture( tex );
    backend.delete_texture( target );
}

TEST( GolMetalRender, DepthTestCloserOccludesFarther )
{
    //	Two full-screen quads, drawn back-to-front in NDC z. With depth
    //	test (LessEqual) enabled, the second (closer) quad should
    //	overwrite the first. Then with depth test disabled, the LAST
    //	draw should win regardless of z. Two-pass test pins both paths.
    constexpr char const* z_param_msl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; };

vertex VertexOut vs_main( uint vid [[vertex_id]],
                          constant float& z [[buffer(0)]] )
{
    float2 verts[3];
    verts[0] = float2( -1.0, -3.0 );
    verts[1] = float2( -1.0,  1.0 );
    verts[2] = float2(  3.0,  1.0 );
    VertexOut out;
    out.position = float4( verts[ vid ], z, 1.0 );
    return out;
}

fragment float4 fs_red()   { return float4( 1.0, 0.0, 0.0, 1.0 ); }
fragment float4 fs_green() { return float4( 0.0, 1.0, 0.0, 1.0 ); }
)MSL";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 32;
    constexpr std::uint32_t H = 32;
    GOL::TextureId color_tex = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    GOL::TextureId depth_tex = backend.gen_texture_2d( W, H, GOL::TextureFormat::Depth32F );
    ASSERT_NE( depth_tex, GOL::kInvalidTextureId );

    //	Two programs : same MSL source, different fragment entry points,
    //	both with depth_test_enabled=true.
    GOL::ProgramDesc pdesc_red;
    pdesc_red.msl_source           = z_param_msl;
    pdesc_red.vertex_entry         = "vs_main";
    pdesc_red.fragment_entry       = "fs_red";
    pdesc_red.depth_test_enabled   = true;
    pdesc_red.depth_write_enabled  = true;
    pdesc_red.depth_compare        = GOL::DepthCompare::LessEqual;

    GOL::ProgramDesc pdesc_green = pdesc_red;
    pdesc_green.fragment_entry = "fs_green";

    GOL::ProgramId prog_red   = backend.create_program( pdesc_red );
    GOL::ProgramId prog_green = backend.create_program( pdesc_green );
    ASSERT_NE( prog_red,   GOL::kInvalidProgramId ) << backend.get_last_error();
    ASSERT_NE( prog_green, GOL::kInvalidProgramId ) << backend.get_last_error();

    //	Per-draw z uniform.
    GOL::BufferId z_far  = backend.gen_buffer();
    GOL::BufferId z_near = backend.gen_buffer();
    float const z_far_val  = 0.7f;
    float const z_near_val = 0.2f;
    backend.buffer_data( z_far,  sizeof( float ), &z_far_val,  GOL::BufferUsage::Static );
    backend.buffer_data( z_near, sizeof( float ), &z_near_val, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment       = color_tex;
    rpd.depth_attachment       = depth_tex;
    rpd.load_action            = GOL::LoadAction::Clear;
    rpd.depth_load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.f;
    rpd.clear_color[ 1 ] = 0.f;
    rpd.clear_color[ 2 ] = 1.f;   // blue base ; if both quads skip, we'd see blue
    rpd.clear_color[ 3 ] = 1.f;
    rpd.clear_depth      = 1.0f;

    //	Pass 1 : red at z=0.7 (far), then green at z=0.2 (near). Both
    //	pass depth test on first draw, green's depth (0.2) < red's
    //	(0.7) so green writes over red. Result : green.
    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.f, 0.f, float( W ), float( H ) );

    backend.bind_program( prog_red );
    backend.bind_vertex_buffer( z_far, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );

    backend.bind_program( prog_green );
    backend.bind_vertex_buffer( z_near, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );

    backend.end_render_pass();
    backend.present();

    std::array< std::uint8_t, 4 > p{};
    backend.read_texture_pixels( color_tex, W / 2, H / 2, 1, 1, p.data(), 4 );
    EXPECT_EQ( p[ 0 ],   0 ) << "expected green-dominant after near covers far";
    EXPECT_EQ( p[ 1 ], 255 );
    EXPECT_EQ( p[ 2 ],   0 );

    //	Pass 2 : green at z=0.2 (near) first, then red at z=0.7 (far).
    //	Green writes its 0.2 depth. Red's z=0.7 fails the LessEqual
    //	test against the existing 0.2, so red is discarded. Result :
    //	still green. This is the actual depth-test proof — the order
    //	of draws does NOT determine the visible pixel ; depth does.
    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.f, 0.f, float( W ), float( H ) );

    backend.bind_program( prog_green );
    backend.bind_vertex_buffer( z_near, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );

    backend.bind_program( prog_red );
    backend.bind_vertex_buffer( z_far, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );

    backend.end_render_pass();
    backend.present();

    backend.read_texture_pixels( color_tex, W / 2, H / 2, 1, 1, p.data(), 4 );
    EXPECT_EQ( p[ 0 ],   0 ) << "depth test should discard later far-quad";
    EXPECT_EQ( p[ 1 ], 255 );
    EXPECT_EQ( p[ 2 ],   0 );

    backend.delete_program( prog_red );
    backend.delete_program( prog_green );
    backend.delete_buffer( z_far );
    backend.delete_buffer( z_near );
    backend.delete_texture( color_tex );
    backend.delete_texture( depth_tex );
}

TEST( GolMetalRender, AlphaBlendBlendsOverClearColor )
{
    //	Clear to red. Then render a 50%-alpha green quad on top with
    //	BlendMode::AlphaBlend. Result should be the alpha-blended mix.
    //	Proves BlendMode::AlphaBlend wires correctly into the pipeline.
    constexpr char const* half_alpha_green_msl = R"MSL(
#include <metal_stdlib>
using namespace metal;

vertex float4 vs_main( uint vid [[vertex_id]] )
{
    float2 verts[3];
    verts[0] = float2( -1.0, -3.0 );
    verts[1] = float2( -1.0,  1.0 );
    verts[2] = float2(  3.0,  1.0 );
    return float4( verts[ vid ], 0.0, 1.0 );
}

fragment float4 fs_main()
{
    //	pre-multiplied : alpha=0.5, color=(0,0.5,0)
    return float4( 0.0, 1.0, 0.0, 0.5 );
}
)MSL";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 32;
    constexpr std::uint32_t H = 32;
    GOL::TextureId target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );

    GOL::ProgramDesc pdesc;
    pdesc.msl_source     = half_alpha_green_msl;
    pdesc.vertex_entry   = "vs_main";
    pdesc.fragment_entry = "fs_main";
    pdesc.blend_mode     = GOL::BlendMode::AlphaBlend;

    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "alpha-blend shader build failed : " << backend.get_last_error();

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;   // red background
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.f, 0.f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	Expected blend math :
    //	  dst.r = src.r * src.a + dst.r * (1 - src.a)
    //	  dst.r = 0.0 * 0.5 + 1.0 * 0.5 = 0.5 ⇒ ~127 in u8
    //	  dst.g = 1.0 * 0.5 + 0.0 * 0.5 = 0.5 ⇒ ~127 in u8
    //	Use a tolerance of ±2 for float-to-u8 conversion rounding.
    std::array< std::uint8_t, 4 > p{};
    backend.read_texture_pixels( target, W / 2, H / 2, 1, 1, p.data(), 4 );
    EXPECT_NEAR( int( p[ 0 ] ), 127, 2 ) << "blended R";
    EXPECT_NEAR( int( p[ 1 ] ), 127, 2 ) << "blended G";
    EXPECT_EQ(    p[ 2 ],         0 )    << "blended B (no contribution from src)";

    backend.delete_program( prog );
    backend.delete_texture( target );
}

TEST( GolMetalRender, PerVertexColorInterpolation )
{
    //	Triangle with three differently-coloured vertices. Fragment
    //	shader receives the rasterizer-interpolated color. We assert
    //	that pixels near each vertex are biased toward that vertex's
    //	color — proves the vertex-attribute pipeline is plumbed
    //	correctly end-to-end.
    constexpr char const* per_vertex_color_msl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VertexIn
{
    float2 pos       [[attribute(0)]];
    float4 color     [[attribute(1)]];
};

struct VertexOut
{
    float4 position [[position]];
    float4 color;
};

vertex VertexOut vs_main( VertexIn in [[stage_in]] )
{
    VertexOut out;
    out.position = float4( in.pos, 0.0, 1.0 );
    out.color    = in.color;
    return out;
}

fragment float4 fs_main( VertexOut in [[stage_in]] )
{
    return in.color;
}
)MSL";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 64;
    constexpr std::uint32_t H = 64;
    GOL::TextureId target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );

    //	Interleaved vertex format : [float2 pos | UChar4Norm color] per
    //	vertex, stride = 8 + 4 = 12 bytes. Three vertices :
    //	  vertex 0 : (-1,-1) red
    //	  vertex 1 : ( 1,-1) green
    //	  vertex 2 : ( 0, 1) blue
    struct Vert { float x, y; std::uint8_t r, g, b, a; };
    Vert const verts[] = {
        { -1.f, -1.f,  255,   0,   0, 255 },
        {  1.f, -1.f,    0, 255,   0, 255 },
        {  0.f,  1.f,    0,   0, 255, 255 },
    };
    static_assert( sizeof( Vert ) == 12, "interleaved vertex stride drift" );

    GOL::BufferId vb = backend.gen_buffer();
    backend.buffer_data( vb, sizeof( verts ), verts, GOL::BufferUsage::Static );

    GOL::VertexAttribute attrs[ 2 ];
    attrs[ 0 ].shader_location = 0;
    attrs[ 0 ].buffer_slot     = 0;
    attrs[ 0 ].offset          = offsetof( Vert, x );
    attrs[ 0 ].format          = GOL::VertexFormat::Float2;

    attrs[ 1 ].shader_location = 1;
    attrs[ 1 ].buffer_slot     = 0;
    attrs[ 1 ].offset          = offsetof( Vert, r );
    attrs[ 1 ].format          = GOL::VertexFormat::UChar4Norm;

    GOL::VertexBufferLayout layouts[ 1 ];
    layouts[ 0 ].slot   = 0;
    layouts[ 0 ].stride = sizeof( Vert );

    GOL::ProgramDesc pdesc;
    pdesc.msl_source     = per_vertex_color_msl;
    pdesc.vertex_entry   = "vs_main";
    pdesc.fragment_entry = "fs_main";
    pdesc.attributes     = attrs;
    pdesc.attribute_nb   = 2;
    pdesc.layouts        = layouts;
    pdesc.layout_nb      = 1;

    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "per-vertex-color shader build failed : " << backend.get_last_error();

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.f, 0.f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.bind_vertex_buffer( vb, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	The triangle in NDC :
    //	  vertex 0 at (-1, -1) → screen (0,    H-1) — RED corner
    //	  vertex 1 at ( 1, -1) → screen (W-1,  H-1) — GREEN corner
    //	  vertex 2 at ( 0,  1) → screen (W/2,  0)   — BLUE corner (Y inverted)
    //
    //	Note Metal's NDC has Y up but texture origin is top-left, so the
    //	mapping inverts Y when sampling. We test the actual screen-space
    //	positions, not the NDC ones.
    auto sample = [ & ]( std::uint32_t x, std::uint32_t y )
    {
        std::array< std::uint8_t, 4 > p{};
        backend.read_texture_pixels( target, x, y, 1, 1, p.data(), 4 );
        return p;
    };

    //	Near each vertex screen-position, the dominant channel should
    //	be that vertex's primary color. Use generous tolerance because
    //	rasterizer interpolation pulls neighbouring vertex contributions.
    auto p_red    = sample( 3,        H - 4    );  // near bottom-left
    auto p_green  = sample( W - 4,    H - 4    );  // near bottom-right
    auto p_blue   = sample( W / 2,    3        );  // near top-middle

    EXPECT_GT( p_red[ 0 ],   128 ) << "red corner expected R-dominant";
    EXPECT_LT( p_red[ 1 ],   128 );
    EXPECT_LT( p_red[ 2 ],   128 );

    EXPECT_LT( p_green[ 0 ], 128 ) << "green corner expected G-dominant";
    EXPECT_GT( p_green[ 1 ], 128 );
    EXPECT_LT( p_green[ 2 ], 128 );

    EXPECT_LT( p_blue[ 0 ],  128 ) << "blue corner expected B-dominant";
    EXPECT_LT( p_blue[ 1 ],  128 );
    EXPECT_GT( p_blue[ 2 ],  128 );

    backend.delete_program( prog );
    backend.delete_buffer( vb );
    backend.delete_texture( target );
}

//	GPU debug-marker integration smoke test. Exercises the
//	RenderPassDescriptor::debug_label path : the backend must call
//	pushDebugGroup at begin_render_pass and the matching popDebugGroup
//	at end_render_pass without leaking the encoder or crashing the
//	command-buffer commit. We can't observe Xcode Frame Capture from a
//	gtest TU ; this catches lifecycle regressions in the marker code.
TEST( GolMetalRender, DebugLabelPushesAndPopsCleanly )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 32;
    constexpr std::uint32_t H = 32;
    GOL::TextureId target = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( target, GOL::kInvalidTextureId );

    GOL::ProgramId prog = backend.create_program_msl( kFullScreenRedMsl,
                                                     "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.debug_label      = "test.debug_label_smoke";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.f, 0.f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	Second pass with debug_label == nullptr — must NOT emit a stray
    //	popDebugGroup (would unbalance the encoder and crash on commit).
    rpd.debug_label = nullptr;
    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.f, 0.f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    backend.delete_program( prog );
    backend.delete_texture( target );
}
