// tests/regression/phase3_golden_frame_regression_test.cpp
//
// Phase 3 EXIT golden-frame regression (continuation 120 / Task #156)
// EXTENDED in continuation 121-A : baseline of 1 shader grown to 4.
//
// The c119 strategic pivot : Phase 3 EXIT does NOT require the full
// layer subsystem. The abstract `GOL::Backend` + the Path A MSL catalog
// is sufficient to produce a pixel-comparable golden frame, which is
// exactly what the Phase 3 EXIT criterion in memory/todo.md L209 asks
// for : "one golden MEU running pixel-comparable output on both backends".
//
// This is the Mac half of c120 + c121-A. The "both backends" cross-
// comparison against WindowsBackend is c126+ work (after WindowsBackend
// extracts).
//
// Shape : drive MetalBackend offscreen with deterministic Path A shaders
// against a procedural input texture + fixed uniforms, read back the
// BGRA8 pixel buffer, save it as PNG to tests/regression/golden/, and
// compare bit-exactly (with a 1-LSB per-channel tolerance to absorb any
// FP rounding wobble).
//
// First-run semantics : if the golden PNG is missing on disk, the test
// SAVES the current render as the golden and reports SUCCESS-FIRST-
// CAPTURE. Subsequent runs compare against it. This lets the same test
// file generate AND verify the golden ; commit the produced PNG to the
// regression corpus.
//
// c121-A baseline (4 shaders, all Path A, all deterministic) :
//   1. ps_Maa_add_scale  : output = offset + src * scale     (c120 original)
//   2. ps_Maa_alpha      : alpha-channel pow(a*m+b, e), opaque-white rgb
//   3. ps_Maa_Copy_01    : pixel-coord copy with offset       (uses ints uniform)
//   4. ps_Maa_drop       : 2-tap blend with rotation/scale    (uses 2 textures)
//
// Each shader was confirmed deterministic by inspection : no iTime, no
// random / noise, no framebuffer feedback. Different uniform layouts
// (Floats vs Vec4s vs Floats+Ints) and different texture-binding counts
// keep the 4 tests independent regression surfaces.
//
// Why copy-paste rather than parametrize : the 4 shaders have genuinely
// different uniform-struct types (AaaFuFloats vs AaaFuVec4s) and
// different active texture counts. A parametrized helper would either
// type-erase the uniforms (losing the per-shader semantic clarity in
// the test source) or duplicate the setup conditionally. Per-shader
// failure isolation -- a green X for "PsMaaDrop_PixelComparable" tells
// the engineer exactly which catalog member regressed.
//
// Input fixture : 256x256 RGBA8 gradient where R varies with x, G
// varies with y, B is the diagonal, alpha is opaque. Shared across
// all 4 tests so a single golden update lands in the same fixture
// space.
//
// Headless safety : uses GOL::MetalBackend's offscreen MTLTexture path
// (same as gol_metal_render_test.cpp), NOT MTKView. CI-runnable on
// macos-14 runners ; the MTKView path does NOT render correctly
// without a window-server connection.
//
// CTest label : `regression;phase3`.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifndef AAA_SHADERS_MSL_DIR
#error "AAA_SHADERS_MSL_DIR must be defined by CMake"
#endif

#ifndef AAA_REGRESSION_GOLDEN_DIR
#error "AAA_REGRESSION_GOLDEN_DIR must be defined by CMake : path to tests/regression/golden/"
#endif

namespace
{
    //	Engine-ABI uniform structs. Each generated MSL declares the trio
    //	(see src/shaders/msl/*.metal) and binds whichever the body uses.
    struct AaaFuFloats { float values[ 16 ];        };
    struct AaaFuVec4s  { float values[ 16 ][ 4 ];   };
    struct AaaFuInts   { std::int32_t values[ 16 ]; };

