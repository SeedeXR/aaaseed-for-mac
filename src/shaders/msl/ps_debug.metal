// ps_debug.metal -- Path A port from ps_debug.frag (c79).
// Texture swizzle debug shader (Franz Hildgen).
// Patches :
//   - `aaa_samp1`     -> `_aaa_tex_0` (single texture binding)
//   - `aaa_fu_int[]`  -> `_aaa_ints.values[]`
//   - `gl_TexCoord[0].st` -> `in.uv.xy`
//   - `blend( col )`  -> `col` pass-through (c72)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuInts { int values[ 16 ]; };

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
    VertexOut          in          [[stage_in]],
    texture2d< float > _aaa_tex_0  [[texture(0)]],
    sampler            _aaa_samp   [[sampler(0)]],
    constant AaaFuInts& _aaa_ints  [[buffer(2)]] )
{
    float4 col = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );

    float g;
    int const c = _aaa_ints.values[ 0 ];
    if     ( c == 0 ) g = col.r;
    else if( c == 1 ) g = col.g;
    else if( c == 2 ) g = col.b;
    else if( c == 3 ) g = col.a;
    else              g = -1.0;

    if( g >= 0.0 )
        col = float4( g, g, g, 1.0 );

    //	`if( aaa_fu_int[1] == 0 ) col = blend( col );` -- blend pass-through (c72)
    return col;
}
