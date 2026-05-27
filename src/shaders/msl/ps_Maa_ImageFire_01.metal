// ps_Maa_ImageFire_01.metal -- Path A port from ps_Maa_ImageFire_01.frag (c87).
// Maa 14 Jan 2008 -- gradient test variant ; close sibling of ps_Maa_Gradient_01.
// Patches :
//   - `aaa_tex2d[0..1]`         -> `_aaa_tex_0..1`
//   - `aaa_fu_float[0..5]`      -> `_aaa_floats.values[0..5]`
//   - `aaa_fu_int[0..2]`        -> `_aaa_ints.values[0..2]`
//   - `aaa_fu_src` / `aaa_fu_out` -> `_aaa_floats.values[8..9]`
//   - `gl_TexCoord[0].st/x/y`   -> `in.uv.xy`
//   - `gl_TexCoord[0]` (vec4 arg to texture2D) -> `in.uv` (engine permissive ; promote to vec2)
//   - `texture2D( tex, scalar )` engine permissive -> `float2(scalar, 0)`
//   - `gl_FragCoord`            -> `in.position`
//   - `gl_Color`                -> opaque-white stub (P3)
//   - `mod( x, 1.0 )` (positive) -> `fmod( x, 1.0 )`
//   - `gray( pixel.xyz )`       -> prelude helper
//   - `blend( dst )`            -> identity (c72)
//   - `gl_FragColor`            -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

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
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float const aaa_fu_src = _aaa_floats.values[ 8 ];
    float const aaa_fu_out = _aaa_floats.values[ 9 ];

    float4 src = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    float4 dst = float4( 0.0, 0.0, 0.0, 1.0 );   // deterministic init
    int   const type = _aaa_ints.values[ 0 ];

    int2  const tex_suv = int2( _aaa_tex_0.get_width(), _aaa_tex_0.get_height() );
    float const dx = 1.0 / float( tex_suv.x );
    float const dy = 1.0 / float( tex_suv.y );

    if( type == 0 )   // normal
    {
        float2 co;
        co.x = in.uv.x * _aaa_floats.values[ 2 ];
        co.y = in.uv.y * _aaa_floats.values[ 3 ];
        float4 pixel = _aaa_tex_1.sample( _aaa_samp, co );
        float  g = gray( pixel.xyz );

        co.x += dx * _aaa_floats.values[ 4 ];
        pixel = _aaa_tex_1.sample( _aaa_samp, co );
        co.x -= dx * _aaa_floats.values[ 4 ];
        float d = fabs( g - gray( pixel.xyz ) );

        co.y += dy * _aaa_floats.values[ 5 ];
        pixel = _aaa_tex_1.sample( _aaa_samp, co );
        d += fabs( g - gray( pixel.xyz ) );

        float2 po = in.uv.xy;
        po.y += d * _aaa_floats.values[ 1 ];
        dst = _aaa_tex_0.sample( _aaa_samp, po );
        dst *= ( 1.0 - d * _aaa_floats.values[ 0 ] );
        //	`if( aaa_fu_int[2] != 0 ) dst = blend(dst);` -- blend pass-through
    }
    else if( type == 1 )
    {
        dst.r = in.position.x * _aaa_floats.values[ 2 ];
        dst.g = in.position.y * _aaa_floats.values[ 3 ];
        dst.b = 1.0;
        dst.a = 1.0;
    }
    else if( type == -1 )
    {
        dst = src;
    }
    else if( type == -2 )
    {
        dst = float4( 1.0, 1.0, 1.0, 1.0 );   // gl_Color stub
    }
    else if( type == -3 )
    {
        dst = src * float4( 1.0, 1.0, 1.0, 1.0 );
        dst.a = 1.0;
    }
    else if( type == -4 )
    {
        dst = src * float4( 1.0, 1.0, 1.0, 1.0 );
    }
    else
    {
        //	`texture2D( tex, scalar )` engine permissive -> `float2(scalar, 0)`
        float const sx = in.position.x * _aaa_floats.values[ 4 ];
        dst.r = fmod( dfdy( _aaa_tex_0.sample( _aaa_samp, float2( sx, 0.0 ) ).r ) * _aaa_floats.values[ 1 ], 1.0 );
        dst.g = fmod( dfdy( in.position.y * _aaa_floats.values[ 5 ] ) * _aaa_floats.values[ 1 ], 1.0 );
        dst.a = 1.0;
    }

    dst *= aaa_fu_out;
    if( aaa_fu_src != 0.0 )
        dst += _aaa_tex_0.sample( _aaa_samp, in.uv.xy ) * aaa_fu_src;
    return dst;
}
