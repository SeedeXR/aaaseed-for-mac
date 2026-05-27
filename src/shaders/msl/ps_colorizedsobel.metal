// ps_colorizedsobel.metal -- Path A port from ps_colorizedsobel.frag (c84).
// Colorized bi-directional Sobel edge detection.
// Patches :
//   - `tex0` -> `_aaa_tex_0`
//   - `fu_float[0]`  (non-aaa_ array) -> `_aaa_floats.values[0]` (NPixels)
//   - `winsize` / `texsize` (individual vec2 uniforms) -> AaaFuFloats[8..11] :
//       [8]  = winsize.x ; [9] = winsize.y
//       [10] = texsize.x ; [11] = texsize.y
//     (engine convention -- individual uniforms past the standard array range)
//   - `gl_TexCoord[0].xy` -> `in.uv.xy`
//   - Source computes `tex_attribs.x = 1 / texsize.x` then **never uses** it -- dead code preserved.
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

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const NPixels = _aaa_floats.values[ 0 ];
    float const delta_x = NPixels;
    float const delta_y = NPixels;

    float2 const uv = in.uv.xy;
    float4 const s1 = _aaa_tex_0.sample( _aaa_samp, uv + float2(  0.0,      delta_y ) );
    float4 const s2 = _aaa_tex_0.sample( _aaa_samp, uv + float2( -delta_x,  delta_y ) );
    float4 const s3 = _aaa_tex_0.sample( _aaa_samp, uv + float2( -delta_x,  0.0 ) );
    float4 const s4 = _aaa_tex_0.sample( _aaa_samp, uv + float2( -delta_x, -delta_y ) );
    float4 const s5 = _aaa_tex_0.sample( _aaa_samp, uv + float2(  0.0,     -delta_y ) );
    float4 const s6 = _aaa_tex_0.sample( _aaa_samp, uv + float2(  delta_x, -delta_y ) );
    float4 const s7 = _aaa_tex_0.sample( _aaa_samp, uv + float2(  delta_x,  0.0 ) );
    float4 const s8 = _aaa_tex_0.sample( _aaa_samp, uv );

    float4 accum  = s8 - s4;
    float4 accum2 = max( accum - s2 + s3 * -2.0, -2.0 ) + s6 + s7 * 2.0;
    accum         = min( accum + s2 + s1 *  2.0,  2.0 ) + s5 * -2.0 - s6;
    (void) accum;

    return float4( accum2.rgb, 0.0 );   // engine source returns accum2 ; preserved
}
