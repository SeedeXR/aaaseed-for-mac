// ps_earth.metal -- Path A port from ps_earth.frag (c80, Franz Hildgen).
// Patches :
//   - `tex0` / `tex1` / `tex2`  -> `_aaa_tex_0..2`
//   - `in float Diffuse`        -> const 1.0 stub (P4 ; unplumbed on Mac)
//   - `in vec3 Specular`        -> const 0.0 stub (P4)
//   - `gl_TexCoord[0].st`       -> `in.uv.xy`
//   - Original source ends with `gl_FragColor = blend( vec4 ( color, 1.0 );`
//     -- a GLSL syntax error (missing `)`). Behaviour-preserving fix on Mac :
//     drop the engine helper, return `float4( color, 1.0 )` directly.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

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
    VertexOut          in          [[stage_in]],
    texture2d< float > _aaa_tex_0  [[texture(0)]],
    texture2d< float > _aaa_tex_1  [[texture(1)]],
    texture2d< float > _aaa_tex_2  [[texture(2)]],
    sampler            _aaa_samp   [[sampler(0)]] )
{
    float  const Diffuse  = 1.0;            // `in float Diffuse` stub
    float3 const Specular = float3( 0.0 );  // `in vec3 Specular` stub

    float2 const uv     = in.uv.xy;
    float2 const clouds = _aaa_tex_2.sample( _aaa_samp, uv ).rg;
    float3 const daytime =
        ( _aaa_tex_0.sample( _aaa_samp, uv ).rgb * Diffuse
          + Specular * clouds.g ) * ( 1.0 - clouds.r )
        + clouds.r * Diffuse;
    float3 const nighttime =
        _aaa_tex_1.sample( _aaa_samp, uv ).rgb
        * ( 1.0 - clouds.r ) * 2.0;

    float3 color = daytime;
    if( Diffuse < 0.1 )
        color = mix( nighttime, daytime, ( Diffuse + 0.1 ) * 5.0 );

    return float4( color, 1.0 );
}
