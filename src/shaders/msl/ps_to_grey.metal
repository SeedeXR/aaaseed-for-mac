// ps_to_grey.metal -- Path A port from ps_to_grey.frag (c81, Maa 2008).
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_src` / `aaa_fu_out` (individual uniforms) -> `_aaa_floats.values[8..9]`
//     (placed past the standard aaa_fu_float[0..7] slots to avoid clash)
//   - `aaa_fu_float[0..1]`  -> `_aaa_floats.values[0..1]`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gray( src.xyz )`     -> prelude helper
//   - `blend( dst )`        -> `dst` pass-through (c72)
//   - `gl_FragColor`        -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

#ifndef AAA_MSL_PRELUDE_DEFINED
#define AAA_MSL_PRELUDE_DEFINED
constexpr constant float3 aaa_gray_conv = float3( 0.2989, 0.5870, 0.1140 );
static inline float gray( float3 a ) { return dot( a.xyz, aaa_gray_conv ); }
static inline float gray( float4 a ) { return dot( a.xyz, aaa_gray_conv ); }
#endif

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
    float const aaa_fu_src = _aaa_floats.values[ 8 ];   // fixed-path tex quantity
    float const aaa_fu_out = _aaa_floats.values[ 9 ];   // shader-output quantity

    float4 const src = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );

    float g = gray( src.xyz );
    g = _aaa_floats.values[ 0 ] + g * ( _aaa_floats.values[ 1 ] - _aaa_floats.values[ 0 ] );
    g *= aaa_fu_out;

    float4 dst;
    dst.r = dst.g = dst.b = g;
    dst.a = src.a;
    dst.rgb += src.rgb * aaa_fu_src;

    return dst;   // blend() pass-through
}
