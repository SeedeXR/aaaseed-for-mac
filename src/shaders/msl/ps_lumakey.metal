// ps_lumakey.metal -- Path A port from ps_lumakey.frag (c79).
// YCbCr-luma-distance keying with smoothstep falloff.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_vec4[0]`      -> `_aaa_vec4s.values[0]`
//   - `aaa_fu_float[0..1]`  -> `_aaa_floats.values[0..1]`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `vec3 std_Y = { x,y,z };` brace-init -> MSL `float3()` constructor
//   - `blend( vec4(...) )`  -> `vec4(...)` pass-through (c72)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };

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
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    constexpr float3 std_Y     = float3(  0.257,   0.504,   0.098  );
    constexpr float3 std_G     = float3( -0.1482, -0.291,   0.4391 );
    constexpr float3 std_B     = float3(  0.440,  -0.368,  -0.0718 );
    constexpr float3 stdbias_x = float3(  0.0625,  0.5,     0.5    );

    float3 const luma_key       = _aaa_vec4s.values[ 0 ].rgb;
    float  const threshold_high = _aaa_floats.values[ 0 ];
    float  const threshold_low  = _aaa_floats.values[ 1 ];

    float4 const color_tex = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    float3 const luma_color = float3(
        dot( color_tex.rgb, std_Y ),
        dot( color_tex.rgb, std_G ),
        dot( color_tex.rgb, std_B ) ) + stdbias_x;

    float const dist  = distance( luma_color, luma_key );
    float const alpha = 1.0 - smoothstep( threshold_low, threshold_high, dist );

    return float4( color_tex.rgb, alpha );   // blend() pass-through (c72)
}
