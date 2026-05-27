// multipler.metal -- Path A port from shader/multipler.frag (c82).
// UV repeat / wrap modulo mxw_maxU and mxw_maxV (rectangular-image-inside-square).
// Patches :
//   - `texUnit` -> `_aaa_tex_0`
//   - mxw uniforms -> AaaFuFloats slots :
//       [0] = mxw_maxV
//       [1] = mxw_maxU
//       [2] = mxw_vertslider_scaleV_0x0_10x0_0x5_mxw
//       [3] = mxw_vertslider_scaleH_0x0_10x0_0x5_mxw
//   - `gl_TexCoord[0].xy` -> `in.uv.xy`
//   - `mod(x, y)` -> MSL `fmod(x, y)` (GLSL mod uses floor ; MSL fmod uses trunc -- behaviour
//     matches for the positive UV range expected here)

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
    float const max_v = _aaa_floats.values[ 0 ];
    float const max_u = _aaa_floats.values[ 1 ];
    float const v     = _aaa_floats.values[ 2 ];
    float const h     = _aaa_floats.values[ 3 ];

    float2 tc = in.uv.xy;
    tc.y = fmod( v * tc.y, max_v );
    tc.x = fmod( h * tc.x, max_u );

    return _aaa_tex_0.sample( _aaa_samp, tc );
}
