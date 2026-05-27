// PeteMetaImage.metal -- Path A port from shader/PeteMetaImage.frag (c88).
// FreeFrame-style tiled lookup with quantised tile centres.
// Patches :
//   - `texUnit` -> `_aaa_tex_0`
//   - mxw_* uniforms -> AaaFuFloats slots :
//       [0] = mxw_maxV
//       [1] = mxw_maxU
//       [2] = mxw_vertslider_scale_0x0_100x0_0x05_mxw
//   - `gl_TexCoord[0].xy` -> `in.uv.xy`
//   - `mod( x, max )` (positive) -> `fmod( x, max )`
//   - **Upstream bug** : engine source ends with `gl_FragColor = c * basecolorc;`
//     where `basecolorc` is **undefined**. Mac port stubs it to opaque-white
//     (the natural identity for the multiply). Documented inline.

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
    float const tiles = _aaa_floats.values[ 2 ];
    float const itiles = 1.0 / tiles;

    float2 tc = in.uv.xy;
    float2 basetc = ( floor( tc * tiles ) * itiles ) + ( itiles * 0.5 );
    (void) basetc;   // engine source computes but never reads it before the basecolorc multiply

    tc.y = fmod( tiles * tc.y, max_v );
    tc.x = fmod( tiles * tc.x, max_u );
    float4 c = _aaa_tex_0.sample( _aaa_samp, tc );

    //	`basecolorc` undefined in engine source -- stubbed to opaque-white identity.
    float4 const basecolorc_stub = float4( 1.0 );
    return c * basecolorc_stub;
}
