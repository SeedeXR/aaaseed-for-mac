// ps_angle_mult.metal -- Path A port from ps_angle_mult.frag (c83).
// Two-texture blend with rotated/offset sub-sample from second texture.
// Patches :
//   - `tex0` / `tex1` -> `_aaa_tex_0` / `_aaa_tex_1`
//   - `fu_float[2..4]` (non-aaa_ array) -> `_aaa_floats.values[2..4]`
//   - `aaa_fu_vec4[0]` -> `_aaa_vec4s.values[0]`
//   - file-scope `CONST` (engine macro) initializers moved inside fs_main (P5)
//   - `gl_TexCoord[0..1].st/t` -> in.uv.xy (single uv set on Mac ; uv[1] stub
//     matches uv[0] until multi-texcoord plumbing lands -- pipeline shape preserved)
//   - `gl_FragColor` -> return value

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

static inline float4 angle_mult_(
    texture2d< float > tex1, sampler samp,
    float2 uv1,
    float angle, float offset_x, float offset_y )
{
    float const x = uv1.x - 0.5 + offset_x;
    float const y = uv1.y - 0.5 + offset_y;
    float2 coord = float2( x * cos( angle ) - y * sin( angle ),
                            x * sin( angle ) + y * cos( angle ) );
    coord += float2( 0.5, 0.5 );
    return tex1.sample( samp, coord );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float  const weight_tex0 = _aaa_floats.values[ 0 ];
    float  const weight_tex1 = _aaa_floats.values[ 1 ];
    float4 const color       = _aaa_vec4s.values[ 0 ];

    float2 const uv0 = in.uv.xy;
    float2 const uv1 = in.uv.xy;   // gl_TexCoord[1] stub == uv[0] on Mac

    float4 const color_1 = _aaa_tex_0.sample( _aaa_samp, uv0 );
    float4 color_2 = angle_mult_( _aaa_tex_1, _aaa_samp, uv1,
                                   _aaa_floats.values[ 4 ],
                                   _aaa_floats.values[ 3 ],
                                   _aaa_floats.values[ 2 ] );
    color_2 *= color;
    return weight_tex0 * color_1 + weight_tex1 * color_1 * color_2;
}
