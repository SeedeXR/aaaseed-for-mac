// star.metal -- Path A port from star.frag (c81).
// 5-branch star shape via polar-folded angle mask + line-distance falloff.
// Patches :
//   - `in VS_out { vec4 pos_world; vec4 pos_ec; vec3 nor_ec; float alpha;
//                  vec4 color; vec4 tex_coor[4]; } fs_in;` (P4c interface
//     block) -> stage_in stubs : tex_coor[0]=float4(in.uv,0,0) ; the rest
//     are not read in the live code path.
//   - `PI2` / `PI` engine globals -> local constexpr.
//   - `aaa_fu_float[0..4]`   -> `_aaa_floats.values[0..4]`
//   - `aaa_fu_vec4[0..1]`    -> `_aaa_vec4s.values[0..1]`
//   - `blend_fs( ... )` in dead `#if 0` branch -- dropped.
//   - `gl_FragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };

constant float PI_  = 3.14159265358979323846;
constant float PI2_ = 2.0 * PI_;
constant float BRANCH_NB = 5.0;

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
    //	fs_in.tex_coor[0].st <- in.uv (P4c stub)
    float2 uv = in.uv.xy;
    uv = ( uv - 0.5 ) * 2.0;

    float const l     = length( uv );
    float       angle = atan2( uv.y, uv.x );
    angle = angle / PI2_ + 0.5;
    angle = fabs( fract( angle * BRANCH_NB ) - 0.5 ) * 2.0;
    angle *= PI_ / BRANCH_NB;

    float const b = _aaa_floats.values[ 2 ];
    float const a = _aaa_floats.values[ 3 ] * b;

    float f = -a * l * cos( angle ) - b * l * sin( angle ) + a * b;
    f /= sqrt( a * a + b * b );

    f = mix( f, 1.0 - l, _aaa_floats.values[ 4 ] );
    f = smoothstep( _aaa_floats.values[ 1 ] * _aaa_floats.values[ 0 ],
                    _aaa_floats.values[ 0 ],
                    1.0 - f );

    return mix( _aaa_vec4s.values[ 0 ], _aaa_vec4s.values[ 1 ], f );
}
