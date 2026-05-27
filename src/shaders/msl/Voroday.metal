// Voroday.metal -- Path A port from Jean/Voroday.frag (c87, Palliaci, CC BY-NC-SA 3.0).
// Voronoi-displaced sphere raymarch with reflection from a cubemap stub.
// Patches :
//   - ShaderToy idiom : iTime/iResolution/iMouse -> AaaFuFloats[0..5]
//   - `iChannel1` (cubemap) -> stubbed to opaque-blue (`float3( 0.1, 0.2, 0.5 )`)
//     -- the engine's iChannel1 is a cube sampler ; Mac side has no cubemap binding
//     in this shader's pipeline. Stub keeps semantics plausible ; documented.
//   - file-scope `float st;` (P5) moved into fs_main
//   - `mat3 rotX/rotY/rotZ` -> MSL `float3x3` constructors
//   - `mat3 * vec3` -- MSL multiplies column-major by default ; preserved
//   - `mainImage(out, in)` -> fs_main wrapper synthesizing fragCoord

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant int   I_MAX_ = 20;
constant float FAR_   = 5.0;
constant float E_     = 0.02;
constant float SCALE_ = 28.0;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float3x3 rotX_( float a )
{
    float c = cos( a );
    float s = sin( a );
    return float3x3( float3( 1.0, 0.0, 0.0 ),
                     float3( 0.0,   c,  -s ),
                     float3( 0.0,   s,   c ) );
}

static inline float3x3 rotY_( float a )
{
    float c = cos( a );
    float s = sin( a );
    return float3x3( float3(  c, 0.0,   s ),
                     float3( 0.0, 1.0, 0.0 ),
                     float3( -s, 0.0,   c ) );
}

static inline float2 random2_( float2 p )
{
    return fract( sin( float2( dot( p, float2( 127.1, 311.7 ) ),
                                dot( p, float2( 269.5, 183.3 ) ) ) ) * 43758.5453 );
}

static inline float3 voronoi_( float2 uv, float iTime )
{
    uv *= SCALE_;
    float2 i_uv = floor( uv );
    uv = fract( uv );

    float  mdist = 1.0;
    float2 mpt   = float2( 0.0 );
    for( int y = -1; y <= 1; y++ )
        for( int x = -1; x <= 1; x++ )
        {
            float2 neighbor = float2( float( x ), float( y ) );
            float2 pt = random2_( i_uv + neighbor );
            pt = 0.5 + 0.5 * sin( iTime + 6.28 * pt );
            float2 diff = neighbor + pt - uv;
            float  dist = length( diff );
            if( dist < mdist )
            {
                mdist = dist;
                mpt   = pt;
            }
        }
    return float3( mdist, mpt );
}

static inline float3 voronoi_sphere_map_( float3 p, float iTime )
{
    float2 uv;
    uv.x = 0.5 + atan2( p.z, p.x ) / ( 2.0 * 3.14159 );
    uv.y = 0.5 - asin(  p.y ) / 3.14159;
    return voronoi_( uv, iTime );
}

static inline float map_( float3 p, float iTime )
{
    p = rotY_( 0.127 * iTime ) * rotX_( 0.127 * iTime ) * p;
    float3 ret = voronoi_sphere_map_( normalize( p ), iTime );
    return length( p ) - 0.5 - 0.05 * ret.x;
}

static inline float march_( float3 pos, float3 dir, float iTime )
{
    float d = 0.0;
    float prec = E_;
    for( int i = -1; i < I_MAX_; i++ )
    {
        prec = map_( pos + dir * d, iTime );
        d += prec;
        if( d < E_ || d > FAR_ ) break;
    }
    return d;
}

static inline float3 camera_( float2 uv )
{
    float  fov   = 1.0;
    float3 forw  = float3( 0.0, 0.0, 1.0 );
    float3 right = float3( 1.0, 0.0, 0.0 );
    float3 up    = float3( 0.0, 1.0, 0.0 );
    return normalize( ( uv.x - 1.0 ) * right + ( uv.y - 0.5 ) * up + fov * forw );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]] )
{
    float const iTime         = _aaa_floats.values[ 0 ];
    float const iResolution_x = _aaa_floats.values[ 1 ];
    float const iResolution_y = _aaa_floats.values[ 2 ];

    float const st = sin( 0.5 * iTime );
    float2 const fragCoord = in.uv.xy * float2( iResolution_x, iResolution_y );
    float2 uv = fragCoord / float2( iResolution_x, iResolution_y );
    uv.x *= iResolution_x / iResolution_y;

    float3 pos = rotX_( iTime * 0.3 ) * float3( 0.2, 0.0, -0.85 + 0.08 * st );
    float3 dir = rotX_( iTime * 0.3 - 0.1 ) * camera_( uv );
    float3 col = float3( 0.942, 0.732, 0.523 );

    float d = march_( pos, dir, iTime );
    if( d < FAR_ )
    {
        float3 p = pos + dir * d;
        col += voronoi_sphere_map_(
            normalize( rotY_( 0.127 * iTime ) * rotX_( 0.127 * iTime ) * p ),
            iTime );

        //	Engine has a cubemap iChannel1.sample(dir) here ; Mac has no cube
        //	binding in this shader's pipeline. Stub to opaque-blue keeps the
        //	`#ifdef LIGHTS` lighting math compile-shape valid ; runtime side
        //	may swap in a real cube sampler later.
        float3 const refl_cube = float3( 0.1, 0.2, 0.5 ) * 0.5;   // REFL_I = 0.5

        float2 e = float2( -1.0, 1.0 ) * 0.005;
        float3 n = normalize( e.yxx * map_( p + e.yxx, iTime )
                            + e.xxy * map_( p + e.xxy, iTime )
                            + e.xyx * map_( p + e.xyx, iTime )
                            + e.yyy * map_( p + e.yyy, iTime ) );
        float3 ev     = normalize( p - pos );
        float3 ref_ev = reflect( ev, n );
        col *= clamp( refl_cube, 0.15, 1.0 );

        float3 light_pos = pos + float3( st, 0.0, -0.1 );
        float3 vl = normalize( light_pos - p );
        float  diffuse  = max( 0.001, dot( vl, n ) );
        float  specular = pow( max( 0.001, dot( vl, ref_ev ) ), 1.0 );
        float  brdf = ( diffuse + specular ) * 0.5 + 0.5;
        col *= brdf;
    }
    else
    {
        col = float3( 0.1, 0.2, 0.5 );   // iChannel1 cubemap stub
    }
    return float4( col, 1.0 );
}
