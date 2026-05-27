// ps_edgeoverlay.metal
//
// Hand-port of vendor/aaaseed-runtime/AAAKernel/Shader/ps_edgeoverlay.frag
// to MSL, continuation 25 (2026-05-26). Path A : direct GLSL -> MSL,
// no SPIR-V / Vulkan IR per project doctrine.
//
// Restored continuation 36 from the inline `kPsEdgeOverlayMsl` string in
// tests/unit/shader_msl_port_test.cpp (which was the source-of-truth
// after continuation 34's bulk-port accidentally overwrote and then
// deleted the on-disk file). The inline string and this file must
// stay byte-equivalent ; tests/unit/shader_msl_port_test.cpp's R"MSL(
// ... )MSL" block IS the authority.
//
// Engine semantics : Sobel-style edge detector ; per-pixel discards
// where the gradient magnitude is below threshold, alpha-fades the
// transition zone. Uses get_gray() helper with engine-canonical NTSC
// luminance weights.
//
// Hand-port differences from tool-generated output :
//   - Custom `EdgeOverlayUniforms` struct + custom `kGreyWeights` /
//     `get_gray()`, rather than the tool's standardized `AaaFuFloats` +
//     prelude. Both compile cleanly via Apple Metal compiler.
//   - Hand-rolled vs_main pass-through ; identical pattern to the tool.

#include <metal_stdlib>
using namespace metal;

struct EdgeOverlayUniforms
{
    float fu[ 16 ];
};

struct VertexOut
{
    float4 position [[position]];
    float2 uv;
};

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ];
    verts[ 0 ] = float2( -1.0, -3.0 );
    verts[ 1 ] = float2( -1.0,  1.0 );
    verts[ 2 ] = float2(  3.0,  1.0 );

    float2 uvs[ 3 ];
    uvs[ 0 ] = float2( 0.0,  2.0 );
    uvs[ 1 ] = float2( 0.0,  0.0 );
    uvs[ 2 ] = float2( 2.0,  0.0 );

    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

constexpr constant float3 kGreyWeights = float3( 0.33333 );

static inline float get_gray( float4 c )
{
    return dot( c.rgb, kGreyWeights );
}

fragment float4 fs_main( VertexOut in                                [[stage_in]],
                         texture2d< float >          aaa_tex2d_0     [[texture(0)]],
                         sampler                     aaa_samp_0      [[sampler(0)]],
                         constant EdgeOverlayUniforms& U             [[buffer(0)]] )
{
    float NPixels   = U.fu[ 0 ];
    float Threshold = U.fu[ 1 ] * U.fu[ 1 ];

    float tex_sx = float( aaa_tex2d_0.get_width()  );
    float tex_sy = float( aaa_tex2d_0.get_height() );
    float delta_x = NPixels / tex_sx;
    float delta_y = NPixels / tex_sy;

    float2 uv = in.uv;

    float4 tmp;
    tmp = aaa_tex2d_0.sample( aaa_samp_0, uv + float2( -delta_x, -delta_y ) );
    float g00 = get_gray( tmp );
    tmp = aaa_tex2d_0.sample( aaa_samp_0, uv + float2(      0.0, -delta_y ) );
    float g01 = get_gray( tmp );
    tmp = aaa_tex2d_0.sample( aaa_samp_0, uv + float2(  delta_x, -delta_y ) );
    float g02 = get_gray( tmp );
    tmp = aaa_tex2d_0.sample( aaa_samp_0, uv + float2( -delta_x,      0.0 ) );
    float g10 = get_gray( tmp );
    tmp = aaa_tex2d_0.sample( aaa_samp_0, uv + float2(  delta_x,      0.0 ) );
    float g12 = get_gray( tmp );
    tmp = aaa_tex2d_0.sample( aaa_samp_0, uv + float2( -delta_x,  delta_y ) );
    float g20 = get_gray( tmp );
    tmp = aaa_tex2d_0.sample( aaa_samp_0, uv + float2(      0.0,  delta_y ) );
    float g21 = get_gray( tmp );
    tmp = aaa_tex2d_0.sample( aaa_samp_0, uv + float2(  delta_x,  delta_y ) );
    float g22 = get_gray( tmp );

    float sx = g22 + 2.0 * g21 + g20 - g02 - g01 * 2.0 - g00;
    float sy = g22 -       g20 + g12 * 2.0 - 2.0 * g10 + g02 - g00;

    float dist = sx * sx + sy * sy;

    if( dist > Threshold )
    {
        discard_fragment();
    }

    float alpha;
    if( dist > ( Threshold * 0.5 ) )
        alpha = 1.0 - smoothstep( Threshold * 0.5, Threshold, dist );
    else
        alpha = 1.0;

    float4 color;
    color.rgb = aaa_tex2d_0.sample( aaa_samp_0, uv ).rgb;
    color.a   = alpha;

    return color;
}
