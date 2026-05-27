// fog.metal -- Path A port from vux_flare_render/fog.frag (c98, c84 carry recovered).
// View-space fog with vertical gradient + sun-tinted ray scattering.
// Patches : engine-camera-cluster (per-shader aaa_cam stub for projection_inverse,
// view_projection_inverse, camera_position).
//   - `aaa_cam.projection_inverse` / `aaa_cam.view_projection_inverse` -> identity float4x4 stubs
//   - `aaa_cam.camera_position` -> float3(0, 0, 0) stub
//   - `aaa_fu_vec4[0..1]` (fog_color_density_low/high) ; `aaa_fu_float[0..11]`
//     (start/sun/depth/y_top/y_bottom/y_gamma) ; `aaa_vu_int[0]` (bypass)
//   - `g_input_texture` -> `_aaa_tex_0` ; `g_depth_texture` -> `_aaa_tex_1`
//   - `clamp_01` engine helper -> `saturate` MSL builtin (c91 lesson)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//	Engine camera-cluster stubs declared as fs_main locals (MSL float4x4 ctor not constexpr).

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
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int const bypass = _aaa_ints.values[ 0 ];

    if( bypass != 0 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    float4x4 const aaa_cam_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );
    float4x4 const aaa_cam_view_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );
    float3 const aaa_cam_camera_position_ = float3( 0.0 );

    float4 const fog_color_density_low  = _aaa_vec4s.values[ 0 ];
    float4 const fog_color_density_high = _aaa_vec4s.values[ 1 ];
    float  const fog_start              = _aaa_floats.values[ 0 ];
    float3 const sun_color              = float3( _aaa_floats.values[ 1 ], _aaa_floats.values[ 2 ], _aaa_floats.values[ 3 ] );
    float3 const sun_direction          = float3( _aaa_floats.values[ 4 ], _aaa_floats.values[ 5 ], _aaa_floats.values[ 6 ] );
    float  const sun_amount             = _aaa_floats.values[ 7 ];
    float  const fog_depth_factor       = _aaa_floats.values[ 8 ];
    float  const fog_y_top              = _aaa_floats.values[ 9 ];
    float  const fog_y_bottom           = _aaa_floats.values[ 10 ];
    float  const fog_y_gamma            = _aaa_floats.values[ 11 ];

    float2 const screen_clip = in.uv * 2.0 - 1.0;
    float depth = _aaa_tex_1.sample( _aaa_samp, in.uv ).r;
    depth = depth * 2.0 - 1.0;

    float4 const screen_coord = float4( screen_clip, depth, 1.0 );
    float4 view_position  = aaa_cam_projection_inverse_      * screen_coord;
    view_position.xyz  /= view_position.w;
    float4 world_position = aaa_cam_view_projection_inverse_ * screen_coord;
    world_position.xyz /= world_position.w;

    float4 base_color = _aaa_tex_0.sample( _aaa_samp, in.uv );

    float d = mix( view_position.z,
                   distance( world_position.xyz, aaa_cam_camera_position_ ),
                   fog_depth_factor );
    d = max( -d - fog_start, 0.0 );

    float const fog_y_range = fog_y_bottom - fog_y_top;
    float fog_norm_y = ( world_position.y - fog_y_top ) / fog_y_range;
    fog_norm_y = saturate( fog_norm_y );
    fog_norm_y = pow( fog_norm_y, fog_y_gamma );

    float4 const fog_color_density = mix( fog_color_density_high, fog_color_density_low, fog_norm_y );
    float  const fog_amount = exp( -d * fog_color_density.w * 0.1 );

    float sun_dot_ray = saturate( dot( sun_direction, normalize( view_position.xyz ) ) );
    sun_dot_ray *= sun_amount;

    float3 const final_fog_color = mix( fog_color_density.rgb, sun_color, sun_dot_ray );

    base_color.rgb = mix( final_fog_color, base_color.rgb, fog_amount );
    base_color.a = 1.0;
    return base_color;
}
