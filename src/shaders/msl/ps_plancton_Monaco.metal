// ps_plancton_Monaco.metal -- Path A port from ps_plancton_Monaco.frag (c89).
// Radial-distance soft splat ; gl_Color tinted ; pow-falloff exponent.
// Patches :
//   - `aaa_fu_float[0]`     -> `_aaa_floats.values[0]`
//   - `aaa_fu_vec4[0]`      -> `_aaa_vec4s.values[0]`
//   - `aaa_att_w`           (P4) -> const 1.0 stub
//   - `sel_x` / `sel_y`     (P4) -> const stubs (unused)
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gl_Color`            -> opaque-white stub (P3)
//   - `discard`             -> `discard_fragment()`

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float  const aaa_att_w = 1.0;   // P4 stub
    float3 const gl_Color_rgb = float3( 1.0 );   // P3 stub

    float2 tc = in.uv.xy * 2.0 - 1.0;
    float  const d_sq = dot( tc, tc );
    if( d_sq > 1.0 )
        discard_fragment();

    float const d = 1.0 - pow( d_sq, _aaa_floats.values[ 0 ] );
    return float4( gl_Color_rgb, aaa_att_w * d ) * _aaa_vec4s.values[ 0 ];
}
