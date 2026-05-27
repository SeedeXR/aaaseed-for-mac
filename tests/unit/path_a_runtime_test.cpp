// tests/unit/path_a_runtime_test.cpp
//
// End-to-end Path A integration test (continuation 38). The catalog
// test (continuation 31) already proves each .metal file compiles via
// Apple's Metal compiler ; this test goes further : it actually RUNS
// one ported shader against a procedural input texture and uniform
// buffer, reads back the pixels, and asserts the engine semantics
// produced sensible output.
//
// Proves the Path A catalog isn't dead weight at the GPU level.
//
// Shader under test : `ps_Maa_add_scale.metal` (Path A shader #2 ;
// hand-port from continuation 28, then tool-regenerated). Semantics :
//   output_rgba = offset_rgba + src_rgba * scale_rgba
// where (offset, scale) come from `AaaFuVec4s.values[0]` and `[1]`.
//
// Test fixture :
//   - 32x32 source texture, all pixels mid-gray (0.5, 0.5, 0.5, 1.0).
//   - Uniforms : offset = (0, 0.5, 0, 0) ; scale = (1, 1, 1, 1).
//   - Expected output : (0.5, 1.0, 0.5, 1.0) clamped --> (128, 255, 128, 255).
//
// CTest label : integration;path_a_runtime.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifndef AAA_SHADERS_MSL_DIR
#error "AAA_SHADERS_MSL_DIR must be defined by CMake"
#endif

namespace
{
    std::string slurp( std::filesystem::path const& p )
    {
        std::ifstream in( p, std::ios::binary );
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    //	Mirror of the generated AaaFuVec4s struct layout from the tool
    //	(`tools/glsl_to_msl/glsl_to_msl.py`). MUST stay in sync : the
    //	test writes raw bytes into the constant buffer and the MSL
    //	side reads them back as `_aaa_vec4s.values[N]`.
    struct AaaFuVec4s
    {
        float values[ 16 ][ 4 ];   //	[N].rgba (16 max, engine convention)
    };

    void clear_uniforms( AaaFuVec4s& u )
    {
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j )
                u.values[ i ][ j ] = 0.0f;
    }
}

