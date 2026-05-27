// ps_gp_galaxy.metal -- Path A port from ps_gp_galaxy.frag (c84).
// Particle sprite with radial-arm modulation + per-vertex selector stubs.
// Patches :
//   - `aaa_tex2d[0]` -- declared but **only the dead `if(false)` branch** uses it ;
//     live path returns synthesized colour. Dropped to avoid an unused binding.
//   - `aaa_fu_float[0..2]` -> `_aaa_floats.values[0..2]`
//   - `aaa_fu_vec4[0]`     -> `_aaa_vec4s.values[0]`
//   - `sel_x` / `sel_y` / `aaa_att_w` (P4 per-vertex) -> const stubs
//   - `gl_TexCoord[0].st`  -> `in.uv.xy`
//   - `gl_Color`           -> opaque-white stub (P3)
//   - `atan(y, x)`         -> MSL `atan2(y, x)`
//   - `discard`            -> MSL `discard_fragment()`

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
    float  const sel_x     = 0.0;   // P4 stub
    float  const aaa_att_w = 1.0;   // P4 stub
    float3 const gl_Color_rgb = float3( 1.0 );

    if( aaa_att_w <= 0.0 )
        discard_fragment();

    float2 tex = in.uv.xy;
    tex -= 0.5;
    tex *= 2.0;
    float d = tex.x * tex.x + tex.y * tex.y;
    if( d > 1.0 )
        discard_fragment();

    if( _aaa_floats.values[ 1 ] != 0.0 )
        d += ( 1.0 - cos( atan2( tex.y, tex.x ) * _aaa_floats.values[ 2 ] + sel_x * 6.2432 ) ) * _aaa_floats.values[ 1 ];

    d = 1.0 - pow( d, _aaa_floats.values[ 0 ] * _aaa_floats.values[ 0 ] );

    float4 texture;
    texture.xyz = gl_Color_rgb;
    texture.w   = d;
    texture.xyz *= _aaa_vec4s.values[ 0 ].xyz;
    texture.w   *= aaa_att_w * _aaa_vec4s.values[ 0 ].w;
    return texture;
}
