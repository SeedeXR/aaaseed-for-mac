// ps_scanly.metal -- Path A port from ps_scanly.frag (c87, 185 LOC).
// Rotoscope-style intensity quantization with optional Sobel-edge masking.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[0..4]`  -> `_aaa_floats.values[0..4]`
//   - file-scope `vec4 specExp = {x, y, z, w};` brace-init -> `float4()` ctor
//   - `const vec3 quant`    -> `constant float3 quant_`
//   - `gl_TexCoord[0].xy/st` -> `in.uv.xy`
//   - `pow( r1, specExp )` (vec4 ^ vec4) -> MSL `pow(vec4, vec4)` (preserved)
//   - `1 / hue.x`           -> `1.0 / hue.x` (MSL strict on int/float promotion)
//   - sobel / sobel2 / scanly helpers preserved as static inlines
//   - `gl_FragColor`        -> return value
// NB : engine source has a `vec3 sobel()` helper but the `main()` only ever
// calls `sobel2()` ; the `sobel()` function is dead. Mac drops it to avoid
// MSL warning about unused functions in libraries.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant float3 quant_ = float3( 0.2125, 0.7154, 0.0721 );

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float3 sobel2_(
    float2 st,
    texture2d< float > tex, sampler samp,
    constant AaaFuFloats& fl )
{
    float2 const stp0 = float2( 1.0 / fl.values[ 0 ], 0.0 );
    float2 const st0p = float2( 0.0, 1.0 / fl.values[ 3 ] );
    float2 const stpp = float2( 1.0 / fl.values[ 2 ], 1.0 / fl.values[ 3 ] );
    float2 const stpm = float2( 1.0 / fl.values[ 2 ], -1.0 / fl.values[ 3 ] );

    float const im1m1 = dot( tex.sample( samp, st - stpp ).rgb, quant_ );
    float const ip1p1 = dot( tex.sample( samp, st + stpp ).rgb, quant_ );
    float const im1p1 = dot( tex.sample( samp, st - stpm ).rgb, quant_ );
    float const ip1m1 = dot( tex.sample( samp, st + stpm ).rgb, quant_ );
    float const im10  = dot( tex.sample( samp, st - stp0 ).rgb, quant_ );
    float const ip10  = dot( tex.sample( samp, st + stp0 ).rgb, quant_ );
    float const i0m1  = dot( tex.sample( samp, st - st0p ).rgb, quant_ );
    float const i0p1  = dot( tex.sample( samp, st + st0p ).rgb, quant_ );

    float const h = -im1p1 - 2.0 * i0p1 - ip1p1 + im1m1 + 2.0 * i0m1 + ip1m1;
    float const v = -im1m1 - 2.0 * im10  - im1p1 + ip1m1 + 2.0 * ip10  + ip1p1;
    float const mag = pow( h * h + v * v, 0.5 );
    return ( mag > fl.values[ 4 ] ) ? float3( 1.0 ) : float3( 0.0 );
}

static inline float3 scanly_(
    float2 uv, float3 sobel_in,
    texture2d< float > tex, sampler samp )
{
    float4 const t = tex.sample( samp, uv );

    float4 hue;
    hue.x = t.x + t.y + t.z;
    hue.y = 1.0 / hue.x;
    float3 color = t.xyz * hue.y;

    hue.z = ( hue.x >= 0.25 ) ? 1.0 : 0.0;
    hue.z *= 0.5;

    hue.y = ( hue.x >= 0.8 ) ? 1.0 : 0.0;
    hue.y *= 0.5;

    hue.x = ( hue.x >= 1.5 ) ? 1.0 : 0.0;
    hue.x *= 1.5;

    hue.x = hue.x + hue.y + hue.z;
    color *= hue.x;

    float3 mask = sobel_in;
    mask.x = ( mask.x < 0.8 ) ? 1.0 : 0.0;
    //	The engine ALSO writes mask.y but never reads it -- preserved as a no-op.
    color *= mask.x;
    return color;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float3 result;
    if( _aaa_floats.values[ 0 ] >= 1.0 )
    {
        float3 const sob = sobel2_( in.uv.xy, _aaa_tex_0, _aaa_samp, _aaa_floats );
        result = scanly_( in.uv.xy, sob, _aaa_tex_0, _aaa_samp );
    }
    else
    {
        result = scanly_( in.uv.xy, float3( 0.0 ), _aaa_tex_0, _aaa_samp );
    }
    return float4( result, 1.0 );
}
