// inkcolor.metal -- Path A port from shader/inkcolor.frag (c82).
// Per-row ink-line effect with adjustable falloff exponent + no colour mult.
// Patches :
//   - `texUnit` -> `_aaa_tex_0`
//   - mxw uniforms -> AaaFuFloats slots :
//       [0] = mxw_maxV
//       [1] = mxw_maxU
//       [2] = mxw_vertslider_scale  (row count)
//       [3] = mxw_vertslider_scale2 (line falloff exponent)
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
    float const max_v = _aaa_floats.values[ 0 ];
    float const max_u = _aaa_floats.values[ 1 ];
    float const tiles = _aaa_floats.values[ 2 ];
    float const line  = _aaa_floats.values[ 3 ];

    float2 fac;
    fac.x = 1.0 / max_u;
    fac.y = 1.0 / max_v;

    float2 mid;
    mid.x = in.uv.x / max_u;
    mid.y = ( floor( in.uv.y * tiles / max_v ) + 0.5 ) / tiles;

    float const brightness = length( _aaa_tex_0.sample( _aaa_samp, mid / fac ).rgb );
    float const dist       = fabs( in.uv.y * fac.y - mid.y ) * tiles * 2.0;

    float4 c = float4( 0.999 );
    if( dist > brightness )
        c = ( dist > 0.95 ) ? float4( 0.0 )
                            : float4( pow( 1.0 - ( dist - brightness ), line ) );
    return c;
}
