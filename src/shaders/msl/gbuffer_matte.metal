// gbuffer_matte.metal -- Path A port from vux_flare_render/gbuffer_matte.frag (c95).
// PBR gbuffer with metallic / specular workflow branching ; tex_y_offset.
// Patches : same family as gbuffer.metal (c95).
//   - SHADING_WORKFLOW macro -> Mac picks METALLIC_WORKFLOW (engine default)
//   - `material.diffuse` / `.roughness` / `.specular` / `.ao_factor` /
//     `.reflection_intensity` / `.ambient_intensity` engine globals
//     -> stubbed to identity (1.0 / vec3(1)) ; runtime can populate later

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

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
    texture2d< float >    _aaa_tex_0    [[texture(0)]],   // diffuse / g_diffuse_texture
    texture2d< float >    _aaa_tex_1    [[texture(1)]],   // normalmap / g_normalmap_texture
    texture2d< float >    _aaa_tex_2    [[texture(2)]],   // roughness
    texture2d< float >    _aaa_tex_3    [[texture(3)]],   // specular_metal
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    //	Per-vertex P4 stubs
    float3 const in_position_world = float3( 0.0 );
    float3 const in_normal_world   = float3( 0.0, 0.0, 1.0 );
    float3x3 const in_tangent_frame = float3x3( float3( 1.0, 0.0, 0.0 ),
                                                 float3( 0.0, 1.0, 0.0 ),
                                                 float3( 0.0, 0.0, 1.0 ) );

    //	material.* engine globals stubbed to identity
    float3 const mat_diffuse              = float3( 1.0 );
    float  const mat_roughness            = 1.0;
    float3 const mat_specular             = float3( 1.0 );
    float  const mat_ao_factor            = 1.0;
    float  const mat_reflection_intensity = 1.0;

    float const tex_y_offset = _aaa_floats.values[ 0 ];

    float2 const tex_coord = float2( in.uv.x, in.uv.y + tex_y_offset );

    if( _aaa_tex_0.sample( _aaa_samp, tex_coord ).a < 0.001 )
        discard_fragment();

    //	Normal-map branch
    float3 normal = _aaa_tex_1.sample( _aaa_samp, tex_coord ).xyz;
    normal = 2.0 * normal - 1.0;
    float3 const surface_normal = normalize( in_tangent_frame * normal );

    float3 const surface_diffuse   = _aaa_tex_0.sample( _aaa_samp, tex_coord ).xyz * mat_diffuse;
    float  const surface_roughness = _aaa_tex_2.sample( _aaa_samp, tex_coord ).x   * mat_roughness;
    //	make_surface_specular( mat.specular, tex_sample ) engine helper -> mat.specular * tex.rgb identity
    float3 const surface_specular  = mat_specular * _aaa_tex_3.sample( _aaa_samp, in.uv ).rgb;
    float3 const surface_emissive  = float3( 0.0 );

    //	METALLIC_WORKFLOW branch (engine default)
    float  const metallic = surface_specular.x;
    float3 const albedo   = surface_diffuse - surface_diffuse * metallic;
    float3 const spec_out = mix( float3( 0.03 ), surface_diffuse, metallic );

    FragmentOut out;
    out.out_diffuse_rough       = float4( albedo, surface_roughness );
    out.out_normal_occlusion    = float4( surface_normal, mat_ao_factor );
    out.out_specular_reflection = float4( spec_out, mat_reflection_intensity );
    out.out_emissive            = float4( surface_emissive, 1.0 );
    return out;
}
