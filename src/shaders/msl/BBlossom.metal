// BBlossom.metal -- Path A port from BBlossom/BBlossom.frag (c96, c86 carry recovered).
// 2-texture composite with specular accent ; single output (NOT MRT despite layout-locations on inputs).
// Patches :
//   - `in VS_out { pos_world, pos_ec, nor_ec, alpha, color, specular, tex_coor[2] } fs_in;`
//     -> stage_in stubs : tex_coor[0/1].st -> in.uv.xy ; color -> float4(1) ; specular -> float3(0)
//   - `aaa_tex2d[0..1]`  -> `_aaa_tex_0..1`
//   - `aaa_fu_vec4[0]`   -> `_aaa_vec4s.values[0]` (specular accent)
//   - `aaa_tex_dim[unit]` engine helper stubbed to 2 (always-2D)
//   - `get_color` helper extracted as static inline
//   - `gl_FragColor`     -> return value (single output)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuVec4s { float4 values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float4 get_color_( float2 uv, texture2d< float > tex, sampler samp )
{
    //	aaa_tex_dim stubbed to 2 (always 2D)
    return tex.sample( samp, uv );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    //	VS_out stubs
    float2 const tex_coor_0 = in.uv.xy;
    float2 const tex_coor_1 = in.uv.xy;
    float4 const fs_color   = float4( 1.0 );
    float3 const fs_specular = float3( 0.0 );

    float4 texture = get_color_( tex_coor_0, _aaa_tex_0, _aaa_samp ) * fs_color;
    texture.rgb += _aaa_tex_1.sample( _aaa_samp, tex_coor_1 ).rgb
                 * fs_specular
                 * _aaa_vec4s.values[ 0 ].rgb
                 * _aaa_vec4s.values[ 0 ].a;
    return texture;
}
