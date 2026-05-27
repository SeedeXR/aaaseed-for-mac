// mosaic.metal -- Path A port from shader/mosaic.frag (c78).
// Patches : `texUnit` -> `_aaa_tex_0` ; mxw `scale` slider ->
// `_aaa_floats.values[0]` ; `gl_TexCoord[0].xy` -> `in.uv.xy`.
// Effect : quantise UVs to integer tile centres (classic TV-mosaic).

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
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    //	[0] = mxw_vertslider_scale_0x0_200x0_0x5_mxw
    float const tiles  = _aaa_floats.values[ 0 ];
    float const itiles = 1.0 / tiles;

    float2 tc = in.uv.xy;
    tc = ( floor( tc * tiles ) * itiles ) + ( itiles * 0.5 );

    return _aaa_tex_0.sample( _aaa_samp, tc );
}
