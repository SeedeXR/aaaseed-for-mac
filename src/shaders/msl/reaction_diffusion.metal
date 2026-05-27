// reaction_diffusion.metal -- Path A port from Jean/reaction_diffusion.frag (c91, c86 carry).
// Gray-Scott reaction-diffusion update step ; 9-point Laplacian convolution.
// Patches :
//   - `in VS_out { tex_coor[TEX_UNIT_NB] } vs_out;` -> stage_in stub :
//        vs_out.tex_coor[0].st -> in.uv.xy
//        vs_out.tex_coor[1].st -> in.uv.xy (single uv set on Mac ; same as [0])
//   - `aaa_tex2d[0..1]` -> `_aaa_tex_0..1`
//   - `aaa_fu_float[0]` -> `_aaa_floats.values[0]` (time)
//   - `textureSize(tex, 0)` -> `int2( tex.get_width(), tex.get_height() )`
//   - `laplacian1` / `laplacian2` / `laplacian3` -- only `laplacian_convolution`
//     is called in live path ; others kept as comments. Mac drops unused helpers.
//   - file-scope `float time = aaa_fu_float[0];` (UBO macro idiom) -> P5
//   - hard-coded `time < 1730` gate in source preserved.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant float2 Diffusion_ = float2( 0.08, 0.03 );
constant float  dt_        = 2.0;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float2 laplacian_convolution_(
    float2 uv,
    texture2d< float > tex, sampler samp )
{
    float2 const texel = 1.0 / float2( tex.get_width(), tex.get_height() );
    float4 const dxy   = float4( texel, -texel.y, 0.0 );

    float2 ret = float2( 0.0 );
    ret += tex.sample( samp, uv                  ).xy * 4.0;
    ret += tex.sample( samp, uv - dxy.xw         ).xy * 2.0;
    ret += tex.sample( samp, uv + dxy.xw         ).xy * 2.0;
    ret += tex.sample( samp, uv + dxy.wz         ).xy * 2.0;
    ret += tex.sample( samp, uv + dxy.wy         ).xy * 2.0;
    ret += tex.sample( samp, uv - dxy.xy         ).xy;
    ret += tex.sample( samp, uv + dxy.xz         ).xy;
    ret += tex.sample( samp, uv + dxy.xy         ).xy;
    ret += tex.sample( samp, uv - dxy.xz         ).xy;
    return ret / 16.0;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const time = _aaa_floats.values[ 0 ];
    float2 const uv = in.uv.xy;   // VS_out tex_coor[0] stub

    if( time < 1730.0 )
    {
        //	Initial-state seed : sample tex_coor[1] (== uv on Mac) from aaa_tex2d[1].
        return _aaa_tex_1.sample( _aaa_samp, uv );
    }

    float const F = uv.y * 0.0042 + 0.014;
    float const k = length( uv - 0.5 ) * 0.021 + 0.082;
    float4 const data = _aaa_tex_0.sample( _aaa_samp, uv );
    float const u = data.x;
    float const v = data.y;

    float2 const Duv = laplacian_convolution_( uv, _aaa_tex_0, _aaa_samp ) * Diffusion_;
    float  const du = Duv.x - u * v * v + F * ( 1.0 - u );
    float  const dv = Duv.y + u * v * v - ( F + k ) * v;

    float4 out_color = float4( 0.0 );
    out_color.xy = clamp( float2( u + du * dt_, v + dv * dt_ ), 0.0, 1.0 );
    return out_color;
}
