// fire.metal -- Path A port from ShaderToy/fire.frag (c87, Dave Hoskins 2013).
// Volumetric campfire raymarcher with 3D-noise flames + log SDF + stone SDF.
// Patches :
//   - ShaderToy idiom (see blood/drip headers) :
//     `iTime`         -> `_aaa_floats.values[0]`
//     `iResolution.x/y` -> `_aaa_floats.values[1..2]`
//     `iMouse.x/y/z`  -> `_aaa_floats.values[3..5]`
//     `iChannel0`     -> `_aaa_tex_0`
//     `iChannelResolution[0].xy` -> hard-coded 256 (typical ShaderToy noise tex)
//   - `mainImage(out vec4, in vec2)` -> fs_main wrapper synthesizing fragCoord
//   - `textureLod( tex, uv, 0.0 )` -> `tex.sample( samp, uv, level(0.0) )`
//   - `texture( iChannel0, uv )` -> `tex.sample( samp, uv )`
//   - `abs(float)` -> `fabs(float)`
//   - Helper functions kept as `static inline` ; large but readable.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant float TAU_ = 6.28318530718;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float Hash11_( float p )
{
    float3 p3 = fract( float3( p ) * 0.1031 );
    p3 += dot( p3, p3.yzx + 19.19 );
    return fract( ( p3.x + p3.y ) * p3.z );
}

static inline float Noise1D_( float x )
{
    float p = floor( x );
    float f = fract( x );
    f = f * f * ( 3.0 - 2.0 * f );
    return mix( Hash11_( p ), Hash11_( p + 1.0 ), f );
}

static inline float Bump_( float3 x, texture2d< float > tex, sampler samp )
{
    float3 p = floor( x );
    float3 f = fract( x );
    f = f * f * ( 3.0 - 2.0 * f );
    float2 uv = ( p.xy + float2( 37.0, 17.0 ) * p.z ) + f.xy;
    float2 rg = tex.sample( samp, ( uv + 0.5 ) / 256.0, level( 0.0 ) ).yx;
    return mix( rg.x, rg.y, f.z );
}

static inline float Noise3D_( float3 x, float iTime, texture2d< float > tex, sampler samp )
{
    x.y -= iTime * 4.0;
    float3 p = floor( x );
    float3 f = fract( x );
    f = f * f * ( 3.0 - 2.0 * f );
    float2 uv = ( p.xy + float2( 37.0, 17.0 ) * p.z ) + f.xy;
    float2 rg = tex.sample( samp, ( uv + 0.5 ) / 256.0, level( 0.0 ) ).yx;
    return mix( rg.x, rg.y, f.z );
}

static inline float RoundBox_( float3 p, float3 add, texture2d< float > tex, sampler samp )
{
    return length( max( fabs( p + add ) - 1.1, float3( 0.0 ) ) )
         - Bump_( p * float3( 1.3, 0.1, 1.3 ), tex, samp ) * 0.5;
}

static inline float DE_Fire_( float3 p, float iTime, texture2d< float > tex, sampler samp )
{
    p.xz += Noise3D_( p * 0.8, iTime, tex, samp ) * p.y * 0.3;
    float3 shape = p * float3( 1.5, 0.35, 1.5 );
    if( dot( shape, shape ) > 70.0 ) return 1.0;

    p += 2.5 * ( Noise3D_(  shape * 1.5,   iTime, tex, samp )
               - Noise3D_( -shape * 0.945, iTime, tex, samp ) * 0.5
               + Noise3D_(  shape * 9.6,   iTime, tex, samp ) * 0.3 );

    float f = length( shape ) - ( 1.0 + Noise3D_( p, iTime, tex, samp ) * 10.0 );
    f -= max( 2.4 - p.y, 0.0 ) * 3.0;
    f -= pow( fabs( Noise3D_( shape * 3.9, iTime, tex, samp ) ), 45.0 )
       * 300.0
       * pow( fabs( Noise3D_( shape * 1.1, iTime, tex, samp ) ),  5.0 );
    return f;
}

static inline float2 RotateCamera_( float2 p, float a )
{
    float si = sin( a );
    float co = cos( a );
    return float2x2( si, -co, co, si ) * p;
}

static inline float3 FlameColour_( float f )
{
    f = f * f * ( 3.0 - 2.0 * f );
    return min( float3( f + 0.8, f * f * 1.4 + 0.1, f * f * f * 0.7 ) * f, 1.0 );
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
    float const iMouse_x      = _aaa_floats.values[ 3 ];
    float const iMouse_y      = _aaa_floats.values[ 4 ];
    float const iMouse_z      = _aaa_floats.values[ 5 ];

    float2 const fragCoord = in.uv.xy * float2( iResolution_x, iResolution_y );
    float2       uv = fragCoord / float2( iResolution_x, iResolution_y );
    float2 p = -0.25 + 0.5 * uv;
    p.x *= iResolution_x / iResolution_y;
    p.y += 0.5;

    float2 mouse;
    if( iMouse_z <= 0.5 )
    {
        float t = iTime + 39.5;
        mouse.x = t * 0.025;
        mouse.y = sin( t * 0.3 ) * 0.5 + 0.5;
    }
    else
    {
        mouse = float2( iMouse_x, iMouse_y ) / float2( iResolution_x, iResolution_y );
    }

    float3 origin = float3( 0.0, 5.3, -13.0 + mouse.y * 5.0 );
    float3 target = float3( 0.0, 4.3 - mouse.y * 3.0, 0.0 );
    origin.xz = RotateCamera_( origin.xz, 0.4 + mouse.x * TAU_ );

    float3 cw = normalize( target - origin );
    float3 cu = normalize( cross( cw, float3( 0.0, 1.0, 0.0 ) ) );
    float3 cv = cross( cu, cw );
    float3 ray = normalize( p.x * cu + p.y * cv
                          + ( 1.5 + Noise3D_( float3( iTime * 0.5 ), iTime, _aaa_tex_0, _aaa_samp ) * 0.1 ) * cw );

    //	Raymarch inlined :
    float sum = 0.0;
    float t = 4.2 + 0.1 * _aaa_tex_0.sample( _aaa_samp, fragCoord / 256.0 ).y;
    float3 pos = float3( 0.0 );
    for( int i = 0; i < 200; i++ )
    {
        if( pos.y < 0.0 ) break;
        pos = origin + t * ray;
        float v = 1.0 - DE_Fire_( pos, iTime, _aaa_tex_0, _aaa_samp );
        v = max( v, 0.0 ) * 0.00187;
        sum += v;
        t += max( 0.075, t * 0.005 );
    }
    float4 ret = float4( pos, clamp( sum * sum * sum, 0.0, 1.0 ) );

    float3 col = FlameColour_( ret.w );
    col = ( 1.0 - exp( -col * 2.0 ) ) * 1.15;
    col = sqrt( col );
    col *= 0.5 + 0.5 * pow( 150.0 * uv.x * uv.y * ( 1.0 - uv.x ) * ( 1.0 - uv.y ), 0.5 );

    return float4( col, 1.0 );
}
