// blood.metal -- Path A port from ShaderToy/blood.frag (c85).
// ShaderToy-style time-evolving blood pool with refraction-like surface shading.
// Patches :
//   - `iTime`           -> `_aaa_floats.values[0]` (Mac convention : ShaderToy `iTime` maps
//                            to AaaFuFloats slot 0 ; runtime side must populate)
//   - `iChannel0` (texture) -> `_aaa_tex_0`
//   - `iResolution.x/y` -> `_aaa_floats.values[1..2]` (resolution in pixels)
//   - ShaderToy `mainImage(out vec4 fragColor, in vec2 fragCoord)` -> fs_main wrapper that
//     synthesizes `fragCoord = in.uv * resolution` ; final write returns fragColor.
//   - `mod( x, K )`     -> MSL `fmod( x, K )`

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float DE_( float2 pp, thread bool& blood, float t )
{
    pp.y += ( 0.4 * sin( 0.5 *  2.3 * pp.x + pp.y )
            + 0.2 * sin( 0.5 *  5.5 * pp.x + pp.y )
            + 0.1  * sin( 0.5 * 13.7 * pp.x )
            + 0.06 * sin( 0.5 * 23.0 * pp.x ) );

    pp += float2( 0.0, 0.4 ) * t;

    float const thresh = 5.3;
    blood = pp.y > thresh;
    return fabs( pp.y - thresh );
}

static inline float3 sceneColor_(
    float2 pp, float iTime,
    texture2d< float > iChannel0, sampler samp )
{
    float const endTime = 16.0;
    float const rewind  = 2.0;
    float t = fmod( iTime, endTime + rewind );
    if( t > endTime )
        t = endTime * ( 1.0 - ( t - endTime ) / rewind );

    bool  blood;
    float const d = DE_( pp, blood, t );

    if( !blood )
    {
        float3 floorCol = iChannel0.sample( samp, -( float2( pp.x / 20.0, pp.y / 33.0 ) + float2( 0.5, 0.02 ) ) ).xyz;
        float  floori   = ( floorCol.x + floorCol.y + floorCol.z ) / 3.0;
        floori = smoothstep( 0.5, 0.53, floori );
        floori = smoothstep( -5.15, 1.0, floori );
        floori = 1.0 - floori;
        floorCol = float3( floori );

        float ao = clamp( smoothstep( 0.0, 0.2, d ), 0.0, 1.0 );
        return mix( 1.0, sqrt( ao ), 0.25 ) * floorCol;
    }
    float h = clamp( smoothstep( 0.0, 0.25, d ), 0.0, 1.0 );
    h = 4.0 * pow( h, 0.2 );
    float3 N = normalize( float3( -dfdx( h ), 1.0, -dfdy( h ) ) );
    float3 L = normalize( float3( 0.5, 0.7, -0.5 ) );
    float  res = pow( dot( N, L ), 6.0 );
    return mix( float3( 1.0, 0.0, 1.0 ), float3( 1.0, 0.8, 0.0 ), 1.0 - res );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const iTime         = _aaa_floats.values[ 0 ];
    float const iResolution_x = _aaa_floats.values[ 1 ];
    float const iResolution_y = _aaa_floats.values[ 2 ];

    float2 fragCoord = in.uv.xy * float2( iResolution_x, iResolution_y );
    float2 uv = fragCoord / float2( iResolution_x, iResolution_y );
    uv.x /= iResolution_y / iResolution_x;

    float4 fragColor;
    fragColor.a   = 1.0;
    fragColor.xyz = sceneColor_( uv * 4.0, iTime, _aaa_tex_0, _aaa_samp );
    return fragColor;
}
