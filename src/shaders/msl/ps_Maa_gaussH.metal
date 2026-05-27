// ps_Maa_gaussH.metal -- Path A port from ps_Maa_gaussH.frag (c83).
// Single-direction Gaussian blur (kernel chooses H or V by `direction` < 0.5).
// Patches :
//   - `tex` (single non-aaa_ sampler) -> `_aaa_tex_0`
//   - 4 individual non-array uniforms -> AaaFuFloats slots :
//       [0] = direction (0 = H, 1 = V)
//       [1] = kernel_size (loop bound)
//       [2] = size_cx
//       [3] = size_cy
//   - `gl_TexCoord[0].st` -> `in.uv.xy`
//   - GLSL `for (float i = ...; i <= K; i += 1.0)` -- MSL accepts float loop
//     counter ; preserved verbatim.

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
    float const direction   = _aaa_floats.values[ 0 ];
    float const kernel_size = _aaa_floats.values[ 1 ];
    float const size_cx     = _aaa_floats.values[ 2 ];
    float const size_cy     = _aaa_floats.values[ 3 ];

    float const rho = 20.0;
    float2 const dir = direction < 0.5 ? float2( 1.0, 0.0 ) : float2( 0.0, 1.0 );
    float  const dx = 1.0 / size_cx;
    float  const dy = 1.0 / size_cy;
    float2 const st = in.uv.xy;

    float4 color  = float4( 0.0 );
    float  weight = 0.0;
    for( float i = -kernel_size; i <= kernel_size; i += 1.0 )
    {
        float const fac = exp( -( i * i ) / ( 2.0 * rho * rho ) );
        weight += fac;
        color  += _aaa_tex_0.sample( _aaa_samp, st + float2( dx * i, dy * i ) * dir ) * fac;
    }

    return color / weight;
}