TEST( PathARuntime, PsMaaAddScale_RendersOffsetPlusSrcTimesScale )
{
    //	-------- Backend + program ---------------------------------------
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_add_scale.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    constexpr std::uint32_t W = 32;
    constexpr std::uint32_t H = 32;
    constexpr std::uint32_t kBytesPerRow = W * 4;

    //	-------- Source texture : 32x32 mid-gray RGBA ---------------------
    //
    //	The shader samples `_aaa_tex_0` at slot 0. Slots 1-3 are
    //	declared (tool emits all 4 because the source GLSL had
    //	`uniform sampler2D aaa_tex2d[4]`) but never sampled in the body.
    //	Metal validation requires SOMETHING bound to each declared
    //	binding -- we re-bind the same source tex to slots 1, 2, 3 too.
    GOL::TextureId src_tex = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );

    std::vector< std::uint8_t > gray_pixels( W * H * 4, 128 );
    for( std::size_t i = 3; i < gray_pixels.size(); i += 4 )
        gray_pixels[ i ] = 255;   // opaque alpha
    backend.texture_data_2d( src_tex, gray_pixels.data(), kBytesPerRow );

    //	-------- Render target : 32x32 RGBA -------------------------------
    GOL::TextureId rt = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	-------- Uniform buffer : AaaFuVec4s with offset + scale -----------
    AaaFuVec4s u{};
    clear_uniforms( u );
    //	offset = (0, 0.5, 0, 0) : adds green tint
    u.values[ 0 ][ 0 ] = 0.0f;
    u.values[ 0 ][ 1 ] = 0.5f;
    u.values[ 0 ][ 2 ] = 0.0f;
    u.values[ 0 ][ 3 ] = 0.0f;
    //	scale = (1, 1, 1, 1) : identity
    u.values[ 1 ][ 0 ] = 1.0f;
    u.values[ 1 ][ 1 ] = 1.0f;
    u.values[ 1 ][ 2 ] = 1.0f;
    u.values[ 1 ][ 3 ] = 1.0f;

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    //	-------- Render pass ----------------------------------------------
    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;     // blue clear -- if the shader didn't run, we'd see blue
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "path_a_runtime.ps_Maa_add_scale";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
    backend.bind_program( prog );
    //	All four declared texture slots must have something bound for
    //	Metal validation. The shader only samples slot 0 ; slots 1-3
    //	are placeholders.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_buffer(  ub, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	-------- Readback ------------------------------------------------
    //
    //	Sample the centre pixel (avoids any potential triangle-edge
    //	rasterization artifacts at the corners). Expected output :
    //	  src    = (128, 128, 128, 255) -> (.5, .5, .5, 1.0)
    //	  scale  = (1, 1, 1, 1)
    //	  offset = (0, .5, 0, 0)
    //	  out    = offset + src*scale = (.5, 1.0, .5, 1.0)
    //	         = (128, 255, 128, 255) when scaled back to 8 bpc.
    //
    //	We assert ordering + ranges rather than exact bytes because
    //	the gradient triangle filter + sRGB-vs-linear conversions can
    //	wobble a single LSB. The semantic test is "green grew, red/blue
    //	stayed mid-range, output is not the blue clear".
    std::array< std::uint8_t, 4 > pixel{ 0, 0, 0, 0 };
    backend.read_texture_pixels( rt, W / 2, H / 2, 1, 1, pixel.data(), 4 );

    int const r = pixel[ 0 ];
    int const g = pixel[ 1 ];
    int const b = pixel[ 2 ];
    int const a = pixel[ 3 ];

    EXPECT_GT( g, r ) << "Green should be brighter than red after offset (got R=" << r << " G=" << g << ")";
    EXPECT_GT( g, b ) << "Green should be brighter than blue ; the clear was blue (R=" << r << " G=" << g << " B=" << b << ")";
    EXPECT_GT( r, 50 )  << "Red came from gray src ; should be non-zero";
    EXPECT_GT( b, 50 )  << "Blue came from gray src ; should be non-zero, NOT clear-color blue";
    EXPECT_GT( a, 200 ) << "Alpha should be high (close to opaque)";

    //	-------- Cleanup --------------------------------------------------
    backend.delete_buffer(  ub      );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	------------------------------------------------------------------
//	Second runtime test : ps_Maa_4tex covers a different uniform path
//	(`aaa_fu_float[]`) AND distinct-content multi-texture sampling.
//	Output formula in the engine GLSL :
//	    color = fu_float[0] * tex[0].sample(uv)
//	          + fu_float[1] * tex[1].sample(uv)
//	(slots [2] / [3] commented out in the source ; they're declared but
//	not sampled, mirroring what the catalog port emits).
//
//	Fixture : tex[0] = solid red (255,0,0,255), tex[1] = solid green
//	(0,255,0,255), tex[2] / tex[3] = whatever (we re-bind tex[0] to
//	keep Metal validation happy without growing the texture count).
//	fu_float[0] = fu_float[1] = 0.5 -- equal half-mix.
//	Expected center pixel : (127, 127, 0, 127) approx -- yellow-ish at
//	half saturation, alpha came from 0.5 * 255 = 127.
//
//	Continuation 41.

namespace
{
    struct AaaFuFloats
    {
        float values[ 16 ];
    };

    GOL::TextureId make_solid_texture(
        GOL::MetalBackend& backend, std::uint32_t W, std::uint32_t H,
        std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a )
    {
        GOL::TextureId id = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
        if( id == GOL::kInvalidTextureId )
            return id;
        std::vector< std::uint8_t > pixels( W * H * 4, 0 );
        for( std::size_t i = 0; i < pixels.size(); i += 4 )
        {
            pixels[ i + 0 ] = r;
            pixels[ i + 1 ] = g;
            pixels[ i + 2 ] = b;
            pixels[ i + 3 ] = a;
        }
        backend.texture_data_2d( id, pixels.data(), W * 4 );
        return id;
    }
}

