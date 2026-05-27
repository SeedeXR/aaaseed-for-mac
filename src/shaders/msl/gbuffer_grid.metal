// gbuffer_grid.metal -- Path A port from vux_flare_render/gbuffer_grid.frag (c95).
// Flat-normal PBR gbuffer ; grid-mesh variant.
// Patches : same family as gbuffer.metal / gbuffer_matte.metal (c95).

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

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
    texture2d< float >    _aaa_tex_1    [[texture(1)]],   // normalmap
    texture2d< float >    _aaa_tex_3    [[texture(3)]],   // specular_metal
    sampler               _aaa_samp     [[sampler(0)]] )
{
    //	Per-vertex P4 stubs (flat-normal branch uses dFdx/dFdy of world pos -- stub identity)
    float3 const in_position_world = float3( in.uv.x, in.uv.y, 0.0 );   // dFdx/dFdy yields a valid normal

    //	material.* engine globals stubbed to identity
    float3 const mat_diffuse              = float3( 1.0 );
    float  const mat_roughness            = 1.0;
    float3 const mat_specular             = float3( 1.0 );
    float  const mat_ao_factor            = 1.0;
    float  const mat_reflection_intensity = 1.0;

    float2 const tex_coord = in.uv;

    if( _aaa_tex_0.sample( _aaa_samp, tex_coord ).a < 0.001 )
        discard_fragment();

    //	FLAT_NORMAL branch (engine source has FLAT_NORMAL == 1 for grid variant)
    float3 const dx = dfdx( in_position_world );
    float3 const dy = dfdy( in_position_world );
    float3 const surface_normal = normalize( cross( dx, dy ) );

    float3 const surface_diffuse   = _aaa_tex_0.sample( _aaa_samp, tex_coord ).xyz * mat_diffuse;
    float  const surface_roughness = mat_roughness;
    //	make_surface_specular stub
    float3 const surface_specular  = mat_specular * _aaa_tex_3.sample( _aaa_samp, in.uv ).rgb;
    float3 const surface_emissive  = float3( 0.0 );

    //	METALLIC_WORKFLOW
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
