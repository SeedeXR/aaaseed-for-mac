// thinks.metal -- Path A port from WebGL/3D/thinks.frag (c93, sebastien durand 2016).
// Spiral-noise volumetric raymarcher with HSV light scattering (Type-2 Supernova fork).
// Patches :
//   - `in ST_AAA_BV BV;` -> stage_in stub : BV.tex_coor[0].xy -> in.uv.xy
//   - `aaa_tex2d[0]`     -> `_aaa_tex_0`
//   - `aaa_fu_float[0..6]` -> `_aaa_floats.values[0..6]`
//   - `aaa_fu_int[0..1]` -> `_aaa_ints.values[0..1]`
//   - `aaa_fu_vec4[0..3]` -> `_aaa_vec4s.values[0..3]`
//   - `texture2D( tex, uv, -100.0 )` (negative LOD bias) -> `tex.sample( samp, uv )`
//     (MSL doesn't take negative LOD bias as second sample arg ; engine bias dropped)
//   - file-scope `float nudge = aaa_fu_float[4];` (UBO macro) -> P5
//   - `mod( x, 5.0 )` -> `fmod( x, 5.0 )`
//   - `gl_FragColor` -> return value
//   - dynamic loop bounds (`aaa_fu_int[0]`, `aaa_fu_int[1]`) -- MSL accepts uniform-bound loops.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

constant int SPIRAL_NOISE_ITER_ = 16;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float hash_( float3 p )
{
    return fract( sin( dot( p, float3( 127.1, 311.7, 758.5453123 ) ) ) * 43758.5453123 );
}

static inline float pn_( float3 x, texture2d< float > tex, sampler samp )
{
    float3 const p = floor( x );
    float3 f = fract( x );
    f *= f * ( 3.0 - f - f );
    float2 const uv = ( p.xy + float2( 37.0, 17.0 ) * p.z ) + f.xy;
    float2 const rg = tex.sample( samp, ( uv + 0.5 ) / 256.0 ).yx;
    return 2.4 * mix( rg.x, rg.y, f.z ) - 1.0;
}

static inline float SpiralNoiseC_( float3 p, float4 id, float nudge, float normalizer, int iter_count )
{
    float iter = 2.0;
    float n    = 2.0 - id.x;
    for( int i = 0; i < iter_count; i++ )
    {
        n += -fabs( sin( p.y * iter ) + cos( p.x * iter ) ) / iter;
        p.xy += float2( p.y, -p.x ) * nudge;
        p.xy *= normalizer;
        p.xz += float2( p.z, -p.x ) * nudge;
        p.xz *= normalizer;
        iter *= id.y + 0.733733;
    }
    return n;
}

static inline float map_( float3 p, float4 id, float nudge, float normalizer, int iter_count, texture2d< float > tex, sampler samp )
{
    float const k = 2.0 * id.w + 0.1;
    return k * ( 0.5 + SpiralNoiseC_( p.zxy * 0.4132 + 333.0, id, nudge, normalizer, iter_count ) * 3.0
               + pn_( p * 8.5, tex, samp ) * 0.12 );
}

static inline float3 hsv2rgb_( float x, float y, float z )
{
    return z + z * y * ( clamp( fabs( fmod( x * 6.0 + float3( 0.0, 4.0, 2.0 ), 6.0 ) - 3.0 ) - 1.0, 0.0, 1.0 ) - 1.0 );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float const nudge      = _aaa_floats.values[ 4 ];
    float const normalizer = 1.0 / sqrt( 1.0 + nudge * nudge );

    float2 const frag_coord = in.uv.xy;
    float4 const sliderVal  = _aaa_vec4s.values[ 0 ];

    float3 const ro = _aaa_vec4s.values[ 2 ].xyz * _aaa_floats.values[ 3 ] * _aaa_vec4s.values[ 2 ].w;
    float3 const rd = normalize( float3( ( frag_coord - 0.5 ) * 2.0, _aaa_floats.values[ 5 ] ) );

    //	renderSuperstructure body inlined
    float const max_dist = 20.0;
    float       td  = 0.0;
    float       t   = 0.3 * hash_( float3( hash_( rd ) ) + _aaa_floats.values[ 0 ] );
    float3      pos = float3( 0.0 );
    float4      sum = float4( 0.0 );

    float const rRef = 2.0 * sliderVal.x;
    float const h    = 0.05 + 0.25 * sliderVal.z;

    int const ray_iter = _aaa_ints.values[ 0 ];
    int const spiral_iter = _aaa_ints.values[ 1 ];

    for( int i = 0; i < ray_iter; i++ )
    {
        if( td > 0.9 || sum.a > 0.99 || t > max_dist )
            break;

        float const a = smoothstep( max_dist, 0.0, t );
        pos = ro + t * rd;
        float const d = fabs( map_( pos, sliderVal, nudge, normalizer, spiral_iter, _aaa_tex_0, _aaa_samp ) ) + 0.07;

        float const lDist = max( length( fmod( pos + 2.5, 5.0 ) - 2.5 ), 0.001 );
        float const noi   = pn_( 0.03 * pos, _aaa_tex_0, _aaa_samp );
        float3 const lightColor = mix(
            hsv2rgb_( noi,       0.5, 0.6 ),
            hsv2rgb_( noi + 0.3, 0.5, 0.6 ),
            smoothstep( rRef * 0.5, rRef * 2.0, lDist ) );
        sum.rgb += a * lightColor / exp( lDist * lDist * lDist * 0.08 ) / 30.0;

        if( d < h )
        {
            td += ( 1.0 - td ) * ( h - d ) + 0.005;
            sum.rgb += sum.a * sum.rgb * 0.25 / lDist;
            sum     += ( 1.0 - sum.a ) * 0.05 * td * a;
        }
        td += 0.015;
        t  += max( d * 0.08 * max( min( lDist, d ), 2.0 ), 0.01 );
    }

    sum = clamp( sum, 0.0, 1.0 );
    sum.xyz *= sum.xyz * ( 3.0 - sum.xyz - sum.xyz );

    return float4( _aaa_floats.values[ 6 ] * sum.xyz + _aaa_vec4s.values[ 3 ].xyz, 1.0 );
}