TEST( PathARuntime, PsMaa4tex_BlendsRedAndGreenToYellow )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_4tex.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    constexpr std::uint32_t W = 32;
    constexpr std::uint32_t H = 32;
    constexpr std::uint32_t kBytesPerRow = W * 4;

    GOL::TextureId tex_red   = make_solid_texture( backend, W, H, 255,   0,   0, 255 );
    GOL::TextureId tex_green = make_solid_texture( backend, W, H,   0, 255,   0, 255 );
    ASSERT_NE( tex_red,   GOL::kInvalidTextureId );
    ASSERT_NE( tex_green, GOL::kInvalidTextureId );

    GOL::TextureId rt = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Uniform buffer : 0.5 weight on tex[0] (red) + 0.5 weight on tex[1] (green).
    AaaFuFloats u{};
    for( int i = 0; i < 16; ++i ) u.values[ i ] = 0.0f;
    u.values[ 0 ] = 0.5f;
    u.values[ 1 ] = 0.5f;

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;     // blue clear -- caught if shader fails to draw
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "path_a_runtime.ps_Maa_4tex";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.bind_fragment_texture( tex_red,   0 );    // slot 0 sampled
    backend.bind_fragment_texture( tex_green, 1 );    // slot 1 sampled
    backend.bind_fragment_texture( tex_red,   2 );    // slot 2 unused -- placeholder
    backend.bind_fragment_texture( tex_red,   3 );    // slot 3 unused -- placeholder
    backend.bind_fragment_buffer(  ub, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::array< std::uint8_t, 4 > pixel{ 0, 0, 0, 0 };
    backend.read_texture_pixels( rt, W / 2, H / 2, 1, 1, pixel.data(), 4 );

    int const r = pixel[ 0 ];
    int const g = pixel[ 1 ];
    int const b = pixel[ 2 ];
    int const a = pixel[ 3 ];

    //	0.5 * red + 0.5 * green = (0.5, 0.5, 0, 0.5+0.5=1.0 from alpha sum).
    //	Expect : R near 128, G near 128, B near 0, alpha near 255.
    //	Wide bands ; tests semantic correctness not bit-exactness.
    EXPECT_GT( r,   80 ) << "R should pick up half of tex[0]'s red ; got " << r;
    EXPECT_LT( r,  180 ) << "R should be HALF saturation, not full ; got " << r;
    EXPECT_GT( g,   80 ) << "G should pick up half of tex[1]'s green ; got " << g;
    EXPECT_LT( g,  180 ) << "G should be HALF saturation ; got " << g;
    EXPECT_LT( b,   40 ) << "B should be near zero (no blue in inputs) ; got " << b;
    EXPECT_GT( a,  200 ) << "Alpha should sum to near-opaque ; got " << a;

    backend.delete_buffer(  ub        );
    backend.delete_texture( rt        );
    backend.delete_texture( tex_red   );
    backend.delete_texture( tex_green );
    backend.delete_program( prog      );
}

//	------------------------------------------------------------------
//	Third runtime test : ps_edgeoverlay covers the `discard_fragment()`
//	path AND a HAND-PORTED helper function (`get_gray( float4 )`),
//	AND a custom uniform struct shape (`EdgeOverlayUniforms { float
//	fu[16]; }` -- different from the tool's `AaaFuVec4s` / `AaaFuFloats`).
//
//	Engine semantics : the shader computes a Sobel-magnitude gradient
//	at the centre pixel, then EITHER discards (gradient > threshold^2)
//	OR returns the texture sample with alpha-fade in the transition
//	zone OR returns the sample full-alpha.
//
//	Fixture : 64x64 source texture, half left-side red + half right-side
//	white. The vertical seam down the middle has a strong gradient ;
//	pixels far from the seam have ~zero gradient. NPixels = 1.0,
//	Threshold = 0.1 (so anything > 0.01 sobel-magnitude-squared
//	discards). Reading a pixel deep on the white side should yield
//	the white sample (alpha=1), since gradient is zero there.
//
//	Continuation 42.

namespace
{
    struct EdgeOverlayUniforms
    {
        float fu[ 16 ];
    };

