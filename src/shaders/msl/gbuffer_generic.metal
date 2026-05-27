// gbuffer_generic.metal -- Path A port from vux_flare_render/gbuffer_generic.frag (c95).
// Generic PBR gbuffer with use_textures toggle ; alpha-discard + flat-or-smooth normal.
// Patches : same MRT family as gbuffer.metal (c95).
//   - `aaa_fu_int[0]` (flat_normal) / `[1]` (use_textures) -> AaaFuInts[0..1]
//   - `aaa_fu_vec4[0]` (uv_xy_fxy) -> AaaFuVec4s[0]
//   - `aaa_fu_float[0]` (alpha_threshold) -> AaaFuFloats[0]
//   - `aaa_material.material` engine global -> identity stubs

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

struct FragmentOut
{
    float4 out_diffuse_rough        [[color(0)]];
    float4 out_normal_occlusion     [[color(1)]];
    float4 out_specular_reflection  [[color(2)]];
    float4 out_emissive             [[color(3)]];
};

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

fragment FragmentOut fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],   // diffuse
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    //	Per-vertex P4 stubs
    float3 const in_position_world = float3( in.uv.x, in.uv.y, 0.0 );
    float3 const in_normal_world   = float3( 0.0, 0.0, 1.0 );

    int    const flat_normal      = _aaa_ints.values[ 0 ];
    int    const use_textures     = _aaa_ints.values[ 1 ];
    float4 const uv_xy_fxy        = _aaa_vec4s.values[ 0 ];
    float  const alpha_threshold  = _aaa_floats.values[ 0 ];

    float2 const uv = ( in.uv + uv_xy_fxy.xy ) * uv_xy_fxy.wz;

    float4 const surface_diffuse_sample = ( use_textures == 1 )
        ? _aaa_tex_0.sample( _aaa_samp, uv )
        : float4( 1.0 );
    if( surface_diffuse_sample.a < alpha_threshold )
        discard_fragment();

    float3 surface_normal;
    if( flat_normal == 1 )
    {
        float3 const dx = dfdx( in_position_world );
        float3 const dy = dfdy( in_position_world );
        surface_normal = normalize( cross( dx, dy ) );
    }
    else
    {
        surface_normal = normalize( in_normal_world );
    }

    //	material.* engine globals -- identity stubs
    float  const mat_roughness            = 1.0;
    float3 const mat_specular             = float3( 1.0 );
    float3 const mat_emissive             = float3( 0.0 );
    float  const mat_ao_factor            = 1.0;
    float  const mat_reflection_intensity = 1.0;

    //	do_surface_material engine helper stubbed to identity passthrough
    FragmentOut out;
    out.out_diffuse_rough       = float4( surface_diffuse_sample.rgb, mat_roughness );
    out.out_normal_occlusion    = float4( surface_normal, mat_ao_factor );
    out.out_specular_reflection = float4( mat_specular, mat_reflection_intensity );
    out.out_emissive            = float4( mat_emissive, 1.0 );
    return out;
}
