// ps_Maarko_v0_2.metal -- Path A port from ps_Maarko_v0_2.frag (c85).
// Threshold-based opacity ramp with pow-falloff ; channel-max or gray pick.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[0..3]`  -> `_aaa_floats.values[0..3]`
//   - `aaa_fu_int[0..1]`    -> `_aaa_ints.values[0..1]`
//   - `aaa_fu_out` individual uniform -> `_aaa_floats.values[8]`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gray( src.xyz )`     -> prelude helper
//   - `discard`             -> MSL `discard_fragment()`
//   - Source initialises `dst` to garbage and writes `dst.xyz += ...` ; Mac initialises
//     to opaque-black for deterministic compile (behaviour-preserving guard).

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

#ifndef AAA_MSL_PRELUDE_DEFINED
#define AAA_MSL_PRELUDE_DEFINED
constexpr constant float3 aaa_gray_conv = float3( 0.2989, 0.5870, 0.1140 );
static inline float gray( float3 a ) { return dot( a.xyz, aaa_gray_conv ); }
static inline float gray( float4 a ) { return dot( a.xyz, aaa_gray_conv ); }
#endif

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
    float const aaa_fu_out = _aaa_floats.values[ 8 ];
    float4 src = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );

    if( _aaa_ints.values[ 1 ] == 1 )
    {
        src.x = 1.0 - src.x;
        src.y = 1.0 - src.y;
        src.z = 1.0 - src.z;
    }

    float g;
    if( _aaa_ints.values[ 0 ] == 0 )
    {
        g = max( max( src.x, src.y ), src.z );
    }
    else
    {
        g = gray( src.xyz );
    }

    if( g < _aaa_floats.values[ 2 ] )
        discard_fragment();

    float const a = ( g - _aaa_floats.values[ 0 ] )
                  / ( _aaa_floats.values[ 1 ] - _aaa_floats.values[ 0 ] );

    float4 dst = float4( 0.0, 0.0, 0.0, 0.0 );   // deterministic init (source leaves uninit)
    dst.a   = _aaa_floats.values[ 2 ] + pow( a, _aaa_floats.values[ 3 ] );
    dst.xyz = src.xyz * aaa_fu_out;
    return dst;
}
