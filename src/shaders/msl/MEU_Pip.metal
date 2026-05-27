// MEU_Pip.metal -- Path A port from MEU/MEU_Pip.frag (c92).
// MEU "Picture-in-picture" : 2-tex blend + bitflag-driven modes (inverse / tint /
// clamp / mask / incrust) + per-tile fuzzy edge.
// Patches :
//   - `in ST_AAA_BV BV;` -> stage_in stub : BV.tex_coor[0].st -> in.uv.xy ;
//     BV.color -> float4(1) (where read)
//   - file-scope `CONST` (UBO macro) -> P5
//   - `TEX_A` / `TEX_B` / `TEX_MASK` -> `_aaa_tex_0..2`
//   - `aaa_fu_vec4[1..7]` -> AaaFuVec4s[1..7]
//   - `aaa_fu_float[0..4]` -> AaaFuFloats[0..4]
//   - `aaa_fu_int[0..3]` -> AaaFuInts[0..3]
//   - Engine helpers : `compute_gray(rgb)` / `gray(rgb)` / `linearstep(a,b,x)` /
//     `clamp_01(x)` all stubbed as inline static helpers.
//     `linearstep(a,b,x)` defined as `clamp((x-a)/(b-a), 0, 1)` (no smoothing).
//   - `discard` -> `discard_fragment()`
//   - `gl_FragColor` -> return value
//   - `texture2D(s,uv)` -> `s.sample(samp, uv)`

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
static inline float compute_gray( float3 a ) { return gray( a ); }
#endif

static inline float linearstep_( float a, float b, float x )
{
    return saturate( ( x - a ) / ( b - a ) );
}

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
    texture2d< float >    _aaa_tex_2    [[texture(2)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float3 const col_gamma         = _aaa_vec4s.values[ 3 ].rgb;
    float3 const col_offset        = _aaa_vec4s.values[ 4 ].rgb;
    float4 const col_back_tint     = _aaa_vec4s.values[ 5 ];
    float3 const col_incrust       = _aaa_vec4s.values[ 6 ].xyz;
    float  const incrust_min       = _aaa_floats.values[ 1 ];
    float  const incrust_max       = _aaa_floats.values[ 2 ];
    float  const mask_incrust_min  = _aaa_floats.values[ 3 ];
    float  const mask_incrust_max  = _aaa_floats.values[ 4 ];
    float4 const src1_mul          = _aaa_vec4s.values[ 1 ];
    float4 const src2_mul          = _aaa_vec4s.values[ 2 ];
    float2 const fuz               = _aaa_vec4s.values[ 7 ].xy;
    bool   const b_inverse         = ( _aaa_ints.values[ 0 ] & 1 ) != 0;
    bool   const b_tint            = ( _aaa_ints.values[ 0 ] & 2 ) != 0;
    bool   const b_clamp           = ( _aaa_ints.values[ 0 ] & 4 ) != 0;
    int    const s_incrust         = _aaa_ints.values[ 1 ];
    bool   const b_mask            = ( _aaa_ints.values[ 3 ] & 1 )   != 0;
    bool   const b_mask_see        = ( _aaa_ints.values[ 3 ] & 16 )  != 0;
    bool   const b_mask_mul        = ( _aaa_ints.values[ 3 ] & 256 ) != 0;

    float const NORMALIZE_COLOR_DIST_FACTOR = 1.0 / sqrt( 3.0 );

    float4 const BV_color = float4( 1.0 );   // BV stub

    float2 const uv = in.uv.xy;   // BV.tex_coor[0].st stub

    float4 mask = float4( 1.0 );
    if( b_mask )
    {
        mask = _aaa_tex_2.sample( _aaa_samp, uv );
        float mask_factor = compute_gray( mask.rgb ) * mask.a;
        if( mask_incrust_min > 0.0 || mask_incrust_max > 0.0 )
            mask_factor = smoothstep( mask_incrust_min, mask_incrust_max, mask_factor );
        if( mask_factor <= 0.0 )
            discard_fragment();
        if( b_mask_mul )
            mask.a = mask_factor;
        else
            mask = float4( 1.0, 1.0, 1.0, mask_factor );
        if( b_mask_see )
            return mask;
    }

    float4 src1 = _aaa_tex_0.sample( _aaa_samp, uv );
    float4 src2 = _aaa_tex_1.sample( _aaa_samp, uv );

    float const f = _aaa_floats.values[ 0 ];
    float4 color = src1 * src1_mul * ( 1.0 - f ) + src2 * src2_mul * f;

    if( b_inverse )
        color.rgb = float3( 1.0 ) - color.rgb;

    switch( s_incrust )
    {
        case 1:
        {
            float const v = gray( color.xyz );
            color.a = smoothstep( incrust_min, incrust_max, v );
            break;
        }
        case 2:
        {
            float3 const diff = color.xyz - col_incrust;
            float const v = length( diff ) * NORMALIZE_COLOR_DIST_FACTOR;
            color.a = linearstep_( incrust_min, incrust_max, v );
            break;
        }
    }

    if( b_tint )
    {
        float const g = gray( color.rgb );
        color.rgb = mix( col_back_tint.rgb, BV_color.rgb, g );
        color.a *= BV_color.a;
    }
    else
    {
        color = color * BV_color;
    }

    if( color.a <= 0.0 )
        discard_fragment();

    color.rgb += col_offset;
    color.rgb  = pow( color.rgb, col_gamma );
    if( b_clamp )
        color = saturate( color );

    float2 const fuz_factor = saturate( min( fract( uv ), fract( float2( 1.0 ) - uv ) ) * fuz );
    color.a *= fuz_factor.x * fuz_factor.y;

    return color * mask;
}
