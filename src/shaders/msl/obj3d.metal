// obj3d.metal -- Path A port from obj3d.frag (c93).
// Generic obj-renderer : per-type mode switch (Toon / BW / 3-color) + optional
// secondary-texture overlay sampled from BV.pos_world.
// Patches :
//   - `in ST_AAA_BV BV;` -> stage_in stub :
//        BV.tex_coor[0].st -> in.uv.xy
//        BV.color          -> float4(1)
//        BV.pos_world.zy   -> float2(0)  (no per-vertex world-pos plumbing ; opt-2 branch returns identity)
//   - `layout(location = 0) out vec4 color_out_0` -- single MRT location ;
//     equivalent to standard `[[color(0)]]` (default). Returned via fs_main.
//     (`color_out_1` is commented out in source ; not a real MRT.)
//   - `aaa_tex2d[0..1]`  -> `_aaa_tex_0..1`
//   - `aaa_tex_dim[unit]` engine helper -> stub to 2 (always-2D) ; runtime side may
//     populate a real aaa_tex_dim buffer once tex-dim plumbing lands.
//   - `aaa_fu_int[0..2]` -> `_aaa_ints.values[0..2]`
//   - `aaa_fu_float[0..1]` -> `_aaa_floats.values[0..1]`
//   - `sampler1D TEX_1D`  declared but unused in live path -> dropped
//   - `gray( c.rgb )`     -> prelude helper
//   - `textureSize(tex, 0)` -> `int2( tex.get_width(), tex.get_height() )`

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

#ifndef AAA_MSL_PRELUDE_DEFINED
#define AAA_MSL_PRELUDE_DEFINED
constexpr constant float3 aaa_gray_conv = float3( 0.2989, 0.5870, 0.1140 );
static inline float gray( float3 a ) { return dot( a.xyz, aaa_gray_conv ); }
static inline float gray( float4 a ) { return dot( a.xyz, aaa_gray_conv ); }
#endif

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
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int   const type = _aaa_ints.values[ 0 ];

    //	BV stubs
    float2 const BV_tex_coor_0 = in.uv.xy;
    float4 const BV_color      = float4( 1.0 );
    float2 const BV_pos_world_zy = float2( 0.0 );

    //	aaa_tex_dim[0] stubbed to 2 (always-2D on Mac until tex-dim plumbing lands)
    float4 src = _aaa_tex_0.sample( _aaa_samp, BV_tex_coor_0 );

    float v = clamp( gray( src.rgb * BV_color.rgb ), 0.0, 1.0 );
    if( type == 1 )
    {
        v = fract( v * ( float( _aaa_ints.values[ 1 ] ) + 0.001 ) );
        v = clamp( src.r - v / float( _aaa_ints.values[ 1 ] ), 0.0, 1.0 );
        src.rgb = float3( v );
    }
    else if( type == 2 )
    {
        v = smoothstep( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ], v );
        src.rgb *= float3( v );
    }
    else if( type == 3 )
    {
        if( v < _aaa_floats.values[ 0 ] )
            src.rgb *= float3( 0.0, 0.0, 0.7 );
        else if( v < _aaa_floats.values[ 1 ] )
            src.rgb *= float3( 1.0, 0.0, 0.0 );
        else
            src.rgb *= float3( 1.0 );
    }

    src *= BV_color;

    if( _aaa_ints.values[ 2 ] > 0 )
    {
        float2 const s = float2( _aaa_tex_1.get_width(), _aaa_tex_1.get_height() );
        float2       uv = BV_pos_world_zy * 0.28 * float2( -1.0, 1.0 );
        uv = ( uv + float2( 0.5, 0.0 ) ) * float2( s.y / s.x, 1.0 ) * 4.0;
        float3 const t1 = _aaa_tex_1.sample( _aaa_samp, uv ).rgb;
        if( _aaa_ints.values[ 2 ] == 1 )
            src.rgb += t1;
        else
            src.rgb *= t1;
    }

    return src;
}
