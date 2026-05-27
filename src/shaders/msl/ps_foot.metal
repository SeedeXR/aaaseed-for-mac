// ps_foot.metal -- Path A port from ps_foot.frag (c81).
// Green->blue swap above thresholds.
// Patches :
//   - `tex0` (non-aaa_ singleton) -> `_aaa_tex_0`
//   - `fu_float[0..1]` (non-aaa_ array) -> `_aaa_floats.values[0..1]`
//   - file-scope assignments moved inside fs_main (P5)
//   - `gl_TexCoord[0].st` -> `in.uv.xy`
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
    float const low_green = _aaa_floats.values[ 0 ];
    float const high_rb   = _aaa_floats.values[ 1 ];

    float4 color = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
    if( color.g > low_green )
        if( ( color.r < high_rb ) && ( color.b < high_rb ) )
        {
            float const temp = color.g;
            color.g = color.b;
            color.b = temp;
        }
    return color;
}
