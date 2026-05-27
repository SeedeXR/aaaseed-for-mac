// ps_Maa_Copy_01.metal -- Path A port from ps_Maa_Copy_01.frag
// Tool-generated via tools/glsl_to_msl/glsl_to_msl.py then hand-
// patched (continuation 69) for :
//   - `aaa_tex_dim[0]==2` -- engine "is this a 2D texture?" flag.
//     On Metal we statically know the binding is texture2d so the
//     branch is always true. Replaced with a compile-time `true`.
//   - `gl_FragCoord.xy` -- Metal exposes the fragment's screen-space
//     position via the [[position]] semantic on the VertexOut struct.
//     Replaced with `in.position.xy`.
//   - `gl_Color` -- GLSL fixed-function vertex color. Falls back to
//     opaque black ; in Path A integration tests we always bind the
//     texture so this branch is unreachable.
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
    VertexOut                in            [[stage_in]],
    texture2d< float >       aaa_tex2d_0   [[texture(0)]],
    sampler                  _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats&    _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&      _aaa_ints     [[buffer(1)]] )
{
    float4 _aaa_fragout = float4( 0.0, 0.0, 0.0, 1.0 );

    //	Engine source asks `if( aaa_tex_dim[0]==2 )` -- guards against
    //	the slot being a 3D texture. In Metal the binding type is
    //	statically declared (texture2d<float>) so the branch is always
    //	taken. Keep the `if(true)` to preserve the engine's nested
    //	scope shape for diff hygiene.
    if( true )
    {
        //	gl_FragCoord -> Metal's fragment [[position]] (already in
        //	pixels, top-left origin).
        float2 uv = in.position.xy;
        uv.x += float( -_aaa_ints.values[ 0 ] + _aaa_ints.values[ 2 ] );
        uv.y += float( -_aaa_ints.values[ 1 ] + _aaa_ints.values[ 3 ] );
        uv /= float2( aaa_tex2d_0.get_width( 0 ), aaa_tex2d_0.get_height( 0 ) );

        if( _aaa_floats.values[ 2 ] > 0.5
            && ( uv.x < 0.0 || 1.0 < uv.x || uv.y < 0.0 || 1.0 < uv.y ) )
            discard_fragment();

        float4 color = float4( 0.0, 0.0, 0.0, 1.0 );
        color.rgb = aaa_tex2d_0.sample( _aaa_samp, uv ).rgb;
        color.a   = 1.0;
        _aaa_fragout = color;
    }
    //	Else-branch's GLSL `gl_Color` fallback is unreachable on Metal --
    //	keep _aaa_fragout's opaque-black default if it ever fires.

    return _aaa_fragout;
}
