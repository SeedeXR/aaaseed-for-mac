// fish_tank_base.metal -- Path A port from Aqua/fish_tank_base.frag (c94, c84 defer).
// Aqua-scene fragment : diffuse + AO + mask blend + projection + caustic accent + fog.
// Patches :
//   - `in VS_out { tex_impli, tex_refle, tex_objec, color, fall_off, z_for_fog,
//     normal, fall_top } fs_in;` -> stage_in stubs :
//        fs_in.tex_impli.st -> in.uv.xy
//        fs_in.tex_objec.st -> in.uv.xy   (single uv set on Mac)
//        fs_in.color        -> float4(1)
//        fs_in.fall_top     -> 1.0
//        fs_in.z_for_fog    -> 1.0
//   - `aaa_tex2d[16]` -- only units 0, 1, 2, 3, 7, 8 are read in live path ;
//     map to `_aaa_tex_0..3` + `_aaa_tex_7..8` (6 actual bindings) -- documented.
//   - `aaa_tex_dim[unit]` -> stubbed to 2 (always-2D, c93)
//   - `compute_fog4( tex, z )` -> identity passthrough (engine helper stub)
//   - `gl_FragCoord.st` -> `in.position.xy`
//   - `aaa_fu_float[0..7]` -> `_aaa_floats.values[0..7]`
//   - `aaa_fu_int[0..3]` -> `_aaa_ints.values[0..3]`
//   - `out vec4 fragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

constant float FISH_ECRAN_MIX_ = 0.35;

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
    texture2d< float >    _aaa_tex_2    [[texture(2)]],
    texture2d< float >    _aaa_tex_3    [[texture(3)]],
    texture2d< float >    _aaa_tex_7    [[texture(7)]],
    texture2d< float >    _aaa_tex_8    [[texture(8)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    //	VS_out stubs
    float2 const tex_impli = in.uv.xy;
    float2 const tex_objec = in.uv.xy;
    float4 const fs_color  = float4( 1.0 );
    float  const fall_top  = 1.0;

    float2 const uv = tex_impli;

    float4 const diffu     = _aaa_tex_0.sample( _aaa_samp, uv );
    float4 const ao        = _aaa_tex_1.sample( _aaa_samp, uv );
    float4 const mask      = _aaa_tex_2.sample( _aaa_samp, uv );
    float4 const diffu_bis = _aaa_tex_3.sample( _aaa_samp, uv );

    float2 proj_uv = in.position.xy * float2( 1.0 / 4096.0, 1.0 / 1630.0 )
                   - float2( 0.10 + ( _aaa_floats.values[ 7 ] + 1.0 ) * 0.237, 0.24 );
    proj_uv *= float2( 3.1, 1.9 );
    float4 projection;
    if( all( max( min( proj_uv, float2( 1.0 ) ), float2( 0.0 ) ) == proj_uv ) )
        projection = _aaa_tex_7.sample( _aaa_samp, proj_uv );
    else
        projection = diffu;

    if( _aaa_ints.values[ 3 ] > 0 )
    {
        if( _aaa_ints.values[ 3 ] == 1 )
            return fs_color;
        //	Engine `aaa_fu_int[3] - 2` selects which of aaa_tex2d[0..3] to sample.
        //	Mac dispatches via if-chain since dynamic-index sampler arrays are
        //	awkward in MSL.
        int const sel = _aaa_ints.values[ 3 ] - 2;
        if(      sel == 0 ) return _aaa_tex_0.sample( _aaa_samp, uv );
        else if( sel == 1 ) return _aaa_tex_1.sample( _aaa_samp, uv );
        else if( sel == 2 ) return _aaa_tex_2.sample( _aaa_samp, uv );
        else                return _aaa_tex_3.sample( _aaa_samp, uv );
    }

    float4 dif = mix( diffu, diffu_bis, _aaa_floats.values[ 4 ] );
    dif        = mix( dif,   dif * ao,  _aaa_floats.values[ 1 ] );
    dif.rgb    = mix( dif.rgb,
                      ( float3( 1.0 - FISH_ECRAN_MIX_ ) + dif.rgb * FISH_ECRAN_MIX_ ) * projection.rgb,
                      _aaa_floats.values[ 5 ] * projection.a );
    float4 tex = mix( fs_color * dif, dif, _aaa_floats.values[ 0 ] );
    tex.rgb *= _aaa_floats.values[ 2 ];

    float fn = fall_top;
    if( fn > 0.0 )
    {
        float const caustic = _aaa_tex_8.sample( _aaa_samp, tex_objec ).r;
        fn = clamp( fn, 0.0, 1.0 );
        float const r = caustic * fn;
        tex.rgb += _aaa_floats.values[ 3 ] * float3( r, r, r * 0.2 ) * mask.r;
    }

    //	compute_fog4(tex, z) engine helper stubbed to identity passthrough
    return tex;
}
