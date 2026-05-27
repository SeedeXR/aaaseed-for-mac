// ps_Maa_gaussV.metal -- Path A port from ps_Maa_gaussV.frag (c83).
// Vertical-only Gaussian blur (kernel size hard-coded to 2.0 ; direction
// fixed to vec2(0,1) via local-variable shadowing of the uniform).
// Patches :
//   - `tex` -> `_aaa_tex_0`
//   - Original GLSL **shadows** the `direction` (float) uniform and
//     `kernel_size` (float) uniform with local `vec2`/`float` of the same
//     name inside main(). MSL forbids redeclaring a parameter with a
//     different type. **Resolved** : drop the uniforms (slots [0]/[1])
//     and use the hard-coded local values from the engine source.
//   - `size_cx` / `size_cy` -> AaaFuFloats[2..3] (kept for parity with gaussH).
//   - `gl_TexCoord[0].st` -> `in.uv.xy`.

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
    float  const kernel_size = 2.0;
    float  const rho         = 20.0;
    float2 const direction   = float2( 0.0, 1.0 );

    float const size_cx = _aaa_floats.values[ 2 ];
    float const size_cy = _aaa_floats.values[ 3 ];

    float const dx = 1.0 / size_cx;
    float const dy = 1.0 / size_cy;
    float2 const st = in.uv.xy;

    float4 color  = float4( 0.0 );
    float  weight = 0.0;
    for( float i = -kernel_size; i <= kernel_size; i += 1.0 )
    {
        float const fac = exp( -( i * i ) / ( 2.0 * rho * rho ) );
        weight += fac;
        color  += _aaa_tex_0.sample( _aaa_samp, st + float2( dx * i, dy * i ) * direction ) * fac;
    }

    return color / weight;
}
