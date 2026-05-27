// ps_colorGradient.metal -- Path A port from ps_colorGradient.frag (c80).
// Four-color Bezier-or-linear spline indexed by source luminance.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[N]`     -> `_aaa_floats.values[N]`
//   - `aaa_fu_vec4[N]`      -> `_aaa_vec4s.values[N]`
//   - `aaa_fu_int[0]`       -> `_aaa_ints.values[0]`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gray(...)` engine helper inlined via aaa_prelude (already-defined)
//   - `greyGradPS(in sampler2D, float)` engine wrapper inlined (sampler
//     argument was dead -- only the float interpolant matters)
//   - `gl_FragColor`        -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

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

static inline float4 c_bezier( float4 c0, float4 c1, float4 c2, float4 c3, float t )
{
    float t2  = t * t;
    float t3  = t2 * t;
    float nt  = 1.0 - t;
    float nt2 = nt * nt;
    float nt3 = nt2 * nt;
    return nt3 * c0 + ( 3.0 * t * nt2 ) * c1 + ( 3.0 * t2 * nt ) * c2 + t3 * c3;
}

static inline float4 c_linear( float4 c0, float4 c1, float4 c2, float4 c3, float t )
{
    if( t <= 0.33 )
        return ( 0.33 - t ) * 3.0 * c0 +          t  * 3.0 * c1;
    else if( t <= 0.66 )
        return ( 0.66 - t ) * 3.0 * c1 + ( t - 0.33 ) * 3.0 * c2;
    else
        return ( 1.0  - t ) * 3.0 * c2 + ( t - 0.66 ) * 3.0 * c3;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float2 coor = in.uv.xy;
    coor.x += _aaa_floats.values[ 5 ] * sin( _aaa_floats.values[ 4 ] * coor.y ) * 0.5;
    float4 const src_color = _aaa_tex_0.sample( _aaa_samp, coor );

    float inter = gray( src_color.xyz );
    inter = _aaa_floats.values[ 3 ] * ( inter + _aaa_floats.values[ 2 ] );

    float4 dst_color;
    if( _aaa_ints.values[ 0 ] == 0 )
        dst_color = c_linear( _aaa_vec4s.values[ 0 ], _aaa_vec4s.values[ 1 ], _aaa_vec4s.values[ 2 ], _aaa_vec4s.values[ 3 ], inter );
    else
        dst_color = c_bezier( _aaa_vec4s.values[ 0 ], _aaa_vec4s.values[ 1 ], _aaa_vec4s.values[ 2 ], _aaa_vec4s.values[ 3 ], inter );

    return dst_color * _aaa_floats.values[ 1 ] + src_color * _aaa_floats.values[ 0 ];
}
