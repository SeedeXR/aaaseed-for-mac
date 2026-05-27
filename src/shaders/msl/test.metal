// test.metal -- Path A port from ShaderToy/test.frag (c89).
// Processing-style swirling color field via iterated UV perturbation.
// Patches :
//   - `time` / `resolution` (individual non-array uniforms) -> AaaFuFloats[0..2]
//     ([0] = time, [1] = resolution.x, [2] = resolution.y) -- ShaderToy idiom
//   - file-scope `float intense = .5;` etc. (P5) -> `constant`
//   - `gl_FragCoord.xy` -> `in.position.xy`
//   - `precision mediump float;` -- MSL has no precision qualifiers, ignored

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant float intense_    = 0.5;
constant float speed_      = 5.0;
constant float2 graininess_ = float2( 1.0, 1.0 );
constant float offset_     = 20.0;
constant int   complexity_ = 38;
constant float Pi_         = 3.14159;

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
    float  const time         = _aaa_floats.values[ 0 ];
    float2 const resolution   = float2( _aaa_floats.values[ 1 ], _aaa_floats.values[ 2 ] );

    float2 p = ( 2.0 * in.position.xy - resolution ) / max( resolution.x, resolution.y );

    for( int i = 1; i < complexity_; i++ )
    {
        float2 newp = p;
        newp.x += graininess_.x / float( i ) * sin( float( i ) * p.y + time / speed_ + 0.3 * float( i ) )       + offset_;
        newp.y += graininess_.y / float( i ) * sin( float( i ) * p.x + time / speed_ + 0.3 * float( i + 100 ) ) + offset_;
        p = newp;
    }

    float3 col = float3(
        intense_ * sin( 3.0 * p.x )      + intense_,
        intense_ * sin( 3.0 * p.y )      + intense_,
        intense_ * sin( p.x + p.y )      + intense_ );
    col.g = col.r;
    return float4( col, 1.0 );
}
