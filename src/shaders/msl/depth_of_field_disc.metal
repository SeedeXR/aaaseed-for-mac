// depth_of_field_disc.metal -- Path A port from vux_flare_render/depth_of_field_disc.frag (c97).
// CoC-driven depth-of-field disc blur ; depth + blur factor sampled from gbuffer.
// Patches : engine kernel cluster (same disc_kernel_ as blur_add_*).
//   - `g_input_texture` -> `_aaa_tex_0` ; `g_depth_coc_texture` -> `_aaa_tex_1`
//   - `aaa_fu_float[0..2]` -> AaaFuFloats[0..2]
//   - `read_depth_and_coc` helper preserved as static inline (samples depth-coc tex)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

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

static inline float2 read_depth_and_coc_( float2 uv, texture2d< float > tex, sampler samp )
{
    return tex.sample( samp, uv ).rg;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float2 const dof_input_size = float2( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ] );
    float  const dof_blur_size  = _aaa_floats.values[ 2 ];

    float2 const sample_offset = 0.5 / dof_input_size;

    float3 const center_color = _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb;
    float3 color_sum = center_color;
    float total_contribution = 1.0;

    float2 const center_depth_blur = read_depth_and_coc_( in.uv, _aaa_tex_1, _aaa_samp );
    float  const center_depth = center_depth_blur.x;
    float  const center_blur  = center_depth_blur.y;

    if( center_blur > 0.0 )
    {
        float const coc_size = center_blur * dof_blur_size;

        for( int i = 0; i < disc_sample_count_; i++ )
        {
            float2 const kernel_value = disc_kernel_[ i ];
            float2 const offset = kernel_value * coc_size;
            float2 const tap_coord = in.uv + offset * sample_offset;

            float3 const tap_color = _aaa_tex_0.sample( _aaa_samp, tap_coord ).rgb;
            float2 const tap_depth_blur = read_depth_and_coc_( tap_coord, _aaa_tex_1, _aaa_samp );
            float  const tap_depth = tap_depth_blur.x;
            float  const tap_blur  = tap_depth_blur.y;
            float  const tap_contribution = ( tap_depth > center_depth ) ? 1.0 : tap_blur;

            color_sum += tap_color * tap_contribution;
            total_contribution += tap_contribution;
        }
    }

    color_sum /= total_contribution;
    return float4( color_sum, 1.0 );
}
