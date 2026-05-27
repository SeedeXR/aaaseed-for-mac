// color_pure.metal -- Path A port from vux_flare_render/color_pure.frag (c77).
// `in vec4 in_color` -- vertex-stage color attribute ; Path A test rig
// has no vertex color stream. Substitute with opaque white.

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

fragment float4 fs_main( VertexOut in [[stage_in]] )
{
    (void) in;
    return float4( 1.0, 1.0, 1.0, 1.0 );   // in_color stub -- opaque white
}
