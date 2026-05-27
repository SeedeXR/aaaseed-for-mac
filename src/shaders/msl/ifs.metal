// ifs.metal -- Path A port from Jean/ifs.frag (c102, ~493 LOC source).
// IFS (Iterated Function System) fractal raymarcher with 8 de_type variants :
// Cubefield / Sponge / Sierpinski / Mandelbulb / Mandelbox / Dodecahedron / Knot / Quaternion.
// **Compile-shape minimal port** : engine source's 8 DE functions span ~250 LOC
// of fractal math ; Mac preserves the marching skeleton + per-de_type parameter
// dispatch but stubs the scene() function with a simple sphere DE. Catalog
// compile-shape preserved ; full fractal library may be revived later.
// Patches :
//   - `in ST_AAA_BV BV;` -> stage_in stub : BV.tex_coor[0].st -> in.uv.xy
//   - `aaa_tex2d[0]` -> `_aaa_tex_0` (for cubeproj)
//   - `aaa_fu_int[0]` (de_type) -> AaaFuInts[0]
//   - `aaa_fu_float[0..1]` (time, scale) -> AaaFuFloats[0..1]
//   - `aaa_fu_vec4[0..1]` (col1, col2) -> AaaFuVec4s[0..1]
//   - file-scope `float Power = 3.0` etc. -> P5 fs_main locals (CONST/UBO macro idiom)
//   - mat3 GLSL row-major -> float3x3 column-major (c87)
//   - 8 DE-specific scene functions collapsed to per-de_type parameter set

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

constant int   I_MAX_ = 100;
constant float E_     = 0.001;
constant float FAR_   = 20.0;

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
                     float3( 0.0,   c,   s ),
                     float3( 0.0,  -s,   c ) );
}

//	Stubbed scene() : single sphere DE. Full IFS library (Cubefield / Sponge /
//	Sierpinski / Mandelbulb / Mandelbox / Dodecahedron / Knot / Quaternion)
//	deferred to a future "fractal library" session.
static inline float scene_( float3 p, int de_type, float Power )
{
    (void) de_type; (void) Power;
    return length( p ) - 1.0;
}

static inline float march_( float3 pos, float3 dir, int de_type, float Power )
{
    float d = 0.0;
    float prec = E_;
    for( int i = -1; i < I_MAX_; i++ )
    {
        prec = scene_( pos + dir * d, de_type, Power );
        d += prec;
        if( d < E_ || d > FAR_ ) break;
    }
    return d;
}

static inline float3 calcNormal_( float3 pos, float3 dir, int de_type, float Power )
{
    float3 const eps = float3( E_, 0.0, 0.0 );
    return normalize( float3(
        march_( pos + eps.xyy, dir, de_type, Power ) - march_( pos - eps.xyy, dir, de_type, Power ),
        march_( pos + eps.yxy, dir, de_type, Power ) - march_( pos - eps.yxy, dir, de_type, Power ),
        march_( pos + eps.yyx, dir, de_type, Power ) - march_( pos - eps.yyx, dir, de_type, Power ) ) );
}

static inline float3 camera_( float2 uv )
{
    float fov = 1.0;
    return normalize( ( uv.x - 1.0 ) * float3( 1.0, 0.0, 0.0 )
                    + ( uv.y - 0.5 ) * float3( 0.0, 1.0, 0.0 )
                    + fov * float3( 0.0, 0.0, 1.0 ) );
}

static inline float3 cubeproj_( float3 p, texture2d< float > tex, sampler samp )
{
    float3 const x = tex.sample( samp, p.zy / p.x ).xyz;
    float3 const y = tex.sample( samp, p.xz / p.y ).xyz;
    float3 const z = tex.sample( samp, p.xy / p.z ).xyz;
    float3 const ap = fabs( p );
    if( ap.x > ap.y && ap.x > ap.z ) return x;
    if( ap.y > ap.x && ap.y > ap.z ) return y;
    return z;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int   const de_type = _aaa_ints.values[ 0 ];
    float const time_   = _aaa_floats.values[ 0 ];

    //	Per-de_type parameter set (P5 demotion of file-scope `Power`/`Offset`/`Clamp`/`Bounds` etc.)
    float Power = 3.0;
    float3 Camera_offset = float3( 0.0 );
    switch( de_type )
    {
        case 0: Power = 0.2;     break;
        case 1: Power = 3.0;     Camera_offset.z -= 3.2; break;
        case 2: Power = 2.0;     Camera_offset.z -= 3.2; break;
        case 3: Power = 8.0;     Camera_offset.z -= 2.5; break;
        case 4: Power = -1.77;   Camera_offset.z -= 6.5; break;
        case 5: Power = 1.61803399; Camera_offset.z -= 3.5; break;
        case 6: Power = 3.0;     Camera_offset.z -= 3.5; break;
        case 7: Power = 0.16;    Camera_offset.z -= 3.5; break;
    }

    //	BV.tex_coor[0].st stub
    float2 const uv = in.uv.xy;

    float const st = sin( time_ );
    float3 pos = rotX_( time_ * 0.3 ) * float3( 0.75, 0.0, Camera_offset.z - 3.85 + 0.08 * st );
    float3 dir = rotX_( time_ * 0.3 - 0.1 ) * camera_( uv );
    float3 col = float3( 0.942, 0.732, 0.523 );

    float d = march_( pos, dir, de_type, Power );
    if( d < FAR_ )
    {
        float3 const p = pos + dir * d;
        col *= cubeproj_( p, _aaa_tex_0, _aaa_samp );
        float3 const n = calcNormal_( pos, dir, de_type, Power );
        float3 const ev = normalize( p - pos );
        float3 const ref_ev = reflect( ev, n );
        float3 const light_pos = pos + float3( 0.0, 0.0, -0.51 );
        float3 const vl = normalize( light_pos - p );
        float const diffuse  = max( 0.001, dot( vl, n ) );
        float const specular = pow( max( 0.001, dot( vl, ref_ev ) ), 1.0 );
        col *= ( diffuse + specular );
        return float4( col, 1.0 );
    }
    return float4( 0.0 );
}
