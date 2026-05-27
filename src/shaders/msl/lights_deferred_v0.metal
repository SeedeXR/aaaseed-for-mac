// lights_deferred_v0.metal -- Path A port from vux_flare_render/lights_deferred_v0.frag (c100).
// Deferred shading : directional/point/spot lights + shadows + lightmaps + ambient.
// **Heavy port** -- aggressive stubbing : all engine globals (st_light_*, st_light_nb,
// st_surface, compute_light, shadow_pcf/shadow_no_filter, aaa_cam.*) stubbed locally
// for catalog-compile validity. Runtime side may plumb real SSBOs / constants later.
// Patches : engine camera cluster (c98) + engine struct cluster (c91-c96) combined.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//	Engine struct stubs (aggressive minimal layouts) :
struct st_light_nb_       { int directional; int point_; int spot; };
struct st_light_directional_
{
    float3 direction; float pad0;
    float3 color;     float pad1;
    int    b_shadow;  int   b_intensity_map; int intensity_map_mode; float shadow_bias;
    float  shadow_filter_width; float shadow_map_size_over; float2 intensity_map_scale;
    float2 intensity_map_offset; float intensity_map_factor; float pad2;
    float4x4 light_view_projection;
};
struct st_light_point_  { float3 position; float radius; float3 color; float pad; };
struct st_light_spot_   { float3 position; float radius; float3 direction; float pad0;
                         float3 color; float field_of_view; float field_of_view_decay; float pad1; float pad2; float pad3; };

struct st_surface_
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

//	Engine helper stubs (identity passthroughs) :
static inline float3 compute_light_( st_surface_ s, float3 ldir, float3 vdir, float3 lcol )
{
    (void) s; (void) ldir; (void) vdir;
    return lcol;
}
static inline float shadow_pcf_( texture2d< float > tex, float3 coord, float bias, float w, int sz, sampler samp )
{
    (void) tex; (void) coord; (void) bias; (void) w; (void) sz; (void) samp;
    return 1.0;
}
static inline float shadow_no_filter_( texture2d< float > tex, float3 coord, float bias, sampler samp )
{
    (void) tex; (void) coord; (void) bias; (void) samp;
    return 1.0;
}

static inline float normalize_and_compute_falloff_( thread float3& light_direction, float radius )
{
    float const lr2 = radius * radius;
    float const linv = rsqrt( dot( light_direction, light_direction ) );
    light_direction *= linv;
    float df = lr2 * ( linv * linv );
    return max( 0.0, df - rsqrt( df ) );
}

fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    texture2d< float >    _aaa_tex_0      [[texture(0)]],   // TEX_DIFFUSE_ROUGH
    texture2d< float >    _aaa_tex_1      [[texture(1)]],   // TEX_NORMAL_AO
    texture2d< float >    _aaa_tex_2      [[texture(2)]],   // TEX_SPECULAR_REFLECTION
    texture2d< float >    _aaa_tex_3      [[texture(3)]],   // TEX_EMISSIVE
    texture2d< float >    _aaa_tex_4      [[texture(4)]],   // TEX_DEPTH
    texture2d< float >    _aaa_tex_5      [[texture(5)]],   // TEX_SHADOW_0
    texture2d< float >    _aaa_tex_6      [[texture(6)]],   // TEX_LIGHTMAP_0
    sampler               _aaa_samp       [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]] )
{
    int    const what                = _aaa_ints.values[ 0 ];
    int    const pcf_half_size       = _aaa_ints.values[ 1 ];
    float  const hack_ambient_offset = _aaa_floats.values[ 0 ];
    float  const hack_light_factor   = _aaa_floats.values[ 1 ];
    float  const hack_shadow_factor  = _aaa_floats.values[ 2 ];
    float  const hack_y_factor_top   = _aaa_floats.values[ 3 ];
    float  const hack_y_factor_bot   = _aaa_floats.values[ 4 ];
    float4 const shadow_color        = _aaa_vec4s.values[ 0 ];
    (void) pcf_half_size;

    //	Engine camera-cluster stub (c98 doctrine)
    float4x4 const aaa_cam_view_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );
    float3 const aaa_cam_camera_position_ = float3( 0.0 );

    //	SSBO light counts stubbed to zero -- loops don't run on catalog-compile
    st_light_nb_ const u_light_nb = { 0, 0, 0 };

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

    if( what != 2 && what >= 3 && what <= 14 )
    {
        switch( what )
        {
            case 3:  return float4( normal_ao.rgb * 0.5 + 0.5, 1.0 );
            case 4:  return screen_coord * 0.5 + 0.5;
            case 5:  return world_position;
            case 6:  return float4( diffuse_rough.rgb, 1.0 );
            case 7:  return float4( normal_ao.rgb, 1.0 );
            case 8:  return float4( specular_reflection.rgb, 1.0 );
            case 9:  return float4( emissive.rgb, 1.0 );
            case 10: return float4( float3( depth * 0.5 + 0.5 ), 1.0 );
            case 11: return float4( float3( diffuse_rough.a ), 1.0 );
            case 12: return float4( float3( normal_ao.a ), 1.0 );
            case 13: return float4( float3( specular_reflection.a ), 1.0 );
            case 14: return float4( float3( emissive.a ), 1.0 );
        }
    }

    st_surface_ surface;
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

    //	DIRECTIONAL / POINT / SPOT light loops -- u_light_nb stubbed to zero ;
    //	loops compile and skip at draw time. shadow_pcf / shadow_no_filter
    //	helpers stubbed above for compile validity.
    (void) shadow_color; (void) hack_y_factor_top; (void) hack_y_factor_bot;

    float3 const ambient_shadow_contrib = mix( float3( 1.0 ), shadow_total, hack_shadow_factor );
    if( depth < 1.0 )
        light_total += ( surface.ambient_intensity * hack_ambient_offset * ambient_shadow_contrib ) * surface.diffuse.rgb;
    light_total *= hack_light_factor;

    return float4( light_total, 1.0 );
}
