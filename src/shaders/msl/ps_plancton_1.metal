// ps_plancton_1.metal -- Path A port from ps_plancton_1.frag (c89).
// Particle sprite with discard + texture sample + gl_Color modulate via blend().
// Patches :
//   - `aaa_tex2d[0]`  -> `_aaa_tex_0`
//   - `aaa_att_w`     (P4 per-vertex) -> const 1.0 stub
//   - `sel_x` / `sel_y` (P4 per-vertex) -> const 0.0 stubs (unused in live path)
//   - `gl_TexCoord[0].st` -> `in.uv.xy`
//   - `gl_Color`      -> opaque-white stub (P3)
//   - `discard`       -> `discard_fragment()`
//   - `blend( x )`    -> identity pass-through (c72)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

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
    VertexOut          in          [[stage_in]],
    texture2d< float > _aaa_tex_0  [[texture(0)]],
    sampler            _aaa_samp   [[sampler(0)]] )
{
    float  const aaa_att_w = 1.0;   // P4 stub
    float4 const gl_Color  = float4( 1.0 );   // P3 stub
    (void) aaa_att_w;

    float2 const tc = in.uv.xy;
    float4 const t  = _aaa_tex_0.sample( _aaa_samp, tc );
    return t * gl_Color;   // blend pass-through
}
