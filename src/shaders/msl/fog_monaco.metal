// fog_monaco.metal -- Path A port from vux_flare_render/fog_monaco.frag (c99).
// View-space fog (Monaco variant) with sun-aligned ray scattering + vdebug modes.
// Patches : engine camera cluster (per-shader aaa_cam stub, c98 doctrine).
//   - `aaa_cam.projection_inverse` / `view_projection_inverse` / `view`
//     -> identity float4x4 fs_main locals
//   - `aaa_fu_vec4[0..5]` (dist_begin/factor/colors/sun) ;
//     `aaa_fu_float[5..11]` (smoothstep edges, sun_amount, dist_gamma, fog_y_top/bottom/gamma)
//   - `aaa_fu_int[0]` (vdebug ; 0 = passthrough, >0 = fog applied, 2/3 = debug)
//   - `g_input_texture` / `g_depth_texture` -> `_aaa_tex_0..1`
//   - `clamp_01` -> `saturate` (c91 lesson)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

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
    int const vdebug = _aaa_ints.values[ 0 ];

    if( vdebug == 0 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    float3 const dist_begin       = _aaa_vec4s.values[ 0 ].xyz;
    float3 const dist_factor      = _aaa_vec4s.values[ 1 ].xyz;
    float4 const fog_color_top    = _aaa_vec4s.values[ 2 ];
    float4 const fog_color_bottom = _aaa_vec4s.values[ 3 ];
    float3 const sun_pos_world    = _aaa_vec4s.values[ 4 ].xyz;
    float3 const sun_color        = _aaa_vec4s.values[ 5 ].xyz;
    float  const sun_amount       = _aaa_floats.values[ 7 ];
    float  const dist_gamma       = _aaa_floats.values[ 8 ];
    float  const fog_y_top        = _aaa_floats.values[ 9 ];
    float  const fog_y_bottom     = _aaa_floats.values[ 10 ];
    float  const fog_y_gamma      = _aaa_floats.values[ 11 ];

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
    float4x4 const aaa_cam_view_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );

    float2 const screen_clip = in.uv * 2.0 - 1.0;
    float depth = _aaa_tex_1.sample( _aaa_samp, in.uv ).r * 2.0 - 1.0;

    float4 const screen_coord = float4( screen_clip, depth, 1.0 );
    float4 view_position = aaa_cam_projection_inverse_ * screen_coord;
    view_position.xyz /= view_position.w;
    float4 world_position = aaa_cam_view_projection_inverse_ * screen_coord;
    world_position.xyz /= world_position.w;

    float4 sun_dir = aaa_cam_view_ * float4( sun_pos_world, 0.0 );
    sun_dir.xyz = normalize( sun_dir.xyz );

    float4 base_color = _aaa_tex_0.sample( _aaa_samp, in.uv );

    float3 v = world_position.xyz;
    v.z = fabs( v.z );
    v.y = -v.y;
    v -= float3( 21.0, 0.0, 4.4 ) + dist_begin;
    v *= dist_factor;
    float d = max( max( v.x, v.y ), v.z );

    if( vdebug >= 2 )
    {
        float4 result;
        if( d < 0.0 )       result = float4( 1.0, 1.0, 1.0, 1.0 );
        else if( d > 1.0 )  result = float4( 0.0, 0.0, 1.0, 1.0 );
        else                result = mix( float4( 1.0, 0.0, 0.0, 1.0 ), float4( 0.0, 1.0, 0.0, 1.0 ), d );
        if( vdebug == 3 )   result *= base_color;
        return result;
    }

    float fog_y_range = fog_y_top - fog_y_bottom;
    float fog_norm_y = ( world_position.y - fog_y_bottom ) / fog_y_range;
    fog_norm_y = saturate( fog_norm_y );
    fog_norm_y = pow( fog_norm_y, fog_y_gamma );

    float3 fog_color = fog_color_top.rgb;
    float sun_dot_ray = saturate( dot( sun_dir.xyz, normalize( view_position.xyz ) ) );
    sun_dot_ray = smoothstep( _aaa_floats.values[ 5 ], _aaa_floats.values[ 6 ], sun_dot_ray );
    sun_dot_ray *= sun_amount;

    fog_color = mix( fog_color_bottom.rgb, fog_color, fog_norm_y );
    fog_color = mix( fog_color, sun_color, sun_dot_ray );

    float fog_amount = saturate( d );
    fog_amount = pow( fog_amount, dist_gamma );

    base_color.rgb = mix( base_color.rgb, fog_color, fog_amount );
    base_color.a = 1.0;
    return base_color;
}
