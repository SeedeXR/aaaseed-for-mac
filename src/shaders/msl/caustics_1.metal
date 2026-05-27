// caustics_1.metal -- Path A port from vux_flare_render/caustics_1.frag (c99, c85 carry).
// World-space caustic pattern projected onto deferred surfaces via depth + normal.
// Patches : engine camera cluster (per-shader aaa_cam stub, c98 doctrine).
//   - `aaa_cam.view_projection_inverse` -> identity float4x4 fs_main local
//   - `aaa_fu_vec4[0]` (direction.xyz) ; `aaa_fu_float[0..7]` (caustic_color / factor /
//     scaling / normal_factor / time)
//   - `aaa_fu_int[0..1]` (S_RENDER, ITERATION_NB)
//   - `g_input_texture` / `g_depth_texture` / `g_normal_texture` -> `_aaa_tex_0..2`
//   - `mod( x, K )` -> `fmod( x, K )` (positive operands)
//   - `PI` engine global -> local constant
//   - ENABLE_NORMAL_FALLOFF=1 branch active in source -> preserved

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

constant float PI_ = 3.14159265358979323846;

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
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int    const S_RENDER       = _aaa_ints.values[ 0 ];
    int    const ITERATION_NB   = _aaa_ints.values[ 1 ];
    float3 const direction      = _aaa_vec4s.values[ 0 ].xyz;
    float3 const caustic_color  = float3( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ], _aaa_floats.values[ 2 ] );
    float  const caustic_factor = _aaa_floats.values[ 3 ];
    float2 const caustic_scaling = float2( _aaa_floats.values[ 4 ], _aaa_floats.values[ 5 ] );
    float  const normal_factor  = _aaa_floats.values[ 6 ];
    float  const time_          = _aaa_floats.values[ 7 ];

    //	Engine camera-cluster stub (c98 doctrine)
    float4x4 const aaa_cam_view_projection_inverse_ = float4x4(
        float4( 1.0, 0.0, 0.0, 0.0 ),
        float4( 0.0, 1.0, 0.0, 0.0 ),
        float4( 0.0, 0.0, 1.0, 0.0 ),
        float4( 0.0, 0.0, 0.0, 1.0 ) );

    if( S_RENDER == 1 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    float2 screen_clip = in.uv * 2.0 - 1.0;
    float depth_base = _aaa_tex_1.sample( _aaa_samp, in.uv ).r;
    float depth = depth_base * 2.0 - 1.0;

    float4 const screen_coord = float4( screen_clip, depth, 1.0 );
    float4 world_position = aaa_cam_view_projection_inverse_ * screen_coord;
    world_position.xyz /= world_position.w;

    float3 const dir = normalize( direction );
    float3 const u = normalize( cross( dir, float3( 0.0, 0.0, 1.0 ) ) );
    float3 const v = cross( u, dir );
    float3x3 const mat_dir = float3x3( float3( u ), float3( v ), float3( dir ) );
    float2 uv = ( world_position.xyz * mat_dir ).xy * caustic_scaling;

    float2 const p = fmod( uv * PI_ * 2.0, PI_ * 2.0 ) - 250.0;
    float2 i = p;
    float c = 1.0;
    for( int n = 0; n < ITERATION_NB; n++ )
    {
        float t = time_ * ( 1.0 - ( 3.5 / float( n + 1 ) ) );
        i = p + float2( cos( t - i.x ) + sin( t + i.y ), sin( t - i.y ) + cos( t + i.x ) );
        c += 1.0 / length( float2( p.x / ( sin( i.x + t ) * caustic_factor ),
                                     p.y / ( cos( i.y + t ) * caustic_factor ) ) );
    }
    c /= float( ITERATION_NB );
    c = 1.17 - pow( c, 1.4 );
    float3 result_color = float3( pow( fabs( c ), 8.0 ) );
    result_color *= caustic_color;

    //	ENABLE_NORMAL_FALLOFF == 1 branch
    float3 const norm = _aaa_tex_2.sample( _aaa_samp, in.uv ).rgb;
    float ndl = clamp( dot( norm, float3( 0.0, 1.0, 0.0 ) ), 0.0, 1.0 );
    ndl = mix( 1.0, ndl, normal_factor );
    result_color *= ndl;

    float4 color = _aaa_tex_0.sample( _aaa_samp, in.uv );
    switch( S_RENDER )
    {
        case 2: color.rgb  = result_color; break;
        case 3: if( depth_base < 1.0 ) color.rgb  = result_color; break;
        case 4: if( depth_base < 1.0 ) color.rgb += result_color; break;
        case 5: if( depth_base < 1.0 ) color.rgb -= result_color; break;
    }

    return float4( color.rgb, 1.0 );
}
