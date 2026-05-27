// exposure_kelvins.metal -- Path A port from vux_flare_render/exposure_kelvins.frag (c86).
// Standalone kelvin colour temperature + gamma + f-stops exposure (subset of post.metal).
// Patches :
//   - `g_input_texture` -> `_aaa_tex_0`
//   - `aaa_fu_float[0..2]` -> AaaFuFloats[0..2]
//   - `layout(location = 0) in vec2 in_texcoord` -> in.uv
//   - engine helpers (`srgb_to_linear`, `linear_to_srgb`, `gamma`, `kelvins_to_rgb`)
//     inlined (same form as post.metal ; pattern reused)

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

static inline float srgb_to_linear_( float c )
{
    if( c < 0.0 ) return 0.0;
    return ( c < 0.04045 ) ? c * ( 1.0 / 12.92 ) : pow( ( c + 0.055 ) * ( 1.0 / 1.055 ), 2.4 );
}

static inline float linear_to_srgb_( float c )
{
    if( c < 0.0 ) return 0.0;
    return ( c < 0.0031308 ) ? c * 12.92 : 1.055 * pow( c, 1.0 / 2.4 ) - 0.055;
}

static inline float gamma_( float color, float g )
{
    return 1.0 - pow( 1.0 - color, g );
}

static inline float3 kelvins_to_rgb_( float p_kelvins )
{
    float3 res;
    float t = clamp( p_kelvins, 1000.0, 40000.0 ) * 0.01;

    if( t <= 66.0 )
    {
        res.r = 1.0;
        res.g = saturate( 0.39008157876901960784 * log( t ) - 0.63184144378862745098 );
    }
    else
    {
        float tt = t - 60.0;
        res.r = saturate( 1.29293618606274509804 * pow( tt, -0.1332047592 ) );
        res.g = saturate( 1.12989086089529411765 * pow( tt, -0.0755148492 ) );
    }
    if( t >= 66.0 )
        res.b = 1.0;
    else if( t <= 19.0 )
        res.b = 0.0;
    else
        res.b = saturate( 0.54320678911019607843 * log( t - 10.0 ) - 1.19625408914 );
    return res;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const kelvin_degrees  = _aaa_floats.values[ 0 ];
    float const gamma_factor    = _aaa_floats.values[ 1 ];
    float const exposure_fstops = _aaa_floats.values[ 2 ];

    float3 col = _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb;

    col.r = srgb_to_linear_( col.r );
    col.g = srgb_to_linear_( col.g );
    col.b = srgb_to_linear_( col.b );

    float3 const bbt = kelvins_to_rgb_( kelvin_degrees );
    col *= bbt;

    float const g = clamp( gamma_factor, 0.0000001, 3.0 );
    col.r = gamma_( col.r, g );
    col.g = gamma_( col.g, g );
    col.b = gamma_( col.b, g );

    col *= pow( 2.0, exposure_fstops );

    col.r = linear_to_srgb_( col.r );
    col.g = linear_to_srgb_( col.g );
    col.b = linear_to_srgb_( col.b );

    return float4( col, 1.0 );
}
