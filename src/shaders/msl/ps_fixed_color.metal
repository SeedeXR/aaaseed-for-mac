// ps_fixed_color.metal -- Path A port from ps_fixed_color.frag (c78).
// Patches :
//   - `in float alpha`         -> const default 1.0 (P4 pattern, c70)
//   - `gl_Color`               -> opaque-white fallback (P3, c70)
//   - `gl_SecondaryColor`      -> float4( 0 ) (legacy 2.x fixed-pipeline,
//                                  no MSL equivalent ; runtime treats
//                                  secondary color as additive black on
//                                  Mac until per-vertex SC plumbing lands)
//   - `gl_FragColor`           -> return value

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
    float const alpha = 1.0;   // `in float alpha` stub (P4)

    float4 color = float4( 1.0, 1.0, 1.0, 1.0 );   // gl_Color stub (P3)
    color += float4( 0.0, 0.0, 0.0, 0.0 );          // gl_SecondaryColor stub
    color  = clamp( color, 0.0, 1.0 );
    color.a = alpha;

    return color;
}
