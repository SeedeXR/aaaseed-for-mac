// blur_add_disc.metal -- Path A port from vux_flare_render/blur_add_disc.frag (c97).
// Two-tex blur-add with 2x disc-pattern sampling (kernel + perpendicular kernel).
// Patches : same engine kernel cluster pattern as blur_add_box.metal (c97).

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

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float2 const blur_add_input_size = float2( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ] );
    float  const blur_add_size       = _aaa_floats.values[ 2 ];
    float  const blur_add_factor     = _aaa_floats.values[ 3 ];
    float  const blur_add_intensity  = _aaa_floats.values[ 4 ];

    float2 sample_offset = 0.5 * blur_add_input_size;
    sample_offset *= blur_add_size;

    float3 const base_color     = _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb;
    float3 const base_color_add = _aaa_tex_1.sample( _aaa_samp, in.uv ).rgb;
    (void) base_color_add;

    float3 color_sum = base_color_add;
    for( int i = 0; i < disc_sample_count_; i++ )
    {
        float2 tap_coord = in.uv + disc_kernel_[ i ] * sample_offset;
        color_sum += _aaa_tex_1.sample( _aaa_samp, tap_coord ).rgb;
        tap_coord = in.uv + disc_kernel_[ i ].yx * sample_offset * 2.0;
        color_sum += _aaa_tex_1.sample( _aaa_samp, tap_coord ).rgb;
    }
    color_sum *= blur_add_intensity / float( disc_sample_count_ * 2 );

    float3 const result = mix( base_color, base_color + color_sum, blur_add_factor );
    return float4( result, 1.0 );
}
