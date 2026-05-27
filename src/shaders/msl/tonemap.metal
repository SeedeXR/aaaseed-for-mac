// tonemap.metal -- Path A port from vux_flare_render/tonemap.frag (c86).
// Uncharted 2 filmic tonemap + inverse gamma + auto-keying exposure scale.
// Patches :
//   - `g_input_texture` -> `_aaa_tex_0`
//   - `aaa_fu_float[0..10]` (Uncharted2 params + keying/average/exposure) -> AaaFuFloats[0..10]
//   - `aaa_vu_int[0]` (bypass) -> AaaFuInts[0]
//   - `layout(location = 0) in vec2 in_texcoord` -> in.uv

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float3 Uncharted2Tonemap_(
    float3 x,
    float shoulder_strength, float linear_strength, float linear_angle,
    float toe_strength, float toe_numerator, float toe_denominator )
{
    return ( ( x * ( shoulder_strength * x + linear_angle * linear_strength )
             + toe_strength * toe_numerator )
           / ( x * ( shoulder_strength * x + linear_strength )
             + toe_strength * toe_denominator ) )
         - toe_numerator / toe_denominator;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float const shoulder_strength      = _aaa_floats.values[ 0 ];
    float const linear_strength        = _aaa_floats.values[ 1 ];
    float const linear_angle           = _aaa_floats.values[ 2 ];
    float const toe_strength           = _aaa_floats.values[ 3 ];
    float const toe_numerator          = _aaa_floats.values[ 4 ];
    float const toe_denominator        = _aaa_floats.values[ 5 ];
    float const linear_white_point     = _aaa_floats.values[ 6 ];
    float const exposure_bias          = _aaa_floats.values[ 7 ];
    float const inverse_gamma          = _aaa_floats.values[ 8 ];
    float const keying                 = _aaa_floats.values[ 9 ];
    float const tone_average           = _aaa_floats.values[ 10 ];
    int   const bypass                 = _aaa_ints.values[ 0 ];

    float3 base_color = _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb;
    if( bypass != 0 )
        return float4( base_color, 1.0 );

    float const a_over_bar = keying * ( 1.0 / tone_average );
    base_color *= a_over_bar;

    float3 const curr = Uncharted2Tonemap_(
        exposure_bias * base_color,
        shoulder_strength, linear_strength, linear_angle,
        toe_strength, toe_numerator, toe_denominator );

    float3 const white_scale = float3( 1.0 ) / Uncharted2Tonemap_(
        float3( linear_white_point ),
        shoulder_strength, linear_strength, linear_angle,
        toe_strength, toe_numerator, toe_denominator );

    float3 const color = curr * white_scale;
    return float4( pow( color, float3( inverse_gamma ) ), 1.0 );
}