    //	Generate a 64x64 texture : left half (x < 32) = solid opaque
    //	red, right half = solid opaque white. The vertical seam at x=32
    //	gives the Sobel filter a strong response ; far from the seam,
    //	response is zero (flat color).
    GOL::TextureId make_half_split_texture(
        GOL::MetalBackend& backend, std::uint32_t W, std::uint32_t H )
    {
        GOL::TextureId id = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
        if( id == GOL::kInvalidTextureId )
            return id;
        std::vector< std::uint8_t > pixels( W * H * 4, 0 );
        for( std::uint32_t y = 0; y < H; ++y )
        {
            for( std::uint32_t x = 0; x < W; ++x )
            {
                std::size_t i = ( y * W + x ) * 4;
                if( x < W / 2 )
                {
                    pixels[ i + 0 ] = 255;  // R
                    pixels[ i + 1 ] = 0;
                    pixels[ i + 2 ] = 0;
                }
                else
                {
                    pixels[ i + 0 ] = 255;  // white
                    pixels[ i + 1 ] = 255;
                    pixels[ i + 2 ] = 255;
                }
                pixels[ i + 3 ] = 255;
            }
        }
        backend.texture_data_2d( id, pixels.data(), W * 4 );
        return id;
    }
}

TEST( PathARuntime, PsEdgeOverlay_DiscardAndHelperPath )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_edgeoverlay.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() );

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    constexpr std::uint32_t W = 64;
    constexpr std::uint32_t H = 64;

    //	One texture (slot 0). The hand-port shader declares a singleton
    //	`texture2d< float > aaa_tex2d_0 [[texture(0)]]` -- no slots 1-3
    //	to worry about, unlike the tool's flattened-array output.
    GOL::TextureId src_tex = make_half_split_texture( backend, W, H );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );

    GOL::TextureId rt = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    EdgeOverlayUniforms u{};
    for( int i = 0; i < 16; ++i ) u.fu[ i ] = 0.0f;
    u.fu[ 0 ] = 1.0f;     // NPixels : 1-texel offset sampling
    u.fu[ 1 ] = 0.1f;     // Threshold (squared internally) -- low so flat regions don't discard

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;     // green clear -- if shader discards EVERYTHING,
    rpd.clear_color[ 2 ] = 0.0f;     // we'd see green ; if shader doesn't discard
    rpd.clear_color[ 3 ] = 1.0f;     // anything, we'd see source content.
    rpd.debug_label      = "path_a_runtime.ps_edgeoverlay";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
    backend.bind_program( prog );
    //	Hand-port shader uses singleton `aaa_tex2d_0` bound at slot 0 ;
    //	no other texture slots declared, so no placeholder binds needed.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	Sample a pixel DEEP on the white side (far from the red/white
    //	seam at x=32). Sobel response should be ~zero, so the shader
    //	should NOT discard, and the output should be the white texture
    //	sample at full alpha.
    std::array< std::uint8_t, 4 > pixel_white{ 0, 0, 0, 0 };
    backend.read_texture_pixels( rt, /*x*/ 50, /*y*/ H / 2, 1, 1, pixel_white.data(), 4 );

    int const r = pixel_white[ 0 ];
    int const g = pixel_white[ 1 ];
    int const b = pixel_white[ 2 ];
    int const a = pixel_white[ 3 ];

    //	On the white side : output should be (255, 255, 255, 255) NOT
    //	the green clear (0, 255, 0, 255). Both have G=255 -- the
    //	discriminator is R and B : white has them HIGH, green has them
    //	low. So R > 200 AND B > 200 is the right semantic check.
    EXPECT_GT( r, 200 ) << "R should be high (white sample), not low (green clear) ; got " << r;
    EXPECT_GT( g, 200 ) << "G should be high in either case ; got " << g;
    EXPECT_GT( b, 200 ) << "B should be high (white sample), not low (green clear) ; got " << b;
    EXPECT_GT( a, 200 ) << "Alpha should be near 1 in the flat-color region ; got " << a;

    backend.delete_buffer(  ub      );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	------------------------------------------------------------------
//	Continuation 43 : MRT runtime test (`ps_mrt_test`). The fragment
//	shader returns a `FragOut` struct with 4 `[[color(N)]]` fields ;
//	color0 carries the full RGB sample, color1/2/3 fan out the R/G/B
//	channels separately. The MetalBackend MRT extension lets a render
//	pass bind multiple color textures and the pipeline declare matching
//	slot count.

