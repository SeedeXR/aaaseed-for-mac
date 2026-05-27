// ps_to_grey_cc.metal -- Path A port from ps_to_grey_cc.frag (c90, Maa 2011).
// Two-texture grey-with-mask blend : src + mask -> luminance-shifted output.
// Patches :
//   - `aaa_tex2d[0..1]`     -> `_aaa_tex_0..1`
//   - `aaa_fu_float[0..4]`  -> `_aaa_floats.values[0..4]`
//   - `aaa_fu_src` / `aaa_fu_out` (individual uniforms) -> AaaFuFloats[8..9]
//     (engine convention past the array range)
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gl_Color`            -> opaque-white stub (P3)
//   - Inline luminance formula `0.2989/0.5870/0.1140 * 2` preserved verbatim
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
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const aaa_fu_src = _aaa_floats.values[ 8 ];
    float const aaa_fu_out = _aaa_floats.values[ 9 ];
    (void) aaa_fu_src; (void) aaa_fu_out;   // declared in engine source but unused in live path
    float4 const gl_Color  = float4( 1.0 );

    float2       ori = in.uv.xy;
    float4       src = _aaa_tex_0.sample( _aaa_samp, ori );

    ori.x = ( ori.x - 0.5 ) * _aaa_floats.values[ 3 ] + 0.5;
    ori.y = ( ori.y - 0.5 ) * _aaa_floats.values[ 4 ] + 0.5;
    float4 const mask = _aaa_tex_1.sample( _aaa_samp, ori );

    float const g = src.x * 0.2989 * 2.0 + src.y * 0.5870 * 2.0 + src.z * 0.1140 * 2.0;

    src.rgb *= ( 1.0 - _aaa_floats.values[ 2 ] );
    float4 dst;
    dst.r = dst.g = dst.b = g * _aaa_floats.values[ 2 ];
    dst.a = src.a;
    src.rgb += dst.rgb;
    src.rgb *= 1.0 + _aaa_floats.values[ 2 ] * ( mask.g * _aaa_floats.values[ 1 ] - 1.0 );

    return src * gl_Color;
}
