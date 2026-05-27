// ps_Maa_normal.metal -- Path A port from ps_Maa_normal.frag
// Tool-generated then hand-patched (continuation 72) for :
//   - `in vec3 transformedNormal` -- c71 substitution : const
//     `float3( 0.0, 0.0, 1.0 )` (forward-Z).
//   - `gl_TexCoord[0].st` -- substitute `in.uv.xy`.
//   - `vec3 nor = { 0., 0., 1. };` -- C99 brace-init not legal in MSL
//     for vector types ; use `float3( 0., 0., 1. )` ctor form.
//   - `blend( src )` -- engine helper from include/blend.glsl ; pulls
//     gl_Color + aaa_tex_env_mode[] + gl_TextureEnvColor[]. All fixed-
//     function. Substitute with `src` pass-through (GL_REPLACE mode --
//     mode == 1 case of blend_low). Catalog test only validates compile.
//
// Doctrine : pure Apple Metal (memory/feedback_nvidia_to_metal.md).

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
    sampler               _aaa_samp     [[sampler(0)]] )
{
    //	`in vec3 transformedNormal` -- forward-Z default (c71 pattern).
    float3 const transformedNormal = float3( 0.0, 0.0, 1.0 );

    float4 src = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );

    float3 const nor = float3( 0.0, 0.0, 1.0 );
    float        gr  = dot( transformedNormal, nor );
    src.r = fabs( transformedNormal.x );
    src.g = fabs( transformedNormal.y );
    src.b = gr;

    //	`blend( src )` -> pass-through (GL_REPLACE branch of engine
    //	blend.glsl). Catalog regression only validates compile.
    return src;
}
