// ps_gp_textflow.metal -- Path A port from ps_gp_textflow.frag (c84).
// Soft circular splat modulated by per-vertex `aaa_att_w`.
// Patches :
//   - `in float aaa_att_w` (P4 per-vertex) -> const 1.0 stub
//   - `aaa_fu_float[0]` -> `_aaa_floats.values[0]` (falloff exponent)
//   - `gl_TexCoord[0].st` -> `in.uv.xy`
//   - `gl_Color` -> opaque-white stub (P3)
//   - `discard` -> MSL `discard_fragment()`

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float  const aaa_att_w = 1.0;          // P4 stub
    float4 const gl_Color  = float4( 1.0 ); // P3 stub

    float2 tex = in.uv.xy - 0.5;
    float  d   = dot( tex, tex );
    if( d > 0.25 )
        discard_fragment();

    d = 1.0 - d * 4.0;
    d = pow( d, _aaa_floats.values[ 0 ] );
    d *= 1.0 - aaa_att_w;
    d = clamp( d, 0.0, 1.0 );

    return float4( d, d, d, 1.0 ) * gl_Color;
}
