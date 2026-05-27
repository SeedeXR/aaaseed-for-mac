// ps_perlinpinpin.metal -- Path A port from ps_perlinpinpin.frag (c81).
// Quad selection + alpha modulation : sub-tile a 4x4 atlas via per-vertex
// (sel_x, sel_y) offsets, fade by per-vertex `aaa_att_w`.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[0]`     -> `_aaa_floats.values[0]`
//   - `in float sel_x` / `in float sel_y` / `in float aaa_att_w`
//     (P4 per-vertex selectors) -> const stubs : sel_x=0 sel_y=0 att=1 ;
//     pipeline shape preserved.
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `discard`             -> MSL `discard_fragment()`
//   - `gl_FragColor`        -> return value

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
    float const sel_x     = 0.0;   // P4 stub : per-vertex tile-x selector
    float const sel_y     = 0.0;   // P4 stub : per-vertex tile-y selector
    float const aaa_att_w = 1.0;   // P4 stub : per-vertex alpha attenuation

    if( aaa_att_w <= 0.0 )
        discard_fragment();

    float2 tex = in.uv.xy;
    tex.x = sel_x + tex.x * 0.25;
    tex.y = sel_y + tex.y * 0.25;

    float4 texture = _aaa_tex_0.sample( _aaa_samp, tex );
    texture.w *= aaa_att_w * _aaa_floats.values[ 0 ];
    return texture;
}
