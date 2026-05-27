// ps_Maa_noise_cheap.metal -- Path A port from ps_Maa_noise_cheap.frag
// Hand-patched (continuation 76) for :
//   - `gl_TexCoord[0].xyz` reads 3 components ; our pass-through vs
//     emits `in.uv` (float2). Fabricate the z via
//     `float3( in.uv.xy, 0.0 )`.
//   - `gl_TexCoord[0].st` → `in.uv.xy`.
//
// No `blend()`, no `transformedNormal`, no fixed-function : the cheapest
// possible Path A port. Doctrine : pure Apple Metal.

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
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float3 location = float3( in.uv.xy, 0.0 );
    float3 floorvec = float3( floor( _aaa_floats.values[ 0 ] * location.x ),
                              floor( _aaa_floats.values[ 0 ] * location.y ),
                              0.0 );
    float3 noise = location * _aaa_floats.values[ 0 ] - floorvec - 0.5;
    noise *= noise;
    location += noise * _aaa_floats.values[ 2 ] + _aaa_floats.values[ 1 ];

    float4 color = _aaa_tex_0.sample( _aaa_samp, location.xy );
    color.a = 1.0;
    return color;
}
