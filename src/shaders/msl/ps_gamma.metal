// ps_gamma.metal -- Path A port from ps_gamma.frag (c78).
// Patches : `aaa_tex2d[0]` -> `_aaa_tex_0` ; `aaa_fu_float[N]` ->
// `_aaa_floats.values[N]` ; `gl_TexCoord[0].st` -> `in.uv.xy` ;
// `blend(color)` -> `color` pass-through (c72 pattern).

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
    float const exposure = _aaa_floats.values[ 1 ];
    float const gamma    = _aaa_floats.values[ 0 ];

    float4 color;
    color.xyz = _aaa_tex_0.sample( _aaa_samp, in.uv.xy ).xyz;
    color    *= exposure;
    color.xyz = pow( color.xyz, float3( gamma ) );
    color.a   = _aaa_tex_0.sample( _aaa_samp, in.uv.xy ).a;

    return color;   // blend() pass-through (c72)
}
