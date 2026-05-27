// ps_Maa_fbm.metal -- Path A port from ps_Maa_fbm.frag (c80).
// Procedural FBM (fractal Brownian motion) value noise + 4-iteration
// folding + optional secondary-texture mix. FULL_PROCEDURAL branch only ;
// LUT-based fallback (with negative-LOD bias) is preserved as comment.
// Patches :
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `aaa_tex2d[1]`        -> `_aaa_tex_1`
//   - `aaa_fu_float[N]`     -> `_aaa_floats.values[N]`
//   - `aaa_fu_vec4[N]`      -> `_aaa_vec4s.values[N]`
//   - `gl_FragColor`        -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float hash_( float n ) { return fract( sin( n ) * 43758.5453123 ); }

static inline float noise_( float3 x )
{
    float3 p = floor( x );
    float3 f = fract( x );
    f = f * f * ( 3.0 - 2.0 * f );

    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return mix(
        mix(
            mix( hash_( n + 0.0   ), hash_( n + 1.0   ), f.x ),
            mix( hash_( n + 157.0 ), hash_( n + 158.0 ), f.x ),
            f.y ),
        mix(
            mix( hash_( n + 113.0 ), hash_( n + 114.0 ), f.x ),
            mix( hash_( n + 270.0 ), hash_( n + 271.0 ), f.x ),
            f.y ),
        f.z );
}

static inline float maa_( float3 v, float exponent )
{
    float g = noise_( v );
    return pow( g, exponent );
}

static inline float map_( float3 p, constant AaaFuFloats& f )
{
    float r;
    r  = 0.5000 * maa_( p, f.values[ 2 ] ); p *= 2.1;
    r += 0.2500 * maa_( p, f.values[ 2 ] ); p *= 2.2;
    r += 0.1250 * maa_( p, f.values[ 2 ] ); p *= 2.3;
    r += 0.0625 * maa_( p, f.values[ 2 ] );
    return f.values[ 0 ] + r * f.values[ 1 ];
}

static inline float3 noise33_( float3 v, constant AaaFuFloats& f, constant AaaFuVec4s& v4 )
{
    float3 q = v - v4.values[ 0 ].xyz * f.values[ 3 ];
    return float3( map_( q, f ), map_( q.yzx, f ), map_( q.zxy, f ) );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float3 p;
    p.xy = in.uv.xy - 0.5;
    p.z  = 0.0;
    p   += _aaa_vec4s.values[ 1 ].xyz;
    p   *= _aaa_vec4s.values[ 2 ].xyz;

    float3 col = noise33_( p, _aaa_floats, _aaa_vec4s );
    col += p;
    col  = noise33_( col, _aaa_floats, _aaa_vec4s );
    col += p;
    col  = noise33_( col, _aaa_floats, _aaa_vec4s );
    col += p;
    col  = noise33_( col, _aaa_floats, _aaa_vec4s );

    col = _aaa_floats.values[ 5 ] * col
        + _aaa_floats.values[ 6 ] * _aaa_tex_1.sample( _aaa_samp,
            in.uv.xy + ( col.xy - 0.5 ) * _aaa_floats.values[ 7 ] ).xyz;

    return float4( col, 1.0 );
}
