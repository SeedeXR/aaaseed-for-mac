// ps_qwartz_compo.metal -- Path A port from Qwartz/ps_qwartz_compo.frag (c89, Maa 2012).
// Two-texture composite : p2's red drives p1's alpha + RGB modulation.
// Patches :
//   - `aaa_tex2d[0..1]`     -> `_aaa_tex_0..1`
//   - `aaa_fu_float[0..4]`  -> `_aaa_floats.values[0..4]`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - Dead `#if 0` branch dropped.

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
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float2 co = in.uv.xy;
    float4 p1 = _aaa_tex_0.sample( _aaa_samp, co );

    co.x = _aaa_floats.values[ 0 ] + co.x * _aaa_floats.values[ 1 ];
    co.y = _aaa_floats.values[ 2 ] + co.y * _aaa_floats.values[ 3 ];
    float4 const p2 = _aaa_tex_1.sample( _aaa_samp, co );

    float const alpha = p2.r;
    p1.a    = alpha;
    p1.rgb *= 1.0 + ( alpha - 1.0 ) * _aaa_floats.values[ 4 ];
    return p1;
}
