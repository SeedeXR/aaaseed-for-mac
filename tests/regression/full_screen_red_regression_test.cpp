// full_screen_red_regression_test.cpp
//
// First regression-corpus entry. Renders a deterministic full-screen-red
// MSL shader to a 64x64 RGBA8 texture, reads the entire pixel buffer
// back, and compares it against an inline golden description.
//
// Why pixel-by-pixel rather than image-file comparison : avoids adding
// a PNG / image-loading dependency for the regression harness. The
// "golden image" is, in this trivial case, "every pixel must be opaque
// red within a tolerance". When the corpus grows (later sessions will
// land MEU frames captured from the Windows reference binary), we will
// vendor `stb_image_write.h` + `stb_image.h` and the harness will
// load .png goldens off disk. For now : code-defined golden is
// sufficient AND has no external dep.
//
// Regression discipline per philosophy.md and todo.md Phase 7 : a
// regression failure blocks the porting step from being marked done.
//
// CTest label : `regression`.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <array>
#include <cstdint>
#include <vector>

namespace
{
    constexpr char const* kFullScreenRedMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; };

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

    constexpr std::uint32_t kWidth  = 64;
    constexpr std::uint32_t kHeight = 64;
}

TEST( Regression, FullScreenRedAllPixelsRedBitExact )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    GOL::TextureId target = backend.gen_texture_2d( kWidth, kHeight,
                                                    GOL::TextureFormat::RGBA8 );
    ASSERT_NE( target, GOL::kInvalidTextureId );

    GOL::ProgramId prog = backend.create_program_msl( kFullScreenRedMsl,
                                                     "vs_main", "fs_main" );
    ASSERT_NE( prog, GOL::kInvalidProgramId )
        << "shader compile failed : " << backend.get_last_error();

    GOL::RenderPassDescriptor rpd;
    rpd.color_attachment = target;
    rpd.load_action      = GOL::LoadAction::Clear;
    rpd.clear_color[ 0 ] = 0.0f;
    rpd.clear_color[ 1 ] = 1.0f;       // green clear -- if the triangle
    rpd.clear_color[ 2 ] = 0.0f;       // didn't actually draw, the
    rpd.clear_color[ 3 ] = 1.0f;       // regression catches it as green.

    ASSERT_TRUE( backend.begin_render_pass( rpd ) );
    backend.set_viewport( 0.0f, 0.0f, float( kWidth ), float( kHeight ) );
    backend.bind_program( prog );
    backend.draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    backend.end_render_pass();
    backend.present();

    //	Read back the entire 64x64 = 4096-pixel buffer. read_texture_pixels
    //	takes BYTES PER ROW (not total bytes) — Metal's getBytes reads h
    //	rows of bytes_per_row bytes each.
    constexpr std::size_t kBytesPerPixel = 4;
    constexpr std::size_t kBytesPerRow   = kWidth * kBytesPerPixel;
    constexpr std::size_t kBufferBytes   = kBytesPerRow * kHeight;
    std::vector< std::uint8_t > pixels( kBufferBytes, 0 );

    backend.read_texture_pixels( target, 0, 0, kWidth, kHeight,
                                 pixels.data(), kBytesPerRow );

    //	Golden : every pixel must be opaque red (255, 0, 0, 255).
    //	Tolerance : zero on R/A, zero on G/B. Trivial shader, deterministic
    //	output ; any deviation is a real regression.
    //
    //	Failure messages tell the caller WHICH pixel and what they got,
    //	so a diff is debuggable without re-running with a different test.
    std::size_t mismatches = 0;
    std::uint32_t first_bad_x = 0;
    std::uint32_t first_bad_y = 0;
    std::array< std::uint8_t, 4 > first_bad_rgba{};
    for( std::uint32_t y = 0; y < kHeight; ++y )
    {
        for( std::uint32_t x = 0; x < kWidth; ++x )
        {
            std::size_t const base = ( y * kWidth + x ) * 4;
            std::uint8_t const r = pixels[ base + 0 ];
            std::uint8_t const g = pixels[ base + 1 ];
            std::uint8_t const b = pixels[ base + 2 ];
            std::uint8_t const a = pixels[ base + 3 ];
            bool const ok = ( r == 255 && g == 0 && b == 0 && a == 255 );
            if( !ok )
            {
                if( mismatches == 0 )
                {
                    first_bad_x = x;
                    first_bad_y = y;
                    first_bad_rgba = { r, g, b, a };
                }
                ++mismatches;
            }
        }
    }

    EXPECT_EQ( mismatches, 0u )
        << "Regression : " << mismatches << " of "
        << ( kWidth * kHeight ) << " pixels deviate from golden red.\n"
        << "First mismatch at (" << first_bad_x << ", " << first_bad_y << ") = "
        << "(R=" << int( first_bad_rgba[0] )
        << " G=" << int( first_bad_rgba[1] )
        << " B=" << int( first_bad_rgba[2] )
        << " A=" << int( first_bad_rgba[3] ) << ")";

    backend.delete_program( prog );
    backend.delete_texture( target );
}
