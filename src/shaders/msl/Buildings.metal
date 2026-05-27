// Buildings.metal -- Path A port from WebGL/3D/Buildings.frag (c88).
// Forked from nimitz's "Neon parallax" (ShaderToy XssXz4).
// Patches :
//   - `time` / `mouse` / `resolution` (individual non-array uniforms)
//     -> ShaderToy convention : AaaFuFloats[0..4] :
//        [0] = time   [1..2] = resolution.xy   [3..4] = mouse.xy
//   - `gl_FragCoord.xy` -> `in.position.xy` (P2 ; pixel coords)
//   - `mat2(0.707,-0.707,0.707,0.707)` GLSL row-major -> MSL `float2x2(col0,col1)`
//     column-major (rotation matrix construction flips)
//   - `sin( buv * K + L ).x` etc. : `sin(float2)` returns float2 ; swizzle preserved
//   - `gl_FragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float pulse_( float cn, float wi, float x )
{
    return 1.0 - smoothstep( 0.0, wi, fabs( x - cn ) );
}

static inline float hash11_( float n )
{
    return fract( sin( n ) * 43758.5453 );
}

static inline float2 hash22_( float2 p )
{
    p = float2( dot( p, float2( 127.1, 311.7 ) ),
                dot( p, float2( 269.5, 183.3 ) ) );
    return fract( sin( p ) * 43758.5453 );
}

static inline float2 field_( float2 p )
{
    float2 n = floor( p );
    float2 f = fract( p );
    float2 m = float2( 1.0 );
    float2 o = hash22_( n ) * 0.17;
    float2 r = f + o - 0.5;
    float  d = fabs( r.x ) + fabs( r.y );
    if( d < m.x )
    {
        m.x = d;
        m.y = hash11_( dot( n, float2( 1.0, 2.0 ) ) );
    }
    return m;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const time         = _aaa_floats.values[ 0 ];
    float const resolution_x = _aaa_floats.values[ 1 ];
    float const resolution_y = _aaa_floats.values[ 2 ];
    (void) _aaa_floats.values[ 3 ];   // mouse.x unused in live path
    (void) _aaa_floats.values[ 4 ];   // mouse.y unused

    float2 uv = in.position.xy / float2( resolution_x, resolution_y ) - 0.5;
    uv.x *= resolution_x / resolution_y * 0.9;
    uv *= 4.0;

    float2 p = uv * 0.01;
    p *= 1.0 / ( p - 1.0 );

    uv.y += time * 1.2;
    uv.x += sin( time * 0.3 ) * 0.8;
    float2 const buv = uv;

    float3 col = float3( 0.0 );
    for( float i = 1.0; i <= 26.0; i += 1.0 )
    {
        float2 rn = field_( uv );
        uv -= p * ( i - 25.0 ) * 0.2;
        rn.x = pulse_( 0.35, 0.02, rn.x + rn.y * 0.15 );
        col += rn.x * float3( sin( rn.y * 10.0 ), cos( rn.y ) * 0.2, sin( rn.y ) * 0.5 );
    }

    //	mat2 GLSL row-major -> MSL float2x2 column-major (rotation flip)
    float2 const bvr = float2x2( float2( 0.707, 0.707 ), float2( -0.707, 0.707 ) ) * buv;
    float2 const sin_bvr_10_1 = sin( bvr * 10.0 + 1.0 );
    float2 const sin_bvr_10_4 = sin( bvr * 10.0 + 4.0 );

    float  const rz2 = 0.4 * ( sin_bvr_10_1.x * 40.0 - 39.5 ) * ( sin( uv.x * 10.0 ) * 0.5 + 0.5 );
    float3 const col2 = float3( 0.2, 0.4, 2.0 ) * rz2 * ( sin( 2.0 + time * 2.1 + ( uv.y * 2.0 + uv.x * 10.0 ) ) * 0.5 + 0.5 );

    float  const rz3 = 0.3 * ( sin_bvr_10_4.y * 40.0 - 39.5 ) * ( sin( uv.x * 10.0 ) * 0.5 + 0.5 );
    float3 const col3 = float3( 1.9, 0.4, 2.0 ) * rz3 * ( sin( time * 4.0 - ( uv.y * 10.0 + uv.x * 2.0 ) ) * 0.5 + 0.5 );

    col = max( max( col, col2 ), col3 );
    return float4( col, 1.0 );
}
