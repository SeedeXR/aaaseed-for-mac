// ps_Toy_fog.metal -- Path A port from ps_Toy_fog.frag (c94, c81 defer recovered).
// ShaderToy-style cloud-layer with Ashima simplex noise + fBm + vertical gradient.
// Patches :
//   - `in ST_AAA_BV BV;` -> **declared but NEVER USED in live mainImage** ; dropped
//     (Mac side keeps the BV stub convention but no actual surface needed).
//   - ShaderToy globals : `iTime` -> AaaFuFloats[0] ; `iResolution.xy` -> [1..2]
//   - file-scope `float cloudDensity = 1.0;` etc. (P5 file-scope) -> `constant`
//   - Ashima `snoise(vec3)` simplex noise preserved as static inline (~70 lines)
//   - `mainImage(out fragColor, in fragCoord)` -> fs_main wrapper synthesizing
//     fragCoord = in.uv * resolution.
//   - `aaa_tex2d[4]` declared but unused -> dropped.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant float cloudDensity_ = 1.0;
constant float noisiness_    = 0.35;
constant float speed_        = 0.1;
constant float cloudHeight_  = 2.5;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float3 mod289_( float3 x ) { return x - floor( x * ( 1.0 / 289.0 ) ) * 289.0; }
static inline float4 mod289_( float4 x ) { return x - floor( x * ( 1.0 / 289.0 ) ) * 289.0; }
static inline float4 permute_( float4 x ) { return mod289_( ( ( x * 34.0 ) + 1.0 ) * x ); }
static inline float4 taylorInvSqrt_( float4 r ) { return 1.79284291400159 - 0.85373472095314 * r; }

static inline float snoise_( float3 v )
{
    constexpr float2 C = float2( 1.0 / 6.0, 1.0 / 3.0 );
    constexpr float4 D = float4( 0.0, 0.5, 1.0, 2.0 );

    float3 i  = floor( v + dot( v, C.yyy ) );
    float3 x0 = v - i + dot( i, C.xxx );

    float3 g = step( x0.yzx, x0.xyz );
    float3 l = 1.0 - g;
    float3 i1 = min( g.xyz, l.zxy );
    float3 i2 = max( g.xyz, l.zxy );

    float3 x1 = x0 - i1 + C.xxx;
    float3 x2 = x0 - i2 + C.yyy;
    float3 x3 = x0 - D.yyy;

    i = mod289_( i );
    float4 p = permute_( permute_( permute_(
                  i.z + float4( 0.0, i1.z, i2.z, 1.0 ) )
                + i.y + float4( 0.0, i1.y, i2.y, 1.0 ) )
                + i.x + float4( 0.0, i1.x, i2.x, 1.0 ) );

    float n_ = 0.142857142857;
    float3 ns = n_ * D.wyz - D.xzx;

    float4 j = p - 49.0 * floor( p * ns.z * ns.z );
    float4 x_ = floor( j * ns.z );
    float4 y_ = floor( j - 7.0 * x_ );

    float4 x = x_ * ns.x + ns.yyyy;
    float4 y = y_ * ns.x + ns.yyyy;
    float4 h = 1.0 - fabs( x ) - fabs( y );

    float4 b0 = float4( x.xy, y.xy );
    float4 b1 = float4( x.zw, y.zw );

    float4 s0 = floor( b0 ) * 2.0 + 1.0;
    float4 s1 = floor( b1 ) * 2.0 + 1.0;
    float4 sh = -step( h, float4( 0.0 ) );

    float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    float3 p0 = float3( a0.xy, h.x );
    float3 p1 = float3( a0.zw, h.y );
    float3 p2 = float3( a1.xy, h.z );
    float3 p3 = float3( a1.zw, h.w );

    float4 norm = taylorInvSqrt_( float4( dot( p0, p0 ), dot( p1, p1 ), dot( p2, p2 ), dot( p3, p3 ) ) );
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    float4 m = max( 0.6 - float4( dot( x0, x0 ), dot( x1, x1 ), dot( x2, x2 ), dot( x3, x3 ) ), 0.0 );
    m = m * m;
    return 42.0 * dot( m * m, float4( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 ), dot( p3, x3 ) ) );
}

constant float maximum_ = 1.0 / 1.0 + 1.0 / 2.0 + 1.0 / 3.0 + 1.0 / 4.0 + 1.0 / 5.0 + 1.0 / 6.0 + 1.0 / 7.0 + 1.0 / 8.0;

static inline float fBm_( float3 uv )
{
    float sum = 0.0;
    for( int i = 0; i < 8; ++i )
    {
        float f = float( i + 1 );
        sum += snoise_( uv * f ) / f;
    }
    return sum / maximum_;
}

static inline float gradient_( float2 uv )
{
    return 1.0 - uv.y * uv.y * cloudHeight_;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const iTime       = _aaa_floats.values[ 0 ];
    float2 const resolution = float2( _aaa_floats.values[ 1 ], _aaa_floats.values[ 2 ] );

    float2 const fragCoord = in.uv.xy * resolution;
    float2 const uv = fragCoord / resolution;
    float3 const p = float3( uv, iTime * speed_ );
    float3 const someRandomOffset = float3( 0.1, 0.3, 0.2 );
    float2 const duv = float2( fBm_( p ), fBm_( p + someRandomOffset ) ) * noisiness_;
    float  const q = gradient_( uv + duv ) * cloudDensity_;
    return float4( q, q, q, 1.0 );
}
