// ps_Maa_drop.metal -- Path A port from ps_Maa_drop.frag
// Tool-generated then hand-patched (continuation 71) for :
//   - `in vec3 transformedNormal` -- a vertex-stage output the
//     engine emits from real geometry's normal-transform pipeline.
//     Our Path A test rig has no vertex normals ; substitute with
//     a constant `float3( 0.0, 0.0, 1.0 )` (forward-pointing) so the
//     shader compiles + produces deterministic output. A future
//     port driving real geometry would thread it through the vertex
//     stage / VertexOut.
//   - `gl_TexCoord[1].st` -- second texture-coordinate set. Our
//     pass-through vertex stage emits only `in.uv` (one UV set) ;
//     substitute with `in.uv.xy` (reuses the same coords for both
//     texture lookups -- functionally degenerate but compiles).
//
// Compiles + runs ; output is not visually meaningful in our test rig
// (no real normals). The catalog regression test only validates the
// MSL compile, not the rendered pixels for this shader.
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

constant float FACTOR        = -0.1f;
constant float FACTOR_SPHERE =  0.45f;

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
    //	`in vec3 transformedNormal` -- vertex-stage normal output.
    //	Path A test rig has no normals ; constant forward (+Z).
    float3 const transformedNormal = float3( 0.0, 0.0, 1.0 );

    float2 c;
    c = transformedNormal.xy * FACTOR_SPHERE + 0.5;
    float4 src1 = _aaa_tex_0.sample( _aaa_samp, c );

    //	gl_TexCoord[1] -> reuse in.uv (single-set on Mac vs.).
    c = in.uv.xy;
    float x = c.x - 0.5;
    float y = c.y - 0.5;

    float si = sin( _aaa_floats.values[ 3 ] );
    float co = cos( _aaa_floats.values[ 3 ] );

    c.x =   x * co + y * si;
    c.y = - x * si + y * co;

    c += transformedNormal.xy * _aaa_floats.values[ 2 ];

    c.x = c.x * _aaa_floats.values[ 0 ] + 0.5;
    c.y = c.y * _aaa_floats.values[ 1 ] + 0.5;
    float4 src2 = _aaa_tex_1.sample( _aaa_samp, c );

    float gr = dot( transformedNormal, float3( 0.0, 0.0, 1.0 ) );

    float4 dst;
    dst.xyz = mix( src1.xyz * 0.8, src2.xyz, gr );
    dst.a   = max( ( 1.0 - gr ), src2.a );

    float3 const n    = transformedNormal;
    float3 const norb = float3( 0.387, 0.387, 0.7 );
    gr = dot( n, norb );
    gr = gr * 5.6 - 4.0;
    if( gr > 0.0 )
    {
        dst.x = max( dst.x, gr );
        dst.y = max( dst.y, gr );
        dst.z = max( dst.z, gr );
        dst.a = max( dst.a, gr );
    }

    return dst;
}
