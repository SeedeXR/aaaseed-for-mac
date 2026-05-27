// ps_chromakey_norm.metal -- Path A port from ps_chromakey_norm.frag (c79).
// Normalized chroma-key distance test with discard + opacity falloff.
// Patches :
//   - `tex0` -> `_aaa_tex_0`
//   - `aaa_fu_vec4[0]` -> `_aaa_vec4s.values[0]`
//   - `fu_float[0..2]` -> `_aaa_floats.values[0..2]`
//   - `gl_TexCoord[0].st` -> `in.uv.xy`
//   - `discard` -> MSL `discard_fragment()`
//   - Source had `vec4 color_tex = texture2D(...).rgb` -- compile-illegal
//     type narrowing on input ; corrected to `vec3` here to match the
//     subsequent `.rgb` accesses (zero-hallucination upstream patch).

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float3 const chroma_color    = _aaa_vec4s.values[ 0 ].rgb;
    float  const orig_per        = _aaa_floats.values[ 0 ];
    float  const threshold       = _aaa_floats.values[ 1 ];
    float  const threshold_delta = _aaa_floats.values[ 2 ] + threshold;

    float3 const color_tex = _aaa_tex_0.sample( _aaa_samp, in.uv.xy ).rgb;
    float  const dist      = distance( normalize( color_tex ), normalize( chroma_color ) );

    if( dist > threshold_delta )
        discard_fragment();

    float alpha;
    if( dist < threshold )
        alpha = 1.0;
    else
        alpha = 1.0 - ( dist - threshold ) / threshold_delta;

    float4 const final_color = float4( color_tex, alpha );
    return final_color * orig_per + ( 1.0 - orig_per ) * float4( color_tex, 1.0 );
}
