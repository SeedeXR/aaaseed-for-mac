// debug_world_map_color_distance.metal -- Path A port from vux_flare_render/debug_world_map_color_distance.frag (c98).
// Per-pixel world-position diff against a reference RGB texture (debug deferred).
// Patches : engine-camera-cluster (per-shader aaa_cam stub).
//   - `aaa_cam.view_projection_inverse` -> identity float4x4 stub
//   - `aaa_vu_int[0..1]` (bypass, mode), `aaa_fu_float[0]` (multiplier)
//   - 3 textures : g_input / g_depth / g_reference -> _aaa_tex_0..2

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

//	Engine camera-cluster stub declared as fs_main local (MSL float4x4 ctor not constexpr).

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
    texture2d< float >    _aaa_tex_2    [[texture(2)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int   const bypass     = _aaa_ints.values[ 0 ];
    int   const mode       = _aaa_ints.values[ 1 ];
    float const multiplier = _aaa_floats.values[ 0 ];

    float4x4 const aaa_cam_view_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );

    if( bypass != 0 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    float2 const screen_clip = in.uv * 2.0 - 1.0;
    float depth_base = _aaa_tex_1.sample( _aaa_samp, in.uv ).r;
    float depth = depth_base * 2.0 - 1.0;

    float4 const screen_coord = float4( screen_clip, depth, 1.0 );
    float4 world_position = aaa_cam_view_projection_inverse_ * screen_coord;
    world_position.xyz /= world_position.w;

    float3 const world_position_ref = _aaa_tex_2.sample( _aaa_samp, in.uv ).xyz;

    float3 diff = fabs( world_position.xyz - world_position_ref );
    if( mode == 1 )
    {
        float const d = dot( diff, diff );
        diff = float3( d );
    }
    diff *= multiplier;

    if( depth_base < 1.0 )
        return float4( diff, 1.0 );
    return float4( 0.0, 0.0, 0.0, 1.0 );
}
