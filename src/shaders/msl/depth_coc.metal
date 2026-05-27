// depth_coc.metal -- Path A port from vux_flare_render/depth_coc.frag (c98).
// Depth + circle-of-confusion (CoC) factor written to RG render target.
// **First engine-camera-cluster port** via per-shader `aaa_cam` matrix stub.
// Patches :
//   - `aaa_cam.projection_inverse` -> identity float4x4 stub at file scope
//   - `aaa_cam.coc_factors` (vec4) -> AaaFuVec4s[0] (runtime side may populate)
//   - `g_depth_texture` -> `_aaa_tex_0`
//   - `out vec2 out_result` -> `float4 [[color(0)]]` with .rg populated, .ba=0
//     (MSL allows partial-component MRT but the safe form is float4 ; the
//     runtime depth-CoC target format determines how .rg/.ba get interpreted)
//   - `saturate` -> MSL `saturate` builtin

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuVec4s { float4 values[ 16 ]; };

//	Engine camera-cluster stub : aaa_cam.projection_inverse identity.
//	Declared as a fs_main local (MSL `float4x4(...)` ctor is not constexpr,
//	so file-scope `constant float4x4` rejects this initializer).

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float calc_coc_factor_( float depth, float4 coc )
{
    float f0 = 1.0 - saturate( ( depth - coc.x ) / max( coc.y - coc.x, 0.01 ) );
    float f1 = saturate( ( depth - coc.z ) / max( coc.w - coc.z, 0.01 ) );
    return saturate( f0 + f1 );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float4 const aaa_cam_coc_factors = _aaa_vec4s.values[ 0 ];
    float4x4 const aaa_cam_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );

    float2 screen_clip = in.uv;
    screen_clip.y = 1.0 - screen_clip.y;
    screen_clip = screen_clip * 2.0 - 1.0;

    float depth = _aaa_tex_0.sample( _aaa_samp, in.uv ).r;
    depth = depth * 2.0 - 1.0;

    float4 const screen_coord = float4( screen_clip, depth, 1.0 );
    float4 view_position = aaa_cam_projection_inverse_ * screen_coord;
    view_position.xyz /= view_position.w;

    return float4( view_position.z, calc_coc_factor_( view_position.z, aaa_cam_coc_factors ), 0.0, 0.0 );
}
