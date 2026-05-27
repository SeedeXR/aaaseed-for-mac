// ps_Maa_Gradient_01.metal -- Path A port from ps_Maa_Gradient_01.frag
// (Maa 14 Jan 2008 -- gradient test).
// Hand-patched (continuation 76) for :
//   - `textureSize(aaa_tex2d[0], 0)` -> `int2( get_width(0), get_height(0) )`.
//   - `gl_TexCoord[0]` -> `in.uv` (3 sites).
//   - `gl_FragCoord` -> `in.position` (3 sites).
//   - `blend(dst)` -> `dst` pass-through (c72 pattern).
//   - `gl_Color` -> opaque-white fallback (c70 pattern, type<0 branches).
//   - `texture2D( tex, scalar )` -- engine GLSL passes a scalar to a
//     sampler2D ; MSL requires float2 ; promoted to `float2(scalar, 0)`.
//   - `src` declared but unused in most branches : init to opaque-black.
//
// Doctrine : pure Apple Metal.

#include <metal_stdlib>
using namespace metal;

struct VertexOut
{
    float4 position [[position]];
    float2 uv;
};

#ifndef AAA_MSL_PRELUDE_DEFINED
#define AAA_MSL_PRELUDE_DEFINED
constexpr constant float3 aaa_gray_conv = float3( 0.2989, 0.5870, 0.1140 );
static inline float gray( float3 a ) { return dot( a.xyz, aaa_gray_conv ); }
static inline float gray( float4 a ) { return dot( a.xyz, aaa_gray_conv ); }
static inline float compute_gray( float3 a ) { return dot( a.xyz, aaa_gray_conv ); }
static inline float compute_gray( float4 a ) { return dot( a.xyz, aaa_gray_conv ); }
#endif

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ),
                          float2( -1.0,  1.0 ),
                          float2(  3.0,  1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),
                          float2( 0.0, 0.0 ),
                          float2( 2.0, 0.0 ) };
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
    float4 src = float4( 0.0, 0.0, 0.0, 1.0 );   // engine left uninit ; opaque-black on Mac
    float4 dst = float4( 0.0, 0.0, 0.0, 1.0 );

    int2  const tex_suv = int2( _aaa_tex_0.get_width( 0 ), _aaa_tex_0.get_height( 0 ) );
    float const dx = 1.0 / float( tex_suv.x );
    float const dy = 1.0 / float( tex_suv.y );

    int const type = _aaa_ints.values[ 0 ];
    if( type == 0 )   // normal
    {
        float2 co;
        float  g;
        float  d;
        float4 pixel;
        co.x = in.uv.x * _aaa_floats.values[ 2 ];
        co.y = in.uv.y * _aaa_floats.values[ 3 ];
        pixel = _aaa_tex_1.sample( _aaa_samp, co );
        g = gray( pixel );

        co.x += dx * _aaa_floats.values[ 4 ];
        pixel = _aaa_tex_1.sample( _aaa_samp, co );
        co.x -= dx * _aaa_floats.values[ 4 ];
        d = fabs( g - gray( pixel.xyz ) );

        co.y += dy * _aaa_floats.values[ 5 ];
        pixel = _aaa_tex_1.sample( _aaa_samp, co );
        d += fabs( g - gray( pixel.xyz ) );

        float2 po = in.uv.xy;
        po.y += d * _aaa_floats.values[ 1 ];
        dst   = _aaa_tex_1.sample( _aaa_samp, po );
        dst  *= ( 1.0 - d * _aaa_floats.values[ 0 ] );
        //	`if( aaa_fu_int[2] != 0 ) dst = blend(dst);` -- blend is
        //	pass-through on Mac (c72), skip the conditional entirely.
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
        //	gl_Color -> opaque-white fallback (c70 pattern).
        dst = float4( 1.0, 1.0, 1.0, 1.0 );
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
        //	`texture2D( tex, scalar )` -- engine passes a scalar UV ;
        //	promote to float2( scalar, 0 ).
        float const sx = in.position.x * _aaa_floats.values[ 4 ];
        dst.r = fmod( dfdy( _aaa_tex_0.sample( _aaa_samp, float2( sx, 0.0 ) ).r ) * _aaa_floats.values[ 1 ], 1.0 );
        dst.g = fmod( dfdy( in.position.y * _aaa_floats.values[ 5 ] ) * _aaa_floats.values[ 1 ], 1.0 );
        dst.a = 1.0;
    }

    return dst;
}
