// ps_Maa_key_chroma_01.metal -- Path A port from ps_Maa_key_chroma_01.frag (c83).
// Normalized-direction chroma keying with grey short-circuit + smoothstep falloff.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_vec4[0]`      -> `_aaa_vec4s.values[0]`
//   - `aaa_fu_float[0..2]`  -> `_aaa_floats.values[0..2]`
//   - `aaa_fu_src` / `aaa_fu_out` -> `_aaa_floats.values[8..9]`
//   - `gray( color_tex.rgb )` -> prelude helper
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gl_FragColor`        -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float3 const chroma_color  = _aaa_vec4s.values[ 0 ].rgb;
    float  const threshold_max = _aaa_floats.values[ 0 ];
    float  const threshold_min = threshold_max - _aaa_floats.values[ 1 ];
    float  const grey_cutoff   = _aaa_floats.values[ 2 ];
    float  const aaa_fu_src    = _aaa_floats.values[ 8 ];
    float  const aaa_fu_out    = _aaa_floats.values[ 9 ];

    float4 const color_tex = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );

    float alpha;
    float const grey = gray( color_tex.rgb );
    if( grey < grey_cutoff )
    {
        alpha = 1.0;
    }
    else
    {
        float const dist = distance( normalize( color_tex.rgb ), normalize( chroma_color ) );
        alpha = smoothstep( threshold_min, threshold_max, dist );
    }

    return float4( color_tex.rgb, alpha ) * aaa_fu_out + color_tex * aaa_fu_src;
}
