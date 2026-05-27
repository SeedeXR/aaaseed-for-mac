// dof.metal -- Path A port from vux_flare_render/dof.frag (c99).
// Depth-of-field via Poisson disc tap + per-tap CoC computation from depth.
// Patches : engine camera + kernel clusters merged.
//   - `aaa_cam.projection_inverse` -> identity float4x4 fs_main local (c98 doctrine)
//   - `disc_sample_count` + `disc_kernel[]` -> 8-tap Poisson constexpr constant (c97 doctrine)
//   - `aaa_fu_float[0..6]` (input_size / blur_size / near_out / near_in / far_in / far_out)
//   - `aaa_fu_int[0..1]` (use_vdebug, use_blur_texture)
//   - 3 texture bindings (g_input / g_depth / g_blur)
//   - `compute_coc` helper extracted ; `out float` params -> `thread float&` (c85)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

constexpr constant int disc_sample_count_ = 8;
constexpr constant float2 disc_kernel_[ disc_sample_count_ ] = {
    float2(  0.0,     1.0    ),
    float2(  0.7071,  0.7071 ),
    float2(  1.0,     0.0    ),
    float2(  0.7071, -0.7071 ),
    float2(  0.0,    -1.0    ),
    float2( -0.7071, -0.7071 ),
    float2( -1.0,     0.0    ),
    float2( -0.7071,  0.7071 )
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

static inline float calc_coc_factor_(
    float depth, float near_in, float near_out, float far_in, float far_out )
{
    float f0 = ( depth + near_in ) / ( -near_out + near_in );
    float f1 = ( depth + far_in ) / ( -far_out + far_in );
    return max( saturate( f0 ), saturate( f1 ) );
}

static inline void compute_coc_(
    float2 p_uv, thread float& p_depth, thread float& p_factor,
    texture2d< float > tex_depth, sampler samp,
    thread float4x4 const& projection_inverse,
    float near_in, float near_out, float far_in, float far_out )
{
    float2 screen_clip = p_uv * 2.0 - 1.0;
    float depth = tex_depth.sample( samp, p_uv ).r * 2.0 - 1.0;
    float4 const screen_coord = float4( screen_clip, depth, 1.0 );
    float4 view_position = projection_inverse * screen_coord;
    view_position.xyz /= view_position.w;
    p_depth  = view_position.z;
    p_factor = calc_coc_factor_( p_depth, near_in, near_out, far_in, far_out );
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
    float2 const input_size_over   = float2( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ] );
    float  const blur_size_asked   = _aaa_floats.values[ 2 ];
    float  const near_out          = _aaa_floats.values[ 3 ];
    float  const near_in           = _aaa_floats.values[ 4 ];
    float  const far_in            = _aaa_floats.values[ 5 ];
    float  const far_out           = _aaa_floats.values[ 6 ];
    int    const use_vdebug        = _aaa_ints.values[ 0 ];
    int    const use_blur_texture  = _aaa_ints.values[ 1 ];

    float4x4 const aaa_cam_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );

    float blur_size = blur_size_asked;
    if( use_blur_texture == 1 )
        blur_size = _aaa_tex_2.sample( _aaa_samp, in.uv ).r * blur_size;

    float3 color = _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb;

    if( blur_size > 0.0 )
    {
        float center_depth, center_blur;
        compute_coc_( in.uv, center_depth, center_blur, _aaa_tex_1, _aaa_samp,
                      aaa_cam_projection_inverse_, near_in, near_out, far_in, far_out );

        if( use_vdebug == 1 )
        {
            color.r = center_blur;
        }
        else if( center_blur > 0.0 )
        {
            float2 const sample_offset = 0.5 * input_size_over;
            float total_contribution = 1.0;
            float const coc_size = center_blur * blur_size;

            for( int i = 0; i < disc_sample_count_; i++ )
            {
                float2 const kernel_value = disc_kernel_[ i ];
                float2 const offset = kernel_value * coc_size;
                float2 const tap_coord = in.uv + offset * sample_offset;

                float3 const tap_color = _aaa_tex_0.sample( _aaa_samp, tap_coord ).rgb;
                float tap_depth, tap_blur;
                compute_coc_( tap_coord, tap_depth, tap_blur, _aaa_tex_1, _aaa_samp,
                              aaa_cam_projection_inverse_, near_in, near_out, far_in, far_out );

                float const tap_contribution = ( tap_depth > center_depth ) ? 1.0 : tap_blur;
                color += tap_color * tap_contribution;
                total_contribution += tap_contribution;
            }
            color /= total_contribution;
        }
    }

    return float4( color, 1.0 );
}
