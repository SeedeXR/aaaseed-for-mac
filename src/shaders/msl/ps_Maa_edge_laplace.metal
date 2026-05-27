// ps_Maa_edge_laplace.metal -- Path A port from ps_Maa_edge_laplace.frag (c80).
// 5x5 Laplacian-of-Gaussian-style edge filter with discard.
// Patches :
//   - `aaa_tex2d[0]` (declared as `aaa_tex2d[0]` size-zero array in
//     source -- upstream typo, used as singleton) -> `_aaa_tex_0`
//   - `aaa_fu_float[0..1]`   -> `_aaa_floats.values[0..1]`
//   - `gl_TexCoord[0].xy/st` -> `in.uv.xy`
//   - `textureSize(tex, 0)`  -> `int2( get_width(), get_height() )`
//   - `discard`              -> MSL `discard_fragment()`
//   - `gray(c)`              -> prelude helper (float4 overload)
//   - `gl_FragColor`         -> return value

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
    float const NPixels   = _aaa_floats.values[ 0 ];
    float const Threshold = _aaa_floats.values[ 1 ];

    int2  const tex_suv = int2( _aaa_tex_0.get_width(), _aaa_tex_0.get_height() );
    float const dx  = NPixels / float( tex_suv.x );
    float const dy  = NPixels / float( tex_suv.y );
    float const d2x = 2.0 * dx;
    float const d2y = 2.0 * dy;

    float2 const uv = in.uv.xy;
    float4 c = 24.0 * _aaa_tex_0.sample( _aaa_samp, uv );

    //	5x5 kernel : centre weight 24, neighbours -1. Total mass = 0.
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2( -d2x, -d2y ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  -dx, -d2y ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  0.0, -d2y ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(   dx, -d2y ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  d2x, -d2y ) );

    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2( -d2x,  -dy ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  -dx,  -dy ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  0.0,  -dy ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(   dx,  -dy ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  d2x,  -dy ) );

    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2( -d2x,  0.0 ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  -dx,  0.0 ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(   dx,  0.0 ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  d2x,  0.0 ) );

    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2( -d2x,   dy ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  -dx,   dy ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  0.0,   dy ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(   dx,   dy ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  d2x,   dy ) );

    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2( -d2x,  d2y ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  -dx,  d2y ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  0.0,  d2y ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(   dx,  d2y ) );
    c -= _aaa_tex_0.sample( _aaa_samp, uv + float2(  d2x,  d2y ) );

    c.xyz = fabs( c.xyz );
    float const g = gray( c );

    if( g > Threshold )
        c = _aaa_tex_0.sample( _aaa_samp, uv );
    else
        discard_fragment();

    return c;
}
