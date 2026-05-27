// ps_halftone.metal -- Path A port from ps_halftone.frag (c80).
// Patches :
//   - `aaa_tex2d[0]`         -> `_aaa_tex_0`
//   - `gl_TexCoord[0].st`    -> `in.uv.xy`
//   - `gl_Color`             -> opaque-white stub (P3, c70)
//   - `noise(vec3)`          engine helper -> inlined hash-noise (matches
//     the form used in ps_Maa_fbm, deterministic)
//   - `vec3 lumconst = {x,y,z};` brace-init -> `float3()` ctor
//   - `vec2 const = 8.0` (scalar-to-vec2) -> `float2()` ctor
//   - `blend( c )`           -> `c` pass-through (c72)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

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

constant float  NOISE_PATCHES = 4.0;
constant float2 DOTS_PER_BIT  = float2( 8.0 );
constant float2 IMG_DIVS      = float2( 8.0 );
constant float3 lumconst      = float3( 0.2, 0.7, 0.1 );

fragment float4 fs_main(
    VertexOut          in          [[stage_in]],
    texture2d< float > _aaa_tex_0  [[texture(0)]],
    sampler            _aaa_samp   [[sampler(0)]] )
{
    (void) DOTS_PER_BIT;
    float4 const gl_Color_stub = float4( 1.0, 1.0, 1.0, 1.0 );

    float4 const scnC = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    float  const lum  = dot( lumconst, scnC.xyz );
    float2 const lx0  = NOISE_PATCHES * IMG_DIVS * in.uv.xy;
    float3 const lx   = float3( lx0.x, lx0.y, lum );

    float n = 0.5 * ( 1.0 + noise_( lx * 20.0 ) );
    float s = lx.z;
    float4 dotC = ( n < s ) ? float4( gl_Color_stub.xyz, 1.0 )
                            : float4( 0.0, 0.0, 0.0, 1.0 );

    return float4( dotC.xyz, scnC.a );   // blend() pass-through
}
