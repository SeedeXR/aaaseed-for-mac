// bloom.metal -- Path A port from vux_flare_render/bloom.frag (c86).
// 9x9 separable bloom : 9 columns horizontally averaged, then summed vertically
// with `bloom_spread` step.
// Patches :
//   - `g_input_texture` -> `_aaa_tex_0`
//   - `aaa_fu_float[0..1]` (bloom_spread, bloom_intensity) -> AaaFuFloats[0..1]
//   - `aaa_vu_int[0]` (bypass) -> AaaFuInts[0]
//   - `layout(location = 0) in vec2 in_texcoord` -> in.uv
//   - `textureSize(tex, 0)` -> `int2( tex.get_width(), tex.get_height() )`
//   - `texelFetch(tex, ivec2 coord, 0)` -> `tex.read( uint2(coord) )`
//     (MSL : integer-pixel fetch goes through `read`, not `sample`)

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
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float const bloom_spread    = _aaa_floats.values[ 0 ];
    float const bloom_intensity = _aaa_floats.values[ 1 ];
    int   const bypass          = _aaa_ints.values[ 0 ];

    if( bypass != 0 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    int2  const size = int2( _aaa_tex_0.get_width(), _aaa_tex_0.get_height() );
    float const uv_x = in.uv.x * float( size.x );

    float3 sum = float3( 0.0 );
    for( int n = 0; n < 9; ++n )
    {
        float const uv_y = in.uv.y * float( size.y ) + bloom_spread * float( n - 4 );
        float4 h_sum = float4( 0.0 );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x - 4.0 * bloom_spread ), int( uv_y ) ) ) );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x - 3.0 * bloom_spread ), int( uv_y ) ) ) );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x - 2.0 * bloom_spread ), int( uv_y ) ) ) );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x - bloom_spread       ), int( uv_y ) ) ) );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x                       ), int( uv_y ) ) ) );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x + bloom_spread       ), int( uv_y ) ) ) );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x + 2.0 * bloom_spread ), int( uv_y ) ) ) );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x + 3.0 * bloom_spread ), int( uv_y ) ) ) );
        h_sum += _aaa_tex_0.read( uint2( int2( int( uv_x + 4.0 * bloom_spread ), int( uv_y ) ) ) );
        sum += h_sum.rgb / 9.0;
    }

    float3 const color = _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb;
    return float4( color + ( sum / 9.0 ) * bloom_intensity, 1.0 );
}