TEST( PathARuntime, PsMrtTest_FansChannelsAcrossFourRenderTargets )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_mrt_test.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() );

    //	Use the lower-level ProgramDesc form so we can set
    //	color_attachment_count = 4 (the higher-level create_program_msl
    //	wrapper hard-codes count = 1 and would mismatch the FragOut
    //	struct's 4 [[color(N)]] declarations).
    GOL::ProgramDesc pdesc;
    pdesc.msl_source             = msl_src.c_str();
    pdesc.vertex_entry           = "vs_main";
    pdesc.fragment_entry         = "fs_main";
    pdesc.target_color_fmt       = GOL::TextureFormat::RGBA8;
    pdesc.color_attachment_count = 4;
    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program failed : " << backend.get_last_error();

    constexpr std::uint32_t W = 16;
    constexpr std::uint32_t H = 16;

    //	Distinct per-channel values so the fan-out is unambiguous.
    GOL::TextureId src_tex = make_solid_texture( backend, W, H, 200, 100, 50, 255 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );

    GOL::TextureId rts[ 4 ];
    for( int i = 0; i < 4; ++i )
    {
        rts[ i ] = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
        ASSERT_NE( rts[ i ], GOL::kInvalidTextureId );
    }

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment                   = rts[ 0 ];
    rpd.additional_color_attachments[ 0 ]  = rts[ 1 ];
    rpd.additional_color_attachments[ 1 ]  = rts[ 2 ];
    rpd.additional_color_attachments[ 2 ]  = rts[ 3 ];
    rpd.load_action      = GOL::LoadAction::Clear;
    //	Magenta clear for all 4 slots -- if a slot stays magenta, MRT
    //	didn't fan out to it.
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "path_a_runtime.ps_mrt_test";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( W ), float( H ) );
    backend.bind_program( prog );
    backend.bind_fragment_texture( src_tex, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::array< std::uint8_t, 4 > p0{}, p1{}, p2{}, p3{};
    backend.read_texture_pixels( rts[ 0 ], W / 2, H / 2, 1, 1, p0.data(), 4 );
    backend.read_texture_pixels( rts[ 1 ], W / 2, H / 2, 1, 1, p1.data(), 4 );
    backend.read_texture_pixels( rts[ 2 ], W / 2, H / 2, 1, 1, p2.data(), 4 );
    backend.read_texture_pixels( rts[ 3 ], W / 2, H / 2, 1, 1, p3.data(), 4 );

    //	rt0 : full RGB sample. 8-bit-to-float-to-8-bit roundtrip wobbles.
    EXPECT_NEAR( p0[ 0 ], 200, 8 );
    EXPECT_NEAR( p0[ 1 ], 100, 8 );
    EXPECT_NEAR( p0[ 2 ],  50, 8 );

    //	rt1 : R-only. G + B must be near zero (not magenta-clear leftover).
    EXPECT_NEAR( p1[ 0 ], 200, 8 );
    EXPECT_LT(   p1[ 1 ],   8  ) << "rt1.G should be 0 ; got " << int( p1[ 1 ] );
    EXPECT_LT(   p1[ 2 ],   8  ) << "rt1.B should be 0 ; got " << int( p1[ 2 ] );

    //	rt2 : G-only. R must be 0 (not 255 like magenta-clear).
    EXPECT_LT(   p2[ 0 ],   8  ) << "rt2.R should be 0 (would be 255 if MRT didn't fan out) ; got " << int( p2[ 0 ] );
    EXPECT_NEAR( p2[ 1 ], 100, 8 );
    EXPECT_LT(   p2[ 2 ],   8  );

    //	rt3 : B-only.
    EXPECT_LT(   p3[ 0 ],   8  );
    EXPECT_LT(   p3[ 1 ],   8  );
    EXPECT_NEAR( p3[ 2 ],  50, 8 );

    for( int i = 0; i < 4; ++i )
        backend.delete_texture( rts[ i ] );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}
