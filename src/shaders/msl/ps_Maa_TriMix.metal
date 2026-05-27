// ps_Maa_TriMix.metal -- Path A port from ps_Maa_TriMix.frag
// Tool-generated then hand-patched (continuation 75) for :
//   - `CONST int mode = aaa_fu_int[0];` at file scope -- references a
//     uniform buffer, can't live at namespace scope in MSL. Moved
//     inside fs_main as a local.
//   - Tool emitted `return;` (void) for the early-exit branch ; MSL
//     fragment must return float4. Patched to `return col;`.
//   - `gl_TexCoord[0].st` → `in.uv.xy`.
//   - `gray( mask.xyz )` -- already provided by aaa_prelude.mslh.
//
// 3-texture mixing : tex0 is the mask (gray-or-alpha derived weight),
// tex1 + tex2 are blended via smoothstep'd grey weight.
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
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    texture2d< float >    _aaa_tex_2    [[texture(2)]],
    texture2d< float >    _aaa_tex_3    [[texture(3)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    //	`CONST int mode = aaa_fu_int[0];` -- file-scope in GLSL ;
    //	on MSL the uniform isn't visible at namespace scope, so move
    //	the binding inside the fragment entry.
    int const mode = _aaa_ints.values[ 0 ];

    float4 mask = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    float  grey;
    if( _aaa_ints.values[ 1 ] == 1 )
        grey = mask.a;
    else
        grey = gray( mask.xyz );
    grey = smoothstep( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ], grey );

    float4 src1 = _aaa_tex_1.sample( _aaa_samp, in.uv.xy );
    src1 *= _aaa_vec4s.values[ 1 ];

    float4 src2 = _aaa_tex_2.sample( _aaa_samp, in.uv.xy );
    src2 *= _aaa_vec4s.values[ 2 ];

    //	Early-exit path : modes 1/2/3 just pick one of the inputs.
    //	MSL fragment must return float4 ; `return col;` not `return;`.
    if( mode <= 3 )
    {
        float4 col = float4( 0.0 );
        if( mode == 1 )
            col = src1;
        else if( mode == 2 )
            col = src2;
        else if( mode == 3 )
            col = mask;
        return col;
    }

    if( mode == 4 )
        src2 = float4( 0.0 );
    else if( mode == 5 )
    {
        src1 = float4( 0.0 );
        grey = 1.0 - grey;
    }

    if( grey > 1.0 )
        grey = 1.0;
    else if( grey < 0.0 )
        grey = 0.0;
    if( _aaa_ints.values[ 1 ] == 1 )
        grey = 1.0 - grey;

    float4 dst;
    dst.rgba = src1.rgba * grey + src2.rgba * ( 1.0 - grey );
    dst *= _aaa_vec4s.values[ 0 ];
    return dst;
}
