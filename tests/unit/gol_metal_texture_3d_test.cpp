// 3D texture smoke tests for `GOL::MetalBackend`. Proves :
//   1. `gen_texture_3d` creates a sampleable MTL::Texture of type 3D.
//   2. The initial-upload path correctly stages a tightly-packed RGBA8
//      volume into the texture (bytesPerImage = bytes_per_row * h).
//   3. An MSL `texture3d<float>` sampler in a fragment shader can read
//      back the uploaded data ; pixels in the rendered target encode the
//      sampled slice color (one z-slice → one solid color).
//
// This is the gating test that unblocks `vol3d_Plane` / `vol3d_Voxel`
// shader baselining : if the backend can author and sample a 3D texture,
// those shaders' volumetric inputs can be supplied at test time.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <array>
#include <cstdint>
#include <vector>

namespace
{
    //	Sample a 4×4×4 RGBA8 volume across a full-screen triangle covering
    //	a 3×1 RGBA8 render target. Each output pixel column corresponds to
    //	a different z-slice of the volume :
    //	  pixel x=0 → z = 0.125 → slice 0
    //	  pixel x=1 → z = 0.375 → slice 1
    //	  pixel x=2 → z = 0.625 → slice 2
    //	The fragment shader recovers its pixel-x from `[[position]]` (in
    //	pixel coords for a render-target) and uses (pixel_x + 0.5) / 3 to
    //	get a stable 0..1 range, then maps that to the slice z. Using a
    //	full-screen triangle (not points) sidesteps the [[point_size]]
    //	requirement and exercises the same texture3d sample path.
    constexpr char const* kVol3dSampleMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct V2F
{
    float4 position [[position]];
};

vertex V2F vs_main( uint vid [[vertex_id]] )
{
    //	Standard oversized full-screen triangle covering the [-1,1]² NDC.
    float2 verts[3];
    verts[0] = float2( -1.0, -3.0 );
    verts[1] = float2( -1.0,  1.0 );
    verts[2] = float2(  3.0,  1.0 );
    V2F out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    return out;
}

fragment float4 fs_main( V2F in [[stage_in]],
                         texture3d<float> vol [[texture(1)]],
                         sampler          s   [[sampler(0)]] )
{
    //	pos.x is in pixel coords (0.5, 1.5, 2.5 for a 3-wide target).
    //	Map to z-slice centres : 0.125, 0.375, 0.625 (for D=4 volume).
    float px = in.position.x;
    int slice = int( floor( px ) );        // 0, 1, or 2
    float z = ( float( slice ) + 0.5 ) / 4.0;
    return vol.sample( s, float3( 0.5, 0.5, z ) );
}
)MSL";
}

TEST( GolMetalTexture3D, GenTexture3DReturnsValidIdForNullData )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    GOL::TextureId vol = backend.gen_texture_3d( 4, 4, 4, GOL::TextureFormat::RGBA8,
                                                  /*data*/ nullptr );
    EXPECT_NE( vol, GOL::kInvalidTextureId );

    backend.delete_texture( vol );
}

TEST( GolMetalTexture3D, GenTexture3DRejectsZeroDimension )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    EXPECT_EQ( backend.gen_texture_3d( 0, 4, 4, GOL::TextureFormat::RGBA8, nullptr ),
               GOL::kInvalidTextureId );
    EXPECT_EQ( backend.gen_texture_3d( 4, 0, 4, GOL::TextureFormat::RGBA8, nullptr ),
               GOL::kInvalidTextureId );
    EXPECT_EQ( backend.gen_texture_3d( 4, 4, 0, GOL::TextureFormat::RGBA8, nullptr ),
               GOL::kInvalidTextureId );
}

TEST( GolMetalTexture3D, SampleVolumeReadsBackPerSliceColors )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    //	Build a 4×4×4 RGBA8 volume where every voxel in z-slice i carries
    //	a slice-distinctive color :
    //	  slice 0 → red    (255,0,0,255)
    //	  slice 1 → green  (0,255,0,255)
    //	  slice 2 → blue   (0,0,255,255)
    //	  slice 3 → yellow (255,255,0,255)
    //	Layout : tightly packed, x fastest, then y, then z.
    constexpr std::uint32_t W = 4;
    constexpr std::uint32_t H = 4;
    constexpr std::uint32_t D = 4;
    std::vector< std::uint8_t > volume( W * H * D * 4, 0 );
    std::uint8_t const slice_color[ D ][ 4 ] = {
        { 255,   0,   0, 255 },
        {   0, 255,   0, 255 },
        {   0,   0, 255, 255 },
        { 255, 255,   0, 255 },
    };
    for( std::uint32_t z = 0; z < D; ++z )
        for( std::uint32_t y = 0; y < H; ++y )
            for( std::uint32_t x = 0; x < W; ++x )
            {
                std::size_t const idx = ( ( z * H + y ) * W + x ) * 4;
                volume[ idx + 0 ] = slice_color[ z ][ 0 ];
                volume[ idx + 1 ] = slice_color[ z ][ 1 ];
                volume[ idx + 2 ] = slice_color[ z ][ 2 ];
                volume[ idx + 3 ] = slice_color[ z ][ 3 ];
            }

    GOL::TextureId vol = backend.gen_texture_3d( W, H, D, GOL::TextureFormat::RGBA8,
                                                  volume.data() );
    ASSERT_NE( vol, GOL::kInvalidTextureId );

    //	1×3 render target — one pixel per sampled slice.
    constexpr std::uint32_t TW = 3;
    constexpr std::uint32_t TH = 1;
    GOL::TextureId target = backend.gen_texture_2d( TW, TH, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( target, GOL::kInvalidTextureId );

    GOL::ProgramId prog = backend.create_program_msl( kVol3dSampleMsl,
                                                      "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "vol3d MSL compile failed : " << backend.get_last_error();

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action = GOL::LoadAction::Clear;
    //	Magenta clear — if any pixel reads back magenta, the draw didn't
    //	cover it. Slice colors are pure primaries, so magenta is
    //	unambiguously the clear path.
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( TW ), float( TH ) );
    backend.bind_program( prog );
    backend.bind_fragment_texture_3d( vol, /*slot*/ 1 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	Read back the 3 sampled pixels and verify each matches its slice.
    std::array< std::uint8_t, TW * TH * 4 > pixels{};
    backend.read_texture_pixels( target, 0, 0, TW, TH, pixels.data(), TW * 4 );

    //	Pixel 0 should be slice 0 → red.
    EXPECT_EQ( pixels[ 0 ], 255 ) << "pixel0 R";
    EXPECT_EQ( pixels[ 1 ],   0 ) << "pixel0 G";
    EXPECT_EQ( pixels[ 2 ],   0 ) << "pixel0 B";
    //	Pixel 1 → slice 1 → green.
    EXPECT_EQ( pixels[ 4 ],   0 ) << "pixel1 R";
    EXPECT_EQ( pixels[ 5 ], 255 ) << "pixel1 G";
    EXPECT_EQ( pixels[ 6 ],   0 ) << "pixel1 B";
    //	Pixel 2 → slice 2 → blue.
    EXPECT_EQ( pixels[ 8 ],   0 ) << "pixel2 R";
    EXPECT_EQ( pixels[ 9 ],   0 ) << "pixel2 G";
    EXPECT_EQ( pixels[ 10 ], 255 ) << "pixel2 B";

    backend.delete_program( prog );
    backend.delete_texture( vol );
    backend.delete_texture( target );
}
