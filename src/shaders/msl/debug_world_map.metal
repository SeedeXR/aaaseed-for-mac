// debug_world_map.metal -- Path A port from vux_flare_render/debug_world_map.frag (c98).
// Per-pixel world (or view) position reconstruction from depth ; useful for
// debugging deferred shading.
// Patches : engine-camera-cluster (same per-shader aaa_cam matrix stub as depth_coc).
//   - `aaa_cam.view_projection_inverse` / `aaa_cam.projection_inverse` -> identity stubs
//   - `aaa_vu_int[0..1]` -> AaaFuInts[0..1] (bypass, mode)
//   - `g_input_texture` -> `_aaa_tex_0` ; `g_depth_texture` -> `_aaa_tex_1`

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuInts { int values[ 16 ]; };

//	Engine camera-cluster stubs declared as fs_main locals (MSL float4x4 ctor
//	is not constexpr).

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
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int const bypass = _aaa_ints.values[ 0 ];
    int const mode   = _aaa_ints.values[ 1 ];

    float4x4 const aaa_cam_view_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );
    float4x4 const aaa_cam_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );

    if( bypass != 0 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    float2 screen_clip = in.uv * 2.0 - 1.0;
    float depth_base = _aaa_tex_1.sample( _aaa_samp, in.uv ).r;
    float depth = depth_base * 2.0 - 1.0;

    float4 const screen_coord = float4( screen_clip, depth, 1.0 );
    float4x4 const mat = ( mode == 1 ) ? aaa_cam_projection_inverse_ : aaa_cam_view_projection_inverse_;

    float4 world_position = mat * screen_coord;
    world_position.xyz /= world_position.w;

    if( depth_base < 1.0 )
        return float4( world_position.xyz, 1.0 );
    return float4( 0.0, 0.0, 0.0, 1.0 );
}
