// Spiral.metal -- Path A port from WebGL/Filters/Spiral.frag (c88).
// Polar log-spiral remap of UV with per-mode debug visualisations.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[0..4]`  -> `_aaa_floats.values[0..4]`
//   - `aaa_fu_int[0..2]`    -> `_aaa_ints.values[0..2]`
//   - `aaa_fu_vec4[0..1]`   -> `_aaa_vec4s.values[0..1]`
//   - `aaa_fu_src` / `aaa_fu_out` -> `_aaa_floats.values[8..9]`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `atan(u, v)` -> MSL `atan2(u, v)` (note GLSL signature is (y, x) ; engine
//     source writes `atan(u, v)` = `atan2(u, v)` -- preserved literal)
//   - `mod(x, K)` (positive) -> `fmod(x, K)`
//   - default-init of gl_FragColor when no branch taken : Mac initialises to opaque-black
//   - `gl_FragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

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
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float const aaa_fu_src = _aaa_floats.values[ 8 ];
    float const aaa_fu_out = _aaa_floats.values[ 9 ];

    float2 uv  = in.uv.xy;
    float4 src = _aaa_tex_0.sample( _aaa_samp, uv );

    float const u = uv.x;
    float const v = uv.y;
    float const angle = atan2( u, v ) / ( 3.14159 * 2.0 ) + 0.5;
    float const a = ( angle + _aaa_floats.values[ 1 ] ) * _aaa_floats.values[ 0 ];
    float const dist = sqrt( u * u + v * v ) * 2.0;
    float       d = dist * _aaa_floats.values[ 2 ];
    float const d_off = _aaa_floats.values[ 3 ] * _aaa_floats.values[ 2 ];

    int const mode = _aaa_ints.values[ 0 ];
    if( mode >= 0 )
    {
        d = log( d ) * pow( _aaa_floats.values[ 4 ], 2.0 ) * 2.0 - d_off + angle * float( _aaa_ints.values[ 1 ] );
        if( _aaa_ints.values[ 2 ] != 0 )
            uv = float2( d, a );
        else
            uv = float2( a, d );
        float4 const fx = _aaa_tex_0.sample( _aaa_samp, uv );
        return aaa_fu_src * src * _aaa_vec4s.values[ 0 ] + aaa_fu_out * fx * _aaa_vec4s.values[ 1 ];
    }
    if( mode == -6 ) return float4( 0.0, fmod( angle * 16.0, 1.0 ), 0.0, 1.0 );
    if( mode == -5 ) return float4( fmod( ( u + 0.5 ) * 16.0, 1.0 ), fmod( ( v + 0.5 ) * 16.0, 1.0 ), 0.0, 1.0 );
    if( mode == -4 ) return float4( u + 0.5, v + 0.5, 0.0, 1.0 );
    if( mode == -3 ) return _aaa_tex_0.sample( _aaa_samp, uv );
    if( mode == -2 ) return float4( d + d_off, a, 0.0, 1.0 ) * _aaa_vec4s.values[ 1 ];
    if( mode == -1 )
    {
        uv.x = d + d_off;
        uv.y = a;
        return _aaa_tex_0.sample( _aaa_samp, uv );
    }
    return float4( 0.0, 0.0, 0.0, 1.0 );   // engine source leaves gl_FragColor uninit if no branch ; deterministic fallback
}
