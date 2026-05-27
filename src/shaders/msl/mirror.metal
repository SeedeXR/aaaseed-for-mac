// mirror.metal -- Path A port from shader/mirror.frag (c78).
// Patches : `texUnit` -> `_aaa_tex_0` ; mxw uniforms (`mxw_maxV`,
// `mxw_vertslider_mirror`) -> `_aaa_floats.values[0..1]` ;
// `gl_TexCoord[0].xy` -> `in.uv.xy`. Effect : mirror along horizontal
// axis above threshold `f`.

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
    //	[0] = mxw_maxV
    //	[1] = mxw_vertslider_mirror_0x0_1x0_0x5_mxw
    float const max_v = _aaa_floats.values[ 0 ];
    float const slide = _aaa_floats.values[ 1 ];
    float const f     = slide * max_v;

    float2 tc = in.uv.xy;
    tc.y = ( tc.y < f ) ? tc.y : ( f - ( tc.y - f ) );

    return _aaa_tex_0.sample( _aaa_samp, tc );
}
