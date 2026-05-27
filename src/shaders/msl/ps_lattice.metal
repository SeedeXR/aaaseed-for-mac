// ps_lattice.metal -- Path A port from ps_lattice.frag (c92).
// Lattice shader : fract-of-scaled-UV threshold discard + diffuse + specular.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `in vec3 DiffuseColor` / `in vec3 SpecularColor` (P4 per-vertex) -> const stubs
//     DiffuseColor = float3(0.8), SpecularColor = float3(0.0)
//   - `aaa_fu_float[0..4]`  -> `_aaa_floats.values[0..4]`
//   - `gl_TexCoord[0].s/t`  -> `in.uv.x/y`
//   - `gl_Color`            -> opaque-white stub (P3)
//   - `discard`             -> `discard_fragment()`
//   - `blend( x )`          -> identity (c72)

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
    float3 const DiffuseColor  = float3( 0.8 );
    float3 const SpecularColor = float3( 0.0 );
    float4 const gl_Color      = float4( 1.0 );

    float const ss = fract( in.uv.x * _aaa_floats.values[ 2 ] );
    float const tt = fract( in.uv.y * _aaa_floats.values[ 3 ] );

    if( ss > _aaa_floats.values[ 0 ] && tt > _aaa_floats.values[ 1 ] )
        discard_fragment();

    float4 SurfaceColor;
    if( _aaa_floats.values[ 4 ] >= 1.0 )
        SurfaceColor = gl_Color;
    else
        SurfaceColor = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );

    float3 const finalColor = SurfaceColor.rgb * DiffuseColor + SpecularColor;
    return float4( finalColor, SurfaceColor.a );   // blend pass-through
}
