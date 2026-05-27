// drip.metal -- Path A port from ShaderToy/drip.frag (c85).
// Procedural drip simulation with SDF + bounce animation.
// Patches :
//   - `iTime`           -> `_aaa_floats.values[0]`
//   - `iResolution.x/y` -> `_aaa_floats.values[1..2]`
//   - `iMouse.x/y/z`    -> `_aaa_floats.values[3..5]` (z = button mask)
//   - file-scope `float seed = 0.25;` etc. (P5 file-scope) moved into fs_main
//   - ShaderToy `mainImage` wrapper -> fs_main with synthesized fragCoord
//   - `mod( x, K )` -> `fmod( x, K )` (positive operands)
//   - `round(x)` -> MSL has `round` ; preserved
//   - Loop bound 1000 capped by `dripDistance` step + early break ; loop in main thread.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant float PI_ = 3.14159265359;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float rand_( float x, float y )
{
    return sin( x + sin( x * 10.0 ) * 0.5 ) - y * 0.2;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const iTime         = _aaa_floats.values[ 0 ];
    float const iResolution_x = _aaa_floats.values[ 1 ];
    float const iResolution_y = _aaa_floats.values[ 2 ];
    float const iMouse_x      = _aaa_floats.values[ 3 ];
    float const iMouse_y      = _aaa_floats.values[ 4 ];
    float const iMouse_z      = _aaa_floats.values[ 5 ];

    float const seed         = 0.25;
    float const dripDistance = 0.1;
    float       density      = 0.75;
    float const bCurve       = 1.5;
    float const bFreq        = 3.5;
    float const bRange       = 0.35;
    float const fallSpeed    = 6.0;
    float const sdfWidth     = 0.18;

    float2 fragCoord = in.uv.xy * float2( iResolution_x, iResolution_y );
    float2 uv = fragCoord / float2( iResolution_x, iResolution_y );
    uv.x *= iResolution_x / iResolution_y;
    uv.y  = 1.0 - uv.y;

    float c = 1.0 / sdfWidth * 0.025;
    float const w = 0.03;

    if( iMouse_z > 0.0 )
    {
        density = iMouse_y / iResolution_y;
        c       = iMouse_x / iResolution_x;
    }

    //	SDF compute :
    float s    = sdfWidth * fabs( ( 1.0 - uv.y ) - 0.75 ) + 0.05;
    float o    = 1.0;
    float drip = 999.0;

    float x = uv.x - sdfWidth;
    x += dripDistance - fmod( x, dripDistance );
    x -= dripDistance;

    for( int i = 0; i < 1000; i++ )
    {
        if( x > uv.x + sdfWidth ) break;
        x += dripDistance;
        float const isLine = round( rand_( x, seed ) + density - 0.5 );
        if( isLine == 0.0 ) continue;

        float       y = rand_( seed, x ) * 0.8 + 0.1;
        float const animTime = iTime + ( y * 10.0 );
        float const bounce = 0.0 - ( bCurve * fmod( animTime, bFreq ) ) * exp( 1.0 - bCurve * fmod( animTime, bFreq ) );
        y += bounce * bRange;
        y = min( y, uv.y );

        float const f = y + fmod( animTime, bFreq ) * fallSpeed * bRange;
        float const d = distance( float2( x, y ), uv );
        o    *= clamp( d / s, 0.0, 1.0 );
        drip  = min( drip, distance( float2( x, f ), uv ) );
    }

    o = min( o, clamp( drip / s, 0.0, 1.0 ) );

    s = sin( uv.x * 20.0 + iTime * 0.2 ) * 0.3 + 0.4;
    //	`distance( 0.0, uv.y )` in source : MSL `distance` is ambiguous on bare
    //	float literals ; fabs is the same thing for scalars.
    float const d_final = o * clamp( fabs( uv.y ) / s, 0.0, 1.0 );

    float const v = 1.0 - smoothstep( c - w, c + w, d_final );
    return float4( v, v - 0.2, 0.0, 1.0 );
}
