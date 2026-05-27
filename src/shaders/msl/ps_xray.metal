// ps_xray.metal -- Path A port from ps_xray.frag (c81).
// X-ray Fresnel effect : opacity = 1 - pow(|dot(N, -I)|, edgefalloff).
// Patches :
//   - `in vec3 N` / `in vec3 I` / `in vec4 Cs` (P4 per-vertex)
//     stubbed to constants on Mac : N=(0,0,1) view-aligned ; I=(0,0,-1) ;
//     Cs=opaque-white. Pipeline shape preserved ; real per-vertex
//     normal+eye-vector plumbing is its own session.
//   - `fu_float[0]` (non-aaa_ array) -> `_aaa_floats.values[0]`
//   - `gl_FragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    (void) in;
    float3 const N  = float3( 0.0, 0.0, 1.0 );    // P4 stub
    float3 const I  = float3( 0.0, 0.0, -1.0 );   // P4 stub
    float4 const Cs = float4( 1.0, 1.0, 1.0, 1.0 );  // P4 stub

    float const edgefalloff = _aaa_floats.values[ 0 ];

    float opac = dot( normalize( N ), normalize( -I ) );
    opac = fabs( opac );
    opac = 1.0 - pow( opac, edgefalloff );

    return float4( opac * Cs.x, opac * Cs.y, opac * Cs.z, opac );
}
