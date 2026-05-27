// light_sprites.metal -- Path A port from vux_flare_render/light_sprites.frag (c86).
// Animated billboard-light sprites with world-position-driven sine modulation.
// Patches :
//   - `g_input_texture` -> `_aaa_tex_0`
//   - `in vec3 in_world_position` (P4 per-vertex) -> const float3( 0, 0, 0 ) stub
//     (world position from vertex stage not yet plumbed Mac-side)
//   - `aaa_fu_int[0]`     -> AaaFuInts[0] (bypass)
//   - `aaa_fu_float[0..3]` -> AaaFuFloats[0..3] (intensity / time / range.x / range.y)
//   - `aaa_fu_vec4[0]`    -> AaaFuVec4s[0] (color_in)
//   - `layout(location = 0) in vec2 in_texcoord` -> in.uv
//   - `discard` -> `discard_fragment()`
//   - `layout(early_fragment_tests)` qualifier -- MSL has `[[early_fragment_tests]]`
//     on fragment functions ; preserved as a comment (the catalog-test compile
//     doesn't require it to validate pipeline shape).
//   - dead `#if 0` aaa_cam branch dropped

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

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
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int    const bypass            = _aaa_ints.values[ 0 ];
    float  const sprite_intensity  = _aaa_floats.values[ 0 ];
    float  const time              = _aaa_floats.values[ 1 ];
    float2 const intensity_range   = float2( _aaa_floats.values[ 2 ], _aaa_floats.values[ 3 ] );
    float4 const color_in          = _aaa_vec4s.values[ 0 ];
    float3 const in_world_position = float3( 0.0 );   // P4 stub

    if( bypass != 0 )
        return float4( 1.0 );

    float4 color = _aaa_tex_0.sample( _aaa_samp, in.uv );
    color.a *= sprite_intensity;

    float animated_intensity = sin( in_world_position.x * 20.0 + time )
                             * cos( in_world_position.y *  5.0 + time )
                             + sin( in_world_position.z *  1.0 + time );
    animated_intensity = saturate( animated_intensity );
    animated_intensity = intensity_range.x + animated_intensity * ( intensity_range.y - intensity_range.x );
    color.a *= animated_intensity;

    float const test = dot( color.rgb, color.aaa );
    if( test <= 0.0 )
        discard_fragment();

    return color * color_in;
}