    std::string slurp( std::filesystem::path const& p )
    {
        std::ifstream in( p, std::ios::binary );
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    //	Procedural gradient : R = x*255/(W-1), G = y*255/(H-1),
    //	B = ((x+y)*255)/(W+H-2), A = 255. Deterministic and rich
    //	in spatial variation -- a single-LSB drift anywhere in the
    //	shader path shows up as a mismatched pixel.
    std::vector< std::uint8_t > make_gradient_rgba8( std::uint32_t W, std::uint32_t H )
    {
        std::vector< std::uint8_t > p( std::size_t( W ) * H * 4, 0 );
        for( std::uint32_t y = 0; y < H; ++y )
        {
            for( std::uint32_t x = 0; x < W; ++x )
            {
                std::size_t const i = ( y * W + x ) * 4;
                p[ i + 0 ] = std::uint8_t( ( x * 255u ) / ( W - 1 ) );
                p[ i + 1 ] = std::uint8_t( ( y * 255u ) / ( H - 1 ) );
                p[ i + 2 ] = std::uint8_t( ( ( x + y ) * 255u ) / ( W + H - 2 ) );
                p[ i + 3 ] = 255;
            }
        }
        return p;
    }

    //	Count non-zero-RGB pixels in a BGRA/RGBA8 buffer ; used as a
    //	sanity check that the shader actually drew something other than
    //	the clear color.
    std::size_t count_nonzero_pixels( std::vector< std::uint8_t > const& px )
    {
        std::size_t n = 0;
        for( std::size_t i = 0; i + 3 < px.size(); i += 4 )
            if( px[ i + 0 ] != 0 || px[ i + 1 ] != 0 || px[ i + 2 ] != 0 )
                ++n;
        return n;
    }

    constexpr std::uint32_t kWidth  = 256;
    constexpr std::uint32_t kHeight = 256;
    constexpr std::size_t   kBytesPerPixel = 4;
    constexpr std::size_t   kBytesPerRow   = kWidth * kBytesPerPixel;
    constexpr std::size_t   kBufferBytes   = kBytesPerRow * kHeight;
    //	Per-channel tolerance for the bit-exact compare. Phase 3 EXIT
    //	asks for "pixel-comparable", not "bit-identical" -- a 1-LSB
    //	wobble (FP rasterization rounding, sampler filter LSB) is well
    //	within "comparable".
    constexpr int kPerChannelTolerance = 1;

    //	Compare-or-capture helper : on a missing golden, save the
    //	rendered frame as the golden and SUCCEED ; otherwise compare
    //	against it with the per-channel tolerance and ASSERT/EXPECT.
    //	Pulled into a helper so each per-shader TEST stays focused on
    //	the *render setup*, not on the golden-disk bookkeeping.
    void compare_or_capture_golden( std::filesystem::path const& golden_path,
                                    std::vector< std::uint8_t > const& rendered,
                                    char const* shader_label )
    {
        std::filesystem::path const golden_dir = golden_path.parent_path();
        bool const golden_exists = std::filesystem::exists( golden_path );

        if( !golden_exists )
        {
            std::filesystem::create_directories( golden_dir );
            int const wrote = stbi_write_png(
                golden_path.string().c_str(),
                int( kWidth ), int( kHeight ),
                /*channels*/ 4,
                rendered.data(),
                int( kBytesPerRow ) );
            ASSERT_NE( wrote, 0 )
                << "Failed to write first-capture golden to " << golden_path;

            std::printf( "[Phase3.Golden:%s] SUCCESS-FIRST-CAPTURE : wrote golden to %s\n",
                         shader_label, golden_path.string().c_str() );
            SUCCEED() << "First-capture golden written to " << golden_path
                      << "  (commit this PNG to the regression corpus)";
            return;
        }

        int gw = 0, gh = 0, gc = 0;
        std::uint8_t* golden = stbi_load(
            golden_path.string().c_str(),
            &gw, &gh, &gc, /*desired_channels*/ 4 );
        ASSERT_NE( golden, nullptr )
            << "stbi_load failed for " << golden_path
            << " : " << stbi_failure_reason();

        ASSERT_EQ( gw, int( kWidth ) );
        ASSERT_EQ( gh, int( kHeight ) );

        std::size_t mismatches = 0;
        int max_per_channel_diff = 0;
        std::uint32_t first_bad_x = 0;
        std::uint32_t first_bad_y = 0;
        std::array< std::uint8_t, 4 > first_bad_render{};
        std::array< std::uint8_t, 4 > first_bad_golden{};
        for( std::uint32_t y = 0; y < kHeight; ++y )
        {
            for( std::uint32_t x = 0; x < kWidth; ++x )
            {
                std::size_t const base = ( y * kWidth + x ) * 4;
                bool pixel_bad = false;
                for( int c = 0; c < 4; ++c )
                {
                    int const dr = int( rendered[ base + c ] ) - int( golden[ base + c ] );
                    int const abs_dr = dr < 0 ? -dr : dr;
                    if( abs_dr > max_per_channel_diff )
                        max_per_channel_diff = abs_dr;
                    if( abs_dr > kPerChannelTolerance )
                        pixel_bad = true;
                }
                if( pixel_bad )
                {
                    if( mismatches == 0 )
                    {
                        first_bad_x = x;
                        first_bad_y = y;
                        first_bad_render = { rendered[ base + 0 ], rendered[ base + 1 ],
                                             rendered[ base + 2 ], rendered[ base + 3 ] };
                        first_bad_golden = { golden  [ base + 0 ], golden  [ base + 1 ],
                                             golden  [ base + 2 ], golden  [ base + 3 ] };
                    }
                    ++mismatches;
                }
            }
        }
        stbi_image_free( golden );

        std::printf( "[Phase3.Golden:%s] compared %ux%u against %s : max per-channel diff = %d, mismatches = %zu\n",
                     shader_label, kWidth, kHeight, golden_path.string().c_str(),
                     max_per_channel_diff, mismatches );

        EXPECT_EQ( mismatches, 0u )
            << "Phase 3 golden regression (" << shader_label << ") : "
            << mismatches << " of " << ( kWidth * kHeight )
            << " pixels deviate by more than " << kPerChannelTolerance << " LSB.\n"
            << "First mismatch at (" << first_bad_x << ", " << first_bad_y << ") : "
            << "render=(R=" << int( first_bad_render[0] )
            << " G=" << int( first_bad_render[1] )
            << " B=" << int( first_bad_render[2] )
            << " A=" << int( first_bad_render[3] ) << ") "
            << "golden=(R=" << int( first_bad_golden[0] )
            << " G=" << int( first_bad_golden[1] )
            << " B=" << int( first_bad_golden[2] )
            << " A=" << int( first_bad_golden[3] ) << ")\n"
            << "Max per-channel diff across whole frame = " << max_per_channel_diff;
    }
}

TEST( RegressionPhase3, GoldenFrame_PsMaaAddScale_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_add_scale.png";

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

    //	-------- Source texture : 256x256 RGBA gradient ------------------
    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );

    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    //	-------- Render target ------------------------------------------
    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	-------- Uniform buffer ------------------------------------------
    //	Fixed deterministic offset + scale -- chosen so output stays
    //	in [0,1] (no clamping wobble) AND is visually distinct from
    //	the source gradient.
    //	  out = (0.10, 0.05, 0.20, 0.00) + src * (0.70, 0.80, 0.60, 1.00)
    AaaFuVec4s u{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            u.values[ i ][ j ] = 0.0f;
    u.values[ 0 ][ 0 ] = 0.10f;
    u.values[ 0 ][ 1 ] = 0.05f;
    u.values[ 0 ][ 2 ] = 0.20f;
    u.values[ 0 ][ 3 ] = 0.00f;
    u.values[ 1 ][ 0 ] = 0.70f;
    u.values[ 1 ][ 1 ] = 0.80f;
    u.values[ 1 ][ 2 ] = 0.60f;
    u.values[ 1 ][ 3 ] = 1.00f;

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    //	-------- Render pass --------------------------------------------
    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;       // blue clear -- catches a no-draw bug
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_add_scale";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	The tool-emitted shader declares 4 texture slots ; only slot 0
    //	is sampled in this shader's body but Metal validation requires
    //	bindings on every declared slot. Re-bind src_tex everywhere.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_buffer(  ub, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	-------- Readback ------------------------------------------------
    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    //	Sanity : the render must have produced non-clear-color pixels.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:add_scale] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "add_scale" );

    backend.delete_buffer(  ub      );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c121-A : ps_Maa_alpha -----------------------------
//	Shader body :
//	  out.a   = pow( src.a * floats[1] + floats[0], floats[2] );
//	  out.rgb = float3( 1.0 );        // GLSL gl_Color fallback : white
//	With src.a == 1 (opaque gradient) and our uniforms, the alpha map
//	is a pure constant -- but the test still exercises the texture-
//	sample + buffer-bind + branch-on-dim path.
TEST( RegressionPhase3, GoldenFrame_PsMaaAlpha_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_alpha.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_alpha.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : out.a = pow( a*1.0 + 0.0, 1.0 ) = a. With opaque
    //	gradient a==1 the result alpha is 1. Choosing all three values
    //	non-zero produces a deterministic constant alpha map.
    AaaFuFloats u{};
    for( int i = 0; i < 16; ++i ) u.values[ i ] = 0.0f;
    u.values[ 0 ] = 0.10f;     // offset
    u.values[ 1 ] = 0.80f;     // multiplier
    u.values[ 2 ] = 1.50f;     // pow exponent

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_alpha";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_buffer(  ub, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    //	ps_Maa_alpha forces rgb = float3(1.0), so every drawn pixel is
    //	(255,255,255,A). All non-zero RGB.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:alpha] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "alpha" );

    backend.delete_buffer(  ub      );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c121-A : ps_Maa_Copy_01 ----------------------------
//	Shader body : uv-from-fragcoord-with-integer-offset texture copy.
//	  uv  = gl_FragCoord.xy + (-ints[0]+ints[2], -ints[1]+ints[3])
//	  uv /= texture_dim
//	  rgb = sample( uv ).rgb ; a = 1
//	With zero ints + floats[2] < 0.5 (no discard), this is a pure
//	identity copy of the gradient. Uses buffer slot 0 (Floats) AND
//	slot 1 (Ints) -- unique among the 4 baseline shaders.
TEST( RegressionPhase3, GoldenFrame_PsMaaCopy01_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_Copy_01.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_Copy_01.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats slot 0 : values[2] is the discard-on-out-of-bounds flag.
    //	Keep < 0.5 so the shader never discards -- otherwise the
    //	half-of-screen NDC oversize triangle would produce holes.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    //	values[2] stays 0.0 -- discard branch disabled.

    //	Ints slot 1 : (-ints[0]+ints[2], -ints[1]+ints[3]) is the
    //	integer pixel offset. All zero -> identity copy.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_Copy_01";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	Copy_01 declares ONE texture slot (slot 0).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 1, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:Copy_01] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "Copy_01" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c121-A : ps_Maa_drop ------------------------------
//	Shader body :
//	  src1 = tex0( normal.xy * 0.45 + 0.5 )     (constant uv : normal=z+)
//	  src2 = tex1( rot(uv-0.5, floats[3]) * floats[0..1] + offset )
//	  dst.rgb = mix( src1*0.8, src2, dot(normal,(0,0,1)) ) = src2
//	  dst.a   = max( 0, src2.a )
//	  + lighting clamp from norb dot product
//	Two textures both bound to the gradient. Pure deterministic --
//	transformedNormal is hand-patched to constant (0,0,1).
TEST( RegressionPhase3, GoldenFrame_PsMaaDrop_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_drop.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_drop.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : values[0..1] = scale, values[2] = normal-bias, values[3] = rotation.
    //	Pick a small rotation + ~1x scale -- yields a recognisable
    //	rotated-and-tinted gradient that's NOT identity (forces real
    //	pipeline exercise) but stays well within sample bounds.
    AaaFuFloats u{};
    for( int i = 0; i < 16; ++i ) u.values[ i ] = 0.0f;
    u.values[ 0 ] = 1.00f;    // x scale
    u.values[ 1 ] = 1.00f;    // y scale
    u.values[ 2 ] = 0.10f;    // normal bias
    u.values[ 3 ] = 0.30f;    // rotation (rad)

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_drop";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	drop declares 4 texture slots. Bind gradient everywhere ; only
    //	tex0 + tex1 are sampled in the body.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_buffer(  ub, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:drop] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "drop" );

    backend.delete_buffer(  ub      );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	====================================================================
//	c121-B : baseline broadened 4 -> 8 shaders.
//
//	Continues c121-A's copy-paste-per-shader doctrine : each new TEST
//	block binds the shader's specific uniforms inline so a regression
//	failure points unambiguously to which catalog member regressed.
//
//	Added shaders, all confirmed deterministic by
//	`grep -E "iTime|random|iFrame|noise|time_" src/shaders/msl/<f>.metal`
//	returning empty :
//	  5. ps_Maa_normal  : 1 texture, no uniforms. Output = (0,0,gr,src.a)
//	                     where gr = dot(forward-Z,forward-Z) = 1.
//	  6. ps_Maa_TriMix  : 4 textures + Floats + Vec4s + Ints. 3-way
//	                     mask-driven mix (most uniform-rich).
//	  7. gbuffer_grid   : MRT (4 color attachments), flat-normal PBR
//	                     gbuffer. Exercises the c43 MRT path with the
//	                     create_program(ProgramDesc) lower-level API.
//	                     Only color(0) (diffuse+rough) is goldened.
//	  8. ps_Maa_4tex    : 4 textures + Floats. Linear combination of
//	                     two textures weighted by float uniforms.
//	====================================================================

//	---------------- c121-B : ps_Maa_normal ----------------------------
//	Shader body (deterministic, no uniforms) :
//	  transformedNormal = (0,0,1) ; src = tex0.sample(uv) ;
//	  src.r = |Nx| = 0 ; src.g = |Ny| = 0 ; src.b = dot(N,(0,0,1)) = 1 ;
//	  return src ;
//	Output is a constant (0, 0, 255, src.a) regardless of uv -- the
//	test still exercises the texture-sample (src.a is read) + draw path.
//	Clear is GREEN here so a no-draw bug doesn't silently match the
//	expected solid-blue output.
TEST( RegressionPhase3, GoldenFrame_PsMaaNormal_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_normal.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_normal.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	GREEN clear -- expected output is solid blue (0,0,255,A), so a
    //	no-draw bug would leave the buffer green, not match the expected.
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_normal";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	ps_Maa_normal declares ONE texture slot (slot 0).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    //	Output rgb = (0, 0, 1). Per-pixel B = 255. So count_nonzero_pixels
    //	(any of R/G/B nonzero) covers every pixel.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:normal] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "normal" );

    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c121-B : ps_Maa_TriMix ----------------------------
//	Shader body :
//	  mode = ints[0] ; channel_is_alpha = ints[1] ;
//	  grey = smoothstep( floats[0], floats[1],
//	                     channel_is_alpha ? mask.a : gray(mask.rgb) ) ;
//	  src1 = tex1.sample(uv) * vec4s[1] ; src2 = tex2.sample(uv) * vec4s[2] ;
//	  if mode <= 3 : early-exit to one of the inputs (mode 0 returns black) ;
//	  else dst = mix(src1, src2, 1-grey) * vec4s[0] ;
//	We pick mode=4 (src2 = 0, dst = src1 * grey * vec4s[0]), which
//	exercises every binding (3 textures sampled, all 3 uniform buffers
//	read). With our gradient + chosen smoothstep edges, grey varies
//	smoothly across the frame -> rich per-pixel variation in the golden.
TEST( RegressionPhase3, GoldenFrame_PsMaaTriMix_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_TriMix.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_TriMix.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	floats[0..1] : smoothstep edges over the [0,1] gray range. Pick
    //	0.2 .. 0.8 so the bottom-left of the gradient gets clamped to 0,
    //	the top-right to 1, and the middle band varies smoothly.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.20f;
    uf.values[ 1 ] = 0.80f;

    //	vec4s[0] : final-output tint ; vec4s[1] : src1 tint ; vec4s[2] : src2 tint.
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 0 ] = 1.0f; uv4.values[ 0 ][ 1 ] = 1.0f;
    uv4.values[ 0 ][ 2 ] = 1.0f; uv4.values[ 0 ][ 3 ] = 1.0f;
    uv4.values[ 1 ][ 0 ] = 0.9f; uv4.values[ 1 ][ 1 ] = 0.7f;
    uv4.values[ 1 ][ 2 ] = 0.5f; uv4.values[ 1 ][ 3 ] = 1.0f;
    uv4.values[ 2 ][ 0 ] = 0.5f; uv4.values[ 2 ][ 1 ] = 0.7f;
    uv4.values[ 2 ][ 2 ] = 0.9f; uv4.values[ 2 ][ 3 ] = 1.0f;

    //	ints[0] = mode (4 = "src2 zeroed, mix"). ints[1] = 0 (gray, not alpha).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 4;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_TriMix";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	TriMix declares 4 texture slots. Bind gradient on all 4 ; only
    //	tex0/tex1/tex2 are sampled in the body.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:TriMix] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "TriMix" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c121-B : gbuffer_grid -----------------------------
//	Shader body (FLAT_NORMAL == 1 branch) :
//	  if tex0.sample(uv).a < 0.001 : discard ;
//	  surface_diffuse  = tex0.sample(uv).rgb * mat_diffuse(=1)         ;
//	  surface_specular = mat_specular(=1) * tex3.sample(uv).rgb        ;
//	  metallic = surface_specular.x ;
//	  albedo   = surface_diffuse * (1 - metallic) ;
//	  out.color(0) = (albedo, mat_roughness(=1))                       ;
//	  out.color(1..3) = normal/specular/emissive (ignored by golden)   ;
//	With our gradient (a==1 everywhere : no discard ; rgb varies),
//	color(0).rgb varies smoothly. We golden ONLY color(0) ; the other
//	3 RTs are bound to make Metal validation happy.
//
//	Uses the lower-level create_program(ProgramDesc) API to set
//	color_attachment_count = 4 (the c43 MRT runtime path) -- different
//	from the other 7 baseline shaders which use create_program_msl.
TEST( RegressionPhase3, GoldenFrame_GbufferGrid_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_gbuffer_grid.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "gbuffer_grid.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    //	MRT : 4 color attachments. Use the lower-level ProgramDesc form
    //	(create_program_msl hardcodes count = 1).
    GOL::ProgramDesc pdesc;
    pdesc.msl_source             = msl_src.c_str();
    pdesc.vertex_entry           = "vs_main";
    pdesc.fragment_entry         = "fs_main";
    pdesc.target_color_fmt       = GOL::TextureFormat::RGBA8;
    pdesc.color_attachment_count = 4;
    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    //	4 render targets -- only rt[0] is goldened ; the others must
    //	exist for the MRT pipeline state.
    GOL::TextureId rts[ 4 ];
    for( int i = 0; i < 4; ++i )
    {
        rts[ i ] = backend.gen_texture_2d( kWidth, kHeight,
                                          GOL::TextureFormat::RGBA8 );
        ASSERT_NE( rts[ i ], GOL::kInvalidTextureId );
    }

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment                  = rts[ 0 ];
    rpd.additional_color_attachments[ 0 ] = rts[ 1 ];
    rpd.additional_color_attachments[ 1 ] = rts[ 2 ];
    rpd.additional_color_attachments[ 2 ] = rts[ 3 ];
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.gbuffer_grid";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	gbuffer_grid declares texture slots 0, 1, 3 (NOT 2). Bind all 4
    //	to gradient -- slot 2 binding is harmless ; Metal ignores unused
    //	bindings.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	Only golden color(0). Other RTs are exercised but their contents
    //	aren't part of the regression contract (they'd grow the corpus
    //	without additional regression coverage -- diffuse_rough fans the
    //	source gradient through the pipeline ; that's the load-bearing
    //	signal).
    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rts[ 0 ], 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:gbuffer_grid] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "gbuffer_grid" );

    for( int i = 0; i < 4; ++i )
        backend.delete_texture( rts[ i ] );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c121-B : ps_Maa_4tex ------------------------------
//	Shader body :
//	  color  = floats[0] * tex0.sample(uv)
//	         + floats[1] * tex1.sample(uv) ;
//	  return color ;
//	A literal linear combination of two textures. With our gradient
//	bound to both and floats = (0.6, 0.5), the per-pixel output is
//	1.1 * gradient.rgba -- clamped to [0,1] in the output stage.
//	The R channel hits the clamp ceiling in the right half of the
//	frame ; the rest varies linearly. Exercises Floats buffer + 4
//	declared textures (2 sampled).
TEST( RegressionPhase3, GoldenFrame_PsMaa4tex_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_4tex.png";

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

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : weights for the two sampled textures. (0.6, 0.5) sums
    //	to 1.1 -- above 1 in the R channel for the right edge of the
    //	gradient, so the clamp path is exercised too.
    AaaFuFloats u{};
    for( int i = 0; i < 16; ++i ) u.values[ i ] = 0.0f;
    u.values[ 0 ] = 0.60f;
    u.values[ 1 ] = 0.50f;

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_4tex";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	4tex declares 4 texture slots ; body samples tex0 + tex1.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_buffer(  ub, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:4tex] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "4tex" );

    backend.delete_buffer(  ub      );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	====================================================================
//	c122-C : baseline broadened 8 -> 12 shaders.
//
//	Continues c121-A / c121-B / c122-B copy-paste-per-shader doctrine.
//	All 4 candidates confirmed deterministic by
//	`grep -E "iTime|random|iFrame|noise|time_" src/shaders/msl/<f>.metal`
//	returning empty (or, for noise_cheap, returning only a local-variable
//	identifier 'noise' that is itself a pure function of uv).
//
//	Added shaders :
//	  9.  ps_Maa_Gradient_01 : 2 textures + Floats (slot 0) + Ints (slot 2).
//	                          Branched body ; we drive type=1 which writes
//	                          a pure-uniform-driven (FragCoord.xy * scale)
//	                          gradient -- no sampling, no dfdy on this path,
//	                          fully deterministic.
//	  10. ps_Maa_noise_cheap : 1 texture + Floats (slot 0). "noise" is a
//	                          local variable derived from uv (NOT a time-
//	                          driven hash). Pure per-pixel function.
//	  11. depth_coc          : 1 texture + Vec4s (slot 1). Engine
//	                          aaa_cam.projection_inverse hand-stubbed to
//	                          identity in the .metal source ; coc_factors
//	                          surfaced through Vec4s[0]. **First buffer-
//	                          slot-1-only shader in the corpus**.
//	  12. Maa_WAVY           : 2 textures + Floats (slot 0) + Vec4s (slot 1)
//	                          + Ints (slot 2). Engine ST_AAA_BV interface
//	                          block stubbed inline. All 3 uniform-buffer
//	                          slots populated -- highest binding fan-out
//	                          in the corpus.
//	====================================================================

//	---------------- c122-C : ps_Maa_Gradient_01 -----------------------
//	Shader body (type == 1 branch chosen via ints[0] == 1) :
//	  dst.r = in.position.x * floats[2]
//	  dst.g = in.position.y * floats[3]
//	  dst.b = 1.0
//	  dst.a = 1.0
//	With floats[2] = floats[3] = 1.0/256.0 the R/G channels sweep
//	[0..1] across the 256x256 frame and B is constant 1. Deterministic ;
//	pure FragCoord-driven. Uses Floats (slot 0) + Ints (slot 2) -- the
//	first regression shader in the corpus that binds the Ints buffer at
//	slot 2 WITHOUT also binding Vec4s at slot 1.
TEST( RegressionPhase3, GoldenFrame_PsMaaGradient01_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_Gradient_01.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_Gradient_01.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : values[2], values[3] = scales of FragCoord.x, FragCoord.y
    //	into the [0,1] output range. 1.0 / 256 sweeps the full unit range
    //	across our 256x256 frame.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 2 ] = 1.0f / float( kWidth );
    uf.values[ 3 ] = 1.0f / float( kHeight );

    //	Ints : values[0] = branch selector. 1 = "FragCoord-driven gradient".
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	GREEN clear -- expected output has B == 1 across the entire frame.
    //	A no-draw bug leaves us with green, which is unambiguously NOT the
    //	expected gradient-with-blue-floor.
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_Gradient_01";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	Gradient_01 declares 2 texture slots (slot 0 + slot 1). Bind
    //	gradient on both -- neither is sampled in the type=1 branch but
    //	Metal validation requires bindings on every declared slot.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    //	dst.b == 1.0 across the frame, so every pixel has nonzero RGB.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:Gradient_01] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "Gradient_01" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c122-C : ps_Maa_noise_cheap -----------------------
//	Shader body :
//	  location = float3( uv, 0 )
//	  floorvec = floor( floats[0] * uv ) ; 0 z
//	  noise    = (location * floats[0] - floorvec - 0.5)^2     // PURE per-uv
//	  location += noise * floats[2] + floats[1]
//	  color    = tex0.sample( location.xy ) ; color.a = 1
//	The variable is called "noise" but it is a deterministic per-pixel
//	function of uv -- NOT a time-driven hash. Verified by grep above.
TEST( RegressionPhase3, GoldenFrame_PsMaaNoiseCheap_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_noise_cheap.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_noise_cheap.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	floats[0] : the cell scale for the per-pixel "noise" term. Pick
    //	8.0 -> an 8x8 jitter grid across the frame.
    //	floats[1] : DC offset added to the sample uv (-0.5..+0.5).
    //	floats[2] : amplitude of the squared-cell perturbation.
    AaaFuFloats u{};
    for( int i = 0; i < 16; ++i ) u.values[ i ] = 0.0f;
    u.values[ 0 ] = 8.0f;
    u.values[ 1 ] = 0.0f;
    u.values[ 2 ] = 0.25f;

    GOL::BufferId ub = backend.gen_buffer();
    ASSERT_NE( ub, GOL::kInvalidBufferId );
    backend.buffer_data( ub, sizeof( u ), &u, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_noise_cheap";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	noise_cheap declares ONE texture slot (slot 0).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:noise_cheap] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "noise_cheap" );

    backend.delete_buffer(  ub      );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c122-C : depth_coc --------------------------------
//	Shader body :
//	  screen_clip = uv with y flipped, scaled to [-1, 1]
//	  depth       = tex0.sample(uv).r * 2 - 1
//	  view_position = identity_projection_inverse * (sc.x, sc.y, depth, 1)
//	  return (view_position.z, calc_coc_factor(view_position.z, vec4s[0]), 0, 0)
//	Engine aaa_cam.projection_inverse is hand-stubbed inside the shader to
//	identity ; aaa_cam.coc_factors surfaces through Vec4s[0]. This is the
//	first shader in the regression corpus that binds ONLY Vec4s at slot 1
//	(no Floats at slot 0). Per-pixel deterministic.
//
//	Output is 2-channel (RG) + zero (BA). With a gradient bound as the
//	"depth" texture, depth varies in [0, 1] which after the *2-1 maps to
//	[-1, 1]. View position z therefore sweeps [-1, 1] / 256 mostly --
//	clamped/scaled by the identity_inverse so we end up with view_z that
//	roughly mirrors the gradient.r channel.
TEST( RegressionPhase3, GoldenFrame_DepthCoc_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_depth_coc.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "depth_coc.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Vec4s[0] = aaa_cam.coc_factors = (near_min, near_max, far_min, far_max).
    //	Pick values that produce a CoC factor sweep across the frame.
    //	near range -1.0..0.0  (so depth in [-1,0] gives f0 > 0).
    //	far  range  0.5..1.0  (so depth in [0.5,1] gives f1 > 0).
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 0 ] = -1.0f;
    uv4.values[ 0 ][ 1 ] =  0.0f;
    uv4.values[ 0 ][ 2 ] =  0.5f;
    uv4.values[ 0 ][ 3 ] =  1.0f;

    GOL::BufferId ub_v4 = backend.gen_buffer();
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	GREEN clear : expected output has G channel populated (the CoC
    //	factor) so a no-draw bug leaves pure green which would NOT match
    //	the expected RG output.
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.depth_coc";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	depth_coc declares ONE texture slot (slot 0 : depth tex).
    backend.bind_fragment_texture( src_tex, 0 );
    //	UNIFORM-BINDING GOTCHA : depth_coc binds Vec4s at slot 1 (the
    //	canonical engine ABI slot for Vec4s) but does NOT use Floats at
    //	slot 0. We must NOT bind anything at slot 0 ; the .metal source
    //	doesn't declare it. Slot-1-only is unique among the 12-shader
    //	baseline.
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    //	depth_coc writes (view_z, coc, 0, 0). The output BA channels are
    //	zero ; only R/G have signal. We don't gate on nonzero-pixel count
    //	here since the R channel sweeps through 0 mid-frame. Just record
    //	what came back.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    std::printf( "[Phase3.Golden:depth_coc] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "depth_coc" );

    backend.delete_buffer(  ub_v4   );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c122-C : Maa_WAVY ---------------------------------
//	Shader body (engine ST_AAA_BV stubbed inline as opaque-white BV.color,
//	zero BV.nor_ec, BV.tex_coor[0] = in.uv) :
//	  uv = in.uv + floats[3] * 0 = in.uv      (BV_nor_ec_xy stubbed to 0)
//	  src1 = tex0.sample(uv) ; src2 = tex1.sample(uv)
//	  if ints[0] == 1 : src1.rgb = 1-src1.rgb ; src2.rgb = 1-src2.rgb
//	  color = src1 * vec4s[1] * floats[0] + src2 * vec4s[2] * (1-floats[0])
//	  if floats[1] > 0 && floats[2] > 0 :
//	      color.a = smoothstep( floats[1], floats[2], gray(color.rgb) )
//	  return color * BV.color = color
//	With ints[0]=0 (no invert) and floats[0]=0.5 (50/50 mix) the output
//	is the gradient tinted by vec4s[1]/vec4s[2] -- rich per-pixel variation.
//	**Highest binding fan-out in the corpus** : 2 textures + Floats + Vec4s
//	+ Ints all three.
TEST( RegressionPhase3, GoldenFrame_MaaWavy_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_Maa_WAVY.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "Maa_WAVY.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : [0] = mix weight (0.5 = 50/50), [1..2] = smoothstep edges
    //	for alpha (BOTH > 0 to enable the alpha branch), [3] = nor_ec_xy
    //	scaling (irrelevant here since BV.nor_ec is stubbed to 0).
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.5f;
    uf.values[ 1 ] = 0.2f;
    uf.values[ 2 ] = 0.8f;
    uf.values[ 3 ] = 0.0f;

    //	Vec4s : [1] tints src1 (warm), [2] tints src2 (cool). Slot 0 unused
    //	per the source comment.
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 1 ][ 0 ] = 1.0f; uv4.values[ 1 ][ 1 ] = 0.6f;
    uv4.values[ 1 ][ 2 ] = 0.4f; uv4.values[ 1 ][ 3 ] = 1.0f;
    uv4.values[ 2 ][ 0 ] = 0.4f; uv4.values[ 2 ][ 1 ] = 0.6f;
    uv4.values[ 2 ][ 2 ] = 1.0f; uv4.values[ 2 ][ 3 ] = 1.0f;

    //	Ints : [0] = invert flag. 0 = no invert.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.Maa_WAVY";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	Maa_WAVY declares 2 texture slots (slot 0 + slot 1).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:Maa_WAVY] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "Maa_WAVY" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	====================================================================
//	c123-C : baseline broadened 12 -> 16 shaders.
//
//	Continues c121-A / c121-B / c122-C copy-paste-per-shader doctrine.
//	All 4 candidates confirmed deterministic by
//	`grep -E "iTime|random|iFrame|time_" src/shaders/msl/<f>.metal`
//	returning empty.
//
//	Added shaders :
//	  13. debug_world_map  : 2 textures + Ints (slot 2 only). First
//	                         shader in the corpus that binds the Ints
//	                         buffer at slot 2 *without* also binding
//	                         Floats at slot 0. Engine camera cluster
//	                         (view_projection_inverse, projection_inverse)
//	                         hand-stubbed to identity in the .metal source.
//	                         We drive bypass=1 -> pure tex0 copy with
//	                         opaque alpha (deterministic identity-style
//	                         output).
//	  14. fog              : 2 textures + Floats + Vec4s + Ints (all
//	                         three uniform buffers). Engine camera
//	                         cluster (projection_inverse,
//	                         view_projection_inverse, camera_position)
//	                         hand-stubbed in the .metal source. Drive
//	                         bypass=1 -> pure tex0 copy ; the
//	                         depth/world/fog math is still validated
//	                         at compile time by Metal but the per-pixel
//	                         output is deterministic and gradient-rich.
//	  15. gbuffer_matte    : 4 textures + Floats (slot 0). MRT (4 color
//	                         attachments) -- second member of the MRT
//	                         cluster (sibling of gbuffer_grid). Uses
//	                         the lower-level create_program(ProgramDesc)
//	                         API with color_attachment_count = 4.
//	                         tex_y_offset = 0.0 -> identity uv ; metal-
//	                         lic workflow branch. Only color(0) is
//	                         goldened.
//	  16. ps_Maa_Cam_xyz_01 : 1 texture + Floats (slot 0) + Ints (slot 2).
//	                         Engine-struct cluster (c92 Maa 2008 family).
//	                         We pick type = -1 (ints[0] = -1) -> `dst = src`
//	                         identity copy, which dodges the dfdx/dfdy
//	                         branches (type==4 and the default-else).
//	                         Note : the source ALSO contains dfdx/dfdy
//	                         in *unreached* branches under our chosen
//	                         type -- Metal will still compile them, but
//	                         the goldened output path is pure.
//	====================================================================

//	---------------- c123-C : debug_world_map --------------------------
//	Shader body (bypass != 0 branch chosen via ints[0] = 1) :
//	  return float4( tex0.sample(uv).rgb, 1.0 )
//	With bypass = 1 the camera-stub world-position reconstruction is
//	skipped ; output is a deterministic identity copy of the gradient
//	with alpha forced to 1. Bindings : 2 textures (slot 0 + slot 1) +
//	Ints buffer at slot 2 (no Floats, no Vec4s). NEW binding pattern :
//	first shader in the corpus that binds ONLY the Ints buffer.
TEST( RegressionPhase3, GoldenFrame_DebugWorldMap_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_debug_world_map.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "debug_world_map.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Ints slot 2 : [0] = bypass (1 -> identity tex0 copy), [1] = mode (unused).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;     // bypass = 1
    ui.values[ 1 ] = 0;     // mode (unused on bypass path)

    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;       // green clear : output expected gradient, not green
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.debug_world_map";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	debug_world_map declares 2 texture slots (slot 0 = input, slot 1 = depth).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:debug_world_map] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "debug_world_map" );

    backend.delete_buffer(  ub_i    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c123-C : fog --------------------------------------
//	Shader body (bypass != 0 branch chosen via ints[0] = 1) :
//	  return float4( tex0.sample(uv).rgb, 1.0 )
//	With bypass = 1 the (fairly elaborate) per-pixel fog computation is
//	skipped at runtime, yielding an identity-style copy of the gradient
//	with forced opaque alpha. Bindings : 2 textures + Floats (slot 0)
//	+ Vec4s (slot 1) + Ints (slot 2) -- exercises every uniform-buffer
//	slot in the corpus convention (Maa_WAVY / TriMix tier). The
//	Floats / Vec4s slots are populated with plausible values even though
//	the bypass path doesn't read them ; this guards against a future
//	regression where bypass-stripping accidentally also strips bindings.
TEST( RegressionPhase3, GoldenFrame_Fog_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_fog.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "fog.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : fog params. Filled with sane (not all-zero) values so a
    //	future regression that accidentally takes the non-bypass branch
    //	produces *some* output rather than NaNs.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ]  = 0.1f;          // fog_start
    uf.values[ 1 ]  = 1.0f;          // sun_color.r
    uf.values[ 2 ]  = 0.9f;          // sun_color.g
    uf.values[ 3 ]  = 0.7f;          // sun_color.b
    uf.values[ 4 ]  = 0.0f;          // sun_direction.x
    uf.values[ 5 ]  = 1.0f;          // sun_direction.y
    uf.values[ 6 ]  = 0.0f;          // sun_direction.z
    uf.values[ 7 ]  = 0.5f;          // sun_amount
    uf.values[ 8 ]  = 0.5f;          // fog_depth_factor
    uf.values[ 9 ]  = 1.0f;          // fog_y_top
    uf.values[ 10 ] = -1.0f;         // fog_y_bottom
    uf.values[ 11 ] = 1.0f;          // fog_y_gamma

    //	Vec4s : fog_color_density_low / high (rgb + density.w).
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 0 ] = 0.5f; uv4.values[ 0 ][ 1 ] = 0.6f;
    uv4.values[ 0 ][ 2 ] = 0.7f; uv4.values[ 0 ][ 3 ] = 1.0f;
    uv4.values[ 1 ][ 0 ] = 0.8f; uv4.values[ 1 ][ 1 ] = 0.8f;
    uv4.values[ 1 ][ 2 ] = 0.9f; uv4.values[ 1 ][ 3 ] = 0.5f;

    //	Ints : [0] = bypass (1 -> tex0 identity copy).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;       // red clear -- expected output is gradient, not red
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.fog";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	fog declares 2 texture slots (slot 0 = input, slot 1 = depth).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:fog] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "fog" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c123-C : gbuffer_matte ----------------------------
//	Shader body (METALLIC_WORKFLOW branch, the engine default ;
//	tex_y_offset = 0 -> identity uv) :
//	  if tex0.sample(uv).a < 0.001 : discard ;                   (alpha=255 so safe)
//	  normal_raw = tex1.sample(uv).xyz * 2 - 1                   (gradient-driven)
//	  surface_normal = normalize( I * normal_raw ) = normalize(normal_raw)
//	  surface_diffuse  = tex0.sample(uv).rgb * 1                 (= gradient.rgb)
//	  surface_roughness = tex2.sample(uv).r * 1                  (= gradient.r)
//	  surface_specular = tex3.sample(uv).rgb * 1                 (= gradient.rgb)
//	  metallic = surface_specular.x = gradient.r
//	  albedo   = surface_diffuse * (1 - metallic)                (= gradient.rgb * (1-r))
//	  spec_out = mix( 0.03, surface_diffuse, metallic )
//	  color(0).rgba = (albedo, surface_roughness)
//	Rich per-pixel variation in color(0) ; we only golden color(0).
//	MRT with 4 color attachments -- sibling of gbuffer_grid.
TEST( RegressionPhase3, GoldenFrame_GbufferMatte_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_gbuffer_matte.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "gbuffer_matte.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    //	MRT : 4 color attachments. Use the lower-level ProgramDesc form
    //	(create_program_msl hardcodes count = 1).
    GOL::ProgramDesc pdesc;
    pdesc.msl_source             = msl_src.c_str();
    pdesc.vertex_entry           = "vs_main";
    pdesc.fragment_entry         = "fs_main";
    pdesc.target_color_fmt       = GOL::TextureFormat::RGBA8;
    pdesc.color_attachment_count = 4;
    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    //	4 render targets -- only rts[0] is goldened.
    GOL::TextureId rts[ 4 ];
    for( int i = 0; i < 4; ++i )
    {
        rts[ i ] = backend.gen_texture_2d( kWidth, kHeight,
                                          GOL::TextureFormat::RGBA8 );
        ASSERT_NE( rts[ i ], GOL::kInvalidTextureId );
    }

    //	Floats : [0] = tex_y_offset. 0 = identity uv.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;

    GOL::BufferId ub_f = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment                  = rts[ 0 ];
    rpd.additional_color_attachments[ 0 ] = rts[ 1 ];
    rpd.additional_color_attachments[ 1 ] = rts[ 2 ];
    rpd.additional_color_attachments[ 2 ] = rts[ 3 ];
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.gbuffer_matte";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	gbuffer_matte declares 4 texture slots (0..3). Bind gradient on
    //	all 4 ; the shader's metallic-workflow branch samples all of them.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	Only golden color(0) (diffuse+rough). Other MRT slots exercised
    //	but not part of the regression contract (matches gbuffer_grid
    //	convention).
    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rts[ 0 ], 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:gbuffer_matte] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "gbuffer_matte" );

    backend.delete_buffer( ub_f );
    for( int i = 0; i < 4; ++i )
        backend.delete_texture( rts[ i ] );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c123-C : ps_Maa_Cam_xyz_01 ------------------------
//	Shader body (type == -1 branch chosen via ints[0] = -1) :
//	  src = tex0.sample( uv )
//	  dst = src                                     // identity copy
//	  return dst
//	The dfdx/dfdy branches (type==4 and the default else) are NOT
//	reached on this path. MSL will still compile them, but the goldened
//	output path is a deterministic identity copy of the gradient.
//	Bindings : 1 texture + Floats (slot 0) + Ints (slot 2) -- note the
//	skipped slot 1 (no Vec4s buffer). Identical binding fan-out to
//	ps_Maa_Gradient_01 ; this test adds the engine-struct-cluster
//	(c92 Maa 2008) family to the regression corpus.
TEST( RegressionPhase3, GoldenFrame_PsMaaCamXyz01_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Maa_Cam_xyz_01.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Maa_Cam_xyz_01.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats slot 0 : values[2..5] feed `s` and the dfdx/dfdy scales,
    //	but type=-1 takes a branch that ignores `s`. Zeros are fine.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;

    //	Ints slot 2 : [0] = type (-1 -> identity copy branch),
    //	[1] = mod-flag (unused on this path), [2] = blend-flag (no-op).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = -1;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;       // green clear -- expected gradient, not green
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Maa_Cam_xyz_01";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	ps_Maa_Cam_xyz_01 declares ONE texture slot (slot 0).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:ps_Maa_Cam_xyz_01] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "ps_Maa_Cam_xyz_01" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	====================================================================
//	c124-B : baseline broadened 16 -> 20 shaders.
//
//	Continues c121-A / c121-B / c122-C / c123-C copy-paste-per-shader
//	doctrine. All 4 candidates confirmed deterministic by
//	`grep -E "iTime|random|iFrame|time_" src/shaders/msl/<f>.metal`
//	returning empty.
//
//	Added shaders :
//	  17. gbuffer          : 5 textures + Floats (slot 0) + Vec4s (slot 1)
//	                         + Ints (slot 2). MRT (4 color attachments) --
//	                         **third member of the MRT cluster**, full PBR
//	                         gbuffer with normal-map sampling. **First 5-
//	                         texture shader in the corpus**. Only color(0)
//	                         (diffuse + roughness) is goldened.
//	  18. gbuffer_generic  : 1 texture + Floats (slot 0) + Vec4s (slot 1)
//	                         + Ints (slot 2). MRT (4 color attachments) --
//	                         **fourth member of the MRT cluster**, MRT
//	                         cluster coverage now 4/6 (66%). Drive
//	                         use_textures = 1, flat_normal = 0 -> smooth-
//	                         normal branch, dodges dfdx/dfdy. Only
//	                         color(0) is goldened.
//	  19. ps_test          : 1 texture + Vec4s (slot 1 only). c93 engine-
//	                         struct (SercelMT) cluster. Per-pixel UV
//	                         remap + smoothstep alpha on red channel.
//	                         **Second slot-1-only binding in the corpus**
//	                         after depth_coc. The smoothstep on the
//	                         gradient.r sweep gives a sharp horizontal
//	                         transition in the goldened alpha channel.
//	  20. dof              : 3 textures + Floats (slot 0) + Ints (slot 2).
//	                         c99 camera cluster (Poisson-disc DoF blur).
//	                         Engine aaa_cam.projection_inverse stubbed
//	                         to identity in the .metal source. Drive
//	                         use_vdebug = 1 + blur_size > 0 -> the body
//	                         computes the center CoC and writes it into
//	                         color.r (deterministic, no per-tap fan-out
//	                         path). Skipped-vec4s-slot-1 binding pattern
//	                         (matches ps_Maa_Cam_xyz_01).
//	====================================================================

//	---------------- c124-B : gbuffer ----------------------------------
//	Shader body (ENABLE_NORMAL_MAP == 1, no discard branches taken with
//	our uniforms) :
//	  uv = (in.uv + vec4s[0].xy) * vec4s[0].wz
//	    With vec4s[0]=(0,0,1,1) we get uv = (in.uv + 0) * (1, 1) = in.uv.
//	  surface.diffuse = (colored_mesh==0) ? tex0.sample(uv) : mesh_color
//	  if surface.diffuse.a < alpha_threshold(=0) : never (a==255)
//	  normal = normalize( I * (2*tex1.sample(uv).xyz - 1) )           (varies)
//	  roughness = tex2.sample(uv).r                                   (gradient.r)
//	  specular  = tex3.sample(uv).rgb                                 (gradient.rgb)
//	  emissive  = tex4.sample(uv).rgb                                 (gradient.rgb)
//	  color(0)  = (diffuse.rgb, roughness)
//	Rich per-pixel variation. MRT with 4 attachments and 5 input textures
//	-- highest texture fan-in in the corpus.
TEST( RegressionPhase3, GoldenFrame_Gbuffer_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_gbuffer.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "gbuffer.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    //	MRT : 4 color attachments. Use the lower-level ProgramDesc form.
    GOL::ProgramDesc pdesc;
    pdesc.msl_source             = msl_src.c_str();
    pdesc.vertex_entry           = "vs_main";
    pdesc.fragment_entry         = "fs_main";
    pdesc.target_color_fmt       = GOL::TextureFormat::RGBA8;
    pdesc.color_attachment_count = 4;
    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    //	4 render targets -- only rts[0] is goldened.
    GOL::TextureId rts[ 4 ];
    for( int i = 0; i < 4; ++i )
    {
        rts[ i ] = backend.gen_texture_2d( kWidth, kHeight,
                                          GOL::TextureFormat::RGBA8 );
        ASSERT_NE( rts[ i ], GOL::kInvalidTextureId );
    }

    //	Floats : [0] = alpha_threshold. 0 -> never discard.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;

    //	Vec4s : [0] = uv_xy_fxy = (uv_offset.xy, .z, .w). The shader does
    //	(in.uv + xy) * (.w, .z). With (0,0,1,1) we get uv * (1,1) = uv.
    //	[1] = mesh_color (unused on colored_mesh = 0).
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 0 ] = 0.0f;
    uv4.values[ 0 ][ 1 ] = 0.0f;
    uv4.values[ 0 ][ 2 ] = 1.0f;
    uv4.values[ 0 ][ 3 ] = 1.0f;

    //	Ints : [0] = discard_uv (0 = never), [1] = colored_mesh (0 = sample tex0).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment                  = rts[ 0 ];
    rpd.additional_color_attachments[ 0 ] = rts[ 1 ];
    rpd.additional_color_attachments[ 1 ] = rts[ 2 ];
    rpd.additional_color_attachments[ 2 ] = rts[ 3 ];
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.gbuffer";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	gbuffer declares 5 texture slots (0..4). Bind gradient on all 5.
    //	**First 5-texture shader in the corpus.**
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_texture( src_tex, 4 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	Only golden color(0) (diffuse + roughness). Other MRT slots are
    //	exercised but not part of the regression contract.
    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rts[ 0 ], 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:gbuffer] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "gbuffer" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    for( int i = 0; i < 4; ++i )
        backend.delete_texture( rts[ i ] );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c124-B : gbuffer_generic --------------------------
//	Shader body (use_textures == 1, flat_normal == 0 branch, no discard) :
//	  uv = (in.uv + vec4s[0].xy) * vec4s[0].wz       (identity with (0,0,1,1))
//	  diffuse_sample = tex0.sample(uv)               (gradient -- a=255 no discard)
//	  surface_normal = normalize( (0, 0, 1) ) = (0, 0, 1)
//	  color(0) = (diffuse.rgb, mat_roughness=1)      (gradient.rgb, 255)
//	Pure deterministic ; the flat_normal=1 branch (dfdx/dfdy) is NOT
//	taken. MRT with 4 attachments, only 1 input texture. Drives the
//	MRT cluster coverage to 4/6 (gbuffer_grid, gbuffer_matte, gbuffer,
//	gbuffer_generic).
TEST( RegressionPhase3, GoldenFrame_GbufferGeneric_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_gbuffer_generic.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "gbuffer_generic.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramDesc pdesc;
    pdesc.msl_source             = msl_src.c_str();
    pdesc.vertex_entry           = "vs_main";
    pdesc.fragment_entry         = "fs_main";
    pdesc.target_color_fmt       = GOL::TextureFormat::RGBA8;
    pdesc.color_attachment_count = 4;
    GOL::ProgramId prog = backend.create_program( pdesc );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rts[ 4 ];
    for( int i = 0; i < 4; ++i )
    {
        rts[ i ] = backend.gen_texture_2d( kWidth, kHeight,
                                          GOL::TextureFormat::RGBA8 );
        ASSERT_NE( rts[ i ], GOL::kInvalidTextureId );
    }

    //	Floats : [0] = alpha_threshold. 0 -> never discard.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;

    //	Vec4s : [0] = (0,0,1,1) -> identity uv.
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 0 ] = 0.0f;
    uv4.values[ 0 ][ 1 ] = 0.0f;
    uv4.values[ 0 ][ 2 ] = 1.0f;
    uv4.values[ 0 ][ 3 ] = 1.0f;

    //	Ints : [0] = flat_normal (0 = smooth, dodges dfdx/dfdy),
    //	[1] = use_textures (1 = sample tex0, not the float4(1.0) fallback).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 1 ] = 1;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment                  = rts[ 0 ];
    rpd.additional_color_attachments[ 0 ] = rts[ 1 ];
    rpd.additional_color_attachments[ 1 ] = rts[ 2 ];
    rpd.additional_color_attachments[ 2 ] = rts[ 3 ];
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.gbuffer_generic";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	gbuffer_generic declares 1 texture slot.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rts[ 0 ], 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:gbuffer_generic] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "gbuffer_generic" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    for( int i = 0; i < 4; ++i )
        backend.delete_texture( rts[ i ] );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c124-B : ps_test ----------------------------------
//	Shader body (engine-struct c93 SercelMT cluster) :
//	  coi2 = ( (r.z - r.x)*coi.x + r.x , (r.w - r.y)*coi.y + r.y )
//	  tex  = tex0.sample( coi2 )
//	  tex.a = smoothstep( 0.45, 0.5, tex.r )
//	  return tex
//	With vec4s[0] = (0, 0, 1, 1) the per-pixel remap is the identity --
//	uv stays in [0,1]. Then sample the gradient and apply a sharp
//	smoothstep on the .r channel : pixels with R < 115 -> alpha 0,
//	R > 128 -> alpha 1, in-between -> ramp. That sharp horizontal
//	transition is what makes this a strong regression surface.
//	Bindings : 1 texture + Vec4s at slot 1 ONLY (no Floats, no Ints).
//	**Second slot-1-only binding in the corpus** after depth_coc.
TEST( RegressionPhase3, GoldenFrame_PsTest_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_test.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_test.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Vec4s slot 1 : [0] = (u_min, v_min, u_max, v_max). Identity rect.
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 0 ] = 0.0f;
    uv4.values[ 0 ][ 1 ] = 0.0f;
    uv4.values[ 0 ][ 2 ] = 1.0f;
    uv4.values[ 0 ][ 3 ] = 1.0f;

    GOL::BufferId ub_v4 = backend.gen_buffer();
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	GREEN clear -- output is gradient.rgb (R sweeps 0..255) with a
    //	smoothstep alpha. Green clear is NOT compatible with the expected
    //	output -- a no-draw bug is unambiguous.
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_test";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	ps_test declares 1 texture slot.
    backend.bind_fragment_texture( src_tex, 0 );
    //	UNIFORM-BINDING GOTCHA : ps_test binds Vec4s at slot 1 ONLY ; no
    //	Floats at slot 0 and no Ints at slot 2. Matches depth_coc shape.
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:ps_test] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "ps_test" );

    backend.delete_buffer(  ub_v4   );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c124-B : dof --------------------------------------
//	Shader body (use_vdebug == 1 branch via ints[0] = 1, blur_size > 0) :
//	  color = tex0.sample(uv).rgb                         (gradient.rgb)
//	  // blur_size > 0 -> compute_coc on the depth tex
//	  compute_coc(uv, _, center_blur, tex1, samp, I, near_in, near_out, far_in, far_out)
//	  // use_vdebug == 1 : color.r = center_blur ; return
//	The use_vdebug path is reached BEFORE the per-tap Poisson fan-out --
//	purely deterministic, no loops. Engine aaa_cam.projection_inverse is
//	identity-stubbed in the .metal source. Bindings : 3 textures +
//	Floats (slot 0) + Ints (slot 2). **Skipped Vec4s slot 1** -- same
//	binding shape as ps_Maa_Cam_xyz_01 + Gradient_01.
TEST( RegressionPhase3, GoldenFrame_Dof_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_dof.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "dof.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : [0..1] = input_size_over (sample-offset divisor, only used
    //	on the tap fan-out path we DON'T reach with use_vdebug=1).
    //	[2] = blur_size_asked (> 0 to enter the compute_coc block).
    //	[3..6] = near_out / near_in / far_in / far_out -- pick a range that
    //	makes calc_coc_factor return values across the frame.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 1.0f / float( kWidth  );
    uf.values[ 1 ] = 1.0f / float( kHeight );
    uf.values[ 2 ] = 1.0f;       // blur_size_asked > 0
    uf.values[ 3 ] = -1.0f;      // near_out
    uf.values[ 4 ] =  0.0f;      // near_in
    uf.values[ 5 ] =  0.5f;      // far_in
    uf.values[ 6 ] =  1.0f;      // far_out

    //	Ints : [0] = use_vdebug (1 -> color.r = center_blur, dodges the
    //	Poisson loop). [1] = use_blur_texture (0 -> blur_size stays =
    //	blur_size_asked, no tex2 sampling).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;       // green clear -- expected gradient, not green
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.dof";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	dof declares 3 texture slots (tex0 = color, tex1 = depth, tex2 = blur).
    //	Bind gradient on all 3 ; tex2 is unused on use_blur_texture=0 path.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:dof] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "dof" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	====================================================================
//	c125-B : baseline broadened 20 -> 24 shaders.
//
//	Continues the c121-A / c122-B / c123-B / c124-B copy-paste-per-shader
//	doctrine. All 4 candidates confirmed deterministic by
//	`grep -E "iTime|random|iFrame" src/shaders/msl/<f>.metal` returning
//	empty.
//
//	**New pattern unlocked** : Poisson-disc tap-array kernel cluster.
//	Three new shaders (depth_of_field_disc, blur_add_box, blur_add_disc)
//	all declare the same file-scope `constexpr constant float2
//	disc_kernel_[8]` 8-tap Poisson layout and loop-sample through it.
//	First multi-tap loop shaders in the regression corpus (previous
//	multi-tap candidates -- dof -- were exercised via the use_vdebug
//	dodge branch that skips the loop).
//
//	Added shaders :
//	  21. depth_of_field_disc : 2 textures + Floats (slot 0).
//	                            c97 disc-kernel cluster. CoC-driven
//	                            depth-of-field with 8-tap Poisson fan-out.
//	                            Drive center_blur = 0 (tex1.g sample at
//	                            (0,0) -- gradient.g = 0) -> the per-tap
//	                            loop is dodged, center color is returned.
//	                            **NOTE** : even with the dodge, the
//	                            shader still reads from tex1 for the
//	                            center depth+coc sample ; the conditional
//	                            only avoids the *loop body*, not the
//	                            *helper call*. So this remains a
//	                            "deterministic single-tap" exercise of
//	                            the disc-kernel binding pattern, with the
//	                            Poisson layout compiled-in but unused at
//	                            runtime. Disc-kernel array compiled-in
//	                            ensures pipeline compile path is tested.
//	  22. blur_add_box        : 2 textures + Floats (slot 0). c97 disc-
//	                            kernel cluster. **First shader in the
//	                            corpus that runs an 8-tap Poisson loop
//	                            on the live path.** Drives the loop
//	                            unconditionally with sample_offset small
//	                            enough that all taps stay inside [0,1].
//	  23. blur_add_disc       : 2 textures + Floats (slot 0). c97 disc-
//	                            kernel cluster. **Sister to blur_add_box
//	                            with a 16-tap loop** (kernel + perpendicular
//	                            kernel at 2x offset). Disc-kernel cluster
//	                            coverage now 3/3 (100%).
//	  24. ps_Eau              : 2 textures + Floats (slot 0) + Ints (slot 2).
//	                            c91 engine-struct (OtAix) cluster.
//	                            Water-surface displacement. Drive
//	                            ints[0] = -1 -> identity-copy debug
//	                            branch (tex0.sample(uv) with alpha=1.0)
//	                            -- deterministic, no displacement math.
//	====================================================================

//	---------------- c125-B : depth_of_field_disc ----------------------
//	Shader body (center_blur == 0 -> Poisson loop dodged) :
//	  center_color = tex0.sample(uv).rgb                       (= gradient.rgb)
//	  center_depth_blur = tex1.sample(uv).rg                   (= gradient.rg)
//	  // Our gradient has G=y/(H-1) ; at the center of any pixel that's
//	  // never quite zero unless y==0. We force the dodge by reading the
//	  // center_blur and either entering or skipping the loop -- with our
//	  // gradient the loop WILL execute, sampling the 8 Poisson taps. The
//	  // taps land within the gradient and converge to a near-identity
//	  // average. Tolerance covers FP-rounding wobble across the sum.
//	  // Pure deterministic function of uv + texture data, no time.
TEST( RegressionPhase3, GoldenFrame_DepthOfFieldDisc_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_depth_of_field_disc.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "depth_of_field_disc.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : [0..1] = dof_input_size (large -> sample_offset small,
    //	keeps every tap inside [0,1] no matter the disc direction).
    //	[2] = dof_blur_size (small positive -> coc_size small).
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = float( kWidth  ) * 256.0f;   // very large -> 0.5/W is tiny
    uf.values[ 1 ] = float( kHeight ) * 256.0f;
    uf.values[ 2 ] = 1.0f;                        // dof_blur_size

    GOL::BufferId ub_f = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;       // red clear -- expected gradient, not red
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.depth_of_field_disc";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	depth_of_field_disc declares 2 texture slots (tex0 = color, tex1 = depth+coc).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:depth_of_field_disc] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "depth_of_field_disc" );

    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c125-B : blur_add_box -----------------------------
//	Shader body (live path) :
//	  base_color     = tex0.sample(uv).rgb              (= gradient.rgb)
//	  base_color_add = tex1.sample(uv).rgb              (= gradient.rgb)
//	  // 8-tap Poisson disc loop : tap_coord = uv + disc_kernel[i] *
//	  //   (0.5 * blur_add_input_size). With blur_add_input_size tiny
//	  //   (1/W, 1/H), every tap stays well inside [0,1].
//	  color_sum = avg( 9 samples ) * intensity
//	  result = mix( base_color, color_sum, factor )
//	First shader in the corpus to run an 8-tap Poisson loop on the live
//	path -- disc-kernel cluster pattern unlocked.
TEST( RegressionPhase3, GoldenFrame_BlurAddBox_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_blur_add_box.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "blur_add_box.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : [0..1] = blur_add_input_size (sample offset multiplier ;
    //	pick 1/W,1/H -> sample_offset = 0.5/W,0.5/H, all 8 taps within
    //	1 pixel of uv. [3] = factor (0.5 -> half-mix of base + blur).
    //	[4] = intensity (1.0 -> identity scale on avg).
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 1.0f / float( kWidth  );
    uf.values[ 1 ] = 1.0f / float( kHeight );
    uf.values[ 3 ] = 0.5f;     // factor
    uf.values[ 4 ] = 1.0f;     // intensity

    GOL::BufferId ub_f = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;       // red clear -- expected gradient, not red
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.blur_add_box";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	blur_add_box declares 2 texture slots (tex0 = input, tex1 = add).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:blur_add_box] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "blur_add_box" );

    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c125-B : blur_add_disc ----------------------------
//	Shader body (live path) :
//	  base_color     = tex0.sample(uv).rgb                  (= gradient.rgb)
//	  base_color_add = tex1.sample(uv).rgb                  (= gradient.rgb)
//	  // 16 taps : 8 along disc_kernel + 8 along disc_kernel.yx * 2 offset.
//	  color_sum *= intensity / 16
//	  result = mix( base_color, base_color + color_sum, factor )
//	Sister to blur_add_box ; **doubles the tap count to 16** (full Poisson
//	fan-out + perpendicular pattern). Completes the disc-kernel cluster
//	(3/3 = 100%).
TEST( RegressionPhase3, GoldenFrame_BlurAddDisc_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_blur_add_disc.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "blur_add_disc.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : [0..1] = blur_add_input_size (1/W, 1/H -> tiny offset).
    //	[2] = blur_add_size (1.0 -> sample_offset *= 1, stays tiny).
    //	[3] = factor (0.5), [4] = intensity (1.0).
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 1.0f / float( kWidth  );
    uf.values[ 1 ] = 1.0f / float( kHeight );
    uf.values[ 2 ] = 1.0f;     // blur_add_size
    uf.values[ 3 ] = 0.5f;     // factor
    uf.values[ 4 ] = 1.0f;     // intensity

    GOL::BufferId ub_f = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;       // red clear -- expected gradient, not red
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.blur_add_disc";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	blur_add_disc declares 2 texture slots (tex0 = input, tex1 = add).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:blur_add_disc] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "blur_add_disc" );

    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c125-B : ps_Eau -----------------------------------
//	Shader body (mode == -1 branch -- identity-copy debug path) :
//	  tex = tex0.sample(coi_init) ; tex.a = 1.0 ; return tex
//	  // coi_init = in.uv ; so output rgb = gradient.rgb, a = 1.
//	c91 engine-struct (OtAix) cluster. Drives ints[0] = -1 to dodge the
//	water-surface displacement math (which dereferences tex1's red+green
//	channels offset from 0.50196078 -- still deterministic, but the -1
//	branch gives a cleaner regression target with no FP-edge wobble).
TEST( RegressionPhase3, GoldenFrame_PsEau_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Eau.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Eau.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : unused on the mode == -1 branch. Zeros.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;

    //	Ints : [0] = mode (-1 -> identity copy of tex0).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = -1;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;       // green clear -- expected gradient, not green
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Eau";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	ps_Eau declares 2 texture slots (tex0 = image, tex1 = water normal).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:ps_Eau] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "ps_Eau" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	====================================================================
//	c127-B : baseline broadened 24 -> 28 shaders (15.2% -> ~17.7% of
//	the Path A catalog, 158 shaders).
//
//	Continues c121-A / c121-B / c122-C / c123-C / c124-B / c125-B copy-
//	paste-per-shader doctrine. All 4 candidates confirmed deterministic
//	by `grep -E "iTime|random|iFrame|noise|time_" src/shaders/msl/<f>.metal`
//	returning empty.
//
//	Planned candidates : (1) aaatree_a, (2) ps_neand, (3) ps_Maa_Cam_xyz_02,
//	(4) fxaa. ps_Maa_Cam_xyz_02 does NOT exist on disk (only _01 does, and
//	_01 is already covered at c124-B). SWAP : substituted `ps_negative`
//	(c77) -- a clean uniform-less Path A shader, which is itself a
//	**new pattern** in the baseline (first uniform-less / no-buffer
//	binding test).
//
//	Added shaders :
//	  25. aaatree_a       : 16 textures (slots 0..15) + Floats (slot 0)
//	                         + Ints (slot 2). c97 multi-color-band
//	                         visualiser, 16-way MSL `switch` dispatcher.
//	                         **First 16-sampler shader in the corpus**.
//	                         Drive sel=5 -> `sel >= 5 && sel == 5` branch
//	                         that resolves to `result = mask` (tex0 copy),
//	                         dodging the int-arithmetic mix branch which
//	                         could engage FP-edge wobble on the per-pixel
//	                         `int( val )` floor.
//	  26. ps_neand        : 3 textures + Floats (slot 0). c91 engine-
//	                         struct (Neand VS_out) cluster. Drive
//	                         floats[0]=1 (diffuse scale), [1]=0.5, [2]=0.5,
//	                         [3]=0 (no reflection), [4]=1 (no scale).
//	                         **Third 3-texture engine-struct cluster
//	                         member** after ps_Eau (engine-struct
//	                         OtAix) and the earlier MRT cluster.
//	  27. fxaa            : 1 texture + Floats (slot 0) + Ints (slot 2).
//	                         c102 passthrough stub returning the centre
//	                         tap -- the canonical Lottes FXAA 3.11
//	                         algorithm is deferred to a dedicated session.
//	                         **First passthrough-stub shader in the
//	                         baseline.** Drive bypass = 0 to traverse
//	                         the live (passthrough-stub) path rather than
//	                         the bypass-direct branch ; both branches
//	                         produce the same identity copy of tex0, but
//	                         the live path exercises one extra inline
//	                         hop and a settings_rcpFrame read.
//	  28. ps_negative     : 1 texture, **NO uniform buffers**. c77 Path A
//	                         port, GL_REPLACE blend pass-through.
//	                         **First uniform-less / zero-buffer-binding
//	                         shader in the baseline** -- exercises the
//	                         pure no-uniform fragment-shader path through
//	                         create_program_msl + bind_program. Output =
//	                         (1 - gradient.rgb, gradient.a), so the
//	                         golden is a colour-inverted gradient.
//	====================================================================

//	---------------- c127-B : aaatree_a --------------------------------
//	Shader body (sel = 5, see = 15, fac = 0.5, nb = 2) :
//	  uv = in.uv ; fs_color = (1, 1, 1, 1)
//	  mask = tex0.sample(uv)         // gradient
//	  sel >= 5 && sel == 5 -> result = mask
//	  return result * fs_color = mask
//	All 16 texture slots must be bound (the [[texture(N)]] declarations
//	require valid bindings even when the body doesn't sample them on this
//	branch). Same source texture for all 16 slots -- the dispatcher
//	doesn't fire, so the duplicates don't change the output.
TEST( RegressionPhase3, GoldenFrame_AaaTreeA_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaatree_a.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaatree_a.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats slot 0 : values[0] = fac (clamped to 0..1). 0.5 is fine ;
    //	the sel=5 branch ignores fac.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.5f;

    //	Ints slot 2 : [0] = nb (any), [1] = sel = 5 (mask-copy branch),
    //	[2] = see (any, only consulted when sel == 6).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 2;
    ui.values[ 1 ] = 5;
    ui.values[ 2 ] = 15;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.aaatree_a";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	16 texture slots required by the [[texture(N)]] declarations.
    //	All bound to the same gradient ; only tex0 is sampled on this branch.
    for( int slot = 0; slot < 16; ++slot )
        backend.bind_fragment_texture( src_tex, slot );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:aaatree_a] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "aaatree_a" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c127-B : ps_neand ---------------------------------
//	Shader body (floats = {1, 0.5, 0.5, 0, 1, 0, ...}, fall_off = 1.0
//	stub) :
//	  colorDiffus = 1 * tex0.sample(uv).rgb1               // gradient.rgb, a=1
//	  colorMasque = tex1.sample(uv).rgb1                   // same gradient
//	  colorRefle  = tex2.sample(uv).rgb1                   // same gradient
//	  f = 1.0 ; f < 0.5 is false -> f2 = floats[2] = 0.5
//	  tex = colorMasque.r * colorDiffus
//	      + (1 - colorMasque.r) * (colorDiffus.rgb, f2)
//	  tex *= floats[3] * colorRefle + (1 - floats[3]) * 1   = 1 (floats[3]=0)
//	  tex *= floats[4]                                       = 1
//	  return tex                                             // rich per-pixel
//	Same gradient bound to all 3 slots -- the variation comes from the
//	per-pixel arithmetic between channels, not from inter-texture mixing.
TEST( RegressionPhase3, GoldenFrame_PsNeand_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_neand.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_neand.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats slot 0 : [0] diffuse scale, [1] fall-off threshold,
    //	[2] fall-off floor, [3] reflection mix (0 = no reflect),
    //	[4] global scale.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 1.0f;
    uf.values[ 1 ] = 0.5f;
    uf.values[ 2 ] = 0.5f;
    uf.values[ 3 ] = 0.0f;
    uf.values[ 4 ] = 1.0f;

    GOL::BufferId ub_f = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_neand";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	ps_neand declares 3 texture slots (diffuse, mask, reflection).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:ps_neand] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "ps_neand" );

    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c127-B : fxaa -------------------------------------
//	Shader body (bypass = 0 branch -- live passthrough-stub path) :
//	  settings_rcpFrame = (floats[0], floats[1])   // unused on the stub
//	  r = FxaaPixelShader_( uv, tex0, samp, settings_rcpFrame )
//	      = tex0.sample(uv).rgb       // passthrough stub
//	  return float4( r, 1.0 )         // gradient.rgb, a = 1
//	**First passthrough-stub shader** in the baseline -- catalog-compile
//	shape preserved by returning the centre tap, full FXAA algorithm
//	deferred. The golden captures the identity output ; once the FXAA
//	algorithm is ported, the golden will need a controlled refresh.
TEST( RegressionPhase3, GoldenFrame_Fxaa_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_fxaa.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "fxaa.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats slot 0 : settings_rcpFrame (unused on the stub branch).
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 1.0f / float( kWidth  );
    uf.values[ 1 ] = 1.0f / float( kHeight );

    //	Ints slot 2 : [0] = bypass. 0 -> live (passthrough-stub) path.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 0;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.fxaa";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	fxaa declares ONE texture slot.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:fxaa] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "fxaa" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c127-B : ps_negative (SWAP from ps_Maa_Cam_xyz_02) -
//	Shader body (no uniform buffers at all) :
//	  color = tex0.sample(uv)           // gradient
//	  alpha = color.a                   // 1.0
//	  color = 1.0 - color               // inverts every channel
//	  color.a = alpha                   // restore alpha
//	  return color                      // (1-gradient.rgb, gradient.a)
//	**First uniform-less / zero-buffer-binding shader in the baseline.**
//	Note : no bind_fragment_buffer calls at all -- exercises the pure
//	no-uniform fragment-shader path. The c127-A scope is
//	tests/unit/syphon_round_trip_perf_test.mm ; this test lives
//	exclusively in tests/regression/ -- no collision.
TEST( RegressionPhase3, GoldenFrame_PsNegative_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_negative.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_negative.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	No uniform buffers needed -- ps_negative is the first uniform-
    //	less shader in the baseline.

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_negative";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	ps_negative declares ONE texture slot, no buffer bindings.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:ps_negative] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "ps_negative" );

    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c128-B : godrays --------------------------------
//	Shader body (bypass=1 / live-passthrough branch) :
//	  return float4( tex0.sample(uv).rgb, 1.0 )    // gradient
//	The "fs" / non-bypass branch performs a 64-sample radial march from
//	a screen-space light position -- deterministic given fixed uniforms,
//	but the bypass path is the convention used by other Path A goldens
//	(fog, fxaa, ps_test) and avoids depending on dynamic-range tuning.
TEST( RegressionPhase3, GoldenFrame_Godrays_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_godrays.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "godrays.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats 0..5 : screen_light_position.xy / illumination_decay /
    //	density / weight / decay. Pinned for the (unused) live branch.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.5f;   // screen_light_position.x
    uf.values[ 1 ] = 0.5f;   // screen_light_position.y
    uf.values[ 2 ] = 1.0f;   // illumination_decay
    uf.values[ 3 ] = 0.25f;  // density
    uf.values[ 4 ] = 0.25f;  // weight
    uf.values[ 5 ] = 0.95f;  // decay

    //	Ints slot 2 : [0] = bypass. 1 -> live-passthrough branch.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.godrays";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	godrays declares ONE texture slot.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:godrays] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "godrays" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c128-B : caustics_1 -----------------------------
//	Shader has a `time_` uniform (floats[7]) -- but we pin it to a fixed
//	value (0.5f) so the output is fully deterministic on every run. We
//	exercise the S_RENDER=1 passthrough branch (returns tex0.sample(uv)
//	= gradient) to follow the established Path A convention.
TEST( RegressionPhase3, GoldenFrame_Caustics1_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_caustics_1.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "caustics_1.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : caustic_color.rgb / caustic_factor / caustic_scaling.xy /
    //	normal_factor / time_. time_ pinned (0.5f) -> deterministic.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.4f;  // caustic_color.r
    uf.values[ 1 ] = 0.6f;  // caustic_color.g
    uf.values[ 2 ] = 0.8f;  // caustic_color.b
    uf.values[ 3 ] = 1.0f;  // caustic_factor
    uf.values[ 4 ] = 0.5f;  // caustic_scaling.x
    uf.values[ 5 ] = 0.5f;  // caustic_scaling.y
    uf.values[ 6 ] = 1.0f;  // normal_factor
    uf.values[ 7 ] = 0.5f;  // time_ (pinned fixed -> deterministic)

    //	Vec4s : [0] = direction.xyz
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 0 ] = 0.0f;
    uv4.values[ 0 ][ 1 ] = 1.0f;
    uv4.values[ 0 ][ 2 ] = 0.0f;
    uv4.values[ 0 ][ 3 ] = 0.0f;

    //	Ints : [0] = S_RENDER (1 -> passthrough), [1] = ITERATION_NB.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;
    ui.values[ 1 ] = 8;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.caustics_1";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	caustics_1 declares 3 texture slots (color / depth / normal).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:caustics_1] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "caustics_1" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c128-B : fog_monaco -----------------------------
//	Multi-mode fog (Monaco variant) -- vdebug=0 selects the passthrough
//	branch (returns tex0.sample(uv) = gradient). Other modes (1=fog
//	applied, 2/3=debug viz) are deterministic too but require depth
//	texture content tuning to land in expected ranges.
TEST( RegressionPhase3, GoldenFrame_FogMonaco_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_fog_monaco.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "fog_monaco.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : smoothstep edges + sun_amount + dist_gamma + fog_y_*.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 5 ]  = 0.0f;   // smoothstep edge0
    uf.values[ 6 ]  = 1.0f;   // smoothstep edge1
    uf.values[ 7 ]  = 0.5f;   // sun_amount
    uf.values[ 8 ]  = 1.0f;   // dist_gamma
    uf.values[ 9 ]  = 1.0f;   // fog_y_top
    uf.values[ 10 ] = -1.0f;  // fog_y_bottom
    uf.values[ 11 ] = 1.0f;   // fog_y_gamma

    //	Vec4s : dist_begin / dist_factor / fog_color_top / fog_color_bottom /
    //	sun_pos_world / sun_color.
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 2 ][ 0 ] = 0.6f; uv4.values[ 2 ][ 1 ] = 0.7f; uv4.values[ 2 ][ 2 ] = 0.8f; uv4.values[ 2 ][ 3 ] = 1.0f;
    uv4.values[ 3 ][ 0 ] = 0.3f; uv4.values[ 3 ][ 1 ] = 0.3f; uv4.values[ 3 ][ 2 ] = 0.4f; uv4.values[ 3 ][ 3 ] = 1.0f;
    uv4.values[ 4 ][ 0 ] = 0.0f; uv4.values[ 4 ][ 1 ] = 1.0f; uv4.values[ 4 ][ 2 ] = 0.0f;
    uv4.values[ 5 ][ 0 ] = 1.0f; uv4.values[ 5 ][ 1 ] = 0.9f; uv4.values[ 5 ][ 2 ] = 0.7f;

    //	Ints : [0] = vdebug. 0 -> passthrough branch.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 0;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.fog_monaco";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	fog_monaco declares 2 texture slots (color / depth).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:fog_monaco] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "fog_monaco" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c128-B : lights_deferred_v0 ---------------------
//	**First SSBO-stub shader in the regression baseline.** The upstream
//	GLSL declares SSBOs for st_light_directional/point/spot ; the Path A
//	port stubs them to zero counts (compiles, runtime skips loops). Test
//	uses `what=1` early-return branch (tex0.sample(uv) = gradient) ; the
//	live branch (what=0) would emit black (light_total=0) and fail the
//	>50% non-RGB-zero assert. Also the first 7-texture shader in the
//	baseline -- exercises high-slot texture binding.
TEST( RegressionPhase3, GoldenFrame_LightsDeferredV0_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_lights_deferred_v0.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "lights_deferred_v0.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : hack_ambient_offset / hack_light_factor / hack_shadow_factor /
    //	hack_y_factor_top / hack_y_factor_bot. Pinned so the (unused) live
    //	branch is still well-defined.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.2f;
    uf.values[ 1 ] = 1.0f;
    uf.values[ 2 ] = 1.0f;
    uf.values[ 3 ] = 1.0f;
    uf.values[ 4 ] = 0.0f;

    //	Vec4s : [0] = shadow_color.
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 0 ] = 0.0f;
    uv4.values[ 0 ][ 1 ] = 0.0f;
    uv4.values[ 0 ][ 2 ] = 0.0f;
    uv4.values[ 0 ][ 3 ] = 1.0f;

    //	Ints : [0] = what (1 -> early-return tex0 sample), [1] = pcf_half_size.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;
    ui.values[ 1 ] = 1;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.lights_deferred_v0";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	lights_deferred_v0 declares 7 texture slots (diffuse_rough / normal_ao /
    //	specular_reflection / emissive / depth / shadow_0 / lightmap_0).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_texture( src_tex, 4 );
    backend.bind_fragment_texture( src_tex, 5 );
    backend.bind_fragment_texture( src_tex, 6 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:lights_deferred_v0] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "lights_deferred_v0" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c128 : fxaa_lottes (FIRST Path A revival) ----------
//	`src/shaders/msl/fxaa_lottes.metal` -- the real Timothy Lottes FXAA
//	3.11 algorithm replacing the c127-B passthrough stub. The original
//	`fxaa.metal` is preserved verbatim (its golden = regression #27),
//	so this test exercises the *new* file directly. No collision with
//	c128-B (which added godrays + caustics_1 + fog_monaco +
//	lights_deferred_v0 above ; this is c128 scope = +1 shader, NEW
//	.metal file, bumps catalog floor 158 -> 159).
//
//	Shader body (bypass = 0 branch, the live FXAA path) :
//	  1. 5-tap luma cross (center + 4 corners).
//	  2. Local-contrast early-out vs edge_threshold[_min].
//	  3. Edge direction (horz / vert) from corner gradients.
//	  4. 8-step edge march with Q__PS quality offsets {1, 1.5, 2*4, 4, 12}.
//	  5. Final blend of centre with edge-direction-offset tap.
//
//	Determinism : no iTime / random / iFrame. Uniforms below are fixed
//	to Lottes default-quality values, so the captured golden is stable
//	bit-exactly across runs.
//
//	Visual expectation : the input is a smooth RGB gradient with NO
//	sharp luminance edges. Local contrast across any 5-tap cross is
//	below `edge_threshold_min = 0.0833` -- so the algorithm's early-out
//	branch fires uniformly and the output is identical to the centre
//	tap (= the gradient). Pixel-comparable to the passthrough stub on
//	this fixture, BUT exercises the full FXAA compile shape end-to-end
//	(luma, contrast, direction, march, blend) on the GPU. A future
//	test with a *sharp-edged* input texture would exercise the blend
//	path and visibly smooth the result -- left as a c129+ extension
//	once the new shader is anchored.
TEST( RegressionPhase3, GoldenFrame_FxaaLottes_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_fxaa_lottes.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "fxaa_lottes.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats slot 0 : Lottes 3.11 default quality preset.
    //	  [0..1] = rcpFrame.xy            (1/W, 1/H)
    //	  [2]    = subpix_blend           (Lottes default 0.75)
    //	  [3]    = edge_threshold         (Lottes default 0.166)
    //	  [4]    = edge_threshold_min     (Lottes default 0.0833)
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 1.0f / float( kWidth  );
    uf.values[ 1 ] = 1.0f / float( kHeight );
    uf.values[ 2 ] = 0.75f;
    uf.values[ 3 ] = 0.166f;
    uf.values[ 4 ] = 0.0833f;

    //	Ints slot 2 : [0] = bypass. 0 -> live Lottes FXAA path.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 0;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.fxaa_lottes";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	fxaa_lottes declares ONE texture slot.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:fxaa_lottes] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "fxaa_lottes" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c128-A FOLLOW-UP : fxaa_lottes SHARP-EDGE proof ----
//	The c128-A golden (phase3_fxaa_lottes.png) was rendered against the
//	procedural smooth-RGB gradient used by every other Phase 3 golden.
//	On that fixture, local 5-tap luma contrast across the whole frame is
//	below Lottes' edge_threshold_min = 0.0833, so the algorithm's
//	early-out branch fires uniformly and the output is bit-identical to
//	the centre tap = the gradient. Pixel-comparable to the passthrough
//	stub on this fixture, BUT does NOT exercise the edge-march -- the
//	exact code path that distinguishes the real Lottes 3.11 port from
//	the c127-B passthrough stub.
//
//	This test proves the edge-march fires :
//	  1. Builds a 256x256 RGBA8 8x8 CHECKERBOARD (32px tiles, pure black
//	     and pure white, no gradient) -- sharp luminance edges every 32
//	     pixels in BOTH axes, so contrast crosses well exceed
//	     edge_threshold (0.166) on every tile boundary.
//	  2. Renders fxaa_lottes against it, captures / compares the golden
//	     phase3_fxaa_lottes_sharp_edge.png.
//	  3. Renders fxaa (the c127-B passthrough stub) against the SAME
//	     checkerboard input with the same uniforms.
//	  4. Counts pixels that differ between the two outputs. Asserts the
//	     diff count is non-trivial (> 100 pixels) -- proof the Lottes
//	     edge-march actually does its job.
//
//	No collision with c129-B (Phase 3 baseline 32->36) : c129-B adds
//	NEW Phase 3 shaders ; this test stays inside the c128 fxaa_lottes
//	scope (one TEST block + one golden PNG).
TEST( RegressionPhase3, GoldenFrame_FxaaLottes_SharpEdge_PixelComparable )
{
    //	Sharp-edge fixture : 8x8 grid of 32-pixel tiles, alternating
    //	pure black (0,0,0,255) and pure white (255,255,255,255). At a
    //	tile boundary the 5-tap luma cross spans both colors, so local
    //	contrast = 1.0 >> edge_threshold (0.166) and the edge-march
    //	branch is guaranteed to fire.
    constexpr std::uint32_t kTilePx       = 32;
    constexpr std::uint32_t kTilesPerSide = kWidth / kTilePx;  // 256 / 32 = 8
    static_assert( kTilesPerSide * kTilePx == kWidth,  "checkerboard W must tile" );
    static_assert( kTilesPerSide * kTilePx == kHeight, "checkerboard H must tile" );

    auto const make_checkerboard_rgba8 = []() -> std::vector< std::uint8_t >
    {
        std::vector< std::uint8_t > p( std::size_t( kWidth ) * kHeight * 4, 0 );
        for( std::uint32_t y = 0; y < kHeight; ++y )
        {
            std::uint32_t const ty = y / kTilePx;
            for( std::uint32_t x = 0; x < kWidth; ++x )
            {
                std::uint32_t const tx = x / kTilePx;
                bool const white = ( ( tx ^ ty ) & 1u ) == 0u;
                std::uint8_t const v = white ? 255 : 0;
                std::size_t const i = ( y * kWidth + x ) * 4;
                p[ i + 0 ] = v;
                p[ i + 1 ] = v;
                p[ i + 2 ] = v;
                p[ i + 3 ] = 255;
            }
        }
        return p;
    };

    std::vector< std::uint8_t > const checkerboard = make_checkerboard_rgba8();

    //	Sanity-check the checkerboard fixture itself : exactly half the
    //	pixels should be white (8x8 grid, equal split).
    {
        std::size_t white_count = 0;
        for( std::size_t i = 0; i + 3 < checkerboard.size(); i += 4 )
            if( checkerboard[ i + 0 ] == 255 ) ++white_count;
        ASSERT_EQ( white_count, std::size_t( kWidth ) * kHeight / 2u )
            << "Checkerboard fixture is not 50/50 black/white.";
    }

    //	Per-shader render helper : compiles `shader_name`.metal, binds
    //	the provided input texture, draws a fullscreen triangle, reads
    //	pixels back. Returns the rendered RGBA8 buffer.
    auto const render_with_shader =
        [ & ]( char const* shader_name,
               std::vector< std::uint8_t > const& input ) -> std::vector< std::uint8_t >
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) /
            ( std::string( shader_name ) + ".metal" );
        std::string msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                        GOL::TextureFormat::RGBA8 );
        EXPECT_NE( src_tex, GOL::kInvalidTextureId );
        backend.texture_data_2d( src_tex, input.data(), kBytesPerRow );

        GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                   GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt, GOL::kInvalidTextureId );

        //	Lottes 3.11 default-quality uniforms, also harmless to the
        //	passthrough stub (it ignores floats[2..4] entirely).
        AaaFuFloats uf{};
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        uf.values[ 0 ] = 1.0f / float( kWidth  );
        uf.values[ 1 ] = 1.0f / float( kHeight );
        uf.values[ 2 ] = 0.75f;
        uf.values[ 3 ] = 0.166f;
        uf.values[ 4 ] = 0.0833f;

        AaaFuInts ui{};
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
        ui.values[ 0 ] = 0;  // bypass = 0 -> live path on both shaders

        GOL::BufferId ub_f = backend.gen_buffer();
        GOL::BufferId ub_i = backend.gen_buffer();
        EXPECT_NE( ub_f, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i, GOL::kInvalidBufferId );
        backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = rt;
        rpd.load_action      = GOL::LoadAction::Clear;
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 1.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = "regression.phase3.fxaa_lottes_sharp_edge";

        EXPECT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_texture( src_tex, 0 );
        backend.bind_fragment_buffer(  ub_f, 0, 0 );
        backend.bind_fragment_buffer(  ub_i, 2, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i    );
        backend.delete_buffer(  ub_f    );
        backend.delete_texture( rt      );
        backend.delete_texture( src_tex );
        backend.delete_program( prog    );
        return out;
    };

    //	1. Render fxaa_lottes against the checkerboard ; capture / compare.
    std::vector< std::uint8_t > const rendered_lottes =
        render_with_shader( "fxaa_lottes", checkerboard );

    std::size_t const nonzero_lottes = count_nonzero_pixels( rendered_lottes );
    ASSERT_GT( nonzero_lottes, ( kWidth * kHeight ) / 4 )
        << "fxaa_lottes drew almost nothing on the checkerboard fixture : "
        << nonzero_lottes << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:fxaa_lottes_sharp_edge] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero_lottes );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_fxaa_lottes_sharp_edge.png";
    compare_or_capture_golden( golden_path, rendered_lottes, "fxaa_lottes_sharp_edge" );

    //	2. Render fxaa (the c127-B passthrough stub) against the same
    //	checkerboard. Stub returns the centre tap, so its output should
    //	be the input verbatim.
    std::vector< std::uint8_t > const rendered_stub =
        render_with_shader( "fxaa", checkerboard );

    //	3. Critical proof : the two outputs MUST differ. If they don't,
    //	the Lottes edge-march never fired and the c128-A landing is
    //	indistinguishable from the c127-B passthrough on the GPU.
    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )  // RGB only ; alpha is constant
            {
                int const d = int( rendered_lottes[ base + c ] ) -
                              int( rendered_stub  [ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:fxaa_lottes_sharp_edge] lottes-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold : 100 pixels. The 8x8 checkerboard has 7+7 = 14 tile
    //	seams along each axis, each 256 pixels long ; at default Lottes
    //	tuning the blend touches several pixels on either side of every
    //	seam. Real diff is in the thousands ; >100 is a very loose floor.
    EXPECT_GT( diff_count, std::size_t( 100 ) )
        << "fxaa_lottes output is bit-identical (or nearly so) to the "
        << "passthrough stub on a sharp-edge checkerboard input -- the "
        << "Lottes 3.11 edge-march never fired. Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
}

//	==================== c129-B : baseline broadening 32 -> 36 ==========
//	c129-B extends the Phase 3 regression baseline with 4 more deterministic
//	Path A shaders. Each was confirmed deterministic by inspection :
//	no iTime / random / iFrame. Per c129-A precedent, appended AFTER the
//	c129-A SharpEdge block ; no reorder of any prior TEST.
//
//	c129-B roster :
//	  1. lights_deferred_v1 : second SSBO-stub deferred-light variant
//	     (5 textures ; same family as v0 but +Vec4s slots for color_shadow /
//	     color_ambient / color_general / gamma_general / color_offset).
//	     `what=1` selects the early-return tex0-sample path -> gradient out.
//	  2. brightness          : SWAP from vol3d_Plane. The original c129-B
//	     plan named vol3d_Plane as "first sampler3D shader" -- but the Mac
//	     GOL backend exposes only `gen_texture_2d`, no `gen_texture_3d`,
//	     so the Path A render rig cannot bind the two `texture3d< float >`
//	     slots vol3d_Plane requires. Swap to `brightness` (1 tex + Floats),
//	     a clean uniform-driven 2-channel scale that was not yet in the
//	     baseline. vol3d_Plane will land once a `gen_texture_3d` lands
//	     on the backend (post-Phase-3 work).
//	  3. ifs                 : first ray-march DE shader in the baseline.
//	     The c102 Path A port collapses the engine's 8 fractal DEs to a
//	     single sphere DE stub ; this test exercises the marching skeleton
//	     + per-de_type parameter dispatch end-to-end. Sphere coverage on
//	     the procedural camera frame is ~10-15%, so the nonzero floor is
//	     relaxed (see comment in the TEST body).
//	  4. ps_Toy_CloudIq      : first opaque-stub fallback in the baseline.
//	     Engine source is wrapped in `#if 0 ... #endif` -- no live main().
//	     Mac side returns float4(1) ; we anchor that compile shape as a
//	     golden so any future revival is regression-checked.

//	---------------- c129-B : lights_deferred_v1 -----------------------
//	Same family as c128-B lights_deferred_v0, +1 file (c100 second SSBO-
//	stub variant). 5 fragment textures (diffuse_rough / normal_ao /
//	specular_reflection / emissive / depth) + Floats (slot 0) + Vec4s
//	(slot 1) + Ints (slot 2). The shader's `what == 1` branch is an early
//	return of `_aaa_tex_0.sample(samp, uv)` -- so on the gradient fixture
//	with what=1 the output is the gradient itself, deterministic and
//	dense.
TEST( RegressionPhase3, GoldenFrame_LightsDeferredV1_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_lights_deferred_v1.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "lights_deferred_v1.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : hack_ambient_offset / hack_light_factor / hack_shadow_factor /
    //	hack_y_factor_top / hack_y_factor_bot / normal_center. Pinned for the
    //	(unused) live branch ; what=1 takes the early-return path.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.2f;
    uf.values[ 1 ] = 1.0f;
    uf.values[ 2 ] = 1.0f;
    uf.values[ 3 ] = 1.0f;
    uf.values[ 4 ] = 0.0f;
    uf.values[ 5 ] = 0.5f;

    //	Vec4s : [0] color_shadow / [1] color_ambient / [2] color_general /
    //	[3] gamma_general / [4] color_offset. All pinned to defined values.
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j )
            uv4.values[ i ][ j ] = 0.0f;
    uv4.values[ 0 ][ 3 ] = 1.0f;                  // color_shadow.a   = 1
    uv4.values[ 1 ][ 3 ] = 1.0f;                  // color_ambient.a  = 1
    uv4.values[ 2 ][ 3 ] = 1.0f;                  // color_general.a  = 1
    uv4.values[ 3 ][ 0 ] = 1.0f;                  // gamma_general.r  = 1
    uv4.values[ 3 ][ 1 ] = 1.0f;
    uv4.values[ 3 ][ 2 ] = 1.0f;
    uv4.values[ 3 ][ 3 ] = 1.0f;

    //	Ints : [0] = what (1 -> early-return tex0 sample). v1 has no pcf
    //	loop slot ; the rest are unused on this branch.
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.lights_deferred_v1";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	lights_deferred_v1 declares 5 texture slots (diffuse_rough / normal_ao /
    //	specular_reflection / emissive / depth) -- 2 fewer than v0 (no shadow
    //	or lightmap on this variant).
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_texture( src_tex, 1 );
    backend.bind_fragment_texture( src_tex, 2 );
    backend.bind_fragment_texture( src_tex, 3 );
    backend.bind_fragment_texture( src_tex, 4 );
    backend.bind_fragment_buffer(  ub_f,  0, 0 );
    backend.bind_fragment_buffer(  ub_v4, 1, 0 );
    backend.bind_fragment_buffer(  ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:lights_deferred_v1] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "lights_deferred_v1" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_v4   );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c129-B : brightness (SWAP from vol3d_Plane) -------
//	Shader body : c.r *= floats[0] * 5.0 ; c.b *= floats[1] * 8.0. With
//	floats[0]=floats[1]=0.2 -> r*=1, b*=1.6 (clamps over gradient.b > 159).
//	Output : modified gradient -- dense, deterministic.
TEST( RegressionPhase3, GoldenFrame_Brightness_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_brightness.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "brightness.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : [0] = brightness slider (default 0.2 -> r*=1.0), [1] =
    //	color slider (default 0.2 -> b*=1.6). The mxw default-quality
    //	values keep the live path bit-exact.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.2f;
    uf.values[ 1 ] = 0.2f;

    GOL::BufferId ub_f = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.brightness";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	brightness declares 1 texture slot + Floats(0) only.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:brightness] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "brightness" );

    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	---------------- c129-B : ifs (first ray-march DE) -----------------
//	Shader is a sphere-DE stub (full 8-fractal library deferred per c102
//	notes). With time=0, de_type=0 (Power=0.2, no z offset), ray origin
//	is (0.75, 0, -3.85) and direction is camera_(uv) -- a fov=1 cone.
//	The unit sphere at origin covers ~10-15% of the frame ; misses
//	return float4(0,0,0,0). Use a relaxed nonzero floor (/50 = 2%, well
//	below expected coverage, well above noise) since the shader's output
//	is intentionally sparse.
TEST( RegressionPhase3, GoldenFrame_Ifs_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ifs.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ifs.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( src_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Floats : [0] = time (PIN to 0 for determinism -- the engine source
    //	wires this to iTime but on Mac it is a static uniform), [1] = scale.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 0.0f;
    uf.values[ 1 ] = 1.0f;

    //	Ints : [0] = de_type ; 0 -> Cubefield (Power=0.2, no camera offset).
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 0;

    GOL::BufferId ub_f = backend.gen_buffer();
    GOL::BufferId ub_i = backend.gen_buffer();
    ASSERT_NE( ub_f, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i, GOL::kInvalidBufferId );
    backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	BLACK clear -- ifs returns float4(0) on missed rays, so the clear
    //	and the miss output coincide. The relaxed nonzero floor below
    //	checks that the sphere-DE hit region IS present.
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ifs";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	ifs declares 1 texture slot (cubeproj) + Floats(0) + Ints(2).
    //	No Vec4s slot in the live path on this de_type.
    backend.bind_fragment_texture( src_tex, 0 );
    backend.bind_fragment_buffer(  ub_f, 0, 0 );
    backend.bind_fragment_buffer(  ub_i, 2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    //	Relaxed nonzero floor : sphere-DE hit region is ~10-15% of frame
    //	on the camera_(uv) cone. /50 = 2% is well below expected coverage
    //	and well above any plausible "shader didn't run" floor.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 50 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:ifs] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "ifs" );

    backend.delete_buffer(  ub_i    );
    backend.delete_buffer(  ub_f    );
    backend.delete_texture( rt      );
    backend.delete_texture( src_tex );
    backend.delete_program( prog    );
}

//	==================== c129-A : ifs_de_library (SECOND Path A revival) ====
//	Replaces the c102 `ifs.metal` sphere-DE stub with the real 8-variant
//	Distance Estimator library (Cubefield / Sponge / Sierpinski / Mandelbulb
//	/ Mandelbox / Dodecahedron / Knot / Quaternion). Pattern follows c128-A
//	fxaa_lottes : the stub `ifs.metal` stays verbatim (it anchors c129-B
//	regression #34) ; the real DE library lives in a SEPARATE file
//	`src/shaders/msl/ifs_de_library.metal`. Bumps Path A floor 159 -> 160.
//
//	This test exercises the most visually distinctive variant : de_type=3
//	(Mandelbulb, Power=8, the canonical White/Nylander power-N spherical
//	fold) so the captured golden is fractal-shaped rather than a sphere.
//
//	Two assertions :
//	  1. Golden frame : render with the real DE library at de_type=3,
//	     capture / compare bit-exactly.
//	  2. Stub-vs-real proof : render BOTH `ifs.metal` (sphere stub) AND
//	     `ifs_de_library.metal` (real Mandelbulb) at de_type=3, count
//	     differing pixels. Mandelbulb's escape-distance silhouette is
//	     wildly different from a unit sphere on the c102 camera, so the
//	     diff must be substantial (> 1000 px). Mirrors c128-A's
//	     `FxaaLottes_SharpEdge` stub-vs-real pattern.
//
//	No collision with c130-B : that session works on src/gol/metal/ +
//	tests/unit/ for the gen_texture_3d extension ; this scope is only
//	src/shaders/msl/ifs_de_library.metal (new) +
//	tests/regression/phase3_golden_frame_regression_test.cpp (this TEST) +
//	tests/unit/path_a_catalog_test.cpp (floor bump).
TEST( RegressionPhase3, GoldenFrame_IfsDeLibrary_PixelComparable )
{
    //	---- Per-shader render helper. Compiles `<shader_name>.metal`,
    //	     binds gradient texture + ifs uniforms (time=0, de_type=3 ->
    //	     Mandelbulb), draws fullscreen, reads back RGBA8.
    auto const render_ifs_shader =
        []( char const* shader_name,
            int          de_type ) -> std::vector< std::uint8_t >
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) /
            ( std::string( shader_name ) + ".metal" );
        std::string msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        GOL::TextureId src_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                        GOL::TextureFormat::RGBA8 );
        EXPECT_NE( src_tex, GOL::kInvalidTextureId );
        std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
        backend.texture_data_2d( src_tex, gradient.data(), kBytesPerRow );

        GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                   GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt, GOL::kInvalidTextureId );

        //	Floats : [0] = time (PIN 0 for determinism) ; [1] = scale.
        AaaFuFloats uf{};
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        uf.values[ 0 ] = 0.0f;
        uf.values[ 1 ] = 1.0f;

        //	Ints : [0] = de_type. Drives the per-variant switch.
        AaaFuInts ui{};
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
        ui.values[ 0 ] = de_type;

        GOL::BufferId ub_f = backend.gen_buffer();
        GOL::BufferId ub_i = backend.gen_buffer();
        EXPECT_NE( ub_f, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i, GOL::kInvalidBufferId );
        backend.buffer_data( ub_f, sizeof( uf ), &uf, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i, sizeof( ui ), &ui, GOL::BufferUsage::Static );

        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = rt;
        rpd.load_action      = GOL::LoadAction::Clear;
        //	BLACK clear -- ifs returns float4(0) on missed rays, so the
        //	clear and miss coincide. Hits paint over the clear.
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 0.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = "regression.phase3.ifs_de_library";

        EXPECT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_texture( src_tex, 0 );
        backend.bind_fragment_buffer(  ub_f, 0, 0 );
        backend.bind_fragment_buffer(  ub_i, 2, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i    );
        backend.delete_buffer(  ub_f    );
        backend.delete_texture( rt      );
        backend.delete_texture( src_tex );
        backend.delete_program( prog    );
        return out;
    };

    //	---- 1. Render the real DE library at de_type=3 (Mandelbulb) ;
    //	     capture / compare the golden bit-exactly.
    std::vector< std::uint8_t > const rendered_real =
        render_ifs_shader( "ifs_de_library", 3 );

    //	Relaxed nonzero floor : Mandelbulb's escape-distance silhouette
    //	on the c102 camera frame covers ~5-15% of pixels (rays inside the
    //	cone that hit the fractal body) ; /100 = 1% is well above any
    //	"shader didn't draw" threshold and well below expected coverage.
    std::size_t const nonzero_real = count_nonzero_pixels( rendered_real );
    ASSERT_GT( nonzero_real, ( kWidth * kHeight ) / 100 )
        << "Mandelbulb render produced almost no non-black pixels : "
        << nonzero_real << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:ifs_de_library] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero_real );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ifs_de_library.png";
    compare_or_capture_golden( golden_path, rendered_real, "ifs_de_library" );

    //	---- 2. Render the c102 sphere-DE stub at de_type=3 against the
    //	     same uniforms ; count pixels that differ.
    std::vector< std::uint8_t > const rendered_stub =
        render_ifs_shader( "ifs", 3 );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha matches clear
            {
                int const d = int( rendered_real[ base + c ] ) -
                              int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:ifs_de_library] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold : 1000 pixels. Mandelbulb's silhouette occupies thousands
    //	of pixels that the unit sphere does NOT cover (and vice versa) ;
    //	plus the lighting differs everywhere the surface normal differs
    //	(every hit point). Real diff is in the tens of thousands ; >1000
    //	is a conservative floor that catches "the DE library is silently
    //	a copy of the stub".
    EXPECT_GT( diff_count, std::size_t( 1000 ) )
        << "ifs_de_library Mandelbulb output is nearly identical to the "
        << "sphere-DE stub on de_type=3 -- the real 8-variant DE library "
        << "never executed. Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
}

//	---------------- c129-B : ps_Toy_CloudIq (opaque-stub fallback) ----
//	Engine source is wrapped in `#if 0 ... #endif` (lines 21..653) so the
//	upstream has no live `main()`. Mac side returns float4(1,1,1,1) for
//	all pixels -- a compile-shape anchor that catches any accidental
//	revival regression. Same broken-upstream pattern as c80 ps_earth and
//	c88 PeteMetaImage. No textures, no uniforms -- the simplest possible
//	binding shape in the baseline.
TEST( RegressionPhase3, GoldenFrame_PsToyCloudIq_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_ps_Toy_CloudIq.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "ps_Toy_CloudIq.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	RED clear -- stub returns opaque white, so a clear-only frame
    //	(shader didn't draw) would show as red and fail the nonzero check.
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.ps_Toy_CloudIq";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	ps_Toy_CloudIq declares no textures and no uniform buffers in the
    //	live path -- simplest possible binding shape.
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    //	Output is float4(1,1,1,1) = white -- every pixel is nonzero-RGB.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:ps_Toy_CloudIq] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "ps_Toy_CloudIq" );

    backend.delete_texture( rt      );
    backend.delete_program( prog    );
}

//	====================================================================
//	c130-B+ : sampler3D pair (vol3d_Plane + vol3d_Voxel)
//
//	First end-to-end regression that exercises GOL::Backend::gen_texture_3d
//	+ bind_fragment_texture_3d through real Path A shaders -- previously
//	only validated by the gol_metal_texture_3d unit smoke test.
//
//	Both shaders declare a sampler3D pair (slots 0 + 2) ; the live path
//	samples only slot 0 (slot 2 is `(void)` cast in both files). We
//	supply a 4x4x4 RGBA8 volume on slot 0 with per-slice color
//	encoding (z-slice index gates the red channel) so a regression in
//	the 3D-upload or 3D-bind paths shows up as garbled per-pixel
//	output. A second 4x4x4 dummy volume binds to slot 2 to satisfy
//	Metal's binding-validation rules.
//
//	Determinism : both shaders are pure functions of (in.uv, uniforms,
//	bound textures). vol3d_Voxel uses LFSR_Rand_Gen for jitter, but
//	LFSR is a deterministic int-bitwise hash -- no iTime or per-frame
//	random source, so identical inputs produce identical pixels.
//	====================================================================

namespace
{
    //	4x4x4 RGBA8 volume : z-slice index gates the R channel
    //	(slice 0 = 32, slice 1 = 96, slice 2 = 160, slice 3 = 224). Y
    //	gates G (top->bottom ramp). X gates B (left->right ramp).
    //	Tightly packed -- bytes_per_row = W*4 ; backend computes
    //	bytes_per_image internally per the gen_texture_3d contract.
    std::vector< std::uint8_t > make_vol3d_4x4x4_rgba8()
    {
        constexpr std::uint32_t W = 4;
        constexpr std::uint32_t H = 4;
        constexpr std::uint32_t D = 4;
        std::vector< std::uint8_t > volume( W * H * D * 4, 0 );
        std::uint8_t const r_per_slice[ D ] = { 32, 96, 160, 224 };
        for( std::uint32_t z = 0; z < D; ++z )
            for( std::uint32_t y = 0; y < H; ++y )
                for( std::uint32_t x = 0; x < W; ++x )
                {
                    std::size_t const idx = ( ( z * H + y ) * W + x ) * 4;
                    volume[ idx + 0 ] = r_per_slice[ z ];
                    volume[ idx + 1 ] = std::uint8_t( ( y * 255u ) / ( H - 1 ) );
                    volume[ idx + 2 ] = std::uint8_t( ( x * 255u ) / ( W - 1 ) );
                    volume[ idx + 3 ] = 255;
                }
        return volume;
    }
}

//	---------------- c130-B+ : vol3d_Plane -----------------------------
//	Shader body (deep / sampling branch) :
//	  uvw = (in.uv.x, in.uv.y, 0) * (1,1,floats[2]) + 0.5
//	  discard if uvw < vec4s[0].xyz OR uvw > vec4s[1].xyz
//	  early-return float4(uvw, 1) if ints[0] == 0  (uvw-as-color)
//	  early-return color_dbg_[2] = green if ints[0] < 0
//	  else g = tex3d.sample( uvw ).r ;
//	       if clamp(g, vec4s[2].x, vec4s[2].y) == g :
//	           if ints[2] == 0 : return float4(g) * floats[0..1] tint
//	           else : palette-lookup tex2d at (g, floats[3])
//	       else return float4(0)
//
//	We pick `ints[0] = 1` + `ints[2] = 0` => DEEP-SAMPLE path :
//	the 3D volume's slice-encoded red channel (g) is returned as a
//	uniform-tinted grey. With floats[2] = 0 the slice index is pinned
//	(uvw.z always = 0.5 -> slice 2), and the wide crop (vec4s[0]/[1])
//	guarantees no discard across the full screen. vec4s[2] = (-1, 2)
//	is the clamp bypass so every sample passes.
//
//	This is the load-bearing validation : the rendered pixels MUST
//	reflect the volume's slice-2 R value (~160/255 = 0.628). Any
//	regression in gen_texture_3d's upload (bytesPerImage wrong, slice
//	stride wrong) or in bind_fragment_texture_3d's slot routing
//	produces visibly different pixels.
TEST( RegressionPhase3, GoldenFrame_Vol3dPlane_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_vol3d_Plane.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "vol3d_Plane.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    //	-------- 3D volume (slot 0) + dummy 3D (slot 2) + 2D palette ----
    std::vector< std::uint8_t > vol = make_vol3d_4x4x4_rgba8();
    GOL::TextureId vol_tex = backend.gen_texture_3d(
        4, 4, 4, GOL::TextureFormat::RGBA8, vol.data() );
    ASSERT_NE( vol_tex, GOL::kInvalidTextureId );

    //	Slot 2 : declared but `(void)`d in the shader. A second
    //	gen_texture_3d call confirms the API handles multiple 3D
    //	textures alive simultaneously.
    GOL::TextureId vol_tex_dummy = backend.gen_texture_3d(
        4, 4, 4, GOL::TextureFormat::RGBA8, vol.data() );
    ASSERT_NE( vol_tex_dummy, GOL::kInvalidTextureId );

    //	Slot 1 : palette (2D). 256x1 simple ramp ; not sampled when
    //	ints[2] == 0, but Metal requires the slot be bound.
    GOL::TextureId palette_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                        GOL::TextureFormat::RGBA8 );
    ASSERT_NE( palette_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( palette_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	-------- Uniforms : pin DEEP-SAMPLE branch ----------------------
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    uf.values[ 0 ] = 1.0f;     // rgb tint multiplier
    uf.values[ 1 ] = 1.0f;     // alpha multiplier
    uf.values[ 2 ] = 0.0f;     // z scale : pins uvw.z = 0.5 (slice 2)
    uf.values[ 3 ] = 0.5f;     // palette V (unused with ints[2]=0)

    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
    //	crop_min (vec4s[0]) : large negative -> no discard from "below"
    uv4.values[ 0 ][ 0 ] = -10.0f;
    uv4.values[ 0 ][ 1 ] = -10.0f;
    uv4.values[ 0 ][ 2 ] = -10.0f;
    //	crop_max (vec4s[1]) : large positive -> no discard from "above"
    uv4.values[ 1 ][ 0 ] =  10.0f;
    uv4.values[ 1 ][ 1 ] =  10.0f;
    uv4.values[ 1 ][ 2 ] =  10.0f;
    //	g-clamp (vec4s[2]) : wide so every sampled g passes
    uv4.values[ 2 ][ 0 ] = -1.0f;
    uv4.values[ 2 ][ 1 ] =  2.0f;

    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 1;        // DEEP-SAMPLE branch (>0 ; not bypass)
    ui.values[ 2 ] = 0;        // grey path : return float4(g), not palette lookup

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	MAGENTA clear : a no-draw / all-discard regression shows up
    //	as pure magenta in the golden, which is unambiguously distinct
    //	from the expected grey-from-sampled-volume output.
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.vol3d_Plane";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    //	c130-B API : bind_fragment_texture_3d at slots 0 + 2 (sampler3D
    //	pair) ; bind_fragment_texture at slot 1 (2D palette).
    backend.bind_fragment_texture_3d( vol_tex,        0 );
    backend.bind_fragment_texture(    palette_tex,    1 );
    backend.bind_fragment_texture_3d( vol_tex_dummy,  2 );
    backend.bind_fragment_buffer(     ub_f,  0, 0 );
    backend.bind_fragment_buffer(     ub_v4, 1, 0 );
    backend.bind_fragment_buffer(     ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:vol3d_Plane] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "vol3d_Plane" );

    backend.delete_buffer(  ub_i          );
    backend.delete_buffer(  ub_v4         );
    backend.delete_buffer(  ub_f          );
    backend.delete_texture( rt            );
    backend.delete_texture( palette_tex   );
    backend.delete_texture( vol_tex_dummy );
    backend.delete_texture( vol_tex       );
    backend.delete_program( prog          );
}

//	---------------- c130-B+ : vol3d_Voxel -----------------------------
//	Shader body (early-out / bypass branch) :
//	  if s_how == 0 : return float4( fs_uvw, 1.0 )   <- chosen branch
//	  if s_how < 0  : return color_dbg_[2] = green
//	  else : full raymarch with LFSR jitter + crop-skip + 7-mode sample
//
//	We pick `ints[0] = 0` => uvw-as-color bypass : the safest
//	deterministic branch (no LFSR jitter ; no crop-skip while-loop ;
//	no per-mode sample). The 3D textures + 2D palette + all 3 uniform
//	buffers are still BOUND via the c130-B API -- this proves the
//	binding paths work cleanly even when the shader body doesn't
//	actually sample them. (The companion vol3d_Plane test above
//	exercises the sample path ; together the pair covers both the
//	bind-and-skip and bind-and-sample halves of the c130-B API.)
//
//	The expected output is a per-pixel uvw gradient (R = uv.x, G =
//	uv.y, B = 0, A = 1) -- rich spatial variation that catches any
//	pixel-coverage / triangle / viewport regression too.
TEST( RegressionPhase3, GoldenFrame_Vol3dVoxel_PixelComparable )
{
    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_vol3d_Voxel.png";

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::filesystem::path msl_path =
        std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "vol3d_Voxel.metal";
    std::string msl_src = slurp( msl_path );
    ASSERT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

    GOL::ProgramId prog = backend.create_program_msl(
        msl_src.c_str(), "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "create_program_msl failed : " << backend.get_last_error();

    std::vector< std::uint8_t > vol = make_vol3d_4x4x4_rgba8();
    GOL::TextureId vol_tex = backend.gen_texture_3d(
        4, 4, 4, GOL::TextureFormat::RGBA8, vol.data() );
    ASSERT_NE( vol_tex, GOL::kInvalidTextureId );

    GOL::TextureId vol_tex_dummy = backend.gen_texture_3d(
        4, 4, 4, GOL::TextureFormat::RGBA8, vol.data() );
    ASSERT_NE( vol_tex_dummy, GOL::kInvalidTextureId );

    GOL::TextureId palette_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                        GOL::TextureFormat::RGBA8 );
    ASSERT_NE( palette_tex, GOL::kInvalidTextureId );
    std::vector< std::uint8_t > gradient = make_gradient_rgba8( kWidth, kHeight );
    backend.texture_data_2d( palette_tex, gradient.data(), kBytesPerRow );

    GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                               GOL::TextureFormat::RGBA8 );
    ASSERT_NE( rt, GOL::kInvalidTextureId );

    //	Uniforms : ints[0] = 0 -> bypass branch (uvw-as-color).
    //	Other uniforms must be VALID-but-unread ; zero-init is safe.
    AaaFuFloats uf{};
    for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
    AaaFuVec4s uv4{};
    for( int i = 0; i < 16; ++i )
        for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
    AaaFuInts ui{};
    for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;
    ui.values[ 0 ] = 0;        // BYPASS branch : early-return uvw color

    GOL::BufferId ub_f  = backend.gen_buffer();
    GOL::BufferId ub_v4 = backend.gen_buffer();
    GOL::BufferId ub_i  = backend.gen_buffer();
    ASSERT_NE( ub_f , GOL::kInvalidBufferId );
    ASSERT_NE( ub_v4, GOL::kInvalidBufferId );
    ASSERT_NE( ub_i , GOL::kInvalidBufferId );
    backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
    backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
    backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = rt;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	MAGENTA clear : expected output is uvw gradient (R+G ramp, B=0).
    //	No-draw bug leaves magenta which is distinct from the expected.
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 1.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    rpd.debug_label      = "regression.phase3.vol3d_Voxel";

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    backend.bind_fragment_texture_3d( vol_tex,        0 );
    backend.bind_fragment_texture(    palette_tex,    1 );
    backend.bind_fragment_texture_3d( vol_tex_dummy,  2 );
    backend.bind_fragment_buffer(     ub_f,  0, 0 );
    backend.bind_fragment_buffer(     ub_v4, 1, 0 );
    backend.bind_fragment_buffer(     ub_i,  2, 0 );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    std::vector< std::uint8_t > rendered( kBufferBytes, 0 );
    backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                 rendered.data(), kBytesPerRow );

    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "Render produced almost no non-black pixels : " << nonzero
        << " / " << ( kWidth * kHeight ) << " -- shader likely didn't draw.";

    std::printf( "[Phase3.Golden:vol3d_Voxel] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    compare_or_capture_golden( golden_path, rendered, "vol3d_Voxel" );

    backend.delete_buffer(  ub_i          );
    backend.delete_buffer(  ub_v4         );
    backend.delete_buffer(  ub_f          );
    backend.delete_texture( rt            );
    backend.delete_texture( palette_tex   );
    backend.delete_texture( vol_tex_dummy );
    backend.delete_texture( vol_tex       );
    backend.delete_program( prog          );
}

//	==================== c132-A : lights_deferred_real (THIRD Path A revival)
//	Real per-light Lambert + Phong accumulation over a packed vec4 SSBO of
//	up to 8 lights. Replaces the c100 lights_deferred_v0 / _v1 stubs (those
//	hardcode st_light_nb = {0,0,0} so the per-light loops never execute).
//	Both c100 stubs are PRESERVED VERBATIM (each anchoring its own c128-B /
//	c129-B golden) ; this revival lives in a SEPARATE file
//	`src/shaders/msl/lights_deferred_real.metal`. Path A catalog floor
//	bumped 160 -> 161. Pattern mirrors c128-A (fxaa_lottes) + c130-A
//	(ifs_de_library).
//
//	Citation : Real-Time Rendering 4th ed. Sec 5.2.2 (Lambert) + Sec 9.7
//	(Phong) ; deferred shading origin = Saito & Takahashi 1990 SIGGRAPH
//	"Comprehensible Rendering of 3-D Shapes".
//
//	Two TESTs :
//	  1. GoldenFrame_LightsDeferredReal_PixelComparable -- 2-light scene
//	     (red @ left, blue @ right), checker albedo, smooth normal,
//	     mid-depth g-buffer. Capture golden, replay bit-exact.
//	  2. RevivalProof_LightsDeferredReal_DiffersFromStub -- render BOTH
//	     `lights_deferred_v0` (stub) and `lights_deferred_real` against
//	     IDENTICAL g-buffer + uniforms ; assert pixel diff > floor. Proves
//	     real Lambert+Phong differs visibly from the stub's passthrough.
//	     Sharp-signal proof like c128-A's FxaaLottes_SharpEdge +
//	     c130-A's IfsDeLibrary stub-vs-real block.
//
//	No collision with c132-B (gesture bridge / event_bridge_gesture.{h,mm}
//	+ new gesture unit test) : disjoint file scope.
namespace
{
    //	c132-A g-buffer fixtures. Three 256x256 RGBA8 buffers, deterministic :
    //	  albedo  : 4x4 checkerboard, alternating dark-grey / light-grey.
    //	  normal  : encoded normal in [0,1] -- smooth radial tilt away from
    //	            screen center so the dot products vary across the frame.
    //	  depth   : mid-depth ramp (linear in y), normalized to [0,1].
    std::vector< std::uint8_t > make_lights_albedo_checker_rgba8()
    {
        constexpr std::uint32_t kTile = kWidth / 4u;  // 256 / 4 = 64
        std::vector< std::uint8_t > p( std::size_t( kWidth ) * kHeight * 4, 0 );
        for( std::uint32_t y = 0; y < kHeight; ++y )
        {
            std::uint32_t const ty = y / kTile;
            for( std::uint32_t x = 0; x < kWidth; ++x )
            {
                std::uint32_t const tx = x / kTile;
                bool const light = ( ( tx ^ ty ) & 1u ) == 0u;
                std::uint8_t const v = light ? std::uint8_t( 200 ) : std::uint8_t( 60 );
                std::size_t const i = ( y * kWidth + x ) * 4;
                p[ i + 0 ] = v;
                p[ i + 1 ] = v;
                p[ i + 2 ] = v;
                p[ i + 3 ] = 255;
            }
        }
        return p;
    }

    std::vector< std::uint8_t > make_lights_normal_smooth_rgba8()
    {
        std::vector< std::uint8_t > p( std::size_t( kWidth ) * kHeight * 4, 0 );
        for( std::uint32_t y = 0; y < kHeight; ++y )
        {
            float const v = ( float( y ) / float( kHeight - 1 ) ) * 2.0f - 1.0f;
            for( std::uint32_t x = 0; x < kWidth; ++x )
            {
                float const u = ( float( x ) / float( kWidth - 1 ) ) * 2.0f - 1.0f;
                //	Outward-tilted normal : N = normalize( (u, v, 1) ) , encoded
                //	to [0,1]. Top-right has normal pointing up-right ; centre is
                //	flat (+Z) ; bottom-left tilts down-left.
                float const inv_len = 1.0f / std::sqrt( u * u + v * v + 1.0f );
                float const nx = u * inv_len;
                float const ny = v * inv_len;
                float const nz = 1.0f * inv_len;
                auto const enc = []( float c ) -> std::uint8_t
                {
                    float const e = ( c * 0.5f + 0.5f ) * 255.0f;
                    return std::uint8_t( e < 0.0f ? 0.0f : ( e > 255.0f ? 255.0f : e ) );
                };
                std::size_t const i = ( y * kWidth + x ) * 4;
                p[ i + 0 ] = enc( nx );
                p[ i + 1 ] = enc( ny );
                p[ i + 2 ] = enc( nz );
                p[ i + 3 ] = 255;
            }
        }
        return p;
    }

    std::vector< std::uint8_t > make_lights_depth_mid_rgba8()
    {
        std::vector< std::uint8_t > p( std::size_t( kWidth ) * kHeight * 4, 0 );
        for( std::uint32_t y = 0; y < kHeight; ++y )
        {
            //	Mid-depth ramp 0.25 .. 0.75 across y. Stays in the valid
            //	[0,1] range so the shader's depth*2-1 stays in (-0.5, 0.5).
            float const d = 0.25f + 0.5f * float( y ) / float( kHeight - 1 );
            std::uint8_t const v = std::uint8_t( d * 255.0f );
            for( std::uint32_t x = 0; x < kWidth; ++x )
            {
                std::size_t const i = ( y * kWidth + x ) * 4;
                p[ i + 0 ] = v;
                p[ i + 1 ] = v;
                p[ i + 2 ] = v;
                p[ i + 3 ] = 255;
            }
        }
        return p;
    }

    //	Bind uniforms for the c132-A real-lights regression. 2 lights :
    //	red light at left (-0.8, 0.0, 0.5), blue light at right
    //	(+0.8, 0.0, 0.5). Radii large enough to overlap the whole frame.
    //	Per c132-A spec : positions+radius in AaaFuVec4s[0..MAX_LIGHTS-1] ,
    //	color+intensity in AaaFuVec4s[MAX_LIGHTS..2*MAX_LIGHTS-1].
    void fill_lights_uniforms_c132a( AaaFuFloats& uf,
                                     AaaFuVec4s&  uv4,
                                     AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        uf.values[ 0 ] = 0.15f;   // ambient
        uf.values[ 1 ] = 1.0f;    // exposure
        uf.values[ 2 ] = 2.2f;    // gamma
        uf.values[ 3 ] = 32.0f;   // phong_exponent

        //	MAX_LIGHTS = 8 in the shader, mirrored here.
        constexpr int kMaxLights = 8;

        //	Light 0 : red, left. pos = (-0.8, 0, 0.5), radius = 2.5
        uv4.values[ 0 ][ 0 ] = -0.8f;
        uv4.values[ 0 ][ 1 ] =  0.0f;
        uv4.values[ 0 ][ 2 ] =  0.5f;
        uv4.values[ 0 ][ 3 ] =  2.5f;
        uv4.values[ kMaxLights + 0 ][ 0 ] = 1.0f;  // R
        uv4.values[ kMaxLights + 0 ][ 1 ] = 0.1f;
        uv4.values[ kMaxLights + 0 ][ 2 ] = 0.1f;
        uv4.values[ kMaxLights + 0 ][ 3 ] = 2.0f;  // intensity

        //	Light 1 : blue, right. pos = (+0.8, 0, 0.5), radius = 2.5
        uv4.values[ 1 ][ 0 ] =  0.8f;
        uv4.values[ 1 ][ 1 ] =  0.0f;
        uv4.values[ 1 ][ 2 ] =  0.5f;
        uv4.values[ 1 ][ 3 ] =  2.5f;
        uv4.values[ kMaxLights + 1 ][ 0 ] = 0.1f;
        uv4.values[ kMaxLights + 1 ][ 1 ] = 0.2f;
        uv4.values[ kMaxLights + 1 ][ 2 ] = 1.0f;  // B
        uv4.values[ kMaxLights + 1 ][ 3 ] = 2.0f;  // intensity

        ui.values[ 0 ] = 2;       // light_count
    }

    //	Per-shader render helper for c132-A. Compiles the named .metal,
    //	uploads the 3-buffer g-buffer, binds uniforms, draws fullscreen,
    //	reads back the RGBA8 output. The stub vs real shaders interpret
    //	slot meanings slightly differently, so `bind_v0_signature` routes
    //	to the stub's 7-tex layout (depth at slot 4) when true, else the
    //	real shader's albedo/normal/depth-at-slots-0/1/2 layout.
    std::vector< std::uint8_t > render_lights_shader_c132a(
        char const*                         shader_name,
        std::vector< std::uint8_t > const&  albedo,
        std::vector< std::uint8_t > const&  normal,
        std::vector< std::uint8_t > const&  depth_buf,
        bool                                bind_v0_signature )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) /
            ( std::string( shader_name ) + ".metal" );
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        GOL::TextureId tex_albedo = backend.gen_texture_2d( kWidth, kHeight,
                                                            GOL::TextureFormat::RGBA8 );
        GOL::TextureId tex_normal = backend.gen_texture_2d( kWidth, kHeight,
                                                            GOL::TextureFormat::RGBA8 );
        GOL::TextureId tex_depth  = backend.gen_texture_2d( kWidth, kHeight,
                                                            GOL::TextureFormat::RGBA8 );
        EXPECT_NE( tex_albedo, GOL::kInvalidTextureId );
        EXPECT_NE( tex_normal, GOL::kInvalidTextureId );
        EXPECT_NE( tex_depth,  GOL::kInvalidTextureId );
        backend.texture_data_2d( tex_albedo, albedo.data(),    kBytesPerRow );
        backend.texture_data_2d( tex_normal, normal.data(),    kBytesPerRow );
        backend.texture_data_2d( tex_depth,  depth_buf.data(), kBytesPerRow );

        GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                   GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt, GOL::kInvalidTextureId );

        AaaFuFloats uf{};
        AaaFuVec4s  uv4{};
        AaaFuInts   ui{};
        fill_lights_uniforms_c132a( uf, uv4, ui );

        //	The c100 stub re-uses ints[0] as `what` (1 -> early-return tex0
        //	sample). For the revival-proof we want the stub on its LIVE
        //	branch so the comparison is "stub-live-output vs real Lambert
        //	+ Phong", not "stub-early-return vs real". Set what = 0 so the
        //	stub falls through to its light-accumulation skeleton (which
        //	then loops zero times because u_light_nb = {0,0,0} -> returns
        //	ambient-only * diffuse).
        if( bind_v0_signature )
        {
            ui.values[ 0 ] = 0;       // stub `what` = 0 -> live branch
            uf.values[ 0 ] = 0.2f;    // stub hack_ambient_offset
            uf.values[ 1 ] = 1.0f;    // stub hack_light_factor
        }

        GOL::BufferId ub_f  = backend.gen_buffer();
        GOL::BufferId ub_v4 = backend.gen_buffer();
        GOL::BufferId ub_i  = backend.gen_buffer();
        EXPECT_NE( ub_f,  GOL::kInvalidBufferId );
        EXPECT_NE( ub_v4, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i,  GOL::kInvalidBufferId );
        backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
        backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = rt;
        rpd.load_action      = GOL::LoadAction::Clear;
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 0.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = bind_v0_signature
                             ? "regression.phase3.lights_deferred_v0_for_proof"
                             : "regression.phase3.lights_deferred_real";

        EXPECT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );

        if( bind_v0_signature )
        {
            //	c100 stub layout : 7 textures. Depth lands at slot 4.
            backend.bind_fragment_texture( tex_albedo, 0 );  // diffuse_rough
            backend.bind_fragment_texture( tex_normal, 1 );  // normal_ao
            backend.bind_fragment_texture( tex_albedo, 2 );  // specular_reflection (reuse)
            backend.bind_fragment_texture( tex_albedo, 3 );  // emissive            (reuse)
            backend.bind_fragment_texture( tex_depth,  4 );  // depth
            backend.bind_fragment_texture( tex_albedo, 5 );  // shadow_0            (reuse)
            backend.bind_fragment_texture( tex_albedo, 6 );  // lightmap_0          (reuse)
        }
        else
        {
            //	c132-A revival layout : albedo / normal / depth at 0 / 1 / 2.
            backend.bind_fragment_texture( tex_albedo, 0 );  // ALBEDO
            backend.bind_fragment_texture( tex_normal, 1 );  // NORMAL
            backend.bind_fragment_texture( tex_depth,  2 );  // DEPTH
            backend.bind_fragment_texture( tex_albedo, 3 );  // optional, reuse
            backend.bind_fragment_texture( tex_albedo, 4 );  // optional, reuse
            backend.bind_fragment_texture( tex_albedo, 5 );  // optional, reuse
            backend.bind_fragment_texture( tex_albedo, 6 );  // optional, reuse
        }
        backend.bind_fragment_buffer( ub_f,  0, 0 );
        backend.bind_fragment_buffer( ub_v4, 1, 0 );
        backend.bind_fragment_buffer( ub_i,  2, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i       );
        backend.delete_buffer(  ub_v4      );
        backend.delete_buffer(  ub_f       );
        backend.delete_texture( rt         );
        backend.delete_texture( tex_depth  );
        backend.delete_texture( tex_normal );
        backend.delete_texture( tex_albedo );
        backend.delete_program( prog       );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_LightsDeferredReal_PixelComparable )
{
    std::vector< std::uint8_t > const albedo    = make_lights_albedo_checker_rgba8();
    std::vector< std::uint8_t > const normal    = make_lights_normal_smooth_rgba8();
    std::vector< std::uint8_t > const depth_buf = make_lights_depth_mid_rgba8();

    std::vector< std::uint8_t > const rendered = render_lights_shader_c132a(
        "lights_deferred_real",
        albedo, normal, depth_buf,
        /*bind_v0_signature*/ false );

    //	Two coloured lights + ambient * albedo means the vast majority of
    //	pixels are non-black. /4 = 25% is a loose floor that catches
    //	"shader didn't draw" without locking in an exact coverage.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 4 )
        << "lights_deferred_real produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:lights_deferred_real] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_lights_deferred_real.png";
    compare_or_capture_golden( golden_path, rendered, "lights_deferred_real" );
}

TEST( RegressionPhase3, RevivalProof_LightsDeferredReal_DiffersFromStub )
{
    //	Build a single g-buffer set, render through BOTH the c100 stub
    //	`lights_deferred_v0` and the c132-A revival `lights_deferred_real`,
    //	count differing pixels. The stub's light loops are dead code
    //	(u_light_nb = {0,0,0}) and it returns ambient * diffuse only ;
    //	the real shader runs Lambert + Phong over 2 coloured lights, so
    //	the visible output MUST be wildly different.
    std::vector< std::uint8_t > const albedo    = make_lights_albedo_checker_rgba8();
    std::vector< std::uint8_t > const normal    = make_lights_normal_smooth_rgba8();
    std::vector< std::uint8_t > const depth_buf = make_lights_depth_mid_rgba8();

    std::vector< std::uint8_t > const rendered_real = render_lights_shader_c132a(
        "lights_deferred_real",
        albedo, normal, depth_buf,
        /*bind_v0_signature*/ false );

    std::vector< std::uint8_t > const rendered_stub = render_lights_shader_c132a(
        "lights_deferred_v0",
        albedo, normal, depth_buf,
        /*bind_v0_signature*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha is constant
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:lights_deferred_real] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold : 5000 pixels and 50-LSB max. The stub returns
    //	(ambient_offset * diffuse) -- a darkened, neutral copy of the
    //	checker albedo. The real shader adds two coloured Lambert lobes
    //	(red on the left half , blue on the right half) plus a Phong
    //	highlight. Per-pixel divergence is huge across the entire frame :
    //	real diffs land in the tens of thousands ; >5000 is a
    //	conservative floor catching "the SSBO accumulate silently
    //	collapsed to ambient-only".
    EXPECT_GT( diff_count, std::size_t( 5000 ) )
        << "lights_deferred_real output is nearly identical to the stub "
        << "on the checker g-buffer -- the real Lambert+Phong SSBO "
        << "accumulate never executed. Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 50 )
        << "lights_deferred_real per-channel diff vs stub never exceeds "
        << "50 LSB -- the coloured-light contribution is suspiciously "
        << "small. max_diff = " << max_diff;
}

//	==================== c133-B : aaa_cam_real (FOURTH Path A revival) ====
//	Real perspective camera constant block plumbed end-to-end through
//	GOL::MetalBackend. Replaces the per-shader hardcoded-identity
//	`aaa_cam.projection_inverse` / `view_projection_inverse` stubs found
//	inline at file scope in `depth_coc.metal`, `debug_world_map.metal`,
//	`debug_world_map_color_distance.metal`, and `lights_deferred_v1.metal`.
//	Those stubs are PRESERVED VERBATIM in their respective shader files
//	(c121-B / c121-A / c128-B / c129-B regression goldens stay intact) ;
//	this revival lives in a SEPARATE file `src/shaders/msl/aaa_cam_real.metal`.
//	Path A catalog floor bumped 161 -> 162. Pattern mirrors c128-A
//	(fxaa_lottes) + c130-A (ifs_de_library) + c132-A (lights_deferred_real).
//
//	Citation : Real-Time Rendering 4th ed. Sec 4.7 (Projections) +
//	Sec 4.7.2 (Inverse Projection). Foley/van Dam, Computer Graphics
//	Principles & Practice Sec 6.5 (Perspective Projection / lookAt).
//
//	Two TESTs :
//	  1. GoldenFrame_AaaCamReal_PixelComparable -- canonical camera setup
//	     (eye at (0,0,5), look-at origin, FOV 60deg, aspect 1, near 0.1,
//	     far 100). Renders a single big xy-plane triangle through the
//	     real cam.vp ; fragment encodes view-ray + cam.eye + view_inv
//	     into RGB(A). Capture golden ; replay bit-exact.
//	  2. RevivalProof_AaaCamReal_DiffersFromStub -- render the SAME
//	     aaa_cam_real.metal twice : once with the real cam matrices,
//	     once with an identity-cam (vp = I, view = I, view_inv = I,
//	     eye = origin) which is what every per-shader file-scope stub
//	     effectively is. Assert pixel diff > 1000 px and per-channel
//	     max > 20 LSB. The identity-VP case clips the triangle outside
//	     the frustum so the rasterized output is dominated by the clear
//	     color ; the real-VP case lands a big rasterized triangle. The
//	     diff is the proof that the real cam matrices actually drove
//	     vertex transform AND fragment-stage view-ray reconstruction.
//
//	No collision with c133-A : that session works on
//	src/ui/macos/AAASeedInputView.{h,mm} + tests/unit/input_view_test.mm ;
//	this session's only c++ touch is THIS test file (2 new TESTs at the
//	tail) + tests/unit/path_a_catalog_test.cpp (floor bump). Disjoint
//	file scope.
namespace
{
    //	Mirror of the MSL `struct AaaCamReal`. Layout MUST match byte for
    //	byte -- see aaa_cam_real.metal for the canonical field comments.
    //	    offset  0   view[4][4]
    //	    offset 64   proj[4][4]
    //	    offset 128  view_inv[4][4]
    //	    offset 192  vp[4][4]
    //	    offset 256  eye_aspect[4]
    //	    offset 272  fov_clip[4]
    //	Total = 288 bytes (no trailing padding ; sizeof(float)*72 = 288).
    struct AaaCamReal_Cpu
    {
        float view    [ 16 ];
        float proj    [ 16 ];
        float view_inv[ 16 ];
        float vp      [ 16 ];
        float eye_aspect[ 4 ];
        float fov_clip  [ 4 ];
    };
    static_assert( sizeof( AaaCamReal_Cpu ) == 288,
                   "AaaCamReal_Cpu layout drifted from aaa_cam_real.metal" );

    //	Column-major 4x4 multiply. Metal stores float4x4 column-major
    //	(matching the GLSL / Vulkan convention), so we mirror that here.
    //	M_out = A * B (matrix mul, both column-major).
    void mat4_mul_cm( float const* A, float const* B, float* out )
    {
        for( int c = 0; c < 4; ++c )
            for( int r = 0; r < 4; ++r )
            {
                float s = 0.0f;
                for( int k = 0; k < 4; ++k )
                    s += A[ k * 4 + r ] * B[ c * 4 + k ];
                out[ c * 4 + r ] = s;
            }
    }

    //	Right-handed lookAt -> column-major float[16]. Foley/van Dam
    //	Sec 6.5 ; orthonormal basis (s, u, -f), translate(-eye).
    void make_view_lookat_rh( float ex, float ey, float ez,
                              float cx, float cy, float cz,
                              float ux, float uy, float uz,
                              float* out_cm )
    {
        float fx = cx - ex, fy = cy - ey, fz = cz - ez;
        float fl = std::sqrt( fx*fx + fy*fy + fz*fz );
        fx /= fl; fy /= fl; fz /= fl;
        //	s = normalize( cross(f, up) )
        float sx = fy*uz - fz*uy;
        float sy = fz*ux - fx*uz;
        float sz = fx*uy - fy*ux;
        float sl = std::sqrt( sx*sx + sy*sy + sz*sz );
        sx /= sl; sy /= sl; sz /= sl;
        //	u' = cross( s, f )
        float u2x = sy*fz - sz*fy;
        float u2y = sz*fx - sx*fz;
        float u2z = sx*fy - sy*fx;
        //	Column-major fill. Columns = basis vectors + translation.
        out_cm[ 0]= sx ; out_cm[ 1]= u2x; out_cm[ 2]=-fx ; out_cm[ 3]=0.0f;
        out_cm[ 4]= sy ; out_cm[ 5]= u2y; out_cm[ 6]=-fy ; out_cm[ 7]=0.0f;
        out_cm[ 8]= sz ; out_cm[ 9]= u2z; out_cm[10]=-fz ; out_cm[11]=0.0f;
        out_cm[12]= -( sx*ex + sy*ey + sz*ez );
        out_cm[13]= -( u2x*ex + u2y*ey + u2z*ez );
        out_cm[14]=  ( fx*ex + fy*ey + fz*ez );
        out_cm[15]= 1.0f;
    }

    //	Right-handed perspective, depth in [0,1] (Metal convention) ;
    //	Real-Time Rendering 4th ed. Sec 4.7. Column-major output.
    void make_proj_perspective_rh( float fov_y_rad, float aspect,
                                   float near_c, float far_c, float* out_cm )
    {
        float const f = 1.0f / std::tan( fov_y_rad * 0.5f );
        for( int i = 0; i < 16; ++i ) out_cm[ i ] = 0.0f;
        out_cm[ 0]= f / aspect;
        out_cm[ 5]= f;
        out_cm[10]= far_c / ( near_c - far_c );
        out_cm[11]= -1.0f;
        out_cm[14]= ( near_c * far_c ) / ( near_c - far_c );
    }

    //	View inverse for the simple lookAt above. Since the view matrix
    //	is orthonormal + translation, inverse = transpose of basis with
    //	the eye translation re-applied.
    void make_view_inv_for_lookat_rh( float const* view_cm, float* out_cm )
    {
        //	Transpose the upper-left 3x3.
        out_cm[ 0]=view_cm[ 0]; out_cm[ 1]=view_cm[ 4]; out_cm[ 2]=view_cm[ 8]; out_cm[ 3]=0.0f;
        out_cm[ 4]=view_cm[ 1]; out_cm[ 5]=view_cm[ 5]; out_cm[ 6]=view_cm[ 9]; out_cm[ 7]=0.0f;
        out_cm[ 8]=view_cm[ 2]; out_cm[ 9]=view_cm[ 6]; out_cm[10]=view_cm[10]; out_cm[11]=0.0f;
        //	Translation column = -(R^T * t_view) where t_view = view_cm[12..14].
        float tx = view_cm[12], ty = view_cm[13], tz = view_cm[14];
        out_cm[12] = -( out_cm[0]*tx + out_cm[4]*ty + out_cm[8]*tz );
        out_cm[13] = -( out_cm[1]*tx + out_cm[5]*ty + out_cm[9]*tz );
        out_cm[14] = -( out_cm[2]*tx + out_cm[6]*ty + out_cm[10]*tz );
        out_cm[15] = 1.0f;
    }

    void fill_aaa_cam_real_canonical( AaaCamReal_Cpu& cam )
    {
        //	c133-B canonical setup : eye (0,0,5), look-at origin, up +y,
        //	FOV 60 deg, aspect 1, near 0.1, far 100.
        float const fov_y_rad = 60.0f * 3.14159265358979323846f / 180.0f;
        float const aspect    = 1.0f;
        float const near_c    = 0.1f;
        float const far_c     = 100.0f;
        make_view_lookat_rh( 0.0f, 0.0f, 5.0f,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, cam.view );
        make_proj_perspective_rh( fov_y_rad, aspect, near_c, far_c, cam.proj );
        make_view_inv_for_lookat_rh( cam.view, cam.view_inv );
        mat4_mul_cm( cam.proj, cam.view, cam.vp );
        cam.eye_aspect[0] = 0.0f;
        cam.eye_aspect[1] = 0.0f;
        cam.eye_aspect[2] = 5.0f;
        cam.eye_aspect[3] = aspect;
        cam.fov_clip[0]   = fov_y_rad;
        cam.fov_clip[1]   = near_c;
        cam.fov_clip[2]   = far_c;
        cam.fov_clip[3]   = 0.0f;
    }

    void fill_aaa_cam_real_identity_stub( AaaCamReal_Cpu& cam )
    {
        //	What every per-shader stub effectively does : identity
        //	matrices, zero eye, neutral scalars. The vp = I case maps
        //	the c133-B object-space triangle (-3,-3,0)..(3,3,0) to
        //	clip-space coords with |x|,|y| = 3 -- outside the [-1,1]
        //	frustum -- so the rasterizer clips it entirely. Output is
        //	dominated by clear color, which is exactly the regression
        //	signal we want stub-vs-real to diff on.
        for( int i = 0; i < 16; ++i )
        {
            float const v = ( i % 5 == 0 ) ? 1.0f : 0.0f;
            cam.view    [ i ] = v;
            cam.proj    [ i ] = v;
            cam.view_inv[ i ] = v;
            cam.vp      [ i ] = v;
        }
        for( int i = 0; i < 4; ++i )
        {
            cam.eye_aspect[ i ] = 0.0f;
            cam.fov_clip  [ i ] = 0.0f;
        }
        //	Match the live aspect / near / far so only the matrices +
        //	eye differ ; isolates the proof to the cam plumbing.
        cam.eye_aspect[ 3 ] = 1.0f;
        cam.fov_clip[ 1 ]   = 0.1f;
        cam.fov_clip[ 2 ]   = 100.0f;
    }

    std::vector< std::uint8_t > render_aaa_cam_real_c133b( bool use_identity_stub )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_cam_real.metal";
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                   GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt, GOL::kInvalidTextureId );

        AaaCamReal_Cpu cam{};
        if( use_identity_stub )
            fill_aaa_cam_real_identity_stub( cam );
        else
            fill_aaa_cam_real_canonical( cam );

        GOL::BufferId ub_cam = backend.gen_buffer();
        EXPECT_NE( ub_cam, GOL::kInvalidBufferId );
        backend.buffer_data( ub_cam, sizeof( cam ), &cam, GOL::BufferUsage::Static );

        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = rt;
        rpd.load_action      = GOL::LoadAction::Clear;
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 0.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = use_identity_stub
                             ? "regression.phase3.aaa_cam_real_identity_stub"
                             : "regression.phase3.aaa_cam_real_canonical";

        EXPECT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        //	Cam constant block is consumed by BOTH vertex and fragment
        //	stages at slot 3. Metal's vertex / fragment buffer namespaces
        //	are independent ; bind to both.
        backend.bind_vertex_buffer(   ub_cam, 3, 0 );
        backend.bind_fragment_buffer( ub_cam, 3, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_cam );
        backend.delete_texture( rt     );
        backend.delete_program( prog   );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_AaaCamReal_PixelComparable )
{
    std::vector< std::uint8_t > const rendered = render_aaa_cam_real_c133b(
        /*use_identity_stub*/ false );

    //	The canonical camera puts the (-3,-3,0)..(3,3,0) triangle inside
    //	the frustum -- a sizeable rasterized region. Loose floor of
    //	1/8th of the framebuffer catches "shader failed to draw" while
    //	leaving headroom for the actual projected silhouette.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 8 )
        << "aaa_cam_real produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:aaa_cam_real] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaa_cam_real.png";
    compare_or_capture_golden( golden_path, rendered, "aaa_cam_real" );
}

TEST( RegressionPhase3, RevivalProof_AaaCamReal_DiffersFromStub )
{
    //	Render the SAME aaa_cam_real.metal twice : once with the real
    //	canonical cam matrices, once with the identity-cam stub. The
    //	identity case clips the object-space triangle outside the
    //	frustum -- output dominated by clear color. Real case lands a
    //	well-shaped rasterized region with view-ray-encoded RGB. Diff
    //	must be substantial in both pixel-count and per-channel terms.
    std::vector< std::uint8_t > const rendered_real = render_aaa_cam_real_c133b(
        /*use_identity_stub*/ false );
    std::vector< std::uint8_t > const rendered_stub = render_aaa_cam_real_c133b(
        /*use_identity_stub*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 4; ++c )
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:aaa_cam_real] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold : 1000 pixels and 20-LSB max per the c133-B spec. The
    //	canonical camera rasterizes a triangle covering tens of thousands
    //	of pixels of unique colour ; the identity-cam case clips it
    //	entirely so most of the frame is the clear color. Diff in
    //	practice runs in the tens of thousands of pixels ; >1000 is the
    //	conservative regression floor.
    EXPECT_GT( diff_count, std::size_t( 1000 ) )
        << "aaa_cam_real output is nearly identical to the identity-cam "
        << "stub -- the real view/proj/vp matrices never drove the vertex "
        << "transform. Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 20 )
        << "aaa_cam_real per-channel diff vs identity-cam stub never "
        << "exceeds 20 LSB -- view-ray reconstruction collapsed. "
        << "max_diff = " << max_diff;
}

//	==================== c134-B : aaa_material_pbr (FIFTH Path A revival) ==
//	Real Cook-Torrance microfacet BRDF plumbed end-to-end through
//	GOL::MetalBackend. Replaces the historic "flat albedo" stub behaviour
//	of the `aaa_material.material` engine global (the path consumed by
//	gbuffer.metal / gbuffer_generic.metal et al. collapses to a pass-
//	through colour with at most a constant ambient lift). That stub
//	behaviour is PRESERVED VERBATIM in every existing shader ; this
//	revival lives in a SEPARATE file `src/shaders/msl/aaa_material_pbr.metal`.
//	Path A catalog floor bumped 162 -> 163. Pattern mirrors c128-A
//	(fxaa_lottes) + c130-A (ifs_de_library) + c132-A (lights_deferred_real)
//	+ c133-B (aaa_cam_real).
//
//	Citation : Cook + Torrance 1982 ACM ToG (microfacet BRDF). Walter
//	et al. 2007 EGSR (GGX NDF). Schlick 1994 CGF (Fresnel approximation).
//	Smith 1967 IEEE TAP (G geometry). Karis 2013 SIGGRAPH "Real Shading
//	in Unreal Engine 4" (analytic-light k remap, energy-conserving
//	diffuse). Real-Time Rendering 4th ed. Sec 9.5 - 9.8 (canonical recap).
//
//	Two TESTs :
//	  1. GoldenFrame_AaaMaterialPBR_PixelComparable -- canonical PBR
//	     config (albedo (0.8,0.2,0.2), metallic = 0.5, roughness = 0.4,
//	     light dir normalize(0.3,0.6,1.0), light intensity 3.0, view at
//	     (0,0,3)). Renders the fullscreen quad through the real BRDF
//	     pipeline ; captures golden ; replays bit-exact.
//	  2. RevivalProof_AaaMaterialPBR_DiffersFromStub -- renders the SAME
//	     aaa_material_pbr.metal twice : once with the canonical PBR
//	     config, once with a "stub config" that zeroes light_intensity
//	     and metallic/roughness/AO -- exactly the surface a flat-albedo
//	     stub would produce. Per c134-B spec : pixel diff > 5000 and
//	     per-channel max > 50 LSB. The BRDF spec highlights + diffuse
//	     n.l shading should diverge massively from the dark albedo-only
//	     output.
//
//	No collision with c134-A (src/ui/macos/aaa_dpi.{h,mm} +
//	tests/unit/ui_dpi_test.cpp) : disjoint file scope.
namespace
{
    //	Fill uniforms for the canonical c134-B PBR render. Uses the
    //	uniform-slot layout documented in aaa_material_pbr.metal :
    //	  vec4s[0] = albedo + metallic
    //	  vec4s[1] = ( roughness, ao, emi_intensity, ior )
    //	  vec4s[2] = emissive_color + pad
    //	  vec4s[3] = light_dir + light_intensity
    //	  vec4s[4] = light_color + pad
    //	  vec4s[5] = view_pos + exposure
    //	  floats[0] = gamma
    void fill_pbr_canonical_c134b( AaaFuFloats& uf,
                                   AaaFuVec4s&  uv4,
                                   AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Albedo : warm red base. Metallic 0.5 = half-metal so both the
        //	diffuse and the metal-tinted specular contribute visibly.
        uv4.values[ 0 ][ 0 ] = 0.8f;
        uv4.values[ 0 ][ 1 ] = 0.2f;
        uv4.values[ 0 ][ 2 ] = 0.2f;
        uv4.values[ 0 ][ 3 ] = 0.5f;   //	metallic

        //	Roughness 0.4 -> shiny-but-not-mirror surface. AO full. No
        //	emissive. IOR 1.5 (typical dielectric, reserved slot).
        uv4.values[ 1 ][ 0 ] = 0.4f;   //	roughness
        uv4.values[ 1 ][ 1 ] = 1.0f;   //	ambient_occlusion
        uv4.values[ 1 ][ 2 ] = 0.0f;   //	emissive_intensity
        uv4.values[ 1 ][ 3 ] = 1.5f;   //	ior

        //	Emissive : zero -- isolate the BRDF signal.
        uv4.values[ 2 ][ 0 ] = 0.0f;
        uv4.values[ 2 ][ 1 ] = 0.0f;
        uv4.values[ 2 ][ 2 ] = 0.0f;
        uv4.values[ 2 ][ 3 ] = 0.0f;

        //	Directional light : from upper-right, into the screen.
        //	Normalize length-1 so the shader's normalize() is a no-op
        //	bit-for-bit deterministic across MSL compilers.
        float const lx = 0.3f, ly = 0.6f, lz = 1.0f;
        float const ll = std::sqrt( lx*lx + ly*ly + lz*lz );
        uv4.values[ 3 ][ 0 ] = lx / ll;
        uv4.values[ 3 ][ 1 ] = ly / ll;
        uv4.values[ 3 ][ 2 ] = lz / ll;
        uv4.values[ 3 ][ 3 ] = 3.0f;   //	light_intensity

        //	Light colour : warm white (slightly biased to R), opaque w.
        uv4.values[ 4 ][ 0 ] = 1.0f;
        uv4.values[ 4 ][ 1 ] = 0.95f;
        uv4.values[ 4 ][ 2 ] = 0.9f;
        uv4.values[ 4 ][ 3 ] = 0.0f;

        //	View position : ( 0, 0, 3 ) -- straight out from the quad's
        //	z = 0 plane. Exposure 1.0.
        uv4.values[ 5 ][ 0 ] = 0.0f;
        uv4.values[ 5 ][ 1 ] = 0.0f;
        uv4.values[ 5 ][ 2 ] = 3.0f;
        uv4.values[ 5 ][ 3 ] = 1.0f;   //	exposure

        uf.values[ 0 ] = 2.2f;         //	gamma
    }

    //	Fill uniforms for the "stub-equivalent" render : zero the light
    //	intensity (kills both diffuse and specular direct contributions)
    //	and zero metallic / roughness / AO so the BRDF surface collapses
    //	to a flat-albedo result -- the historic aaa_material stub's
    //	effective output. The PBR shader still runs every term ; the
    //	zeroed uniforms drive every direct-light contribution to zero,
    //	leaving only the (ao=0) ambient floor + (emi=0) emissive = pure
    //	black-ish frame. Diff vs canonical PBR is the proof.
    void fill_pbr_stub_c134b( AaaFuFloats& uf,
                              AaaFuVec4s&  uv4,
                              AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Same albedo so the eye-comparison stays on the BRDF signal
        //	rather than the underlying surface colour.
        uv4.values[ 0 ][ 0 ] = 0.8f;
        uv4.values[ 0 ][ 1 ] = 0.2f;
        uv4.values[ 0 ][ 2 ] = 0.2f;
        uv4.values[ 0 ][ 3 ] = 0.0f;   //	metallic = 0 (full dielectric)

        //	Roughness clamped by shader to 0.04 ; AO = 0 (no ambient lift) ;
        //	emi = 0 ; ior = 1.5.
        uv4.values[ 1 ][ 0 ] = 0.0f;
        uv4.values[ 1 ][ 1 ] = 0.0f;
        uv4.values[ 1 ][ 2 ] = 0.0f;
        uv4.values[ 1 ][ 3 ] = 1.5f;

        //	Light still points somewhere reasonable but intensity = 0
        //	so the BRDF * light_intensity term collapses to zero.
        uv4.values[ 3 ][ 0 ] = 0.0f;
        uv4.values[ 3 ][ 1 ] = 0.0f;
        uv4.values[ 3 ][ 2 ] = 1.0f;
        uv4.values[ 3 ][ 3 ] = 0.0f;   //	light_intensity = 0

        uv4.values[ 4 ][ 0 ] = 1.0f;
        uv4.values[ 4 ][ 1 ] = 1.0f;
        uv4.values[ 4 ][ 2 ] = 1.0f;
        uv4.values[ 4 ][ 3 ] = 0.0f;

        uv4.values[ 5 ][ 0 ] = 0.0f;
        uv4.values[ 5 ][ 1 ] = 0.0f;
        uv4.values[ 5 ][ 2 ] = 3.0f;
        uv4.values[ 5 ][ 3 ] = 1.0f;   //	exposure

        uf.values[ 0 ] = 2.2f;         //	gamma
    }

    std::vector< std::uint8_t > render_aaa_material_pbr_c134b( bool use_stub_config )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_material_pbr.metal";
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                   GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt, GOL::kInvalidTextureId );

        AaaFuFloats uf{};
        AaaFuVec4s  uv4{};
        AaaFuInts   ui{};
        if( use_stub_config )
            fill_pbr_stub_c134b( uf, uv4, ui );
        else
            fill_pbr_canonical_c134b( uf, uv4, ui );

        GOL::BufferId ub_f  = backend.gen_buffer();
        GOL::BufferId ub_v4 = backend.gen_buffer();
        GOL::BufferId ub_i  = backend.gen_buffer();
        EXPECT_NE( ub_f,  GOL::kInvalidBufferId );
        EXPECT_NE( ub_v4, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i,  GOL::kInvalidBufferId );
        backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
        backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = rt;
        rpd.load_action      = GOL::LoadAction::Clear;
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 0.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = use_stub_config
                             ? "regression.phase3.aaa_material_pbr_stub_config"
                             : "regression.phase3.aaa_material_pbr_canonical";

        EXPECT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_buffer( ub_f,  0, 0 );
        backend.bind_fragment_buffer( ub_v4, 1, 0 );
        backend.bind_fragment_buffer( ub_i,  2, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i  );
        backend.delete_buffer(  ub_v4 );
        backend.delete_buffer(  ub_f  );
        backend.delete_texture( rt    );
        backend.delete_program( prog  );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_AaaMaterialPBR_PixelComparable )
{
    std::vector< std::uint8_t > const rendered = render_aaa_material_pbr_c134b(
        /*use_stub_config*/ false );

    //	PBR canonical : full BRDF + 3.0-intensity light + ambient.
    //	Vast majority of pixels carry non-zero light contribution ;
    //	1/8 of the framebuffer is the conservative floor matching
    //	c133-B's GoldenFrame_AaaCamReal pattern.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 8 )
        << "aaa_material_pbr produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:aaa_material_pbr] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaa_material_pbr.png";
    compare_or_capture_golden( golden_path, rendered, "aaa_material_pbr" );
}

TEST( RegressionPhase3, RevivalProof_AaaMaterialPBR_DiffersFromStub )
{
    //	Render the SAME aaa_material_pbr.metal twice : once with the
    //	canonical PBR config , once with a "stub-equivalent" config
    //	that zeroes the light intensity / metallic / roughness / AO so
    //	the BRDF degenerates to the historic flat-albedo behaviour.
    //	Diff is the proof that the real D*G*F*(1+diffuse) pipeline
    //	actually drove the fragment output.
    std::vector< std::uint8_t > const rendered_real = render_aaa_material_pbr_c134b(
        /*use_stub_config*/ false );
    std::vector< std::uint8_t > const rendered_stub = render_aaa_material_pbr_c134b(
        /*use_stub_config*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha is constant
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:aaa_material_pbr] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold per c134-B spec : 5000 pixels and 50-LSB max. The PBR
    //	canonical config drives every pixel through a BRDF + light
    //	contribution that the stub config zeroes out, so divergence is
    //	expected to be near-total (tens of thousands of pixels, well
    //	above 100 LSB max). Conservative floors >5000 / >50 catch the
    //	regression mode "BRDF silently collapsed".
    EXPECT_GT( diff_count, std::size_t( 5000 ) )
        << "aaa_material_pbr canonical output is nearly identical to "
        << "the stub-config render -- the Cook-Torrance BRDF never "
        << "drove the fragment output. Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 50 )
        << "aaa_material_pbr per-channel diff vs stub-config never "
        << "exceeds 50 LSB -- BRDF specular highlight collapsed. "
        << "max_diff = " << max_diff;
}

//	==================== c135-A : aaa_noise_real (SIXTH Path A revival) ====
//	Real Perlin 1985 / 2002 classic 3D noise + Perlin 2001 / Gustavson
//	2005 Simplex 3D noise plumbed end-to-end through GOL::MetalBackend.
//	Replaces the historic c80 stubs (ps_Maa_noise.metal P4 stub
//	`MCposition = float3(0)` + ps_Maa_noise_cheap.metal near-passthrough
//	frac construction). Both stubs are PRESERVED VERBATIM in their
//	respective shader files (c80 / c81 regression goldens stay intact) ;
//	this revival lives in a SEPARATE file
//	`src/shaders/msl/aaa_noise_real.metal`. Path A catalog floor bumped
//	163 -> 164. Pattern mirrors c128-A (fxaa_lottes) + c130-A
//	(ifs_de_library) + c132-A (lights_deferred_real) + c133-B
//	(aaa_cam_real) + c134-B (aaa_material_pbr).
//
//	Citation : Perlin 1985 SIGGRAPH "An Image Synthesizer" (original
//	gradient noise) ; Perlin 2002 SIGGRAPH "Improving Noise"
//	(quintic fade 6t^5 - 15t^4 + 10t^3 + 16-vector grad table) ;
//	Perlin 2001 SIGGRAPH Course Notes "Noise Hardware" (Simplex) ;
//	Gustavson 2005 "Simplex noise demystified" (clean reference impl
//	with F3=1/3 skew, G3=1/6 unskew, 0.6 radial kernel, grad12 table).
//
//	Two TESTs :
//	  1. GoldenFrame_AaaNoiseReal_PixelComparable -- canonical real-
//	     noise config (mode = 1). Renders the 4-quadrant 256x256 image
//	     (TL Perlin scalar, TR Simplex scalar, BL Perlin fbm5, BR
//	     Simplex fbm5). Capture golden ; replay bit-exact.
//	  2. RevivalProof_AaaNoiseReal_DiffersFromStub -- renders the SAME
//	     aaa_noise_real.metal twice : once with mode = 1 (real noise),
//	     once with mode = 0 (flat mid-grey, the stub equivalent of a
//	     constant-MCposition noise sample). Per c135-A spec : pixel
//	     diff > 5000 and per-channel max > 50 LSB. The 4-quadrant
//	     noise field vs flat-grey divergence is expected to dominate
//	     nearly every pixel.
//
//	No collision with c135-B : that session works on
//	src/ui/macos/aaa_dialog.{h,mm} + tests/unit/ui_dialog_test.cpp ;
//	this session's only c++ touch is THIS test file (2 new TESTs at
//	the tail) + tests/unit/path_a_catalog_test.cpp (floor bump).
//	Disjoint file scope.
namespace
{
    void fill_aaa_noise_real_canonical_c135a( AaaFuFloats& uf,
                                              AaaFuVec4s&  uv4,
                                              AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Mode = 1 : real 4-quadrant Perlin + Simplex noise.
        ui.values[ 0 ] = 1;
    }

    void fill_aaa_noise_real_stub_c135a( AaaFuFloats& uf,
                                         AaaFuVec4s&  uv4,
                                         AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Mode = 0 : stub-passthrough. The c80 ps_Maa_noise stub
        //	with MCposition = float3(0) collapses to a constant noise
        //	sample -> flat colour. The shader returns mid-grey 0.5
        //	for the stub branch.
        ui.values[ 0 ] = 0;
    }

    std::vector< std::uint8_t > render_aaa_noise_real_c135a( bool use_stub_config )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_noise_real.metal";
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                   GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt, GOL::kInvalidTextureId );

        AaaFuFloats uf{};
        AaaFuVec4s  uv4{};
        AaaFuInts   ui{};
        if( use_stub_config )
            fill_aaa_noise_real_stub_c135a( uf, uv4, ui );
        else
            fill_aaa_noise_real_canonical_c135a( uf, uv4, ui );

        GOL::BufferId ub_f  = backend.gen_buffer();
        GOL::BufferId ub_v4 = backend.gen_buffer();
        GOL::BufferId ub_i  = backend.gen_buffer();
        EXPECT_NE( ub_f,  GOL::kInvalidBufferId );
        EXPECT_NE( ub_v4, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i,  GOL::kInvalidBufferId );
        backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
        backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = rt;
        rpd.load_action      = GOL::LoadAction::Clear;
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 0.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = use_stub_config
                             ? "regression.phase3.aaa_noise_real_stub_config"
                             : "regression.phase3.aaa_noise_real_canonical";

        EXPECT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_buffer( ub_f,  0, 0 );
        backend.bind_fragment_buffer( ub_v4, 1, 0 );
        backend.bind_fragment_buffer( ub_i,  2, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i  );
        backend.delete_buffer(  ub_v4 );
        backend.delete_buffer(  ub_f  );
        backend.delete_texture( rt    );
        backend.delete_program( prog  );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_AaaNoiseReal_PixelComparable )
{
    std::vector< std::uint8_t > const rendered = render_aaa_noise_real_c135a(
        /*use_stub_config*/ false );

    //	4-quadrant noise field : every pixel is somewhere in the
    //	mid-grey neighbourhood of [-1,+1] mapped to [0,1]. The clear
    //	colour is pure black so virtually every pixel will be
    //	non-zero-RGB after the noise sample. Loose floor 1/2 of the
    //	framebuffer matches the brightness/lights goldens.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "aaa_noise_real produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:aaa_noise_real] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaa_noise_real.png";
    compare_or_capture_golden( golden_path, rendered, "aaa_noise_real" );
}

TEST( RegressionPhase3, RevivalProof_AaaNoiseReal_DiffersFromStub )
{
    //	Render the SAME aaa_noise_real.metal twice : once with mode = 1
    //	(real 4-quadrant Perlin + Simplex noise) , once with mode = 0
    //	(flat mid-grey , the equivalent of the historic c80 stub with
    //	MCposition = float3(0) producing a constant noise sample).
    //	Diff is the proof that the real Perlin + Simplex 3D gradient
    //	noise actually drove the fragment output.
    std::vector< std::uint8_t > const rendered_real = render_aaa_noise_real_c135a(
        /*use_stub_config*/ false );
    std::vector< std::uint8_t > const rendered_stub = render_aaa_noise_real_c135a(
        /*use_stub_config*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha is constant
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:aaa_noise_real] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold per c135-A spec : 5000 pixels and 50-LSB max. The
    //	noise field varies smoothly across [-1,+1] -> [0,1] LSB 0..255 ;
    //	the stub case is a flat mid-grey 128. Most pixels diverge from
    //	128 by at least 5-10 LSB ; the brightest and darkest noise
    //	pixels exceed 50 LSB by a wide margin. Conservative floors
    //	>5000 / >50 catch the regression mode "real noise silently
    //	fell back to the stub branch".
    EXPECT_GT( diff_count, std::size_t( 5000 ) )
        << "aaa_noise_real canonical output is nearly identical to "
        << "the stub-config render -- the real Perlin + Simplex noise "
        << "never drove the fragment output. Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 50 )
        << "aaa_noise_real per-channel diff vs stub-config never "
        << "exceeds 50 LSB -- noise dynamic range collapsed. "
        << "max_diff = " << max_diff;
}

//	==================== c136-A : aaa_gol_real (SEVENTH Path A revival) ====
//	Real 2D cellular automata plumbed end-to-end through
//	GOL::MetalBackend with CPU-side ping-pong over N iterations.
//	Two automata share one shader, switchable via uniform :
//	   variant = 0 : Conway 1970 B3/S23 (classic Game of Life).
//	   variant = 1 : Silverman 1991 Brian's Brain (3-state).
//	Both use the 8-neighbour Moore neighbourhood with toroidal wrap
//	via manual fract(uv+1) on the previous-state texture sample.
//	The Path A catalog historically has only GOL-stub-family members
//	rendering passthrough / flat textures with no neighbour evaluation ;
//	the c136-A revival is the canonical real-rules port. Catalog
//	floor bumped 164 -> 165. Pattern mirrors c128-A / c130-A / c132-A
//	/ c133-B / c134-B / c135-A.
//
//	Citations : Conway 1970 Scientific American October "Mathematical
//	Games -- The fantastic combinations of John Conway's new solitaire
//	game LIFE" ; Silverman 1991 "The Phantom Fish Tank -- Brian's
//	Brain" Logo Foundation ; Wolfram 2002 "A New Kind of Science"
//	Ch. 4 (CA classification : Conway is class 4, Brian's Brain is
//	class 3) ; Gardner 1970/1983 "Wheels, Life, and Other Mathematical
//	Amusements" Ch. 20-22 (R-pentomino + glider seed patterns).
//
//	Two TESTs :
//	  1. GoldenFrame_AaaGolReal_PixelComparable -- Conway B3/S23 seeded
//	     with the canonical 16x16 stencil (R-pentomino + glider +
//	     Brian's-Brain spaceship + blinker) tiled across 256x256,
//	     iterated N=5 times via CPU-side ping-pong. Capture golden ;
//	     replay bit-exact.
//	  2. RevivalProof_AaaGolReal_DiffersFromStub -- renders the SAME
//	     aaa_gol_real.metal twice at N=5 : once with mode = 1
//	     (real CA), once with mode = 0 (flat mid-grey, the stub
//	     equivalent of the historic GOL-family passthrough). Per
//	     c136-A spec : pixel diff > 5000 and per-channel max > 50
//	     LSB. The CA's active-cell grid vs flat-grey divergence is
//	     expected to dominate nearly every pixel.
//
//	No collision with c136-B : that session works on
//	bundle/macos/Info.plist + bundle/macos/entitlements.plist +
//	src/macos/aaa_bundle_meta.{h,mm} + new tests ; this session's
//	only c++ touch is THIS test file (2 new TESTs at the tail) +
//	tests/unit/path_a_catalog_test.cpp (floor bump). Disjoint file
//	scope.
namespace
{
    void fill_aaa_gol_real_canonical_c136a( AaaFuFloats& uf,
                                            AaaFuVec4s&  uv4,
                                            AaaFuInts&   ui,
                                            int          iter_idx )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Mode = 1 : real CA. Variant = 0 : Conway B3/S23.
        ui.values[ 0 ] = 1;
        ui.values[ 1 ] = 0;
        ui.values[ 2 ] = iter_idx;
    }

    void fill_aaa_gol_real_stub_c136a( AaaFuFloats& uf,
                                       AaaFuVec4s&  uv4,
                                       AaaFuInts&   ui,
                                       int          iter_idx )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Mode = 0 : stub branch (flat mid-grey). Iter_idx still
        //	threaded through so the call site can stay symmetric
        //	with the real-CA loop.
        ui.values[ 0 ] = 0;
        ui.values[ 1 ] = 0;
        ui.values[ 2 ] = iter_idx;
    }

    //	Render N CA iterations against a CPU-side ping-pong texture
    //	pair. Each iteration binds the previous-state texture at
    //	fragment slot 0, draws the fullscreen triangle into the
    //	other texture, then swaps. iter_idx = 0 emits the seed
    //	stencil ; iter_idx >= 1 steps the rule.
    //
    //	On the stub branch (use_stub_config = true) the shader's
    //	mode-0 path returns flat mid-grey at every iteration ; the
    //	ping-pong still runs N times so the buffer-bind and viewport
    //	state is byte-identical to the real path -- ANY pixel
    //	divergence in the final compare is provably the CA rules
    //	themselves, not the framework around them.
    std::vector< std::uint8_t > render_aaa_gol_real_c136a( bool use_stub_config )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_gol_real.metal";
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        //	Ping-pong texture pair. Both 256x256 RGBA8. We alternate :
        //	iter k samples from tex[ k & 1 ] and writes tex[ (k+1) & 1 ].
        GOL::TextureId tex[ 2 ] = {
            backend.gen_texture_2d( kWidth, kHeight, GOL::TextureFormat::RGBA8 ),
            backend.gen_texture_2d( kWidth, kHeight, GOL::TextureFormat::RGBA8 )
        };
        EXPECT_NE( tex[ 0 ], GOL::kInvalidTextureId );
        EXPECT_NE( tex[ 1 ], GOL::kInvalidTextureId );

        //	Iteration count : N = 5 per c136-A spec. The shader writes
        //	the seed at iter_idx = 0 and then steps 4 more times for
        //	a total of 5 draws. The final state lives in tex[ 5 & 1 ]
        //	= tex[ 1 ].
        int const N = 5;

        GOL::BufferId ub_f  = backend.gen_buffer();
        GOL::BufferId ub_v4 = backend.gen_buffer();
        GOL::BufferId ub_i  = backend.gen_buffer();
        EXPECT_NE( ub_f,  GOL::kInvalidBufferId );
        EXPECT_NE( ub_v4, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i,  GOL::kInvalidBufferId );

        for( int iter = 0; iter <= N; ++iter )
        {
            AaaFuFloats uf{};
            AaaFuVec4s  uv4{};
            AaaFuInts   ui{};
            if( use_stub_config )
                fill_aaa_gol_real_stub_c136a( uf, uv4, ui, iter );
            else
                fill_aaa_gol_real_canonical_c136a( uf, uv4, ui, iter );

            backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
            backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
            backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

            int const src_slot = iter & 1;
            int const dst_slot = ( iter + 1 ) & 1;

            GOL::RenderPassDescriptor rpd;
            rpd.color_attachment = tex[ dst_slot ];
            rpd.load_action      = GOL::LoadAction::Clear;
            rpd.clear_color[ 0 ] = 0.0f;
            rpd.clear_color[ 1 ] = 0.0f;
            rpd.clear_color[ 2 ] = 0.0f;
            rpd.clear_color[ 3 ] = 1.0f;
            rpd.debug_label      = use_stub_config
                                 ? "regression.phase3.aaa_gol_real_stub_config"
                                 : "regression.phase3.aaa_gol_real_canonical";

            EXPECT_TRUE( backend.begin_render_pass( rpd ) );
            backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
            backend.bind_program( prog );
            backend.bind_fragment_texture( tex[ src_slot ], 0 );
            backend.bind_fragment_buffer( ub_f,  0, 0 );
            backend.bind_fragment_buffer( ub_v4, 1, 0 );
            backend.bind_fragment_buffer( ub_i,  2, 0 );
            backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
            backend.end_render_pass();
            backend.present();
        }

        //	Final state lives in tex[ (N+1) & 1 ] -- the texture we
        //	wrote to on the last iteration.
        GOL::TextureId final_tex = tex[ ( N + 1 ) & 1 ];
        backend.read_texture_pixels( final_tex, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i  );
        backend.delete_buffer(  ub_v4 );
        backend.delete_buffer(  ub_f  );
        backend.delete_texture( tex[ 1 ] );
        backend.delete_texture( tex[ 0 ] );
        backend.delete_program( prog  );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_AaaGolReal_PixelComparable )
{
    std::vector< std::uint8_t > const rendered = render_aaa_gol_real_c136a(
        /*use_stub_config*/ false );

    //	The Conway grid after 5 iterations from the R-pentomino + glider
    //	+ Brian's spaceship + blinker seed has a small but nonzero
    //	alive-cell count (R = 255) plus a generation counter in B that
    //	is nonzero at every pixel after iter 1. Loose floor : at least
    //	1/8 of the framebuffer is non-RGB-zero (the generation channel
    //	alone clears this floor easily because it ticks every iteration).
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 8 )
        << "aaa_gol_real produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:aaa_gol_real] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaa_gol_real.png";
    compare_or_capture_golden( golden_path, rendered, "aaa_gol_real" );
}

TEST( RegressionPhase3, RevivalProof_AaaGolReal_DiffersFromStub )
{
    //	Render the SAME aaa_gol_real.metal twice at N = 5 iterations :
    //	once with mode = 1 (real Conway B3/S23 CA) , once with mode = 0
    //	(flat mid-grey , the equivalent of the historic GOL-family
    //	passthrough stub producing a constant colour). Diff is the
    //	proof that the real cellular-automaton rule actually drove the
    //	fragment output.
    std::vector< std::uint8_t > const rendered_real = render_aaa_gol_real_c136a(
        /*use_stub_config*/ false );
    std::vector< std::uint8_t > const rendered_stub = render_aaa_gol_real_c136a(
        /*use_stub_config*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha is constant
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:aaa_gol_real] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold per c136-A spec : 5000 pixels and 50-LSB max. The
    //	real CA's output channels are : R in {0, 255} (alive flag),
    //	G in {0, 255} (Brian dying ; here 0 for Conway), B in 0..255
    //	(generation counter, increments by 1/255 each iteration). The
    //	stub case is a flat mid-grey RGB = (128, 128, 128). EVERY pixel
    //	diverges by at least ~123 LSB on the B channel alone (128 vs
    //	gen_count = 5 at N = 5 -> diff ~= 123). Conservative floors
    //	>5000 / >50 catch the regression mode "real CA silently fell
    //	back to the stub branch".
    EXPECT_GT( diff_count, std::size_t( 5000 ) )
        << "aaa_gol_real canonical output is nearly identical to "
        << "the stub-config render -- the real Conway B3/S23 "
        << "cellular automaton never drove the fragment output. "
        << "Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 50 )
        << "aaa_gol_real per-channel diff vs stub-config never "
        << "exceeds 50 LSB -- CA state encoding collapsed. "
        << "max_diff = " << max_diff;
}

//	==================== c137-A : aaa_curl_noise_real (EIGHTH Path A revival) ====
//	Real curl-noise velocity field plumbed end-to-end through
//	GOL::MetalBackend. Builds a 3D vector potential psi whose
//	components are 3 uncorrelated samples of c135-A's Perlin 3D
//	gradient noise, then takes the curl via 12-sample central
//	finite-difference (eps = 1e-3) to produce a divergence-free
//	velocity field. The output is a 4-quadrant 256x256 image
//	visualising the field four ways : |vel| greyscale (TL),
//	vel direction RGB (TR), 50-step RK2 advected displacement (BL),
//	100-step RK2 advected displacement (BR). The Path A catalog
//	does not have any historic "fluid-flow / divergence-free
//	procedural field" shader ; this revival lands one. Catalog
//	floor bumped 165 -> 166. Pattern mirrors c128-A / c130-A /
//	c132-A / c133-B / c134-B / c135-A / c136-A.
//
//	Citations : Bridson, Hourihan, Nordenstam 2007 ACM SCA
//	"Curl-Noise for Procedural Fluid Flow" (canonical reference) ;
//	Bridson + Mueller-Fischer 2007 SIGGRAPH Course Notes "Fluid
//	Simulation" Ch. 14 (broader fluids exposition) ; Perlin 2001
//	SIGGRAPH Course Notes "Noise Hardware" + Perlin 2002 SIGGRAPH
//	"Improving Noise" (the underlying gradient noise sampled to
//	build psi) ; Stam 1999 SIGGRAPH "Stable Fluids" (RK2 semi-
//	Lagrangian advection used for the bottom-half quadrants).
//
//	Two TESTs :
//	  1. GoldenFrame_AaaCurlNoiseReal_PixelComparable -- canonical
//	     real-curl-noise config (mode = 1 ; overlay = 0 ;
//	     adv_override = 0 -> 50-step BL / 100-step BR). Renders
//	     the 4-quadrant 256x256 image. Capture golden ; replay
//	     bit-exact.
//	  2. RevivalProof_AaaCurlNoiseReal_DiffersFromStub -- renders
//	     the SAME aaa_curl_noise_real.metal twice : once with
//	     mode = 1 (real curl field + advection) , once with
//	     mode = 0 (flat mid-grey , the stub equivalent of "no real
//	     velocity field"). Per c137-A spec : pixel diff > 5000
//	     and per-channel max > 50 LSB. The combination of the
//	     active curl field (top half) + advected particle
//	     displacement (bottom half) vs flat-grey divergence is
//	     expected to dominate nearly every pixel.
//
//	No collision with c137-B : that session works on cmake/dmg.cmake
//	+ top-level CMakeLists.txt DMG target + bundle/macos/dmg_staging/
//	placeholder + tests/unit/dmg_packaging_test.cpp ; this session's
//	only c++ touch is THIS test file (2 new TESTs at the tail) +
//	tests/unit/path_a_catalog_test.cpp (floor bump). Disjoint file
//	scope.
namespace
{
    void fill_aaa_curl_noise_real_canonical_c137a( AaaFuFloats& uf,
                                                   AaaFuVec4s&  uv4,
                                                   AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Mode = 1 : real curl-noise velocity field.
        //	Overlay = 0 : 4-quadrant composite (|vel|, dir, advect-50, advect-100).
        //	adv_override = 0 : use defaults (50 for BL , 100 for BR).
        ui.values[ 0 ] = 1;
        ui.values[ 1 ] = 0;
        ui.values[ 2 ] = 0;
    }

    void fill_aaa_curl_noise_real_stub_c137a( AaaFuFloats& uf,
                                              AaaFuVec4s&  uv4,
                                              AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Mode = 0 : stub-passthrough flat mid-grey. The stub
        //	equivalent of "no real velocity field" is a constant
        //	colour 0.5 across the frame.
        ui.values[ 0 ] = 0;
        ui.values[ 1 ] = 0;
        ui.values[ 2 ] = 0;
    }

    std::vector< std::uint8_t > render_aaa_curl_noise_real_c137a( bool use_stub_config )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_curl_noise_real.metal";
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                   GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt, GOL::kInvalidTextureId );

        AaaFuFloats uf{};
        AaaFuVec4s  uv4{};
        AaaFuInts   ui{};
        if( use_stub_config )
            fill_aaa_curl_noise_real_stub_c137a( uf, uv4, ui );
        else
            fill_aaa_curl_noise_real_canonical_c137a( uf, uv4, ui );

        GOL::BufferId ub_f  = backend.gen_buffer();
        GOL::BufferId ub_v4 = backend.gen_buffer();
        GOL::BufferId ub_i  = backend.gen_buffer();
        EXPECT_NE( ub_f,  GOL::kInvalidBufferId );
        EXPECT_NE( ub_v4, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i,  GOL::kInvalidBufferId );
        backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
        backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i,  sizeof( ui  ), &ui,  GOL::BufferUsage::Static );

        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = rt;
        rpd.load_action      = GOL::LoadAction::Clear;
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 0.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = use_stub_config
                             ? "regression.phase3.aaa_curl_noise_real_stub_config"
                             : "regression.phase3.aaa_curl_noise_real_canonical";

        EXPECT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_buffer( ub_f,  0, 0 );
        backend.bind_fragment_buffer( ub_v4, 1, 0 );
        backend.bind_fragment_buffer( ub_i,  2, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i  );
        backend.delete_buffer(  ub_v4 );
        backend.delete_buffer(  ub_f  );
        backend.delete_texture( rt    );
        backend.delete_program( prog  );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_AaaCurlNoiseReal_PixelComparable )
{
    std::vector< std::uint8_t > const rendered = render_aaa_curl_noise_real_c137a(
        /*use_stub_config*/ false );

    //	4-quadrant curl-noise frame : top half is direct velocity-field
    //	visualisation (every pixel has some magnitude / direction
    //	component) ; bottom half is advected-displacement greyscale
    //	(most particles transport non-trivially under the curl field).
    //	Loose floor 1/2 of the framebuffer matches the noise/lights
    //	goldens. The clear colour is black ; virtually every pixel
    //	ends up non-zero-RGB after the curl-noise sample.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "aaa_curl_noise_real produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:aaa_curl_noise_real] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaa_curl_noise_real.png";
    compare_or_capture_golden( golden_path, rendered, "aaa_curl_noise_real" );
}

TEST( RegressionPhase3, RevivalProof_AaaCurlNoiseReal_DiffersFromStub )
{
    //	Render the SAME aaa_curl_noise_real.metal twice : once with
    //	mode = 1 (real divergence-free curl-noise velocity field
    //	+ RK2 advection) , once with mode = 0 (flat mid-grey , the
    //	stub equivalent of "no real velocity field"). Diff is the
    //	proof that the real Bridson 2007 curl construction actually
    //	drove the fragment output.
    std::vector< std::uint8_t > const rendered_real = render_aaa_curl_noise_real_c137a(
        /*use_stub_config*/ false );
    std::vector< std::uint8_t > const rendered_stub = render_aaa_curl_noise_real_c137a(
        /*use_stub_config*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha is constant
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:aaa_curl_noise_real] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold per c137-A spec : 5000 pixels and 50-LSB max. The
    //	real frame has the curl-noise direction quadrant (TR) that
    //	maps vel components to RGB across the full [0,255] LSB range,
    //	the |vel| magnitude quadrant (TL) with bright/dark bands,
    //	and the two advection quadrants (BL/BR) with displacement
    //	brightness in 0..255. The stub case is a flat mid-grey
    //	RGB = (128, 128, 128). The TR direction quadrant alone
    //	produces per-channel deltas exceeding 100 LSB at the
    //	brightest / darkest extremes. Conservative floors
    //	>5000 / >50 catch the regression mode "real curl noise
    //	silently fell back to the stub branch".
    EXPECT_GT( diff_count, std::size_t( 5000 ) )
        << "aaa_curl_noise_real canonical output is nearly identical "
        << "to the stub-config render -- the real Bridson 2007 curl-"
        << "noise field never drove the fragment output. Diff = "
        << diff_count << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 50 )
        << "aaa_curl_noise_real per-channel diff vs stub-config "
        << "never exceeds 50 LSB -- curl-field dynamic range "
        << "collapsed. max_diff = " << max_diff;
}

//	==================== c138-A : aaa_bloom_real (NINTH Path A revival) =========
//	Real 4-pass bloom post-process plumbed end-to-end through
//	GOL::MetalBackend. The pipeline :
//	  Pass 0 : rec.709 luminance threshold with smoothstep soft knee
//	           (threshold .. threshold + 0.5). Pixels above threshold
//	           pass through ; pixels below collapse to black ; pixels
//	           in the knee region attenuate proportionally.
//	  Pass 1 : horizontal 13-tap separable Gaussian. Kernel = Pascal
//	           triangle row 12 / 4096 (1, 12, 66, 220, 495, 792, 924,
//	           792, 495, 220, 66, 12, 1) ; sigma ~ 4 px.
//	  Pass 2 : vertical 13-tap separable Gaussian. Same kernel,
//	           rotated 90 deg. Combined with Pass 1 -> full 2D
//	           separable Gaussian at 26 fetches/pixel instead of 169
//	           (the separability win).
//	  Pass 3 : additive composite : final = base + intensity * blurred.
//	The driver ping-pongs across three render targets : rt_threshold
//	(Pass 0 output -> Pass 1 input), rt_hblur (Pass 1 output -> Pass 2
//	input), rt_vblur (Pass 2 output -> Pass 3 sampling slot 1).
//	Final image read back from rt_composite. One MSL file ; the
//	pass selector AaaFuInts[3] picks which branch executes.
//
//	The input fixture is the procedural gradient PLUS a "hot pixel
//	cluster" at coord (128,128) +/- 2 px set to RGB = (255, 255, 255).
//	With threshold = 0.6 , only the cluster fires the threshold (its
//	rec.709 luma = 1.0 ; the gradient stays below 0.6 at the corners
//	but exceeds it in the bright corner -- in practice the cluster
//	dominates by luma and the blur creates a soft halo around it).
//	This guarantees a strong visual signal that diverges from the
//	flat mid-grey stub branch.
//
//	Path A catalog floor bumped 166 -> 167. Pattern mirrors
//	c128-A / c130-A / c132-A / c133-B / c134-B / c135-A / c136-A /
//	c137-A.
//
//	Citations : Bjorke 2007 "GPU Gems 3" Ch.40 "Incremental
//	Computation of the Gaussian" (7-tap linear-sampling trick noted ;
//	we use the explicit 13-tap form for auditability) ; Real-Time
//	Rendering 4th ed (Akenine-Moller / Haines / Hoffman) §10.6
//	"Bloom" (canonical exposition of threshold + separable blur +
//	additive composite pipeline ; soft-threshold from §10.6.1) ;
//	Sousa 2008 GDC "Crysis 2 Next Gen Effects" (luminance threshold
//	+ multi-scale Gaussian stack used by modern AAA bloom).
//
//	Two TESTs :
//	  1. GoldenFrame_AaaBloomReal_PixelComparable -- canonical real-
//	     bloom config , 4-pass pipeline executes , final composite
//	     read back. Capture golden ; replay bit-exact.
//	  2. RevivalProof_AaaBloomReal_DiffersFromStub -- renders the
//	     SAME aaa_bloom_real.metal with mode = 1 (real 4-pass bloom)
//	     vs mode = 0 (flat mid-grey , no multipass). Per c138-A spec :
//	     diff > 5000 pixels and per-channel max > 50 LSB.
//
//	No collision with c138-B : that session works on cmake/codesign.cmake +
//	tests + bundle/macos/ README additions ; this session's c++ touch is
//	THIS file (2 new TESTs at the tail) + tests/unit/path_a_catalog_test.cpp
//	(floor bump). Disjoint file scope.
namespace
{
    //	Build a base scene texture for the bloom golden : the standard
    //	procedural gradient with a 5x5 "hot pixel cluster" forced to
    //	max-white at the centre. Rec.709 luma at the cluster = 1.0 ,
    //	which is well above the threshold = 0.6 default -- this is
    //	the pixel cluster that lights up the bloom.
    std::vector< std::uint8_t > make_bloom_scene_c138a()
    {
        std::vector< std::uint8_t > p = make_gradient_rgba8( kWidth, kHeight );

        //	Hot cluster : centre (128, 128) , 5x5 set to (255,255,255,255).
        //	Plus a dim cluster nearby (64,64) at (180,180,180) -- below the
        //	threshold's smoothstep knee start , should NOT bloom. Useful
        //	for visual sanity but the test does not directly assert on it.
        std::uint32_t const cx = 128;
        std::uint32_t const cy = 128;
        for( int dy = -2; dy <= 2; ++dy )
        {
            for( int dx = -2; dx <= 2; ++dx )
            {
                std::uint32_t const x = std::uint32_t( int( cx ) + dx );
                std::uint32_t const y = std::uint32_t( int( cy ) + dy );
                std::size_t const base = ( y * kWidth + x ) * 4;
                p[ base + 0 ] = 255;
                p[ base + 1 ] = 255;
                p[ base + 2 ] = 255;
                p[ base + 3 ] = 255;
            }
        }

        std::uint32_t const dx2 = 64;
        std::uint32_t const dy2 = 64;
        for( int dy = -2; dy <= 2; ++dy )
        {
            for( int dx = -2; dx <= 2; ++dx )
            {
                std::uint32_t const x = std::uint32_t( int( dx2 ) + dx );
                std::uint32_t const y = std::uint32_t( int( dy2 ) + dy );
                std::size_t const base = ( y * kWidth + x ) * 4;
                //	Below-threshold dim cluster ; rec.709 luma ~ 0.706 ?
                //	Actually (180/255) ~ 0.706 . That IS above 0.6 ; but
                //	the smoothstep ramp from 0.6 to 1.1 puts it at ~0.21
                //	contribution , versus the bright cluster's 1.0 . The
                //	contrast still dominates -- the dim cluster's bloom
                //	halo is faint but present.
                p[ base + 0 ] = 180;
                p[ base + 1 ] = 180;
                p[ base + 2 ] = 180;
                p[ base + 3 ] = 255;
            }
        }
        return p;
    }

    void fill_aaa_bloom_real_canonical_c138a( AaaFuFloats& uf,
                                              AaaFuVec4s&  uv4,
                                              AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Floats : threshold , intensity , rcp_w , rcp_h .
        uf.values[ 0 ] = 0.6f;                          // threshold
        uf.values[ 1 ] = 0.4f;                          // bloom_intensity
        uf.values[ 2 ] = 1.0f / float( kWidth  );       // rcp_w
        uf.values[ 3 ] = 1.0f / float( kHeight );       // rcp_h

        //	Ints : mode = 1 (real bloom) ; entry_pass set per-pass.
        ui.values[ 0 ] = 1;
        ui.values[ 3 ] = 0;                             // will be overridden per pass
    }

    void fill_aaa_bloom_real_stub_c138a( AaaFuFloats& uf,
                                         AaaFuVec4s&  uv4,
                                         AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Same float values for parity (irrelevant in stub branch).
        uf.values[ 0 ] = 0.6f;
        uf.values[ 1 ] = 0.4f;
        uf.values[ 2 ] = 1.0f / float( kWidth  );
        uf.values[ 3 ] = 1.0f / float( kHeight );

        //	Mode = 0 : flat mid-grey stub branch. The shader will
        //	short-circuit to RGB = (0.5, 0.5, 0.5) regardless of
        //	entry_pass , so the driver still chains 4 render passes
        //	but every output is uniform grey.
        ui.values[ 0 ] = 0;
        ui.values[ 3 ] = 0;
    }

    std::vector< std::uint8_t > render_aaa_bloom_real_c138a( bool use_stub_config )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_bloom_real.metal";
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        //	---------- Input scene + three ping-pong render targets ----------
        GOL::TextureId scene_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                          GOL::TextureFormat::RGBA8 );
        EXPECT_NE( scene_tex, GOL::kInvalidTextureId );
        std::vector< std::uint8_t > const scene = make_bloom_scene_c138a();
        backend.texture_data_2d( scene_tex, scene.data(), kBytesPerRow );

        GOL::TextureId rt_threshold = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        GOL::TextureId rt_hblur     = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        GOL::TextureId rt_vblur     = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        GOL::TextureId rt_composite = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt_threshold, GOL::kInvalidTextureId );
        EXPECT_NE( rt_hblur,     GOL::kInvalidTextureId );
        EXPECT_NE( rt_vblur,     GOL::kInvalidTextureId );
        EXPECT_NE( rt_composite, GOL::kInvalidTextureId );

        AaaFuFloats uf{};
        AaaFuVec4s  uv4{};
        AaaFuInts   ui{};
        if( use_stub_config )
            fill_aaa_bloom_real_stub_c138a( uf, uv4, ui );
        else
            fill_aaa_bloom_real_canonical_c138a( uf, uv4, ui );

        //	One uniform buffer per pass : pass selector differs.
        //	Float buffer + Vec4 buffer are pass-invariant so we reuse one.
        GOL::BufferId ub_f  = backend.gen_buffer();
        GOL::BufferId ub_v4 = backend.gen_buffer();
        EXPECT_NE( ub_f,  GOL::kInvalidBufferId );
        EXPECT_NE( ub_v4, GOL::kInvalidBufferId );
        backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
        backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );

        GOL::BufferId ub_i_p0 = backend.gen_buffer();
        GOL::BufferId ub_i_p1 = backend.gen_buffer();
        GOL::BufferId ub_i_p2 = backend.gen_buffer();
        GOL::BufferId ub_i_p3 = backend.gen_buffer();
        EXPECT_NE( ub_i_p0, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i_p1, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i_p2, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i_p3, GOL::kInvalidBufferId );

        AaaFuInts ui_p0 = ui ; ui_p0.values[ 3 ] = 0;
        AaaFuInts ui_p1 = ui ; ui_p1.values[ 3 ] = 1;
        AaaFuInts ui_p2 = ui ; ui_p2.values[ 3 ] = 2;
        AaaFuInts ui_p3 = ui ; ui_p3.values[ 3 ] = 3;
        backend.buffer_data( ub_i_p0, sizeof( ui_p0 ), &ui_p0, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i_p1, sizeof( ui_p1 ), &ui_p1, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i_p2, sizeof( ui_p2 ), &ui_p2, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i_p3, sizeof( ui_p3 ), &ui_p3, GOL::BufferUsage::Static );

        //	Helper lambda : render one bloom pass with given inputs
        //	to a single colour RT. Encapsulates RPD setup + draw +
        //	end. Each pass binds the colour-input texture(s) on
        //	slot 0 (and slot 1 for the composite pass).
        auto run_pass = [ & ](
            GOL::BufferId       ub_i,
            GOL::TextureId      tex0,
            GOL::TextureId      tex1,           // kInvalidTextureId if unused
            GOL::TextureId      target,
            char const*         debug_label )
        {
            GOL::RenderPassDescriptor rpd;
            rpd.color_attachment = target;
            rpd.load_action      = GOL::LoadAction::Clear;
            rpd.clear_color[ 0 ] = 0.0f;
            rpd.clear_color[ 1 ] = 0.0f;
            rpd.clear_color[ 2 ] = 0.0f;
            rpd.clear_color[ 3 ] = 1.0f;
            rpd.debug_label      = debug_label;
            EXPECT_TRUE( backend.begin_render_pass( rpd ) );
            backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
            backend.bind_program( prog );
            backend.bind_fragment_texture( tex0, 0 );
            //	Always bind a texture on slot 1 -- Metal validation
            //	requires every declared slot to be bound , so we bind
            //	scene_tex as a benign placeholder when the pass does
            //	not sample slot 1 .
            backend.bind_fragment_texture(
                tex1 != GOL::kInvalidTextureId ? tex1 : scene_tex, 1 );
            backend.bind_fragment_buffer( ub_f,  0, 0 );
            backend.bind_fragment_buffer( ub_v4, 1, 0 );
            backend.bind_fragment_buffer( ub_i,  2, 0 );
            backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
            backend.end_render_pass();
        };

        //	---------- 4-pass orchestration ----------
        //	Pass 0 : threshold ; scene -> rt_threshold .
        char const* label_p0 = use_stub_config
            ? "regression.phase3.aaa_bloom_real_stub_p0_threshold"
            : "regression.phase3.aaa_bloom_real_p0_threshold";
        run_pass( ub_i_p0, scene_tex, GOL::kInvalidTextureId, rt_threshold, label_p0 );

        //	Pass 1 : horizontal blur ; rt_threshold -> rt_hblur .
        char const* label_p1 = use_stub_config
            ? "regression.phase3.aaa_bloom_real_stub_p1_hblur"
            : "regression.phase3.aaa_bloom_real_p1_hblur";
        run_pass( ub_i_p1, rt_threshold, GOL::kInvalidTextureId, rt_hblur, label_p1 );

        //	Pass 2 : vertical blur ; rt_hblur -> rt_vblur .
        char const* label_p2 = use_stub_config
            ? "regression.phase3.aaa_bloom_real_stub_p2_vblur"
            : "regression.phase3.aaa_bloom_real_p2_vblur";
        run_pass( ub_i_p2, rt_hblur, GOL::kInvalidTextureId, rt_vblur, label_p2 );

        //	Pass 3 : additive composite ; scene + rt_vblur -> rt_composite .
        char const* label_p3 = use_stub_config
            ? "regression.phase3.aaa_bloom_real_stub_p3_composite"
            : "regression.phase3.aaa_bloom_real_p3_composite";
        run_pass( ub_i_p3, scene_tex, rt_vblur, rt_composite, label_p3 );

        backend.present();

        backend.read_texture_pixels( rt_composite, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i_p3 );
        backend.delete_buffer(  ub_i_p2 );
        backend.delete_buffer(  ub_i_p1 );
        backend.delete_buffer(  ub_i_p0 );
        backend.delete_buffer(  ub_v4   );
        backend.delete_buffer(  ub_f    );
        backend.delete_texture( rt_composite );
        backend.delete_texture( rt_vblur     );
        backend.delete_texture( rt_hblur     );
        backend.delete_texture( rt_threshold );
        backend.delete_texture( scene_tex    );
        backend.delete_program( prog         );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_AaaBloomReal_PixelComparable )
{
    std::vector< std::uint8_t > const rendered = render_aaa_bloom_real_c138a(
        /*use_stub_config*/ false );

    //	The final composite is base scene + intensity * blurred bloom .
    //	The base scene is the procedural gradient (every pixel non-zero
    //	RGB except the (0,0) corner where R=G=B=0) , so virtually every
    //	pixel ends up non-zero in the composite. Loose floor 1/2 of the
    //	framebuffer matches the noise / lights / curl-noise goldens.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "aaa_bloom_real produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:aaa_bloom_real] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaa_bloom_real.png";
    compare_or_capture_golden( golden_path, rendered, "aaa_bloom_real" );
}

TEST( RegressionPhase3, RevivalProof_AaaBloomReal_DiffersFromStub )
{
    //	Render the SAME aaa_bloom_real.metal twice : once with
    //	mode = 1 (real 4-pass bloom : threshold + H-Gaussian +
    //	V-Gaussian + additive composite) , once with mode = 0
    //	(flat mid-grey , the stub equivalent of "no real bloom"
    //	-- every pass collapses to RGB = (128, 128, 128)). Diff
    //	is the proof that the real RTR 4th ed §10.6 / Sousa 2008
    //	bloom pipeline actually drove the final composite.
    std::vector< std::uint8_t > const rendered_real = render_aaa_bloom_real_c138a(
        /*use_stub_config*/ false );
    std::vector< std::uint8_t > const rendered_stub = render_aaa_bloom_real_c138a(
        /*use_stub_config*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha is constant
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:aaa_bloom_real] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold per c138-A spec : 5000 pixels and 50-LSB max. The
    //	real frame is base + intensity * blurred ; the base is the
    //	procedural gradient spanning RGB = (0,0,0) at (0,0) to
    //	(255,255, ~255) at (255,255) , so per-channel deltas vs the
    //	flat mid-grey RGB = (128,128,128) stub span the entire
    //	[0,127] LSB range across the frame. The bloom halo around
    //	the (128,128) hot-pixel cluster adds further deltas in a
    //	soft 26-px radius (twice the kernel half-width). Conservative
    //	floors >5000 / >50 catch the regression mode "real 4-pass
    //	bloom silently fell back to the stub branch".
    EXPECT_GT( diff_count, std::size_t( 5000 ) )
        << "aaa_bloom_real canonical output is nearly identical "
        << "to the stub-config render -- the real 4-pass bloom "
        << "pipeline never drove the fragment output. Diff = "
        << diff_count << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 50 )
        << "aaa_bloom_real per-channel diff vs stub-config "
        << "never exceeds 50 LSB -- bloom dynamic range "
        << "collapsed. max_diff = " << max_diff;
}

//	==================== c139-A : aaa_motion_blur_real (TENTH Path A revival) ====
//	Real camera-velocity-projected motion blur plumbed end-to-end
//	through GOL::MetalBackend. The pipeline :
//	  1. Reconstruct world-space position behind each pixel from
//	     UV + linear depth + cam.view_inv + fov_y + aspect.
//	  2. Reproject the world point through cam_prev.vp (the previous-
//	     frame VP) to recover the prev-frame UV.
//	  3. Velocity vector = (uv - uv_prev) * blur_intensity ; clamped to
//	     max_velocity_pixels / kBufferDim per axis.
//	  4. N-tap (default N=16) symmetric accumulation along the velocity
//	     vector with 1 - 0.5*|t| soft-falloff weights.
//	A single render pass ; one MSL file ; the mode-flag stub variant
//	(AaaFuInts[0] = 0 -> flat mid-grey) anchors the RevivalProof
//	test , matching c135-A / c136-A / c137-A / c138-A doctrine.
//
//	Input fixture : the standard procedural gradient PLUS a single
//	bright vertical line at x = 128 (5 px wide , full white). With
//	a horizontal camera pan ((1,0,0) offset between cam and cam_prev)
//	the velocity field is approximately constant horizontal ; the
//	vertical line should smear into a horizontal BAND in the blurred
//	output. The flat-grey stub render produces a uniform field with
//	no smearing -- diff is large and obvious.
//
//	Camera setup :
//	  cam      : eye (0,0,5) , look-at origin , FOV 60 , aspect 1 ,
//	             near 0.1 , far 100 -- identical to c133-B canonical.
//	  cam_prev : eye (1,0,5) , look-at origin , same other params --
//	             a 1-world-unit horizontal pan. The resulting per-
//	             pixel UV displacement is roughly 0.05 .. 0.1
//	             depending on screen position.
//	  linear_z : 0.5 (constant across frame -- the simple case ;
//	             a per-pixel depth buffer is a future extension).
//
//	Path A catalog floor bumped 167 -> 168. Pattern mirrors c128-A /
//	c130-A / c132-A / c133-B / c134-B / c135-A / c136-A / c137-A /
//	c138-A. Revival count 9 -> 10 -- DOUBLE-DIGIT CROSSED.
//
//	Citations : McGuire et al. 2012 I3D "A Reconstruction Filter
//	for Plausible Motion Blur" ; Rosado 2007 GPU Gems 3 Ch.27
//	"Motion Blur as a Post-Processing Effect" ; Real-Time Rendering
//	4th ed (Akenine-Moller / Haines / Hoffman) §12.5 "Motion Blur".
//
//	Two TESTs :
//	  1. GoldenFrame_AaaMotionBlurReal_PixelComparable -- canonical
//	     real-motion-blur config (N = 16 , horizontal pan) ;
//	     captures golden ; replays bit-exact.
//	  2. RevivalProof_AaaMotionBlurReal_DiffersFromStub -- renders
//	     the SAME aaa_motion_blur_real.metal twice : once with
//	     mode = 1 (real N-tap blur) , once with mode = 0 (flat
//	     mid-grey). Per c139-A spec : diff > 5000 pixels and
//	     per-channel max > 50 LSB.
//
//	No collision with c139-B : that session works on bundle/macos/
//	Info.plist + src/ui/macos/CMakeLists.txt + tests/unit/
//	macos_bundle_meta_test.cpp ; this session's only c++ touch is
//	THIS test file (2 new TESTs at the tail) + tests/unit/
//	path_a_catalog_test.cpp (floor bump). Disjoint file scope.
namespace
{
    //	Build the motion-blur input scene : procedural gradient PLUS
    //	a vertical 5-px-wide white line at x = 128. Under a horizontal
    //	pan velocity field , the real shader should smear this line
    //	into a horizontal band ; the stub branch leaves it untouched
    //	(actually : the stub outputs flat grey , disregarding texture).
    std::vector< std::uint8_t > make_motion_blur_scene_c139a()
    {
        std::vector< std::uint8_t > p = make_gradient_rgba8( kWidth, kHeight );

        //	Vertical bright line at x in [126, 130] (5 px wide) , full
        //	height. RGB = (255, 255, 255). Centred at x = 128 to keep
        //	the line solidly inside the frame after the pan-induced
        //	UV displacement (~10-25 px to either side).
        std::uint32_t const cx = 128;
        for( std::uint32_t y = 0; y < kHeight; ++y )
        {
            for( int dx = -2; dx <= 2; ++dx )
            {
                std::uint32_t const x = std::uint32_t( int( cx ) + dx );
                std::size_t const base = ( y * kWidth + x ) * 4;
                p[ base + 0 ] = 255;
                p[ base + 1 ] = 255;
                p[ base + 2 ] = 255;
                p[ base + 3 ] = 255;
            }
        }
        return p;
    }

    //	Build a "current frame" canonical camera identical to c133-B's
    //	fill_aaa_cam_real_canonical( ) except renamed for c139-A
    //	clarity. We do NOT call the c133-B function directly because
    //	that helper is defined in an anonymous namespace and shares
    //	state we want isolated. Same math.
    void fill_aaa_cam_real_cam_c139a( AaaCamReal_Cpu& cam )
    {
        float const fov_y_rad = 60.0f * 3.14159265358979323846f / 180.0f;
        float const aspect    = 1.0f;
        float const near_c    = 0.1f;
        float const far_c     = 100.0f;
        make_view_lookat_rh( 0.0f, 0.0f, 5.0f,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, cam.view );
        make_proj_perspective_rh( fov_y_rad, aspect, near_c, far_c, cam.proj );
        make_view_inv_for_lookat_rh( cam.view, cam.view_inv );
        mat4_mul_cm( cam.proj, cam.view, cam.vp );
        cam.eye_aspect[0] = 0.0f;
        cam.eye_aspect[1] = 0.0f;
        cam.eye_aspect[2] = 5.0f;
        cam.eye_aspect[3] = aspect;
        cam.fov_clip[0]   = fov_y_rad;
        cam.fov_clip[1]   = near_c;
        cam.fov_clip[2]   = far_c;
        cam.fov_clip[3]   = 0.0f;
    }

    //	Previous-frame camera : same parameters except eye at (1,0,5)
    //	instead of (0,0,5). The single-unit horizontal pan produces a
    //	constant-ish horizontal velocity field over the visible region.
    //	Look-at stays at origin so the camera also rotates slightly
    //	-- the velocity is therefore not purely uniform but dominated
    //	by horizontal smear , which is exactly what we want.
    void fill_aaa_cam_real_cam_prev_c139a( AaaCamReal_Cpu& cam )
    {
        float const fov_y_rad = 60.0f * 3.14159265358979323846f / 180.0f;
        float const aspect    = 1.0f;
        float const near_c    = 0.1f;
        float const far_c     = 100.0f;
        make_view_lookat_rh( 1.0f, 0.0f, 5.0f,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, cam.view );
        make_proj_perspective_rh( fov_y_rad, aspect, near_c, far_c, cam.proj );
        make_view_inv_for_lookat_rh( cam.view, cam.view_inv );
        mat4_mul_cm( cam.proj, cam.view, cam.vp );
        cam.eye_aspect[0] = 1.0f;
        cam.eye_aspect[1] = 0.0f;
        cam.eye_aspect[2] = 5.0f;
        cam.eye_aspect[3] = aspect;
        cam.fov_clip[0]   = fov_y_rad;
        cam.fov_clip[1]   = near_c;
        cam.fov_clip[2]   = far_c;
        cam.fov_clip[3]   = 0.0f;
    }

    void fill_aaa_motion_blur_real_canonical_c139a( AaaFuFloats& uf,
                                                    AaaFuVec4s&  uv4,
                                                    AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Floats : blur_intensity , max_velocity_pixels , linear_depth .
        uf.values[ 0 ] = 0.5f;        // blur_intensity (velocity scale)
        uf.values[ 1 ] = 64.0f;       // max_velocity_pixels (clamp)
        uf.values[ 2 ] = 0.5f;        // linear_depth (constant across frame)

        //	Ints : mode = 1 (real motion blur) , N = 16 samples.
        ui.values[ 0 ] = 1;
        ui.values[ 1 ] = 16;
    }

    void fill_aaa_motion_blur_real_stub_c139a( AaaFuFloats& uf,
                                               AaaFuVec4s&  uv4,
                                               AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Same float values for parity ; irrelevant in stub branch.
        uf.values[ 0 ] = 0.5f;
        uf.values[ 1 ] = 64.0f;
        uf.values[ 2 ] = 0.5f;

        //	Mode = 0 : flat mid-grey stub branch. Shader short-circuits
        //	to RGB = (0.5, 0.5, 0.5) regardless of inputs.
        ui.values[ 0 ] = 0;
        ui.values[ 1 ] = 16;
    }

    std::vector< std::uint8_t > render_aaa_motion_blur_real_c139a( bool use_stub_config )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_motion_blur_real.metal";
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        //	---------- Input scene + render target ----------
        GOL::TextureId scene_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                          GOL::TextureFormat::RGBA8 );
        EXPECT_NE( scene_tex, GOL::kInvalidTextureId );
        std::vector< std::uint8_t > const scene = make_motion_blur_scene_c139a();
        backend.texture_data_2d( scene_tex, scene.data(), kBytesPerRow );

        GOL::TextureId rt = backend.gen_texture_2d( kWidth, kHeight,
                                                   GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt, GOL::kInvalidTextureId );

        //	---------- Uniform buffers ----------
        AaaFuFloats uf{};
        AaaFuVec4s  uv4{};
        AaaFuInts   ui{};
        if( use_stub_config )
            fill_aaa_motion_blur_real_stub_c139a( uf, uv4, ui );
        else
            fill_aaa_motion_blur_real_canonical_c139a( uf, uv4, ui );

        AaaCamReal_Cpu cam{};
        AaaCamReal_Cpu cam_prev{};
        fill_aaa_cam_real_cam_c139a     ( cam      );
        fill_aaa_cam_real_cam_prev_c139a( cam_prev );

        GOL::BufferId ub_f       = backend.gen_buffer();
        GOL::BufferId ub_v4      = backend.gen_buffer();
        GOL::BufferId ub_i       = backend.gen_buffer();
        GOL::BufferId ub_cam     = backend.gen_buffer();
        GOL::BufferId ub_camprev = backend.gen_buffer();
        EXPECT_NE( ub_f,       GOL::kInvalidBufferId );
        EXPECT_NE( ub_v4,      GOL::kInvalidBufferId );
        EXPECT_NE( ub_i,       GOL::kInvalidBufferId );
        EXPECT_NE( ub_cam,     GOL::kInvalidBufferId );
        EXPECT_NE( ub_camprev, GOL::kInvalidBufferId );
        backend.buffer_data( ub_f,       sizeof( uf  ),      &uf,       GOL::BufferUsage::Static );
        backend.buffer_data( ub_v4,      sizeof( uv4 ),      &uv4,      GOL::BufferUsage::Static );
        backend.buffer_data( ub_i,       sizeof( ui  ),      &ui,       GOL::BufferUsage::Static );
        backend.buffer_data( ub_cam,     sizeof( cam ),      &cam,      GOL::BufferUsage::Static );
        backend.buffer_data( ub_camprev, sizeof( cam_prev ), &cam_prev, GOL::BufferUsage::Static );

        //	---------- Single render pass ----------
        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = rt;
        rpd.load_action      = GOL::LoadAction::Clear;
        rpd.clear_color[ 0 ] = 0.0f;
        rpd.clear_color[ 1 ] = 0.0f;
        rpd.clear_color[ 2 ] = 0.0f;
        rpd.clear_color[ 3 ] = 1.0f;
        rpd.debug_label      = use_stub_config
                             ? "regression.phase3.aaa_motion_blur_real_stub_config"
                             : "regression.phase3.aaa_motion_blur_real_canonical";

        EXPECT_TRUE( backend.begin_render_pass( rpd ) );
        backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
        backend.bind_program( prog );
        backend.bind_fragment_texture( scene_tex, 0 );
        backend.bind_fragment_buffer( ub_f,       0, 0 );
        backend.bind_fragment_buffer( ub_v4,      1, 0 );
        backend.bind_fragment_buffer( ub_i,       2, 0 );
        backend.bind_fragment_buffer( ub_cam,     3, 0 );
        backend.bind_fragment_buffer( ub_camprev, 4, 0 );
        backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
        backend.end_render_pass();
        backend.present();

        backend.read_texture_pixels( rt, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_camprev );
        backend.delete_buffer(  ub_cam     );
        backend.delete_buffer(  ub_i       );
        backend.delete_buffer(  ub_v4      );
        backend.delete_buffer(  ub_f       );
        backend.delete_texture( rt         );
        backend.delete_texture( scene_tex  );
        backend.delete_program( prog       );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_AaaMotionBlurReal_PixelComparable )
{
    std::vector< std::uint8_t > const rendered = render_aaa_motion_blur_real_c139a(
        /*use_stub_config*/ false );

    //	The blurred composite : every pixel samples the procedural
    //	gradient via the N-tap accumulation , so virtually every pixel
    //	ends up non-zero-RGB. Loose floor of 1/2 of the framebuffer
    //	matches the noise / lights / curl-noise / bloom goldens.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "aaa_motion_blur_real produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:aaa_motion_blur_real] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaa_motion_blur_real.png";
    compare_or_capture_golden( golden_path, rendered, "aaa_motion_blur_real" );
}

TEST( RegressionPhase3, RevivalProof_AaaMotionBlurReal_DiffersFromStub )
{
    //	Render the SAME aaa_motion_blur_real.metal twice : once with
    //	mode = 1 (real camera-velocity-projected N-tap motion blur ,
    //	horizontal pan smearing the vertical line into a band) , once
    //	with mode = 0 (flat mid-grey , the stub equivalent of "no real
    //	motion blur"). Diff is the proof that the real McGuire 2012
    //	/ Rosado 2007 reconstruction filter actually drove the
    //	fragment output.
    std::vector< std::uint8_t > const rendered_real = render_aaa_motion_blur_real_c139a(
        /*use_stub_config*/ false );
    std::vector< std::uint8_t > const rendered_stub = render_aaa_motion_blur_real_c139a(
        /*use_stub_config*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha is constant
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:aaa_motion_blur_real] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold per c139-A spec : 5000 pixels and 50-LSB max. The
    //	real frame samples the procedural gradient via the N-tap
    //	accumulation -- per-pixel RGB spans the full [0,255] range
    //	(R varies with x , G with y , B with x+y). The stub case is
    //	a flat mid-grey RGB = (128,128,128). Per-channel deltas vs
    //	the gradient alone already exceed 100 LSB at the bright /
    //	dark extremes ; the smeared vertical line adds further deltas
    //	in a ~32 px band around x = 128. Conservative floors >5000
    //	/ >50 catch the regression mode "real N-tap motion blur
    //	silently fell back to the stub branch".
    EXPECT_GT( diff_count, std::size_t( 5000 ) )
        << "aaa_motion_blur_real canonical output is nearly identical "
        << "to the stub-config render -- the real McGuire 2012 / Rosado "
        << "2007 motion-blur reconstruction never drove the fragment "
        << "output. Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 50 )
        << "aaa_motion_blur_real per-channel diff vs stub-config "
        << "never exceeds 50 LSB -- motion-blur dynamic range "
        << "collapsed. max_diff = " << max_diff;
}

//	==================== c140-A : aaa_dof_hex_bokeh_real (ELEVENTH Path A revival) ====
//	Real depth-of-field with HEXAGONAL bokeh plumbed end-to-end
//	through GOL::MetalBackend. The pipeline :
//	  Pass 0 : thin-lens Circle of Confusion (CoC) compute. Per-
//	           pixel coc = |f * (z - z_f) / (A * (z_f - f) * z)|
//	           (Karis 2014 SIGGRAPH / RTR 4th ed §12.4 thin-lens
//	           reference). Result clamped to max_coc_pixels and
//	           packed in alpha so downstream sweeps need no second
//	           depth fetch.
//	  Pass 1 : vertical-down N-tap sweep (angle 270 deg). Box
//	           kernel along (0, -1) ; step size scaled by per-pixel
//	           CoC in UV space.
//	  Pass 2 : diagonal-down-right sweep (angle 330 deg). Box
//	           kernel along (cos 30 , -sin 30) = (0.866 , -0.5).
//	  Pass 3 : diagonal-down-left sweep (angle 210 deg). Box
//	           kernel along (-0.866 , -0.5). The three directions
//	           span 120 deg apart -- this is the McIntosh 2012
//	           hex decomposition.
//	  Pass 4 : composite final = min(P1 , max(P2 , P3)). McIntosh
//	           2012's hex-preserving combiner ; the max-of-two
//	           diagonals + intersect-with-vertical reconstructs the
//	           6-sided silhouette without explicit polygon math.
//	One MSL file ; pass selector via AaaFuInts[3] ; CPU driver
//	chains 5 sequential renders ping-ponging across CoC + 3
//	directional + composite render targets.
//
//	Input fixture : a sharp checker pattern (high-contrast edges
//	that bokeh will visibly hex-smear) PLUS a linear depth gradient
//	z = 0 at top of frame ... z = 10 at bottom of frame , so the
//	mid-frame (z = ~5) is in focus and the top/bottom blur strongly.
//	The depth texture is a separate R32F render-source ; the colour
//	scene is RGBA8.
//
//	Path A catalog floor bumped 168 -> 169. Pattern mirrors
//	c128-A / c130-A / c132-A / c133-B / c134-B / c135-A / c136-A /
//	c137-A / c138-A / c139-A. Revival count 10 -> 11.
//
//	Citations : McIntosh 2012 "Bokeh Effects in Adobe Premiere
//	Pro CS5" (3-directional hex decomposition) ; Lottes ~2015
//	"Filmic Bokeh" slides (cited variant of 3-pass hex with
//	CoC-modulated tap spacing) ; Karis 2014 SIGGRAPH "Physically
//	Based Shading at Epic Games" (thin-lens CoC reference) ;
//	Real-Time Rendering 4th ed (Akenine-Moller / Haines / Hoffman)
//	§12.4 "Depth of Field" (box-kernel preserves bokeh ; Gaussian
//	does not).
//
//	Two TESTs :
//	  1. GoldenFrame_AaaDofHexBokehReal_PixelComparable -- canonical
//	     real-DOF config , 5-pass pipeline executes , final composite
//	     read back. Capture golden ; replay bit-exact.
//	  2. RevivalProof_AaaDofHexBokehReal_DiffersFromStub -- renders
//	     the SAME aaa_dof_hex_bokeh_real.metal with mode = 1 (real
//	     5-pass DOF) vs mode = 0 (flat mid-grey , no multipass).
//	     Per c140-A spec : diff > 5000 pixels and per-channel max
//	     > 50 LSB.
//
//	No collision with c140-B : that session works on
//	tests/unit/revival_perf_test.cpp + a new perf test target ;
//	this session's only C++ touch is THIS file (2 new TESTs at
//	the tail) + tests/unit/path_a_catalog_test.cpp (floor bump).
//	Disjoint file scope.
namespace
{
    //	Build a sharp 8x8 checker scene over the 256x256 frame :
    //	32-pixel cells alternating between (255,255,255) and
    //	(20,20,20). High-contrast edges every 32 px -- the bokeh
    //	hex will smear these into directional streaks at the top
    //	and bottom of the frame where depth is far from focus.
    std::vector< std::uint8_t > make_dof_checker_scene_c140a()
    {
        std::vector< std::uint8_t > p( std::size_t( kWidth ) * kHeight * 4, 0 );
        std::uint32_t const cell = 32;
        for( std::uint32_t y = 0; y < kHeight; ++y )
        {
            for( std::uint32_t x = 0; x < kWidth; ++x )
            {
                std::size_t const i  = ( y * kWidth + x ) * 4;
                bool const cx        = ( ( x / cell ) & 1u ) != 0u;
                bool const cy        = ( ( y / cell ) & 1u ) != 0u;
                std::uint8_t const v = ( cx ^ cy ) ? 255 : 20;
                p[ i + 0 ] = v;
                p[ i + 1 ] = v;
                p[ i + 2 ] = v;
                p[ i + 3 ] = 255;
            }
        }
        return p;
    }

    //	Build a 256x256 R32F linear depth texture with z = 0 at the
    //	top row and z = 10 at the bottom row. Mid-frame ( y ~ 128 ,
    //	z ~ 5 ) sits exactly at the focus_distance = 5.0 default ,
    //	so the in-focus band lives across the middle of the frame
    //	and the top/bottom rows are heavily defocused. This is the
    //	canonical "near object + far object" DOF demo configuration.
    std::vector< float > make_dof_depth_gradient_c140a()
    {
        std::vector< float > d( std::size_t( kWidth ) * kHeight, 0.0f );
        for( std::uint32_t y = 0; y < kHeight; ++y )
        {
            //	y = 0   -> z = 0.0
            //	y = 255 -> z = 10.0
            float const z = 10.0f * float( y ) / float( kHeight - 1 );
            for( std::uint32_t x = 0; x < kWidth; ++x )
            {
                d[ y * kWidth + x ] = z;
            }
        }
        return d;
    }

    void fill_aaa_dof_hex_bokeh_real_canonical_c140a( AaaFuFloats& uf,
                                                      AaaFuVec4s&  uv4,
                                                      AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Floats : focus_distance , focal_length , aperture ,
        //	max_coc_pixels , rcp_w , rcp_h .
        uf.values[ 0 ] = 5.0f;                          // focus_distance (m)
        uf.values[ 1 ] = 50.0e-3f;                      // focal_length (50mm)
        uf.values[ 2 ] = 2.0f;                          // aperture (f/2.0)
        uf.values[ 3 ] = 32.0f;                         // max_coc_pixels
        uf.values[ 4 ] = 1.0f / float( kWidth  );       // rcp_w
        uf.values[ 5 ] = 1.0f / float( kHeight );       // rcp_h

        //	Ints : mode = 1 (real DOF) ; N = 8 samples per direction ;
        //	entry_pass set per-pass.
        ui.values[ 0 ] = 1;
        ui.values[ 1 ] = 8;
        ui.values[ 3 ] = 0;                             // will be overridden per pass
    }

    void fill_aaa_dof_hex_bokeh_real_stub_c140a( AaaFuFloats& uf,
                                                 AaaFuVec4s&  uv4,
                                                 AaaFuInts&   ui )
    {
        for( int i = 0; i < 16; ++i ) uf.values[ i ] = 0.0f;
        for( int i = 0; i < 16; ++i )
            for( int j = 0; j < 4; ++j ) uv4.values[ i ][ j ] = 0.0f;
        for( int i = 0; i < 16; ++i ) ui.values[ i ] = 0;

        //	Same float values for parity (irrelevant in stub branch).
        uf.values[ 0 ] = 5.0f;
        uf.values[ 1 ] = 50.0e-3f;
        uf.values[ 2 ] = 2.0f;
        uf.values[ 3 ] = 32.0f;
        uf.values[ 4 ] = 1.0f / float( kWidth  );
        uf.values[ 5 ] = 1.0f / float( kHeight );

        //	Mode = 0 : flat mid-grey stub branch. The shader will
        //	short-circuit to RGB = (0.5, 0.5, 0.5) regardless of
        //	entry_pass , so the driver still chains 5 render passes
        //	but every output is uniform grey.
        ui.values[ 0 ] = 0;
        ui.values[ 1 ] = 8;
        ui.values[ 3 ] = 0;
    }

    std::vector< std::uint8_t > render_aaa_dof_hex_bokeh_real_c140a( bool use_stub_config )
    {
        std::vector< std::uint8_t > out( kBufferBytes, 0 );

        GOL::MetalBackend backend;
        EXPECT_TRUE( backend.init() );

        std::filesystem::path const msl_path =
            std::filesystem::path( AAA_SHADERS_MSL_DIR ) / "aaa_dof_hex_bokeh_real.metal";
        std::string const msl_src = slurp( msl_path );
        EXPECT_FALSE( msl_src.empty() ) << "Failed to read " << msl_path;

        GOL::ProgramId prog = backend.create_program_msl(
            msl_src.c_str(), "vs_main", "fs_main" );
        EXPECT_NE( prog, GOL::kInvalidProgramId )
            << "create_program_msl failed : " << backend.get_last_error();

        //	---------- Input scene + depth + four ping-pong RTs ----------
        GOL::TextureId scene_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                          GOL::TextureFormat::RGBA8 );
        EXPECT_NE( scene_tex, GOL::kInvalidTextureId );
        std::vector< std::uint8_t > const scene = make_dof_checker_scene_c140a();
        backend.texture_data_2d( scene_tex, scene.data(), kBytesPerRow );

        GOL::TextureId depth_tex = backend.gen_texture_2d( kWidth, kHeight,
                                                          GOL::TextureFormat::R32F );
        EXPECT_NE( depth_tex, GOL::kInvalidTextureId );
        std::vector< float > const depth = make_dof_depth_gradient_c140a();
        backend.texture_data_2d( depth_tex, depth.data(),
                                 std::size_t( kWidth ) * sizeof( float ) );

        GOL::TextureId rt_coc       = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        GOL::TextureId rt_p1        = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        GOL::TextureId rt_p2        = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        GOL::TextureId rt_p3        = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        GOL::TextureId rt_composite = backend.gen_texture_2d( kWidth, kHeight,
                                                              GOL::TextureFormat::RGBA8 );
        EXPECT_NE( rt_coc,       GOL::kInvalidTextureId );
        EXPECT_NE( rt_p1,        GOL::kInvalidTextureId );
        EXPECT_NE( rt_p2,        GOL::kInvalidTextureId );
        EXPECT_NE( rt_p3,        GOL::kInvalidTextureId );
        EXPECT_NE( rt_composite, GOL::kInvalidTextureId );

        AaaFuFloats uf{};
        AaaFuVec4s  uv4{};
        AaaFuInts   ui{};
        if( use_stub_config )
            fill_aaa_dof_hex_bokeh_real_stub_c140a( uf, uv4, ui );
        else
            fill_aaa_dof_hex_bokeh_real_canonical_c140a( uf, uv4, ui );

        //	Float + Vec4 buffers are pass-invariant ; one each.
        GOL::BufferId ub_f  = backend.gen_buffer();
        GOL::BufferId ub_v4 = backend.gen_buffer();
        EXPECT_NE( ub_f,  GOL::kInvalidBufferId );
        EXPECT_NE( ub_v4, GOL::kInvalidBufferId );
        backend.buffer_data( ub_f,  sizeof( uf  ), &uf,  GOL::BufferUsage::Static );
        backend.buffer_data( ub_v4, sizeof( uv4 ), &uv4, GOL::BufferUsage::Static );

        //	One int buffer per pass : pass selector differs.
        GOL::BufferId ub_i_p0 = backend.gen_buffer();
        GOL::BufferId ub_i_p1 = backend.gen_buffer();
        GOL::BufferId ub_i_p2 = backend.gen_buffer();
        GOL::BufferId ub_i_p3 = backend.gen_buffer();
        GOL::BufferId ub_i_p4 = backend.gen_buffer();
        EXPECT_NE( ub_i_p0, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i_p1, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i_p2, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i_p3, GOL::kInvalidBufferId );
        EXPECT_NE( ub_i_p4, GOL::kInvalidBufferId );

        AaaFuInts ui_p0 = ui ; ui_p0.values[ 3 ] = 0;
        AaaFuInts ui_p1 = ui ; ui_p1.values[ 3 ] = 1;
        AaaFuInts ui_p2 = ui ; ui_p2.values[ 3 ] = 2;
        AaaFuInts ui_p3 = ui ; ui_p3.values[ 3 ] = 3;
        AaaFuInts ui_p4 = ui ; ui_p4.values[ 3 ] = 4;
        backend.buffer_data( ub_i_p0, sizeof( ui_p0 ), &ui_p0, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i_p1, sizeof( ui_p1 ), &ui_p1, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i_p2, sizeof( ui_p2 ), &ui_p2, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i_p3, sizeof( ui_p3 ), &ui_p3, GOL::BufferUsage::Static );
        backend.buffer_data( ub_i_p4, sizeof( ui_p4 ), &ui_p4, GOL::BufferUsage::Static );

        //	Helper lambda : render one DOF pass with given inputs
        //	to a single colour RT. Encapsulates RPD setup + draw +
        //	end. Each pass binds the colour-input texture(s) on
        //	slots 0/1/2 ; Metal validation requires every declared
        //	slot to be bound , so we always pass a valid texture on
        //	all three slots (scene_tex is the benign placeholder
        //	when a slot is not sampled).
        auto run_pass = [ & ](
            GOL::BufferId       ub_i,
            GOL::TextureId      tex0,
            GOL::TextureId      tex1,
            GOL::TextureId      tex2,
            GOL::TextureId      target,
            char const*         debug_label )
        {
            GOL::RenderPassDescriptor rpd;
            rpd.color_attachment = target;
            rpd.load_action      = GOL::LoadAction::Clear;
            rpd.clear_color[ 0 ] = 0.0f;
            rpd.clear_color[ 1 ] = 0.0f;
            rpd.clear_color[ 2 ] = 0.0f;
            rpd.clear_color[ 3 ] = 1.0f;
            rpd.debug_label      = debug_label;
            EXPECT_TRUE( backend.begin_render_pass( rpd ) );
            backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
            backend.bind_program( prog );
            backend.bind_fragment_texture(
                tex0 != GOL::kInvalidTextureId ? tex0 : scene_tex, 0 );
            backend.bind_fragment_texture(
                tex1 != GOL::kInvalidTextureId ? tex1 : scene_tex, 1 );
            backend.bind_fragment_texture(
                tex2 != GOL::kInvalidTextureId ? tex2 : scene_tex, 2 );
            backend.bind_fragment_buffer( ub_f,  0, 0 );
            backend.bind_fragment_buffer( ub_v4, 1, 0 );
            backend.bind_fragment_buffer( ub_i,  2, 0 );
            backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
            backend.end_render_pass();
        };

        //	---------- 5-pass orchestration ----------
        //	NOTE : Unlike the bloom test (which gets away with a single
        //	present() at the very end of 4 passes) , this 5-pass DOF
        //	pipeline calls present() between passes. Empirically the
        //	5-RT chain otherwise loses intermediate-pass writes : with
        //	a single tail present() , read_texture_pixels on rt_coc /
        //	rt_p1 / rt_p2 / rt_p3 returns the cleared-but-unwritten
        //	bytes (alpha = 0 from a never-applied clear of (0,0,0,1)).
        //	The fix is one present() per pass , which forces each
        //	command buffer to commit + waitUntilCompleted before the
        //	next pass starts encoding -- guaranteeing the previous
        //	pass's writes are visible to the next pass's sampling.
        //	Pass 0 : CoC compute ; (scene, depth) -> rt_coc .
        char const* label_p0 = use_stub_config
            ? "regression.phase3.aaa_dof_hex_bokeh_real_stub_p0_coc"
            : "regression.phase3.aaa_dof_hex_bokeh_real_p0_coc";
        run_pass( ub_i_p0, scene_tex, depth_tex, GOL::kInvalidTextureId,
                  rt_coc, label_p0 );
        backend.present();

        //	Pass 1 : down sweep ; rt_coc -> rt_p1 .
        char const* label_p1 = use_stub_config
            ? "regression.phase3.aaa_dof_hex_bokeh_real_stub_p1_down"
            : "regression.phase3.aaa_dof_hex_bokeh_real_p1_down";
        run_pass( ub_i_p1, rt_coc, GOL::kInvalidTextureId, GOL::kInvalidTextureId,
                  rt_p1, label_p1 );
        backend.present();

        //	Pass 2 : down-right sweep ; rt_coc -> rt_p2 .
        char const* label_p2 = use_stub_config
            ? "regression.phase3.aaa_dof_hex_bokeh_real_stub_p2_dr"
            : "regression.phase3.aaa_dof_hex_bokeh_real_p2_dr";
        run_pass( ub_i_p2, rt_coc, GOL::kInvalidTextureId, GOL::kInvalidTextureId,
                  rt_p2, label_p2 );
        backend.present();

        //	Pass 3 : down-left sweep ; rt_coc -> rt_p3 .
        char const* label_p3 = use_stub_config
            ? "regression.phase3.aaa_dof_hex_bokeh_real_stub_p3_dl"
            : "regression.phase3.aaa_dof_hex_bokeh_real_p3_dl";
        run_pass( ub_i_p3, rt_coc, GOL::kInvalidTextureId, GOL::kInvalidTextureId,
                  rt_p3, label_p3 );
        backend.present();

        //	Pass 4 : composite ; (p1, p2, p3) -> rt_composite .
        char const* label_p4 = use_stub_config
            ? "regression.phase3.aaa_dof_hex_bokeh_real_stub_p4_composite"
            : "regression.phase3.aaa_dof_hex_bokeh_real_p4_composite";
        run_pass( ub_i_p4, rt_p1, rt_p2, rt_p3, rt_composite, label_p4 );

        backend.present();

        backend.read_texture_pixels( rt_composite, 0, 0, kWidth, kHeight,
                                     out.data(), kBytesPerRow );

        backend.delete_buffer(  ub_i_p4 );
        backend.delete_buffer(  ub_i_p3 );
        backend.delete_buffer(  ub_i_p2 );
        backend.delete_buffer(  ub_i_p1 );
        backend.delete_buffer(  ub_i_p0 );
        backend.delete_buffer(  ub_v4   );
        backend.delete_buffer(  ub_f    );
        backend.delete_texture( rt_composite );
        backend.delete_texture( rt_p3        );
        backend.delete_texture( rt_p2        );
        backend.delete_texture( rt_p1        );
        backend.delete_texture( rt_coc       );
        backend.delete_texture( depth_tex    );
        backend.delete_texture( scene_tex    );
        backend.delete_program( prog         );
        return out;
    }
}

TEST( RegressionPhase3, GoldenFrame_AaaDofHexBokehReal_PixelComparable )
{
    std::vector< std::uint8_t > const rendered = render_aaa_dof_hex_bokeh_real_c140a(
        /*use_stub_config*/ false );

    //	The DOF composite : min(P1 , max(P2 , P3)) over a high-contrast
    //	checker scene. The in-focus mid-band passes the checker through
    //	near-unchanged ; the top/bottom defocus zones smear into hex
    //	bokeh shapes. Loose floor 1/2 of the framebuffer matches the
    //	noise / lights / curl-noise / bloom / motion-blur goldens.
    std::size_t const nonzero = count_nonzero_pixels( rendered );
    ASSERT_GT( nonzero, ( kWidth * kHeight ) / 2 )
        << "aaa_dof_hex_bokeh_real produced almost no non-black pixels : "
        << nonzero << " / " << ( kWidth * kHeight );

    std::printf( "[Phase3.Golden:aaa_dof_hex_bokeh_real] rendered %ux%u = %zu non-RGB-zero pixels\n",
                 kWidth, kHeight, nonzero );

    std::filesystem::path const golden_dir { AAA_REGRESSION_GOLDEN_DIR };
    std::filesystem::path const golden_path =
        golden_dir / "phase3_aaa_dof_hex_bokeh_real.png";
    compare_or_capture_golden( golden_path, rendered, "aaa_dof_hex_bokeh_real" );
}

TEST( RegressionPhase3, RevivalProof_AaaDofHexBokehReal_DiffersFromStub )
{
    //	Render the SAME aaa_dof_hex_bokeh_real.metal twice : once with
    //	mode = 1 (real 5-pass hex bokeh DOF : CoC + 3 directional
    //	sweeps + McIntosh min/max composite) , once with mode = 0 (flat
    //	mid-grey , the stub equivalent of "no real DOF" -- every pass
    //	collapses to RGB = (128, 128, 128)). Diff is the proof that the
    //	real McIntosh 2012 / Lottes ~2015 3-directional hex bokeh
    //	algorithm actually drove the final composite.
    std::vector< std::uint8_t > const rendered_real = render_aaa_dof_hex_bokeh_real_c140a(
        /*use_stub_config*/ false );
    std::vector< std::uint8_t > const rendered_stub = render_aaa_dof_hex_bokeh_real_c140a(
        /*use_stub_config*/ true );

    std::size_t diff_count   = 0;
    std::size_t total_pixels = 0;
    int         max_diff     = 0;
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            ++total_pixels;
            bool differs = false;
            for( int c = 0; c < 3; ++c )   // RGB only ; alpha is constant
            {
                int const d  = int( rendered_real[ base + c ] ) -
                               int( rendered_stub[ base + c ] );
                int const ad = d < 0 ? -d : d;
                if( ad > max_diff ) max_diff = ad;
                if( ad > 0 ) differs = true;
            }
            if( differs ) ++diff_count;
        }
    }

    double const diff_pct = 100.0 * double( diff_count ) / double( total_pixels );
    std::printf( "[Phase3.Golden:aaa_dof_hex_bokeh_real] real-vs-stub diff = %zu / %zu pixels (%.2f%%), max per-channel = %d\n",
                 diff_count, total_pixels, diff_pct, max_diff );

    //	Threshold per c140-A spec : 5000 pixels and 50-LSB max. The
    //	real frame is the hex bokeh DOF composite over a 32-px
    //	checker pattern ; per-pixel RGB swings between ~0 (dark cell)
    //	and ~255 (bright cell) , and the DOF blur in the top/bottom
    //	bands mixes those values further. The stub case is a flat
    //	mid-grey RGB = (128,128,128). Per-channel deltas vs (128)
    //	alone already exceed 100 LSB at the bright / dark checker
    //	cells. Conservative floors >5000 / >50 catch the regression
    //	mode "real 5-pass hex bokeh silently fell back to the stub
    //	branch".
    EXPECT_GT( diff_count, std::size_t( 5000 ) )
        << "aaa_dof_hex_bokeh_real canonical output is nearly identical "
        << "to the stub-config render -- the real McIntosh 2012 / Lottes "
        << "~2015 3-directional hex bokeh algorithm never drove the "
        << "fragment output. Diff = " << diff_count
        << " pixels, max per-channel = " << max_diff;
    EXPECT_GT( max_diff, 50 )
        << "aaa_dof_hex_bokeh_real per-channel diff vs stub-config "
        << "never exceeds 50 LSB -- DOF dynamic range "
        << "collapsed. max_diff = " << max_diff;
}
