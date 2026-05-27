// post.metal -- Path A port from vux_flare_render/post.frag (c84).
// Post-process : Kelvin colour temperature + gamma + f-stops exposure +
// fade-to-colour, with sRGB <-> linear conversion.
// Patches :
//   - `g_input_texture` -> `_aaa_tex_0`
//   - `aaa_fu_float[0..2]` (Kelvin/Gamma/FStops) + [8..9] (fade_bottom/top) -> values[N]
//   - `aaa_vu_int[0]` (bypass) -> `_aaa_ints.values[0]` (engine has aaa_vu_int + aaa_fu_int ;
//     Mac merges both into AaaFuInts since the runtime upload path is shared)
//   - `aaa_fu_vec4[0]` (color_factor) + [1] (fade_color) -> values[0..1]
//   - `layout(location = 0) in vec2 in_texcoord` -> in.uv
//   - engine helpers `clamp_01` / `saturate` / `srgb_to_linear` / `linear_to_srgb`
//     inlined here (saturate -> MSL `saturate` builtin ; clamp_01 -> saturate ;
//     srgb / linear curves : engine uses the standard sRGB approximation, inlined)
//   - `gl_FragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

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
    return ( c <= 0.04045 ) ? ( c / 12.92 ) : pow( ( c + 0.055 ) / 1.055, 2.4 );
}
static inline float linear_to_srgb_( float c )
{
    return ( c <= 0.0031308 ) ? ( 12.92 * c ) : ( 1.055 * pow( c, 1.0 / 2.4 ) - 0.055 );
}

static inline float3 apply_gamma_( float3 color, float3 gamma )
{
    return float3( 1.0 ) - pow( float3( 1.0 ) - color, gamma );
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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float  const Kelvins      = _aaa_floats.values[ 0 ];
    float  const Gamma        = _aaa_floats.values[ 1 ];
    float  const FStops       = _aaa_floats.values[ 2 ];
    float  const fade_bottom  = _aaa_floats.values[ 8 ];
    float  const fade_top     = _aaa_floats.values[ 9 ];
    int    const bypass       = _aaa_ints.values[ 0 ];
    float4 const color_factor = _aaa_vec4s.values[ 0 ];
    float4 const fade_color   = _aaa_vec4s.values[ 1 ];

    if( bypass != 0 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    float3 col = _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb;
    col = saturate( col * color_factor.rgb );

    col.r = srgb_to_linear_( col.r );
    col.g = srgb_to_linear_( col.g );
    col.b = srgb_to_linear_( col.b );

    col *= kelvins_to_rgb_( Kelvins );

    float gam = clamp( Gamma, 0.0000001, 3.0 );
    col = apply_gamma_( col, float3( gam ) );

    col = col * pow( 2.0, FStops );

    col.r = linear_to_srgb_( col.r );
    col.g = linear_to_srgb_( col.g );
    col.b = linear_to_srgb_( col.b );

    float fade = smoothstep( fade_top, fade_bottom, in.uv.y );
    float4 result;
    result.rgb = clamp( mix( col, fade_color.rgb, fade ), 0.0, 1.0 );
    result.a   = 1.0;
    return result;
}
