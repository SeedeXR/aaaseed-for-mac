// ps_Toy_CloudIq.metal -- Path A port from ps_Toy_CloudIq.frag (c101).
// **UPSTREAM ENTIRELY DEAD** : the engine source's body is wrapped in `#if 0`
// (line 21) ... `#endif` (line 653), so **no live `void main()` exists** in the
// GLSL source. The file declares uniforms and an `in ST_AAA_BV BV` interface
// block, but the rendering body is the Inigo Quilez cloud raymarcher in a
// disabled preprocessor block.
//
// Same broken-upstream pattern as c80 ps_earth (missing `)`) and c88
// PeteMetaImage (undefined `basecolorc` symbol) -- the live source would also
// fail to compile under any GLSL driver since `main()` is absent.
//
// Mac side : minimal stub fs_main that returns opaque-white (identity output)
// so the catalog compile test passes. The dead Inigo Quilez raymarcher body
// is preserved in source for archival ; if revived, port the LUT-noise branch
// + raymarch + render functions following the c87 fire.metal pattern.
//
// **Final engine-struct heavy port** -- engine-struct cluster fully cleared.

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
    return float4( 1.0, 1.0, 1.0, 1.0 );   // upstream body dead in #if 0 block
}
