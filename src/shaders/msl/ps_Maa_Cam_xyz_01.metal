// ps_Maa_Cam_xyz_01.metal -- Path A port from ps_Maa_Cam_xyz_01.frag (c92, Maa 2008).
// Per-pixel z-driven mode switch ; close sibling of ps_Maa_Gradient_01 / ps_Maa_ImageFire_01.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[2..5]`  -> `_aaa_floats.values[2..5]`
//   - `aaa_fu_int[0..2]`    -> `_aaa_ints.values[0..2]`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gl_FragCoord.x/y/z`  -> `in.position.x/y/z`
//   - `gl_Color`            -> opaque-white stub (P3)
//   - `blend( src )`        -> identity (c72)
//   - `texture2D( tex, scalar )` (engine permissive) -> `float2(scalar, 0)`
//   - `mod( x, 1.0 )` (positive) -> `fmod( x, 1.0 )`
//   - `gl_FragColor`        -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

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
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float4 src = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    if( _aaa_ints.values[ 2 ] != 0 )
        src = src;   // blend pass-through

    float       s = _aaa_floats.values[ 2 ] + _aaa_floats.values[ 3 ] * in.position.z;
    if( _aaa_ints.values[ 1 ] != 0 )
        s = fmod( s, 1.0 );

    int const type = _aaa_ints.values[ 0 ];
    float4 dst = float4( 0.0, 0.0, 0.0, 1.0 );   // deterministic init

    if( type == 0 )
    {
        dst   = src;
        dst.a = dst.a * s;
    }
    else if( type == 1 )
    {
        dst.r = dst.g = dst.b = s;
        dst.a = 1.0;
    }
    else if( type == 2 )
    {
        dst.r = dst.g = dst.b = 1.0;
        dst.a = s;
    }
    else if( type == -1 )
    {
        dst = src;
    }
    else if( type == -2 )
    {
        dst = float4( 1.0 );   // gl_Color stub
    }
    else if( type == -3 )
    {
        dst = src * float4( 1.0 );
        dst.a = 1.0;
    }
    else if( type == -4 )
    {
        dst = src * float4( 1.0 );
    }
    else if( type == 4 )
    {
        float const sx = in.position.x * _aaa_floats.values[ 4 ];
        float const sy = in.position.y * _aaa_floats.values[ 5 ];
        dst.r = dfdx( _aaa_tex_0.sample( _aaa_samp, float2( sx, 0.0 ) ).r ) * 32.0;
        dst.g = dfdy( _aaa_tex_0.sample( _aaa_samp, float2( sy, 0.0 ) ).g ) * 32.0;
        dst.b = 1.0;
        dst.a = 1.0;
    }
    else
    {
        dst.r = fmod( dfdx( in.position.x * _aaa_floats.values[ 4 ] ), 1.0 );
        dst.g = fmod( dfdy( in.position.y * _aaa_floats.values[ 5 ] ), 1.0 );
        dst.b = src.b;
        dst.a = s;
    }
    return dst;
}
