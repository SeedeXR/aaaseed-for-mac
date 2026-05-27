// draw_sprite.metal -- Path A port from flex/draw_sprite.frag (c96).
// Particle sprite renderer : circular/box mask + texture branch + normal MRT.
// Patches :
//   - 2-target MRT (out_result, out_normal) -> `FragmentOut [[color(0..1)]]` (c94 pattern)
//   - `st_part_custom` engine struct + per-vertex inputs (in_uv_dens / in_color /
//     in_normal / in_custom) -> stage_in stubs : in_uv_dens.xy -> in.uv.xy ;
//     in_color -> float4(1) ; in_normal -> float3(0, 0, 1) ; in_custom dropped.
//   - `aaa_samp0` -> `_aaa_tex_0`
//   - `aaa_fu_float[0..1]` -> AaaFuFloats[0..1] (hardness, roundness)
//   - `aaa_fu_int[0]` -> AaaFuInts[0] (s_test)
//   - `discard` -> `discard_fragment()`
//   - `flat` qualifier on uint/int -> dropped (Mac side has no per-vertex flat plumbing yet)

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

struct FragmentOut
{
    float4 out_result [[color(0)]];
    float4 out_normal [[color(1)]];
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

fragment FragmentOut fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    //	Per-vertex stubs
    float2 const in_uv_dens = in.uv.xy;
    float4 const in_color   = float4( 1.0 );
    float3 const in_normal  = float3( 0.0, 0.0, 1.0 );

    float const hardness  = _aaa_floats.values[ 0 ];
    float const roundness = _aaa_floats.values[ 1 ];
    int   const s_test    = _aaa_ints.values[ 0 ];

    FragmentOut out;

    if( s_test == 0 )
    {
        float2 uv = in_uv_dens * 2.0 - 1.0;
        float const d_in = mix( max( fabs( uv.x ), fabs( uv.y ) ), dot( uv, uv ), roundness );
        if( d_in > 1.0 )
            discard_fragment();
        float const d = 1.0 - pow( d_in, hardness );
        out.out_result = float4( 1.0, 1.0, 1.0, d ) * in_color;
    }
    else
    {
        out.out_result = _aaa_tex_0.sample( _aaa_samp, in_uv_dens ) * in_color;
        if( out.out_result.a < 0.1 )
            discard_fragment();
    }

    out.out_normal = float4( in_normal, 1.0 );
    return out;
}
