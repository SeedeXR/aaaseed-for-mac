// ps_neand.metal -- Path A port from Neand/ps_neand.frag (c91, c86 carry recovered).
// Diffuse + mask + reflection compositing with three-stage fall-off ramp.
// Patches :
//   - `in VS_out { tex_impli, tex_refle, color, fall_off, z_to_eye, normal } fs_in;`
//     -> stage_in stub : only `fs_in.tex_impli.st` and `fs_in.fall_off` read in live path.
//        `fs_in.tex_impli.st` -> in.uv.xy
//        `fs_in.fall_off`     -> float 1.0 stub (no per-vertex falloff plumbing ; identity = "full")
//   - `aaa_tex2d[0..2]`     -> `_aaa_tex_0..2`
//   - `aaa_fu_float[0..4]`  -> `_aaa_floats.values[0..4]`
//   - `get_fall_off( f )`   engine helper -> identity passthrough (`return f`) ; preserves shape
//   - `out vec4 fragColor`  -> return value
//   - `l_get_tex_rgb1`      helper preserved as static inline (force RGB1 alpha)

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

static inline float4 l_get_tex_rgb1_(
    int unit, float2 uv,
    texture2d< float > t0, texture2d< float > t1, texture2d< float > t2,
    sampler samp )
{
    float4 c;
    if( unit == 0 )      c = t0.sample( samp, uv );
    else if( unit == 1 ) c = t1.sample( samp, uv );
    else                 c = t2.sample( samp, uv );
    return float4( c.xyz, 1.0 );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    texture2d< float >    _aaa_tex_2    [[texture(2)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    //	VS_out stubs (only tex_impli + fall_off read in live path)
    float2 const tex_impli = in.uv.xy;
    float  const fall_off  = 1.0;   // P4 stub : per-vertex falloff unplumbed

    float4 const colorDiffus = _aaa_floats.values[ 0 ] * l_get_tex_rgb1_( 0, tex_impli, _aaa_tex_0, _aaa_tex_1, _aaa_tex_2, _aaa_samp );
    float4 const colorMasque = l_get_tex_rgb1_( 1, tex_impli, _aaa_tex_0, _aaa_tex_1, _aaa_tex_2, _aaa_samp );
    float4 const colorRefle  = l_get_tex_rgb1_( 2, tex_impli, _aaa_tex_0, _aaa_tex_1, _aaa_tex_2, _aaa_samp );

    //	get_fall_off engine helper stubbed to identity
    float const f = fall_off;
    float f2;
    if( f < _aaa_floats.values[ 1 ] )
        f2 = _aaa_floats.values[ 2 ] + ( 1.0 - _aaa_floats.values[ 1 ] - 0.1 ) * ( 1.0 - ( 1.0 / _aaa_floats.values[ 1 ] ) * f );
    else
        f2 = _aaa_floats.values[ 2 ];

    float4 tex = colorMasque.r * colorDiffus
               + ( 1.0 - colorMasque.r ) * float4( colorDiffus.r, colorDiffus.g, colorDiffus.b, f2 );

    tex *= _aaa_floats.values[ 3 ] * colorRefle + ( 1.0 - _aaa_floats.values[ 3 ] ) * float4( 1.0 );
    tex *= _aaa_floats.values[ 4 ];
    return tex;
}
