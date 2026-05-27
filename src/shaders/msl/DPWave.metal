// DPWave.metal -- Path A port from Jean/DPWave.frag (c91, c84 carry recovered).
// Procedural water-surface : voronoi + simplex noise + getwavesHI iter ; blends col1<->col2 by value.
// Patches :
//   - `in ST_AAA_BV BV;` -> stage_in stub : BV.tex_coor[0].st -> in.uv.xy
//   - file-scope `int noise_fn = aaa_fu_int[0];` etc. (engine UBO-macro idiom) -> P5
//     (moved into fs_main since AaaFuFloats binding is fragment-function-scoped)
//   - `aaa_tex2d[4]`      -- declared but **unused** in live path ; dropped
//   - `aaa_fu_*` slots    -> AaaFuFloats/Vec4s/Ints as usual
//   - `mod( x, K )`       -> `fmod( x, K )` for positives
//   - `gl_FragColor`      -> return value
//   - `voronoi` / `noise2` / `getwavesHI` / `fbm` helpers preserved as static inlines

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

constant float SCALE_                 = 8.0;
constant float GOLDEN_ANGLE_RADIAN_   = 2.39996;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float2 random2_( float2 p )
{
    return fract( sin( float2( dot( p, float2( 127.1, 311.7 ) ),
                               dot( p, float2( 269.5, 183.3 ) ) ) ) * 43758.5453 );
}

static inline float2 hash_( float2 p )
{
    p = float2( dot( p, float2( 127.1, 311.7 ) ),
                dot( p, float2( 269.5, 183.3 ) ) );
    return -1.0 + 2.0 * fract( sin( p ) * 43758.5453123 );
}

static inline float noise_( float2 p )
{
    float const K1 = 0.366025404;
    float const K2 = 0.211324865;
    float2 i = floor( p + ( p.x + p.y ) * K1 );
    float2 a = p - i + ( i.x + i.y ) * K2;
    float2 o = step( a.yx, a.xy );
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
    float3 h = max( 0.5 - float3( dot( a, a ), dot( b, b ), dot( c, c ) ), 0.0 );
    float3 n = h * h * h * h
             * float3( dot( a, hash_( i + 0.0 ) ),
                       dot( b, hash_( i + o ) ),
                       dot( c, hash_( i + 1.0 ) ) );
    return dot( n, float3( 70.0 ) );
}

static inline float random_( float2 st )
{
    return fract( sin( dot( st, float2( 12.9898, 78.233 ) ) ) * 43758.5453123 );
}

static inline float noise2_( float2 st )
{
    float2 i = floor( st );
    float2 f = fract( st );
    float a = random_( i );
    float b = random_( i + float2( 1.0, 0.0 ) );
    float c = random_( i + float2( 0.0, 1.0 ) );
    float d = random_( i + float2( 1.0, 1.0 ) );
    float2 u = f * f * ( 3.0 - 2.0 * f );
    return mix( a, b, u.x ) + ( c - a ) * u.y * ( 1.0 - u.x ) + ( d - b ) * u.x * u.y;
}

static inline float wave_( float2 uv, float2 emitter, float speed, float phase, float time )
{
    float dst = distance( uv, emitter );
    return pow( 0.5 + 0.5 * sin( dst * phase - time * speed ), 5.0 );
}

static inline float getwavesHI_( float2 uv, float time )
{
    float w = 0.0;
    float sw = 0.0;
    float iter = 0.0;
    float ww = 1.0;
    uv += time * 0.5;
    for( int i = 0; i < 24; i++ )
    {
        w += ww * wave_( uv * 0.06, float2( sin( iter ), cos( iter ) ) * 10.0, 2.0 + iter * 0.08, 2.0 + iter * 3.0, time );
        sw += ww;
        ww = mix( ww, 0.0115, 0.4 );
        iter += GOLDEN_ANGLE_RADIAN_;
    }
    return w / sw;
}

static inline float fbm_( float2 uv, int noise_fn, float time )
{
    float ret;
    if( noise_fn == 0 )
    {
        ret =          noise_(  uv );
        uv *= 2.;
        ret += 0.5   * noise_(  uv );
        uv *= 2.;
        ret += 0.125 * noise_(  uv );
        uv *= 2.;
        ret += 0.0625 * noise_( uv );
    }
    else if( noise_fn == 1 )
    {
        ret =          noise2_(  uv );
        uv *= 2.;
        ret += 0.5   * noise2_(  uv );
        uv *= 2.;
        ret += 0.125 * noise2_(  uv );
        uv *= 2.;
        ret += 0.0625 * noise2_( uv );
    }
    else
    {
        ret = getwavesHI_( uv, time );
    }
    return ret;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int    const noise_fn = _aaa_ints.values[ 0 ];
    float  const time     = _aaa_floats.values[ 0 ];
    float  const scale    = _aaa_floats.values[ 1 ];
    float4 const col1     = _aaa_vec4s.values[ 0 ];
    float4 const col2     = _aaa_vec4s.values[ 1 ];

    //	ST_AAA_BV stub : only BV.tex_coor[0].st read in live path
    float2 uv = in.uv.xy;
    uv = ( uv - 0.5 ) * 2.0;

    float const val = fbm_( uv * scale + time, noise_fn, time );
    return col1 * val + col2 * ( 1.0 - val );
}
