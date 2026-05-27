// Toon.metal -- Path A port from Toon.frag (c79).
// Original by Philip Rideout, (c) 2005 3Dlabs Inc. Ltd.
// Patches :
//   - `in float f`              -> const default 0.5 (P4 stub on Mac)
//   - `DiffuseColor` / `PhongColor` (vec3) -> AaaFuVec4s[0..1] .rgb
//   - `Edge` / `Phong` (float)             -> AaaFuFloats[0..1]
//   - `gl_FragColor`                       -> return value

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    (void) in;
    //	`in float f` stub : without per-vertex normals plumbing on Mac
    //	(deferred), substitute a constant. Pipeline shape preserved.
    float const f = 0.5;

    //	[0..1] vec4 = DiffuseColor / PhongColor
    //	[0..1] float = Edge / Phong
    float3 const DiffuseColor = _aaa_vec4s.values[ 0 ].rgb;
    float3 const PhongColor   = _aaa_vec4s.values[ 1 ].rgb;
    float  const Edge         = _aaa_floats.values[ 0 ];
    float  const Phong        = _aaa_floats.values[ 1 ];

    float3 color = DiffuseColor;
    if( fabs( f ) < Edge )
        color = float3( 0.0 );
    if( f > Phong )
        color = PhongColor;

    return float4( color, 1.0 );
}
