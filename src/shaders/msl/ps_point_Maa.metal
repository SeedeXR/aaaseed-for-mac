// ps_point_Maa.metal -- Path A port from ps_point_Maa.frag (c80, Maa 2016).
// Particle / point-sprite shader using a geometry-shader interface block.
// Patches :
//   - `in GS_out { vec2 uv; vec4 color; } gs_in;` (P4c interface block)
//     -> stubbed via fragment stage_in : `gs_in.uv` <- in.uv ;
//        `gs_in.color` <- opaque-white (no geometry stage on Mac yet)
//   - `aaa_fu_float[0..2]`  -> `_aaa_floats.values[0..2]`
//   - `aaa_fu_int[0]`       -> `_aaa_ints.values[0]`
//   - `discard`             -> MSL `discard_fragment()`
//   - `gl_FragColor`        -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float2 const gs_uv    = in.uv.xy;
    float4 const gs_color = float4( 1.0, 1.0, 1.0, 1.0 );

    float d;
    if( _aaa_ints.values[ 0 ] == 0 )
    {
        d = max( fabs( gs_uv.x - 0.5 ), fabs( gs_uv.y - 0.5 ) ) * 2.0;
    }
    else
    {
        float2 uv = gs_uv - 0.5;
        d = dot( uv, uv );
        if( d > 0.25 )
            discard_fragment();
        d *= 4.0;
    }
    d = smoothstep( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ], d );

    float4 src = float4( 1.0, 1.0, 1.0, 1.0 - d );
    src.rgb *= ( 1.0 - _aaa_floats.values[ 2 ] );

    return src * gs_color;
}
