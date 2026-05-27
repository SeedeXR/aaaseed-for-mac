// pool1.metal -- Path A port from pool1.frag (c94).
// **First proper multi-target MRT port** (2 outputs : color + id-mask).
// Establishes the MSL fragment-return-struct pattern : `struct FragmentOut {
// float4 c0 [[color(0)]]; float4 c1 [[color(1)]]; };` returned from fs_main.
// Patches :
//   - `layout(location = N) out vec4 color_out_N` -> `FragmentOut.color_out_N [[color(N)]]`
//   - `in float sel_x` / `sel_y` / `aaa_att_w` (P4 per-vertex) -> const stubs
//   - `aaa_tex2d[0]`  -> `_aaa_tex_0`
//   - `aaa_fu_vec4[0]` -> `_aaa_vec4s.values[0]`
//   - `gl_TexCoord[0].st` -> `in.uv.xy`
//   - `gl_Color`     -> opaque-white stub (P3)
//   - `discard`      -> `discard_fragment()`
//   - MSL pipeline state at draw time must declare 2 color attachments to match.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuVec4s { float4 values[ 16 ]; };

struct FragmentOut
{
    float4 color_out_0 [[color(0)]];
    float4 color_out_1 [[color(1)]];
};

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

fragment FragmentOut fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float const sel_x     = 1.0;   // P4 stubs
    float const sel_y     = 0.0;
    float const aaa_att_w = 1.0;
    float3 const gl_Color = float3( 1.0 );

    float2 tex = in.uv.xy - 0.5;
    tex = float2( tex.x * sel_x + tex.y * sel_y,
                   tex.x * sel_y - tex.y * sel_x ) + 0.5;

    float4 texture = _aaa_tex_0.sample( _aaa_samp, tex );
    if( texture.a <= 0.03 )
        discard_fragment();

    texture.a *= aaa_att_w * _aaa_vec4s.values[ 0 ].a;
    if( texture.a <= 0.0003 )
        discard_fragment();

    texture.rgb *= _aaa_vec4s.values[ 0 ].rgb * gl_Color;

    FragmentOut out;
    out.color_out_0 = texture;
    out.color_out_1 = float4( 0.0, 0.0, 0.0, 1.0 );   // id-mask : engine stubs to black
    return out;
}
