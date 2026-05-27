// ps_Maa_by_n.metal -- Path A port from ps_Maa_by_n.frag (c84).
// Polar-coordinate kaleidoscope : N-fold rotational symmetry + UV warp + texture.
// Patches :
//   - `aaa_samp0` -> `_aaa_tex_0`
//   - `aaa_fu_float[0..2]` -> `_aaa_floats.values[0..2]`
//   - `aaa_fu_int[0..1]`   -> `_aaa_ints.values[0..1]`
//   - `aaa_fu_vec4[0]`     -> `_aaa_vec4s.values[0]`
//   - `gl_TexCoord[0].xy`  -> `in.uv.xy`
//   - `atan(y, x)`         -> MSL `atan2(y, x)`
//   - `mod(x, K)`          -> MSL `fmod(x, K)`
//   - `abs(...)` on float  -> `fabs(...)`
//   - `gl_FragColor`       -> return value

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
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float2 uv = in.uv.xy;
    uv = ( uv - 0.5 ) * 2.0;

    float       angle = atan2( uv.y, uv.x );
    float const d     = length( uv );

    angle = angle / ( 3.14159 * 2.0 );
    if( angle < 0.0 )
        angle += 1.0;

    angle -= _aaa_floats.values[ 1 ];
    angle *= _aaa_floats.values[ 0 ];

    if( _aaa_ints.values[ 1 ] == 0 )
    {
        angle = fmod( fabs( angle ), 2.0 );
        if( angle >= 1.0 )
            angle = 2.0 - angle;
    }
    else
    {
        angle = fmod( fabs( angle ), 1.0 );
    }

    float const a = angle;

    //	f / "1 + tan^2 = sec^2" reduction is computed but **not used** further --
    //	preserved for parity with engine source.
    float f = fmod( angle * _aaa_floats.values[ 0 ] * 2.0 / 3.14159, 1.0 ) * 2.0;
    if( f > 1.0 )
        f = 2.0 - f;
    f = tan( f * 3.14159 * 0.25 );
    f = sqrt( 1.0 + f * f );
    (void) f;

    angle *= 3.14159 * 0.5;

    uv.x = cos( angle ) * d;
    uv.y = sin( angle ) * d;

    uv -= _aaa_vec4s.values[ 0 ].xy;
    uv *= _aaa_vec4s.values[ 0 ].zw;

    float4 color = _aaa_tex_0.sample( _aaa_samp, uv );
    if( _aaa_ints.values[ 0 ] == -1 )
        color = float4( a, a, a, 1.0 );
    if( any( uv < float2( 0.0 ) ) || any( uv > float2( 1.0 ) ) )
        color *= _aaa_floats.values[ 2 ];
    return color;
}
