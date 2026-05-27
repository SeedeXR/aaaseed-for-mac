// ps_fixed_main.metal -- Path A port from ps_fixed_main.frag (c94, c77 BV-defer recovered).
// **First 4-target MRT port** (gbuffer outputs : diffuse-rough, normal-occlusion,
// specular-reflection, emissive). Establishes the 4-color-attachment pattern.
// Patches :
//   - `layout(location = N, index = 0) out vec4 out_*` -> `FragmentOut [[color(N)]]`
//   - `in ST_AAA_BV BV;` -> stage_in stub : BV.tex_coor[0].st -> in.uv.xy ;
//     BV.color -> float4(1) ; BV.nor_world.xyz -> float3(0, 0, 1)
//   - `aaa_tex2d[0]`   -> `_aaa_tex_0`
//   - `aaa_tex_dim[0]` engine helper stubbed to 2 (always-2D, c93 lesson)
//   - `get_color` helper extracted as static inline
//   - `gl_FragColor` no longer used -- replaced by FragmentOut return

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct FragmentOut
{
    float4 out_diffuse_rough        [[color(0)]];
    float4 out_normal_occlusion     [[color(1)]];
    float4 out_specular_reflection  [[color(2)]];
    float4 out_emissive             [[color(3)]];
};

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
    //	aaa_tex_dim[unit] stubbed to 2 (always-2D)
    return tex.sample( samp, uv );
}

fragment FragmentOut fs_main(
    VertexOut          in          [[stage_in]],
    texture2d< float > _aaa_tex_0  [[texture(0)]],
    sampler            _aaa_samp   [[sampler(0)]] )
{
    //	BV stubs
    float2 const BV_tex_coor_0 = in.uv.xy;
    float4 const BV_color      = float4( 1.0 );
    float3 const BV_nor_world  = float3( 0.0, 0.0, 1.0 );

    float4 texture = get_color_( BV_tex_coor_0, _aaa_tex_0, _aaa_samp );
    texture *= BV_color;

    FragmentOut out;
    out.out_diffuse_rough       = float4( texture.rgb, 1.0 );
    out.out_normal_occlusion    = float4( BV_nor_world, 0.5 );
    out.out_specular_reflection = float4( 0.0 );
    out.out_emissive            = float4( 0.0, 0.0, 0.0, 1.0 );
    return out;
}
