// godrays.metal -- Path A port from vux_flare_render/godrays.frag (c99).
// Radial light-scatter (god rays) : 64-sample march from screen-space light pos.
// Patches : (clean ; no aaa_cam needed despite cluster listing)
//   - `g_input_texture` -> `_aaa_tex_0`
//   - `aaa_fu_float[0..5]` (screen_light_position.xy / illumination_decay /
//     density / weight / decay) -> AaaFuFloats[0..5]
//   - `aaa_vu_int[0]` (bypass) -> AaaFuInts[0]
//   - `layout(location = 0) in vec2 in_texcoord` -> `in.uv`

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

constant int NUM_SAMPLES_ = 64;

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
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int const bypass = _aaa_ints.values[ 0 ];

    if( bypass != 0 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    float2 const godrays_screen_light_position = float2( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ] );
    float  const godrays_illumination_decay    = _aaa_floats.values[ 2 ];
    float  const godrays_density               = _aaa_floats.values[ 3 ];
    float  const godrays_weight                = _aaa_floats.values[ 4 ];
    float  const godrays_decay                 = _aaa_floats.values[ 5 ];

    float2 delta_text_coord = in.uv - godrays_screen_light_position;
    delta_text_coord *= ( 1.0 / float( NUM_SAMPLES_ ) ) * godrays_density;

    float3 color = _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb;
    float  base_decay = godrays_illumination_decay;
    float2 uv = in.uv;

    for( int i = 0; i < NUM_SAMPLES_; i++ )
    {
        uv -= delta_text_coord;
        float3 color_sample = _aaa_tex_0.sample( _aaa_samp, uv ).rgb;
        color_sample *= base_decay * godrays_weight;
        color += color_sample;
        base_decay *= godrays_decay;
    }

    return float4( clamp( color, 0.0, 1.0 ), 1.0 );
}
