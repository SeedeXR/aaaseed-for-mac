// force_field.metal -- Path A port from flex/force_field.frag (c86).
// Signed-distance-field gradient-driven force lookup (2D).
// Patches :
//   - `g_input_texture_dist` -> `_aaa_tex_0`
//   - `g_input_texture_mask` -> `_aaa_tex_1`
//   - `aaa_fu_float[9..13]` (pullback / df_min / df_max / field_str / field_gamma)
//     -> `_aaa_floats.values[9..13]` (engine slot mapping preserved)
//   - `aaa_vu_int[0]` (bypass) -> AaaFuInts[0] (vu/fu merge convention)
//   - `settings_rcpFrame` hard-coded as `vec2(1/512, 1/128)` (engine default)
//   - `layout(location = 0) in vec2 in_texcoord` -> in.uv

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

constant float2 settings_rcpFrame = float2( 1.0 / 512.0, 1.0 / 128.0 );

static inline float calcDist_(
    float2 p,
    texture2d< float > tex_dist, texture2d< float > tex_mask, sampler samp )
{
    return tex_dist.sample( samp, p ).r
         * ( tex_mask.sample( samp, p ).r * -2.0 + 1.0 );
}

static inline float2 comp_gradient_(
    float2 p,
    texture2d< float > tex_dist, texture2d< float > tex_mask, sampler samp )
{
    float3 const step_ = float3( settings_rcpFrame.xy, 0.0 );
    float2 n;
    n.x = calcDist_( p + step_.xz, tex_dist, tex_mask, samp )
        - calcDist_( p - step_.xz, tex_dist, tex_mask, samp );
    n.y = calcDist_( p + step_.zy, tex_dist, tex_mask, samp )
        - calcDist_( p - step_.zy, tex_dist, tex_mask, samp );
    return n;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const pullback_distance = _aaa_floats.values[ 9 ];
    float const df_min            = _aaa_floats.values[ 10 ];
    float const df_max            = _aaa_floats.values[ 11 ];
    float const field_str         = _aaa_floats.values[ 12 ];
    float const field_gamma       = _aaa_floats.values[ 13 ];
    (void) pullback_distance;   // declared in engine source ; not used in live path

    float  const df   = calcDist_( in.uv, _aaa_tex_0, _aaa_tex_1, _aaa_samp );
    float2 const grad = comp_gradient_( in.uv, _aaa_tex_0, _aaa_tex_1, _aaa_samp );

    float  const field_dist_factor = field_str * pow( ( df - df_max ) / ( df_min - df_max ), field_gamma );
    float2 const force = grad * field_dist_factor;
    return float4( force, 0.0, 1.0 );
}
