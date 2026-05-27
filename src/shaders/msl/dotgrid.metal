// dotgrid.metal -- Path A port from shader/dotgrid.frag (c82).
// Per-tile dot mask : dot whose radius = |tile-base-colour|.
// Patches :
//   - `texUnit` -> `_aaa_tex_0`
//   - mxw uniforms -> AaaFuFloats slots :
//       [0] = mxw_maxV
//       [1] = mxw_maxU
//       [2] = mxw_vertslider_scale_0x0_100x0_1x0_mxw  (tile count)
//   - `gl_TexCoord[0].x/y` -> `in.uv.x/y`

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
    float  const max_v = _aaa_floats.values[ 0 ];
    float  const max_u = _aaa_floats.values[ 1 ];
    float  const tiles = _aaa_floats.values[ 2 ];

    float2 fac;
    fac.x = 1.0 / max_u;
    fac.y = 1.0 / max_v;
    float const itiles = 1.0 / tiles;

    float2 mid;
    mid.x = floor( in.uv.x * tiles / max_u ) * itiles + ( itiles * 0.5 );
    mid.y = floor( in.uv.y * tiles / max_v ) * itiles + ( itiles * 0.5 );
    float4 const basecolor = _aaa_tex_0.sample( _aaa_samp, mid / fac );

    float const radius = length( basecolor.rgb );
    float const dist   = length( fabs( in.uv.xy * fac - mid ) ) * tiles * 2.0;

    return ( dist > radius ) ? float4( 0.0 ) : float4( 1.0 );
}
