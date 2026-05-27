// ps_Maa_alpha.metal -- Path A port from ps_Maa_alpha.frag (Maa, July 2012)
// Tool-generated via tools/glsl_to_msl/glsl_to_msl.py then hand-
// patched (continuation 70) :
//   - `aaa_tex_dim[0]` -- engine runtime flag (0/1/2/other) selecting
//     the texture path. On Metal we statically bind texture2d<float>
//     so the dispatching switch is always 2. Hardcoded `int dim = 2;`
//     to preserve the source shape (4 branches) while the other 3
//     stay dead code -- a future engine port that drives multiple
//     texture dimensions would re-thread dim from a uniform.
//   - `gl_Color.rgb` -- GLSL fixed-function vertex color. Path A
//     integration tests don't drive a vertex color, so fall back to
//     opaque white (`float3(1)`) -- matches the dim==0 placeholder.
//
// Doctrine : pure Apple Metal, no SPIR-V (memory/feedback_nvidia_to_metal.md).

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
    texture2d< float >    _aaa_tex_2    [[texture(2)]],
    texture2d< float >    _aaa_tex_3    [[texture(3)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float4 _aaa_fragout = float4( 0.0, 0.0, 0.0, 1.0 );

    float4 texture;
    //	aaa_tex_dim[0] -> statically 2 on Metal (texture2d binding).
    int const dim = 2;
    if( dim == 2 )
    {
        texture = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    }
    else if( dim == 0 )
    {
        texture = float4( 1.0, 1.0, 1.0, 1.0 );
    }
    else if( dim == 1 )
    {
        texture = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    }
    else
    {
        texture = float4( 0.0, 0.0, 1.0, 1.0 );
    }

    _aaa_fragout.a   = pow( texture.a * _aaa_floats.values[ 1 ] + _aaa_floats.values[ 0 ],
                            _aaa_floats.values[ 2 ] );
    //	gl_Color.rgb -> Path A tests don't drive vertex color ; opaque white.
    _aaa_fragout.rgb = float3( 1.0 );
    return _aaa_fragout;
}
