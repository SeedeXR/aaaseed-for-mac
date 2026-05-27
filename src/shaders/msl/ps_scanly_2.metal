// ps_scanly_2.metal -- Path A port from ps_scanly_2.frag (c81).
// 3x3 Sobel-with-quantize : edge -> black, else quantized colour.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[0..3]`  -> `_aaa_floats.values[0..3]`
//   - file-scope `const vec3 quant` -> `constant float3`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `ivec3 irgb = ivec3( rgb.rgb )` -> `int3()` truncation preserved
//   - `gl_FragColor`        -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant float3 quant = float3( 0.2125, 0.7154, 0.0721 );

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float2 const st  = in.uv.xy;
    float3 rgb       = _aaa_tex_0.sample( _aaa_samp, st ).rgb;

    float2 const stp0 = float2( 1.0 / _aaa_floats.values[ 2 ], 0.0 );
    float2 const st0p = float2( 0.0, 1.0 / _aaa_floats.values[ 3 ] );
    float2 const stpp = float2( 1.0 / _aaa_floats.values[ 2 ], 1.0 / _aaa_floats.values[ 3 ] );
    float2 const stpm = float2( 1.0 / _aaa_floats.values[ 2 ], -1.0 / _aaa_floats.values[ 3 ] );

    float const im1m1 = dot( _aaa_tex_0.sample( _aaa_samp, st - stpp ).rgb, quant );
    float const ip1p1 = dot( _aaa_tex_0.sample( _aaa_samp, st + stpp ).rgb, quant );
    float const im1p1 = dot( _aaa_tex_0.sample( _aaa_samp, st - stpm ).rgb, quant );
    float const ip1m1 = dot( _aaa_tex_0.sample( _aaa_samp, st + stpm ).rgb, quant );
    float const im10  = dot( _aaa_tex_0.sample( _aaa_samp, st - stp0 ).rgb, quant );
    float const ip10  = dot( _aaa_tex_0.sample( _aaa_samp, st + stp0 ).rgb, quant );
    float const i0m1  = dot( _aaa_tex_0.sample( _aaa_samp, st - st0p ).rgb, quant );
    float const i0p1  = dot( _aaa_tex_0.sample( _aaa_samp, st + st0p ).rgb, quant );

    float const h = -im1p1 - 2.0 * i0p1 - ip1p1 + im1m1 + 2.0 * i0m1 + ip1m1;
    float const v = -im1m1 - 2.0 * im10  - im1p1 + ip1m1 + 2.0 * ip10  + ip1p1;
    float const mag = pow( h * h + v * v, 0.5 );

    if( mag > _aaa_floats.values[ 1 ] )
        return float4( 0.0, 0.0, 0.0, 1.0 );

    rgb *= _aaa_floats.values[ 0 ];
    rgb += float3( 0.5 );
    int3  const irgb = int3( rgb );
    rgb = float3( irgb ) / _aaa_floats.values[ 0 ];
    return float4( rgb, 1.0 );
}
