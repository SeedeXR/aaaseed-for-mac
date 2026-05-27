// ps_matrix42_block.metal -- Path A port from ps_matrix42_block.frag (c84).
// Z-depth-driven layered fill : background tex if far ; mid-band stipple discard ;
// front-facing tex sample ; back-facing material-diffuse stipple lattice.
// Patches :
//   - `aaa_tex2d[0]`        -> `_aaa_tex_0`
//   - `aaa_fu_float[0..3]`  -> `_aaa_floats.values[0..3]`
//   - `gl_TexCoord[0].st`   -> `in.uv.xy`
//   - `gl_FragCoord`        -> `in.position` (P2 ; xy in pixel coords)
//   - `gl_Color`            -> opaque-white stub (P3)
//   - `gl_FrontMaterial.diffuse` -> opaque-white stub (legacy fixed pipeline ;
//     no MSL/core-GL equivalent ; documented for runtime side)
//   - `gl_FrontFacing`      -> true on Mac (front-face culling handled by pipeline state)
//   - `mod( x, K )` -> MSL `fmod( x, K )` (positive operands)
//   - `discard`             -> MSL `discard_fragment()`
//   - `blend( tex )`        -> identity pass-through (c72)

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
    bool const front_facing = true;   // Mac : pipeline cull state handles backface

    float const frag_z = in.position.z;
    float4 tex;

    if( frag_z > 0.999 )
    {
        tex = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
        tex.xyz *= ( 1.0 - frag_z ) * 1000.0;
        return tex;   // blend pass-through
    }

    float z = _aaa_floats.values[ 0 ] + _aaa_floats.values[ 1 ] * frag_z;
    if( front_facing )
    {
        if( z < _aaa_floats.values[ 2 ] )
            discard_fragment();
        if( z < _aaa_floats.values[ 3 ] )
        {
            float zz = ( z - _aaa_floats.values[ 2 ] ) / ( _aaa_floats.values[ 3 ] - _aaa_floats.values[ 2 ] );
            zz = ( 1.0 - zz ) * 32.0;
            if( fmod( in.position.y, 32.0 ) < zz )
                discard_fragment();
            if( fmod( in.position.x, 32.0 ) < zz )
                discard_fragment();
        }
        tex = _aaa_tex_0.sample( _aaa_samp, in.uv.xy );
        return tex;
    }
    else
    {
        if( fmod( in.uv.y * 200.0 + 4.0, 16.0 ) > 1.0
         && fmod( in.uv.x * 200.0 + 4.0, 16.0 ) > 1.0 )
            discard_fragment();

        //	gl_FrontMaterial.diffuse stub (legacy fixed pipeline ; no Mac equiv)
        return float4( 1.0, 1.0, 1.0, 1.0 );
    }
}
