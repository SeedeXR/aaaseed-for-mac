// vol3d_Plane.metal -- Path A port from Voxel/vol3d_Plane.frag (c100).
// **First sampler3D pair member ported.** Plane-cut through 3D volume :
// crop + palette map.
// Patches :
//   - `in VS_out { pos_world, color } fs_in;` -> stage_in stub : pos_world -> in.uv.xy
//     mapped to xyz with z = 0 ; color -> float4(1).
//   - `sampler3D aaa_samp0/aaa_samp2` -> `texture3d< float > _aaa_tex_3d_0/_aaa_tex_3d_2`
//   - `sampler2D aaa_samp1` -> `texture2d< float > _aaa_tex_1` (palette)
//   - `aaa_tex_dim[unit]` engine helper -> stub-to-2 (always 2D, c93)
//   - `aaa_fu_float[0..4]` / `aaa_fu_int[0..2]` / `aaa_fu_vec4[0..2]` standard buffers
//   - file-scope `const vec4 color_dbg[4]` -> `constexpr constant float4 color_dbg_[4]`
//   - `get_gradient_3d` / `get_normal` helpers declared but **never called in live path** ; dropped
//   - `discard` -> `discard_fragment()`
//   - `out vec4 fragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

constexpr constant float4 color_dbg_[ 4 ] = {
    float4( 1.0, 1.0, 1.0, 1.0 ),
    float4( 1.0, 0.0, 0.0, 1.0 ),
    float4( 0.0, 1.0, 0.0, 1.0 ),
    float4( 0.0, 0.0, 1.0, 1.0 )
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

static inline float4 get_color0_(
    float3 at, texture3d< float > tex3d, texture2d< float > tex_palette, sampler samp,
    constant AaaFuFloats& fl, constant AaaFuVec4s& v4, constant AaaFuInts& ii )
{
    float const g = tex3d.sample( samp, at ).r;
    if( g == clamp( g, v4.values[ 2 ].x, v4.values[ 2 ].y ) )
    {
        if( ii.values[ 2 ] == 0 )
            return float4( g );
        return tex_palette.sample( samp, float2( g, fl.values[ 3 ] ) );
    }
    return float4( 0.0 );
}

fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    texture3d< float >    _aaa_tex_3d_0   [[texture(0)]],
    texture2d< float >    _aaa_tex_1      [[texture(1)]],
    texture3d< float >    _aaa_tex_3d_2   [[texture(2)]],
    sampler               _aaa_samp       [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]] )
{
    (void) _aaa_tex_3d_2;   // declared per engine source ; not read in live path
    //	VS_out stubs : pos_world stub via in.uv (3D coord) ; fs_in.color -> float4(1)
    float3 const pos_world = float3( in.uv.x, in.uv.y, 0.0 );
    float4 const fs_color  = float4( 1.0 );

    float3 const uvw = pos_world * float3( 1.0, 1.0, _aaa_floats.values[ 2 ] ) + 0.5;

    if( any( uvw < _aaa_vec4s.values[ 0 ].xyz ) )
        discard_fragment();
    if( any( uvw > _aaa_vec4s.values[ 1 ].xyz ) )
        discard_fragment();

    if( _aaa_ints.values[ 0 ] <= 0 )
    {
        if( _aaa_ints.values[ 0 ] == 0 )
            return float4( uvw, 1.0 );
        //	aaa_tex_dim[N] stubbed to 2 -> color_dbg_[2]
        return color_dbg_[ 2 ];
    }

    float4 tex = get_color0_( uvw, _aaa_tex_3d_0, _aaa_tex_1, _aaa_samp, _aaa_floats, _aaa_vec4s, _aaa_ints );
    tex.rgb *= _aaa_floats.values[ 0 ];
    tex.a   *= _aaa_floats.values[ 1 ];
    return tex * fs_color;
}
