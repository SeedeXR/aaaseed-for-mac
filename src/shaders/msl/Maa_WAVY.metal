// Maa_WAVY.metal -- Path A port from Maa_WAVY.frag (c90, c79 BV-defer recovered).
// First **ST_AAA_BV interface block stub** port. Pattern carry-over from
// c87 star.metal's VS_out stub : surface only the read BV members via fragment
// stage_in + opaque-white defaults until real engine-side BV plumbing lands.
// Patches :
//   - `in ST_AAA_BV BV;` (engine interface block) -> stage_in stubs :
//        BV.tex_coor[0].st -> in.uv.xy
//        BV.nor_ec.xy      -> float2( 0.0 )   (no per-vertex normal yet)
//        BV.color          -> float4( 1.0 )   (opaque-white)
//   - `aaa_tex2d[0..1]`     -> `_aaa_tex_0..1`
//   - `aaa_fu_float[0..3]`  -> `_aaa_floats.values[0..3]`
//   - `aaa_fu_int[0]`       -> `_aaa_ints.values[0]`
//   - `aaa_fu_vec4[1..2]`   -> `_aaa_vec4s.values[1..2]` (engine indexes 1,2 ; slot 0 unused)
//   - `gray( color.xyz )`   -> prelude helper
//   - `gl_FragColor`        -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

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
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    //	ST_AAA_BV interface-block stubs (c87 star pattern)
    float2 const BV_tex_coor_0 = in.uv.xy;
    float2 const BV_nor_ec_xy  = float2( 0.0 );
    float4 const BV_color      = float4( 1.0 );

    float2 const uv = BV_tex_coor_0 + _aaa_floats.values[ 3 ] * BV_nor_ec_xy;

    float4 src1 = _aaa_tex_0.sample( _aaa_samp, uv );
    float4 src2 = _aaa_tex_1.sample( _aaa_samp, uv );

    if( _aaa_ints.values[ 0 ] == 1 )
    {
        src1.rgb = float3( 1.0 ) - src1.rgb;
        src2.rgb = float3( 1.0 ) - src2.rgb;
    }

    float const f = _aaa_floats.values[ 0 ];
    float4 color = src1 * _aaa_vec4s.values[ 1 ] * f
                 + src2 * _aaa_vec4s.values[ 2 ] * ( 1.0 - f );
    if( _aaa_floats.values[ 1 ] > 0.0 && _aaa_floats.values[ 2 ] > 0.0 )
        color.a = smoothstep( _aaa_floats.values[ 1 ], _aaa_floats.values[ 2 ], gray( color.xyz ) );

    return color * BV_color;
}
