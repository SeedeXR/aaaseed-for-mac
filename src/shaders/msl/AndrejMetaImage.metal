// AndrejMetaImage.metal -- Path A port from shader/AndrejMetaImage.frag (c82).
// Per-channel multiplier (red, blue, green, alpha sliders).
// Patches :
//   - `texUnit` -> `_aaa_tex_0`
//   - mxw uniforms -> AaaFuFloats slots :
//       [0] = mxw_vertslider_red_*
//       [1] = mxw_vertslider_blue_*
//       [2] = mxw_vertslider_green_*
//       [3] = mxw_vertslider_alpha_*
//   - `gl_TexCoord[0].xy` -> `in.uv.xy`

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const r_s = _aaa_floats.values[ 0 ];
    float const b_s = _aaa_floats.values[ 1 ];
    float const g_s = _aaa_floats.values[ 2 ];
    float const a_s = _aaa_floats.values[ 3 ];

    float4 col = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    col.r *= r_s;
    col.g *= b_s;   // engine source maps "blue slider" to .g multiplier (preserved)
    col.b *= g_s;   // engine source maps "green slider" to .b multiplier (preserved)
    col.a *= a_s;
    return col;
}
