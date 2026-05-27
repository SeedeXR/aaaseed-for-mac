// blur.metal -- Path A port from shader/blur.frag (c82).
// 8-tap radial blur with adjustable radius.
// Patches :
//   - `texUnit` -> `_aaa_tex_0`
//   - mxw uniform -> AaaFuFloats[0] (brightness slider)
//   - file-scope `vec4 c1 = vec4(0); vec2 move; vec2 texCoord;`
//     (P5 file-scope decls) -> locals inside fs_main
//   - `dolookup(float i)` helper -> inlined (or could stay as static inline
//     but inlining keeps the .metal self-contained)
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
    //	[0] = mxw_vertslider_brightness_0x0_2x0_1x0_mxw
    float const j = _aaa_floats.values[ 0 ];
    float2 const tc = in.uv.xy;

    float  i = 1.0 / 512.0;
    float4 c1;
    float2 move;

    move = float2( -i,  i ); c1  = _aaa_tex_0.sample( _aaa_samp, tc + move );
    move = float2(  i,  i ); c1 += _aaa_tex_0.sample( _aaa_samp, tc + move );
    move = float2(  i, -i ); c1 += _aaa_tex_0.sample( _aaa_samp, tc + move );
    move = float2( -i, -i ); c1 += _aaa_tex_0.sample( _aaa_samp, tc + move );

    i += i * j;

    move = float2( 0.0,   i ); c1 += _aaa_tex_0.sample( _aaa_samp, tc + move );
    move = float2(   i, 0.0 ); c1 += _aaa_tex_0.sample( _aaa_samp, tc + move );
    move = float2( 0.0,  -i ); c1 += _aaa_tex_0.sample( _aaa_samp, tc + move );
    move = float2(  -i, 0.0 ); c1 += _aaa_tex_0.sample( _aaa_samp, tc + move );

    float4 result = c1 * 0.17;
    result.a = 1.0;
    return result;
}
