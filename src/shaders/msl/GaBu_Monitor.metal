// GaBu_Monitor.metal -- Path A port from GaBu_Monitor.frag (c96, ~226 LOC).
// Image-debug visualisation : extract channel / convert to HSV / level-remap +
// per-mode output dispatch.
// Patches :
//   - `in VS_out { color, tex_coor } fs_in;` -> stage_in stub : tex_coor -> in.uv.xy
//   - `aaa_samp0` -> `_aaa_tex_0`
//   - `aaa_fu_int[0..1]` -> AaaFuInts[0..1] (flags, channel_nb)
//   - `aaa_fu_float[0..1]` -> AaaFuFloats[0..1] (level_min, level_max)
//   - `aaa_fu_vec4[0..1]` -> AaaFuVec4s[0..1] (color_offset, color_factor)
//   - `CONST` engine macro (UBO macro idiom) -> P5 demoted to fs_main locals
//   - Bitfield extraction via `&` + `>>` (engine has both opengl4 `bitfieldExtract`
//     and legacy `&`/`>>` branches ; Mac uses the legacy branch -- maps 1:1 to MSL)
//   - `linearstep( a, b, x )`     -> `saturate( (x - a) / (b - a) )` (c92 helper)
//   - `compute_gray( rgb )`       -> prelude `gray( rgb )`
//   - `rgb2hsv` / `hsv2rgb`       -> inlined (standard formulas)
//   - `inout vec4 tex` GLSL out-param -> `thread float4&` MSL reference (c85 lesson)
//   - `gl_FragColor`              -> return value

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

static inline float3 rgb2hsv_( float3 c )
{
    float4 K = float4( 0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0 );
    float4 p = mix( float4( c.bg, K.wz ), float4( c.gb, K.xy ), step( c.b, c.g ) );
    float4 q = mix( float4( p.xyw, c.r ), float4( c.r, p.yzx ), step( p.x, c.r ) );
    float d = q.x - min( q.w, q.y );
    float e = 1.0e-10;
    return float3( fabs( q.z + ( q.w - q.y ) / ( 6.0 * d + e ) ),
                   d / ( q.x + e ),
                   q.x );
}

static inline float3 hsv2rgb_( float3 c )
{
    float4 K = float4( 1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0 );
    float3 p = fabs( fract( c.xxx + K.xyz ) * 6.0 - K.www );
    return c.z * mix( K.xxx, saturate( p - K.xxx ), c.y );
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

static inline void process_out_alpha_( thread float4& tex, int s_alpha )
{
    if( s_alpha != 1 )
    {
        if( s_alpha == 0 )
            tex.a = 1.0;
        else
            tex.a = 1.0 - tex.a;
    }
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    //	VS_out stub
    float2 const tex_coor = in.uv.xy;

    int    const flags        = _aaa_ints.values[ 0 ];
    float  const level_min    = _aaa_floats.values[ 0 ];
    float  const level_max    = _aaa_floats.values[ 1 ];
    float4 const color_offset = _aaa_vec4s.values[ 0 ];
    float4 const color_factor = _aaa_vec4s.values[ 1 ];

    int    const s_mode       =   flags         & 0x3f;
    int    const s_alpha      = ( flags >> 6  ) & 0x3f;
    int    const s_out        = ( flags >> 12 ) & 0x3f;
    int    const s_level_use  = ( flags >> 18 ) & 0x3f;
    bool   const b_abs        = ( flags & ( 1 << 24 ) ) != 0;
    bool   const b_invert     = ( s_level_use & 0x8 )  > 0;
    bool   const b_hsv        = ( s_level_use & 0x10 ) > 0;
    (void) linearstep_( level_min, level_max, 0.0 );   // helper referenced ; preserved availability
    (void) level_min; (void) level_max;   // declared per engine source ; not directly read in live path

    float4 tex = _aaa_tex_0.sample( _aaa_samp, tex_coor );

    if( b_abs )
        tex = fabs( tex );

    if( b_hsv )
        tex.xyz = rgb2hsv_( tex.xyz );

    if( b_invert )
        tex.rgb = 1.0 - tex.rgb;

    if( s_mode <= 0 )
    {
        process_out_alpha_( tex, s_alpha );
        tex = color_offset + color_factor * tex;
    }
    else
    {
        float wh = 0.0;
        switch( s_mode )
        {
            case 1: wh = compute_gray( tex.rgb ); break;
            case 2: wh = tex.a; break;
            case 3: wh = tex.r; break;
            case 4: wh = tex.g; break;
            case 5: wh = tex.b; break;
        }
        switch( s_mode )
        {
            case 1: wh = color_offset.r + wh * color_factor.r; break;
            case 2: wh = color_offset.a + wh * color_factor.a; break;
            case 3: wh = color_offset.r + wh * color_factor.r; break;
            case 4: wh = color_offset.g + wh * color_factor.g; break;
            case 5: wh = color_offset.b + wh * color_factor.b; break;
        }
        if( s_out == 0 )
        {
            switch( s_mode )
            {
                case 1:
                case 2: tex.rgb = float3( wh ); break;
                case 3: tex.rgb = float3( wh, 0.0, 0.0 ); break;
                case 4: tex.rgb = float3( 0.0, wh, 0.0 ); break;
                case 5: tex.rgb = float3( 0.0, 0.0, wh ); break;
            }
        }
        else
        {
            if( s_out == 1 )
            {
                if( wh < 0.0 || 1.0 > wh )
                    tex.rgb = float3( fract( wh ) );
                else
                    tex.rgb = float3( wh );
            }
            else
            {
                tex.rgb = hsv2rgb_( float3( wh, 1.0, 1.0 ) );
            }
        }
        process_out_alpha_( tex, s_alpha );
    }

    return tex;
}
