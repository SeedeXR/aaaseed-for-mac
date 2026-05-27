// ps_halftone_dot.metal -- Path A port from ps_halftone_dot.frag (c90).
// Halftone-dot rasterisation : per-tile circular dot whose radius = floor(lum*4).
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[0..2]`  -> `_aaa_floats.values[0..2]`
//   - `aaa_fu_vec4[0]`      -> `_aaa_vec4s.values[0]` (luminance weights)
//   - file-scope assignments (NOISE_PATCHES, DOTS_PER_BIT, lumconst,
//     use_color, black_transparent) -> P5 (moved into fs_main)
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gl_Color`            -> opaque-white stub (P3)
//   - `mod( x, K )` (positive) -> `fmod( x, K )`
//   - `blend( x )`          -> identity pass-through (c72)
//   - `vec2 = { x, y }`     brace-init -> `float2()` ctor
//   - `vec3 = { aaa_fu_vec4[0].rgb }` -- engine-permissive 1-element brace
//     init from a vec3 ; MSL needs explicit `.rgb` assignment

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

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float  const NOISE_PATCHES     = 4.0;
    float2 const DOTS_PER_BIT      = float2( _aaa_floats.values[ 0 ], _aaa_floats.values[ 0 ] );
    float3 const lumconst          = _aaa_vec4s.values[ 0 ].rgb;
    float  const use_color         = _aaa_floats.values[ 1 ];
    float  const black_transparent = _aaa_floats.values[ 2 ];
    (void) NOISE_PATCHES;

    float4 const gl_Color = float4( 1.0 );

    float4 const scnC = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    float  const lum  = dot( lumconst, scnC.xyz );
    float2 const lx0  = DOTS_PER_BIT * in.uv.xy;
    float3 const lx   = float3( lx0.x, lx0.y, lum );

    float3 pos_mod;
    pos_mod.x = fmod( lx.x, 1.0 );
    pos_mod.y = fmod( lx.y, 1.0 );
    pos_mod.z = fmod( lx.z, 4.0 );
    float2 const delta = pos_mod.xy - float2( 0.5 );
    float  const d = dot( delta, delta );
    float  const rSquared = ( pos_mod.z * pos_mod.z ) / 2.0;

    float const alpha = ( black_transparent >= 1.0 ) ? 0.0 : 1.0;

    if( d < rSquared )
    {
        if( use_color >= 1.0 )
            return float4( gl_Color.xyz, scnC.a );
        return float4( scnC.xyz, scnC.a );
    }
    return float4( 0.0, 0.0, 0.0, alpha );
}
