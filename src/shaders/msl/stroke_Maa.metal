// stroke_Maa.metal -- Path A port from stroke_Maa.frag (c89, Maa 2016).
// Engine source's main body is entirely commented out ; live path is just
// `gl_FragColor = gl_Color;`. On Mac : `gl_Color` -> opaque-white stub (P3 ;
// no per-vertex color plumbing yet). Whole shader returns float4(1.0).
// Documented commented-out original logic in source header for archival.

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
    return float4( 1.0, 1.0, 1.0, 1.0 );   // gl_Color stub (P3)
}
