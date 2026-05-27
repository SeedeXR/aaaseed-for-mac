// darkglow.metal -- Path A port from shader/darkglow.frag (c78).
// Patches : `texUnit` -> `_aaa_tex_0` ; mxw `limit` / `factor` sliders ->
// `_aaa_floats.values[0..1]` ; `gl_TexCoord[0].xy` -> `in.uv.xy`.
// Effect : if max(r,g,b) below `limit`, accelerate via pow(c, factor).

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
    //	[0] = mxw_vertslider_limit_0x0_1x0_0x2_mxw
    //	[1] = mxw_vertslider_factor_0x0_10x0_0x4_mxw
    float const limit  = _aaa_floats.values[ 0 ];
    float const factor = _aaa_floats.values[ 1 ];

    float4 c = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    float  m = max( max( c.r, c.g ), c.b );
    c.rgb = ( m < limit ) ? pow( c.rgb, float3( factor ) ) : c.rgb;
    return c;
}
