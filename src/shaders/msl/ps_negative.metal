// ps_negative.metal -- Path A port from ps_negative.frag (c77).
// Patches : `gl_TexCoord[0].st` -> `in.uv.xy` ; `blend(color)` ->
// `color` pass-through (GL_REPLACE behavior, c72 pattern).

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

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
    VertexOut          in          [[stage_in]],
    texture2d< float > _aaa_tex_0  [[texture(0)]],
    sampler            _aaa_samp   [[sampler(0)]] )
{
    float4 color = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    float  alpha = color.a;
    color = 1.0 - color;
    color.a = alpha;
    return color;
}
