// ps_sepia.metal -- Path A port from ps_sepia.frag (c81).
// Sepia tone transfer (light/dark + desaturate + toning).
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[0..1]`  -> `_aaa_floats.values[0..1]`
//   - `aaa_fu_vec4[0]`      -> `_aaa_vec4s.values[0]`
//   - file-scope assignments (`float desaturate = ...`) moved inside fs_main (P5)
//   - `const vec3 DarkColor = {x,y,z};` brace-init -> `float3()` ctor
//   - `lerp` -> MSL `mix` (P6b)
//   - `blend( ... )` -> identity pass-through (c72)
//   - `gl_TexCoord[0].st` -> `in.uv.xy`

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

static inline float3 sepia_( float3 inColor, float3 LightColor, float3 DarkColor, float3 grayXfer, float desaturate, float toning )
{
    float3 scnColor = LightColor * inColor;
    float  g        = dot( grayXfer, scnColor );
    float3 muted    = mix( scnColor, float3( g ), desaturate );
    float3 sep      = mix( DarkColor, LightColor, g );
    return mix( muted, sep, toning );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float  const desaturate = _aaa_floats.values[ 1 ];
    float  const toning     = _aaa_floats.values[ 0 ];
    float3 const LightColor = _aaa_vec4s.values[ 0 ].rgb;
    float3 const DarkColor  = float3( 0.2, 0.05, 0.0 );
    float3 const grayXfer   = float3( 0.3, 0.59, 0.11 );

    float4 const color = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    float3 const result = sepia_( color.rgb, LightColor, DarkColor, grayXfer, desaturate, toning );
    return float4( result, color.a );   // blend() pass-through
}
