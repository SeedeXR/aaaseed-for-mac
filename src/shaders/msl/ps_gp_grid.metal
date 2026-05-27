// ps_gp_grid.metal -- Path A port from ps_gp_grid.frag (c79).
// Patches : `aaa_fu_float[N]` -> `_aaa_floats.values[N]` ; `aaa_fu_int[0]`
// -> `_aaa_ints.values[0]` ; `aaa_fu_vec4[N]` -> `_aaa_vec4s.values[N]` ;
// `gl_TexCoord[0].st` -> `in.uv.xy` ; `gl_FragColor` -> return value.
// Effect : Cartesian grid lookup -- returns one of three palette colours
// based on cell position.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

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
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float2 uv = in.uv.xy;
    uv.x *= _aaa_floats.values[ 0 ];
    uv.y *= _aaa_floats.values[ 1 ];

    float const rep = float( _aaa_ints.values[ 0 ] );
    float const d   = _aaa_floats.values[ 3 ] * 0.5;
    float2 uvb = fract( ( uv + d ) / rep ) * rep;

    if( any( uvb < float2( _aaa_floats.values[ 3 ] ) ) )
        return _aaa_vec4s.values[ 2 ];

    uv = fabs( fract( uv - 0.5 ) - 0.5 );
    float const f = min( uv.x, uv.y );
    return _aaa_vec4s.values[ ( f < ( _aaa_floats.values[ 2 ] * 0.5 ) ) ? 1 : 0 ];
}
