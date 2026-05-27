// ps_Eau.metal -- Path A port from OtAix/ps_Eau.frag (c91, c84 carry recovered).
// Water-surface displacement : sample aaa_tex2d[1] (water normal) to perturb
// aaa_tex2d[0] (image) sample coords ; multi-mode debug paths preserved.
// Patches :
//   - `in VS_out { tex_impli, color, normal } fs_in;` -> stage_in stub :
//        fs_in.tex_impli.st -> in.uv.xy   (only member read in live path)
//   - `aaa_tex2d[0..1]`   -> `_aaa_tex_0..1`
//   - `aaa_fu_float[0..1]` -> `_aaa_floats.values[0..1]`
//   - `aaa_fu_int[0]`     -> `_aaa_ints.values[0]`
//   - `out vec4 fragColor` -> return value
//   - Commented-out alternative implementation preserved as source archival.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

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
    //	VS_out stub : only tex_impli is read in live path
    float2 const coi_init = in.uv.xy;
    float2 const co_init  = float2( 1.0, 1.0 ) - coi_init;

    int   const mode = _aaa_ints.values[ 0 ];
    float4 tex = float4( 0.0, 0.0, 0.0, 1.0 );

    if( mode == -4 )
    {
        tex = _aaa_tex_1.sample( _aaa_samp, co_init );
        tex.b = 0.5;
        tex.a = 1.0;
    }
    else if( mode == -3 )
    {
        tex = _aaa_tex_1.sample( _aaa_samp, co_init );
        tex.r = tex.b;
        tex.g = tex.b;
        tex.a = 1.0;
    }
    else if( mode == -2 )
    {
        tex = _aaa_tex_1.sample( _aaa_samp, co_init );
        tex.a = 1.0;
    }
    else if( mode == -1 )
    {
        tex = _aaa_tex_0.sample( _aaa_samp, coi_init );
        tex.a = 1.0;
    }
    else
    {
        float2 coi = coi_init;
        float2 co  = co_init;

        float4 eau = _aaa_tex_1.sample( _aaa_samp, co );
        float4 img = _aaa_tex_0.sample( _aaa_samp, coi );

        float inf = img.a;
        eau.r -= 0.50196078;
        eau.g -= 0.50196078;
        inf *= ( ( eau.b - 0.50196078 ) < 0.0 )
             ? ( 0.50196078 - eau.b )
             : ( eau.b - 0.50196078 );
        coi.x += eau.r * _aaa_floats.values[ 0 ] * inf;
        coi.y += eau.g * _aaa_floats.values[ 0 ] * inf;
        img = _aaa_tex_0.sample( _aaa_samp, coi );

        float const f = dot( float2( 0.0, -1.0 ), eau.rg );
        img.rgb *= 1.0 + f * _aaa_floats.values[ 1 ] * inf;
        img.a = 1.0;
        tex = img;
        tex.a = 1.0;
    }

    return tex;
}
