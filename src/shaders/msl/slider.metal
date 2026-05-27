// slider.metal -- Path A port from shader/slider.frag (c78).
// Patches : 4 individual mxw_* uniforms -> `AaaFuFloats.values[0..3]`
// (mxw convention : Mac binds per-frame as one buffer, slots match
// runtime upload order) ; `gl_TexCoord[0].xy` -> `in.uv.xy` ; texUnit
// -> `_aaa_tex_0`. Clamps tex coords via min/max against the 4 sliders.

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
    //	mxw uniform-name -> slot mapping (runtime must upload in this order) :
    //	  [0] = mxw_vertslider_v1coord_0x0_1x0_1x0_mxw
    //	  [1] = mxw_vertslider_u1coord_0x0_1x0_1x0_mxw
    //	  [2] = mxw_vertslider_v2coord_0x0_1x0_0x0_mxw
    //	  [3] = mxw_vertslider_u2coord_0x0_1x0_0x0_mxw
    float const v1 = _aaa_floats.values[ 0 ];
    float const u1 = _aaa_floats.values[ 1 ];
    float const v2 = _aaa_floats.values[ 2 ];
    float const u2 = _aaa_floats.values[ 3 ];

    float2 tc = in.uv.xy;
    tc.y = min( tc.y, v1 );
    tc.x = min( tc.x, u1 );
    tc.y = max( tc.y, v2 );
    tc.x = max( tc.x, u2 );

    return _aaa_tex_0.sample( _aaa_samp, tc );
}
