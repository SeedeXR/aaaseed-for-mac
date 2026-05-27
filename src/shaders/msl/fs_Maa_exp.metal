// fs_Maa_exp.metal -- Path A port from fs_Maa_exp.frag (c79).
// Patches :
//   - `in VS_out { vec4 pos_world; vec4 pos_ec; vec4 color; } vs_out`
//     (P4c interface block) -> opaque-white stub for vs_out.color on Mac
//     until vertex-side interface plumbing lands.
//   - Macros F1..F4 reference aaa_fu_float[0..3] but the body never
//     uses them in the live code path -- preserved as comments below.
//   - `gl_FragColor` -> return value.

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
    //	vs_out.color via interface block (P4c) -- opaque-white stub.
    //	F1..F4 macros (aaa_fu_float[0..3]) referenced in commented-out
    //	debug lines only ; not in live path.
    return float4( 1.0, 1.0, 1.0, 1.0 );
}
