// ps_test.metal -- Path A port from SercelMT/ps_test.frag (c93).
// Per-pixel UV remap from aaa_fu_vec4[0] (rect bounds) + smoothstep alpha
// thresholding on red channel.
// Patches :
//   - `in VS_out { tex_impli, color, vertex, normal, tex_coord, fall_off } fs_in;`
//     -> stage_in stub : only `fs_in.tex_impli.st` -> in.uv.xy read in live path
//   - `aaa_tex2d[0]`    -> `_aaa_tex_0`
//   - `aaa_fu_vec4[0]`  -> `_aaa_vec4s.values[0]`
//   - `out vec4 fragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuVec4s { float4 values[ 16 ]; };

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
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float2 const coi = in.uv.xy;   // VS_out tex_impli stub
    float4 const r   = _aaa_vec4s.values[ 0 ];   // [.x=u_min, .y=v_min, .z=u_max, .w=v_max]

    float2 const coi2 = float2(
        ( r.z - r.x ) * coi.x + r.x,
        ( r.w - r.y ) * coi.y + r.y );

    float4 tex = _aaa_tex_0.sample( _aaa_samp, coi2 );
    tex.a = smoothstep( 0.45, 0.5, tex.r );
    return tex;
}
