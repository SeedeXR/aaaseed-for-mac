// gbuffer.metal -- Path A port from vux_flare_render/gbuffer.frag (c95).
// Full PBR gbuffer pass : diffuse + normal-map + roughness + specular + emissive.
// Patches :
//   - 4-target MRT (out_diffuse_rough/normal_occlusion/specular_reflection/emissive)
//     -> `FragmentOut [[color(0..3)]]` (c94 pattern)
//   - Per-vertex inputs (in_position_world / in_normal_world / in_texcoord / in_tangent_frame)
//     -> stubbed via in.uv ; tangent_frame -> identity float3x3 ; world/normal -> constants
//   - `st_surface` engine struct -> local Mac-side struct
//   - `aaa_material.material` / `material.*` engine globals -> AaaFuFloats slots 8+
//   - `do_surface_material( surface, mat )` engine helper -> identity passthrough
//   - 5 texture bindings (diffuse / normalmap / roughness / specular / emissive)
//     -> `_aaa_tex_0..4`
//   - `aaa_fu_vec4[0..1]`, `aaa_fu_int[0..1]`, `aaa_fu_float[0]` -> standard buffers

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

struct st_surface_
{
    float3 world_position;
    float4 diffuse;
    float3 normal;
    float  roughness;
    float3 specular;
    float3 emissive;
    float  ao_factor;
    float  reflection_intensity;
    float  ambient_intensity;
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
    texture2d< float >    _aaa_tex_1    [[texture(1)]],   // normalmap
    texture2d< float >    _aaa_tex_2    [[texture(2)]],   // roughness
    texture2d< float >    _aaa_tex_3    [[texture(3)]],   // specular_metal
    texture2d< float >    _aaa_tex_4    [[texture(4)]],   // emissive
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    //	Per-vertex P4 stubs
    float3 const in_position_world = float3( 0.0 );
    float3 const in_normal_world   = float3( 0.0, 0.0, 1.0 );
    float3x3 const in_tangent_frame = float3x3( float3( 1.0, 0.0, 0.0 ),
                                                 float3( 0.0, 1.0, 0.0 ),
                                                 float3( 0.0, 0.0, 1.0 ) );

    float4 const uv_xy_fxy    = _aaa_vec4s.values[ 0 ];
    float4 const mesh_color   = _aaa_vec4s.values[ 1 ];
    int    const discard_uv   = _aaa_ints.values[ 0 ];
    int    const colored_mesh = _aaa_ints.values[ 1 ];
    float  const alpha_threshold = _aaa_floats.values[ 0 ];

    float2 const uv = ( in.uv + uv_xy_fxy.xy ) * uv_xy_fxy.wz;

    st_surface_ surface;
    surface.world_position       = in_position_world;
    surface.diffuse              = ( colored_mesh == 1 ) ? mesh_color
                                                          : _aaa_tex_0.sample( _aaa_samp, uv );
    if( discard_uv == 1 )
    {
        if( uv.x < 0.0 || uv.x > 1.0 ) discard_fragment();
        if( uv.y < 0.0 || uv.y > 1.0 ) discard_fragment();
    }
    if( surface.diffuse.a < alpha_threshold )
        discard_fragment();

    //	Normal-map branch (ENABLE_NORMAL_MAP == 1 from source)
    float3 normal = _aaa_tex_1.sample( _aaa_samp, uv ).xyz;
    normal = 2.0 * normal - 1.0;
    surface.normal = normalize( in_tangent_frame * normal );

    surface.roughness            = _aaa_tex_2.sample( _aaa_samp, uv ).r;
    surface.specular             = _aaa_tex_3.sample( _aaa_samp, uv ).rgb;
    surface.emissive             = _aaa_tex_4.sample( _aaa_samp, uv ).rgb;
    surface.ao_factor            = 1.0;
    surface.reflection_intensity = 1.0;
    surface.ambient_intensity    = 1.0;

    //	`do_surface_material( surface, aaa_material.material )` engine helper
    //	stubbed to identity passthrough (no per-shader material modulation).

    FragmentOut out;
    out.out_diffuse_rough       = float4( surface.diffuse.rgb, surface.roughness );
    out.out_normal_occlusion    = float4( surface.normal, surface.ao_factor );
    out.out_specular_reflection = float4( surface.specular, surface.reflection_intensity );
    out.out_emissive            = float4( surface.emissive, 1.0 );
    return out;
}
