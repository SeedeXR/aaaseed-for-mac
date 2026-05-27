// sobel.metal -- Path A port from shader/sobel.frag (c85).
// Horizontal Sobel edge kernel : [-1 -2 -1 ; 0 0 0 ; +1 +2 +1].
// Patches :
//   - `texUnit` -> `_aaa_tex_0`
//   - `gl_TexCoord[0].xy` -> `in.uv.xy`
//   - Weighted luminance (4/16/1)/21 reduction preserved
//   - `gl_FragColor` -> return value

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

fragment float4 fs_main(
    VertexOut          in          [[stage_in]],
    texture2d< float > _aaa_tex_0  [[texture(0)]],
    sampler            _aaa_samp   [[sampler(0)]] )
{
    float  const i = 1.0 / 1024.0;
    float2 const tc = in.uv.xy;
    float4 c1 = float4( 0.0 );

    //	Top row (negative)
    c1 -= 1.0 * _aaa_tex_0.sample( _aaa_samp, tc + float2( -i,  i ) );
    c1 -= 2.0 * _aaa_tex_0.sample( _aaa_samp, tc + float2( 0.0, i ) );
    c1 -= 1.0 * _aaa_tex_0.sample( _aaa_samp, tc + float2(  i,  i ) );

    //	Bottom row (positive)
    c1 += 1.0 * _aaa_tex_0.sample( _aaa_samp, tc + float2( -i, -i ) );
    c1 += 2.0 * _aaa_tex_0.sample( _aaa_samp, tc + float2( 0.0, -i ) );
    c1 += 1.0 * _aaa_tex_0.sample( _aaa_samp, tc + float2(  i, -i ) );

    float const b = ( c1.x * 4.0 / 21.0 + c1.y * 16.0 / 21.0 + c1.z * 1.0 / 21.0 );
    float4 result = float4( b );
    result.a = 1.0;
    return result;
}
