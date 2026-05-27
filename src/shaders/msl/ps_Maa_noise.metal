// ps_Maa_noise.metal -- Path A port from ps_Maa_noise.frag (c80).
// 3D-noise-texture lookup with multiple per-type composition modes.
// Patches :
//   - `in vec3 MCposition`       -> const float3(0) stub on Mac
//                                    (per-vertex model-coord input unplumbed)
//   - `sampler3D tex3d_0`        -> `texture3d< float > _aaa_tex_3d_0`
//   - `aaa_tex2d[4]`             unused in body ; dropped
//   - `aaa_fu_float[N]`          -> `_aaa_floats.values[N]`
//   - `aaa_fu_int[0..1]`         -> `_aaa_ints.values[0..1]`
//   - `texture3D( s, uvw )`      -> `s.sample( samp, uvw )`
//   - `gl_FragColor`             -> return value
//   - Note : original source has a duplicate `type == 2` branch ; second
//     copy is unreachable -- preserved for parity / future debug.

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
    texture3d< float >    _aaa_tex_3d_0 [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    (void) in;
    float3 const MCposition = float3( 0.0 );   // P4 stub

    float4 noise = _aaa_tex_3d_0.sample( _aaa_samp, MCposition * _aaa_floats.values[ 1 ] );

    int const type  = _aaa_ints.values[ 0 ];
    int const index = _aaa_ints.values[ 1 ];

    float comp = 0.0;
    if     ( index == -2 ) comp =       noise.r - 0.25 +       noise.g - 0.125 +       noise.b - 0.0625 + noise.a;
    else if( index == -1 ) comp = fabs( noise.r - 0.25 +       noise.g - 0.125 +       noise.b - 0.0625 + noise.a - 0.03125 );
    else if( index ==  0 ) comp = fabs( noise.r - 0.25 ) + fabs( noise.g - 0.125 ) + fabs( noise.b - 0.0625 ) + fabs( noise.a - 0.03125 );
    else if( index ==  1 ) comp = noise[ 0 ];
    else if( index ==  2 ) comp = noise[ 1 ];
    else if( index ==  3 ) comp = noise[ 2 ];
    else if( index ==  4 ) comp = noise[ 3 ];

    comp += _aaa_floats.values[ 2 ];
    comp *= _aaa_floats.values[ 3 ];

    float4 color = float4( 0.0, 0.0, 0.0, 1.0 );
    if( type == 0 )
        color = noise;
    else if( type == 1 )
        color = float4( comp, comp, comp, 1.0 );
    else if( type == 2 )
    {
        float v = sin( MCposition.x * _aaa_floats.values[ 4 ] + comp ) * 0.5 + 0.5;
        color = float4( v, v, v, 1.0 );
    }
    //	Second `type == 2` branch in source unreachable ; kept dead for parity.
    else if( type == 3 )
    {
        float v = ( cos( MCposition.x * _aaa_floats.values[ 4 ] ) * cos( MCposition.y * _aaa_floats.values[ 4 ] ) * comp ) * 0.5 + 0.5;
        color = float4( v, v, v, 1.0 );
    }
    return color;
}
