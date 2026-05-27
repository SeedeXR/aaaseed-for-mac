// ps_Maa_suns.metal -- Path A port from ps_Maa_suns.frag (c89, Maa).
// Radial-burst "sun rays" from a centred origin with ray count + width.
// Patches :
//   - `aaa_fu_float[0..7]` -> AaaFuFloats[0..7]
//   - `aaa_fu_int[0]`      -> AaaFuInts[0] (ray count)
//   - `aaa_fu_vec4[0..1]`  -> AaaFuVec4s[0..1]
//   - `CONST` engine macro at file scope (referencing aaa_fu_float[N]) -> moved
//     into fs_main and demoted to `float const` (P5 ; engine `CONST` is a macro
//     trick to make these "uniform-init" pseudo-constants ; MSL forbids it).
//   - `PI2` engine global -> local `constant float PI2_`
//   - `gl_TexCoord[0].x/y` -> `in.uv.xy`
//   - `atan(y, x)` -> MSL `atan2(y, x)` ; `mod(x, 1.0)` -> `fmod(x, 1.0)`

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

constant float PI2_ = 6.28318530718;

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
    float const sun_radius = _aaa_floats.values[ 4 ] / 2.0;
    float const ray_radius = _aaa_floats.values[ 5 ] / 2.0;
    float const ray        = _aaa_floats.values[ 6 ];
    float const rot_offset = _aaa_floats.values[ 7 ] * PI2_;

    float2 v;
    v.x = ( in.uv.x - 0.5 - _aaa_floats.values[ 2 ] ) * _aaa_floats.values[ 0 ];
    v.y = ( in.uv.y - 0.5 - _aaa_floats.values[ 3 ] ) * _aaa_floats.values[ 1 ];

    float const d = length( v );

    float f;
    if( d < sun_radius )
        f = 0.0;
    else if( d > ray_radius )
        f = 1.0;
    else
    {
        float a = atan2( v.y, v.x ) + rot_offset;
        f = fmod( a * float( _aaa_ints.values[ 0 ] ) * 0.15915, 1.0 );
        f = ( f < ray ) ? 0.0 : 1.0;
    }

    return mix( _aaa_vec4s.values[ 0 ], _aaa_vec4s.values[ 1 ], f );
}
