// vol3d_Voxel.metal -- Path A port from Voxel/vol3d_Voxel.frag (c101, ~380 LOC source).
// Volume raymarcher : crop + LFSR noise jitter + 7-mode dispatch (color / normal /
// light / gradient / accumulate variants). Completes sampler3D pair (c100 vol3d_Plane).
// Patches : aggressive stub strategy (c100 heavy-port doctrine).
//   - `in VS_out { pos_world, color, ecPosition, nor_world, uvw, x_world, y_world,
//     z_world, ray, light_in } fs_in;` -> stage_in stubs (only uvw / ecPosition / light_in
//     / x_world / y_world / z_world read in live raymarch path)
//   - sampler3D aaa_samp0 / sampler2D aaa_samp1 / sampler3D aaa_samp2 -> texture3d/2d/3d bindings
//   - `gl_NormalMatrix` GLSL fixed-pipeline uniform -> identity float3x3 stub
//   - `aaa_tex_dim[]` engine helper -> 2 (always-2D, c93)
//   - `CONST` engine macro (UBO macro idiom) -> P5 moved into fs_main
//   - `LFSR_Rand_Gen` int-bitwise preserved verbatim (c88 lesson)
//   - while(true) ... break loops + macros TEST_SKIP / BEGIN_LOOP / END_LOOP / ACCUMULATE
//     unrolled / inlined per mode -- writing compact form for catalog-compile shape only

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

static inline int LFSR_Rand_Gen_( int n )
{
    n = ( n << 13 ) ^ n;
    return ( n * ( n * n * 15731 + 789221 ) + 1376312589 ) & 0x7fffffff;
}

static inline float noise3f_( float3 p )
{
    int3 ip = int3( floor( p ) );
    int  n  = ip.x + ip.y * 57 + ip.z * 113;
    return float( LFSR_Rand_Gen_( n ) ) / 1073741824.0;
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
    (void) _aaa_tex_3d_2;
    //	VS_out stubs : only uvw / ecPosition / light_in / x/y/z_world read
    float3 const fs_uvw         = float3( in.uv.x, in.uv.y, 0.0 );
    float3 const fs_ecPosition  = float3( 0.0, 0.0, -1.0 );
    float3 const fs_light_in    = float3( 0.0, 0.0, 1.0 );
    float3 const fs_x_world     = float3( 1.0, 0.0, 0.0 );
    float3 const fs_y_world     = float3( 0.0, 1.0, 0.0 );
    float3 const fs_z_world     = float3( 0.0, 0.0, 1.0 );

    //	CONST demotions (P5)
    float const f_color    = _aaa_floats.values[ 0 ];
    float const f_alpha    = _aaa_floats.values[ 1 ];
    float const fz         = _aaa_floats.values[ 2 ];
    float const palette_v  = _aaa_floats.values[ 3 ];
    float4 const color_scale = float4( float3( f_color ), f_alpha );
    int   const s_how      = _aaa_ints.values[ 0 ];
    int   const step_nb    = _aaa_ints.values[ 1 ];
    bool  const b_map_grey = _aaa_ints.values[ 2 ] != 0;
    bool  const b_use_green = _aaa_ints.values[ 3 ] != 0;
    float3 const crop_min  = _aaa_vec4s.values[ 0 ].xyz;
    float3 const crop_max  = _aaa_vec4s.values[ 1 ].xyz;
    float3 const color_light = _aaa_vec4s.values[ 7 ].rgb;
    float  const f_acc       = _aaa_vec4s.values[ 7 ].a;
    (void) palette_v; (void) b_use_green; (void) color_light; (void) f_acc;

    //	gl_NormalMatrix stub (identity)
    float3x3 const gl_NormalMatrix_ = float3x3(
        float3( 1.0, 0.0, 0.0 ),
        float3( 0.0, 1.0, 0.0 ),
        float3( 0.0, 0.0, 1.0 ) );
    (void) gl_NormalMatrix_;

    //	Early-out modes
    if( s_how <= 0 )
    {
        if( s_how == 0 )
            return float4( fs_uvw, 1.0 );
        //	color_dbg[ aaa_tex_dim[N] ] with aaa_tex_dim stubbed to 2
        return float4( 0.0, 1.0, 0.0, 1.0 );
    }

    //	Raymarch setup
    float3 const ec = fs_ecPosition;
    float3 const ec_nor = -ec / ec.z;
    float3 d_uvw;
    d_uvw.x = dot( fs_x_world, ec_nor );
    d_uvw.y = dot( fs_y_world, ec_nor );
    d_uvw.z = dot( fs_z_world, ec_nor );
    d_uvw.z *= fz;

    int    const nb_it       = int( float( step_nb ) * fz );
    float  const nb_it_over  = 1.0 / float( nb_it );
    float  const f           = 1.732 * nb_it_over;
    d_uvw *= f;
    float3 uvw = fs_uvw;
    float  const cor = ( -ec.z - floor( -ec.z / f ) * f ) / f;
    uvw += d_uvw * ( 1.0 - cor );
    uvw += d_uvw * noise3f_( uvw * 80000.0 ) * 0.5;

    //	Crop-skip loop (simplified for catalog-compile)
    for( int safety = 0; safety < 256; safety++ )
    {
        bool const in_crop = all( uvw >= crop_min ) && all( uvw <= crop_max );
        if( in_crop ) break;
        bool const out_of_unit = any( uvw < float3( 0.0 ) ) || any( uvw > float3( 1.0 ) );
        if( out_of_unit ) discard_fragment();
        uvw += d_uvw;
    }

    float4 acc = float4( 0.0 );

    //	7-mode dispatch -- compile-shape only ; aggressive simplification.
    //	Each mode normally accumulates via BEGIN_LOOP_CH1 / END_LOOP_CH1 macros ;
    //	Mac collapses to a single per-mode sample for compile validity.
    float4 tex = float4( 0.0 );
    if( s_how == 1 )
    {
        tex = _aaa_tex_3d_0.sample( _aaa_samp, uvw );
        tex *= color_scale;
        acc = tex;
    }
    else if( s_how == 2 || s_how == 3 || s_how == 4 || s_how == 5 || s_how == 6 )
    {
        tex = _aaa_tex_3d_0.sample( _aaa_samp, uvw );
        tex *= color_scale;
        acc = tex;
    }
    else
    {
        acc = _aaa_tex_3d_0.sample( _aaa_samp, uvw ) * color_scale;
    }

    if( b_map_grey )
        acc.rgb = _aaa_tex_1.sample( _aaa_samp, float2( acc.a, palette_v ) ).rgb;

    return acc;
}
