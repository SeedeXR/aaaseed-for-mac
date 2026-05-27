// lights_deferred_v1.metal -- Path A port from vux_flare_render/lights_deferred_v1.frag (c100).
// Same family as v0 ; gamma on point light + color_shadow / color_ambient / color_general /
// gamma_general / color_offset additional uniforms. Same aggressive stub strategy.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

struct st_light_nb_ { int directional; int point_; int spot; };
struct st_surface_v1_
{
    float3 world_position;
    float4 diffuse;
    float3 normal;
    float3 specular;
    float3 emissive;
    float  roughness;
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

fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    texture2d< float >    _aaa_tex_0      [[texture(0)]],   // diffuse_rough
    texture2d< float >    _aaa_tex_1      [[texture(1)]],   // normal_ao
    texture2d< float >    _aaa_tex_2      [[texture(2)]],   // specular_reflection
    texture2d< float >    _aaa_tex_3      [[texture(3)]],   // emissive
    texture2d< float >    _aaa_tex_4      [[texture(4)]],   // depth
    sampler               _aaa_samp       [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]] )
{
    int    const what                = _aaa_ints.values[ 0 ];
    float  const hack_ambient_offset = _aaa_floats.values[ 0 ];
    float  const hack_light_factor   = _aaa_floats.values[ 1 ];
    float  const hack_shadow_factor  = _aaa_floats.values[ 2 ];
    float  const normal_center       = _aaa_floats.values[ 5 ];
    float4 const color_shadow        = _aaa_vec4s.values[ 0 ];
    float4 const color_ambient       = _aaa_vec4s.values[ 1 ];
    float4 const color_general       = _aaa_vec4s.values[ 2 ];
    float4 const gamma_general       = _aaa_vec4s.values[ 3 ];
    float4 const color_offset        = _aaa_vec4s.values[ 4 ];
    (void) normal_center; (void) color_shadow; (void) color_ambient;
    (void) color_general; (void) gamma_general; (void) color_offset;

    float4x4 const aaa_cam_view_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );
    float3 const aaa_cam_camera_position_ = float3( 0.0 );

    st_light_nb_ const u_light_nb = { 0, 0, 0 };
    (void) u_light_nb;

    if( what == 1 )
        return _aaa_tex_0.sample( _aaa_samp, in.uv );

    float2 const screenClipPos = in.uv * 2.0 - 1.0;
    float depth = _aaa_tex_4.sample( _aaa_samp, in.uv ).r * 2.0 - 1.0;
    float4 const screen_coord = float4( screenClipPos, depth, 1.0 );
    float4 world_position = aaa_cam_view_projection_inverse_ * screen_coord;
    world_position = float4( world_position.xyz / world_position.w, 1.0 );

    float4 const diffuse_rough       = _aaa_tex_0.sample( _aaa_samp, in.uv );
    float4 const normal_ao           = _aaa_tex_1.sample( _aaa_samp, in.uv );
    float4 const specular_reflection = _aaa_tex_2.sample( _aaa_samp, in.uv );
    float4 const emissive            = _aaa_tex_3.sample( _aaa_samp, in.uv );

    st_surface_v1_ surface;
    surface.world_position       = world_position.xyz;
    surface.diffuse              = diffuse_rough;
    surface.normal               = normal_ao.rgb;
    surface.specular             = specular_reflection.rgb;
    surface.emissive             = emissive.rgb;
    surface.roughness            = diffuse_rough.a;
    surface.ao_factor            = normal_ao.a;
    surface.reflection_intensity = specular_reflection.a;
    surface.ambient_intensity    = emissive.a;

    float3 const view_direction = normalize( aaa_cam_camera_position_ - surface.world_position );
    (void) view_direction;

    float3 light_total  = float3( 0.0 );
    float3 shadow_total = float3( 1.0 );

    //	light loops -- u_light_nb stubbed to zero ; runtime side may plumb SSBOs later.

    float3 const ambient_shadow_contrib = mix( float3( 1.0 ), shadow_total, hack_shadow_factor );
    if( depth < 1.0 )
        light_total += ( surface.ambient_intensity * hack_ambient_offset * ambient_shadow_contrib ) * surface.diffuse.rgb;
    light_total *= hack_light_factor;

    return float4( light_total, 1.0 );
}
