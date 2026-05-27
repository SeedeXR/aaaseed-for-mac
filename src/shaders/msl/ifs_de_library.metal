// ifs_de_library.metal -- Path A REVIVAL of the 8-variant IFS Distance Estimator
// library for the c102 Jean/ifs.frag port.
//
// c129-A revival #2 (second after c128's fxaa_lottes) : the c102 Path A port
// of `ifs.frag` shipped a compile-shape-only stub -- the marching skeleton,
// per-de_type Power + Camera_offset dispatch, calcNormal, and cubeproj all
// landed verbatim, but the eight DE-specific scene functions were collapsed
// to a single `length(p) - 1.0` sphere. This file restores the real fractal
// math, keeping the original `ifs.metal` PRESERVED verbatim (it anchors
// regression #34's golden -- see phase3_golden_frame_regression_test.cpp).
//
// Pattern : c128-A's fxaa.metal / fxaa_lottes.metal split. Both shaders live
// in the catalog ; the stub anchors the c129-B baseline, the revival proves
// the real algorithms execute on Apple Metal. Bumps Path A floor 159 -> 160.
//
// References (all 8 DEs traced to public, citable sources) :
//
//   - Inigo Quilez, "Distance to fractals"
//     https://iquilezles.org/articles/distfunctions/
//     -> Mandelbulb (power-8 spherical fold), Mandelbox (box+sphere fold),
//        Sierpinski (tetrahedral fold), Sponge (recursive cube subdivision).
//
//   - Daniel White & Paul Nylander, "Mandelbulb : 3D analog of the
//     Mandelbrot set", 2009 -- the canonical power-N spherical fold.
//
//   - Tom Lowe, "Mandelbox" SkyTopia 2010 -- box-fold + sphere-fold + scale
//     with Power treated as scale (negative values flip the inside-out
//     parity, matching c102's Power = -1.77 default).
//
//   - Knighty / Fractal Forums "Knightyan" recipes : the Dodecahedron
//     icosahedral kaleidoscope + Knot trefoil/torus folds.
//
//   - Mandelbulber project (mandelbulber.com) -- Quaternion 4D fold
//     reference (q = q^2 + c lifted to 4 components).
//
// Each DE function below cites the specific recipe it implements.
//
// What is preserved verbatim from c102 `ifs.metal` :
//   - `vs_main` (P5 verts + UVs)
//   - `rotX_`, `camera_`, `cubeproj_`, `march_`, `calcNormal_` helpers
//   - `fs_main` body : per-de_type Power + Camera_offset switch, ray origin
//     `rotX_(time_*0.3) * float3(0.75, 0, Camera_offset.z - 3.85 + 0.08*sin(time))`,
//     lighting (diffuse + specular, cubeproj sampled, exit on `d < FAR_`).
//
// What changes : `scene_()`. Stub's `length(p) - 1.0` -> switch over
// de_type dispatching to one of 8 DE functions, each ~10-30 LOC of the
// real fractal math.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

constant int   I_MAX_   = 100;
constant float E_       = 0.001;
constant float FAR_     = 20.0;
constant int   DE_ITER_ = 8;   // iterations per DE (Iquilez recommends 6-12 ;
                               // 8 is the c102-doc default and the Mandelbulber
                               // "fast preview" tier ; keeps shader fast enough
                               // for the regression rig).

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

//	---------------- DE 0 : Cubefield ---------------------------------
//	Repeating axis-aligned cube grid : fold space by mod() onto unit
//	cell, subtract a fixed cube radius (Power treated as cube half-side
//	per c102's Power=0.2 default). Reference : Iquilez `sdBox` + domain
//	repetition (https://iquilezles.org/articles/distfunctions/#sdBox).
static inline float de_cubefield_( float3 p, float Power )
{
    float3 q = fract( p * 0.5 ) * 2.0 - 1.0;
    float3 d = fabs( q ) - Power;
    return min( max( d.x, max( d.y, d.z ) ), 0.0 )
         + length( max( d, float3( 0.0 ) ) );
}

//	---------------- DE 1 : Menger Sponge -----------------------------
//	Iquilez's recursive cube subdivision (the canonical 3D Cantor set).
//	Each iteration : scale p by 3, fold-abs into the first octant,
//	subtract a cross-shaped void. Power=3.0 is the recursive scale.
//	Reference : Iquilez "Menger sponge" article
//	(https://iquilezles.org/articles/menger/).
static inline float de_sponge_( float3 p, float Power )
{
    float d   = length( max( fabs( p ) - 1.0, float3( 0.0 ) ) );
    float s   = 1.0;
    for( int i = 0; i < 4; ++i )
    {
        float3 a = fract( p * s ) * 2.0 - 1.0;
        s       *= Power;
        float3 r = fabs( 1.0 - 3.0 * fabs( a ) );
        float  da = max( r.x, r.y );
        float  db = max( r.y, r.z );
        float  dc = max( r.z, r.x );
        float  c  = ( min( da, min( db, dc ) ) - 1.0 ) / s;
        d = max( d, c );
    }
    return d;
}

//	---------------- DE 2 : Sierpinski Tetrahedron --------------------
//	Classical tetrahedral kaleidoscopic IFS : reflect p across 3
//	mirror planes (the 4 tet vertices), translate, scale by 2.
//	Iterating produces a Sierpinski tetrahedron. Power treated as
//	scale (c102 default 2.0 is the canonical Sierpinski ratio).
//	Reference : Iquilez "kifs" (kaleidoscopic IFS) note + Mandelbulber
//	`Tetrahedron` formula.
static inline float de_sierpinski_( float3 p, float Power )
{
    for( int i = 0; i < DE_ITER_; ++i )
    {
        if( p.x + p.y < 0.0 ) { float t = -p.y; p.y = -p.x; p.x = t; }
        if( p.x + p.z < 0.0 ) { float t = -p.z; p.z = -p.x; p.x = t; }
        if( p.y + p.z < 0.0 ) { float t = -p.z; p.z = -p.y; p.y = t; }
        p = p * Power - float3( Power - 1.0 );
    }
    return length( p ) * pow( Power, -float( DE_ITER_ ) );
}

//	---------------- DE 3 : Mandelbulb --------------------------------
//	Daniel White / Paul Nylander's power-N spherical fold (2009). For
//	each iteration : convert p to spherical (r, theta, phi), raise r
//	to Power, multiply angles by Power, convert back, add c=p0. The
//	escape-distance estimator uses the Hubbard-Douady formula
//	`0.5 * log(r) * r / dr`. Reference : Iquilez "Mandelbulb" article
//	(https://iquilezles.org/articles/mandelbulb/). c102 default Power=8.
static inline float de_mandelbulb_( float3 p, float Power )
{
    float3 z  = p;
    float  dr = 1.0;
    float  r  = 0.0;
    for( int i = 0; i < DE_ITER_; ++i )
    {
        r = length( z );
        if( r > 2.0 ) break;
        float theta = acos( z.z / max( r, 1e-6 ) );
        float phi   = atan2( z.y, z.x );
        dr = pow( r, Power - 1.0 ) * Power * dr + 1.0;
        float zr = pow( r, Power );
        theta *= Power;
        phi   *= Power;
        z = zr * float3( sin( theta ) * cos( phi ),
                         sin( phi )   * sin( theta ),
                         cos( theta ) );
        z += p;
    }
    return 0.5 * log( max( r, 1e-6 ) ) * r / max( dr, 1e-6 );
}

//	---------------- DE 4 : Mandelbox ---------------------------------
//	Tom Lowe's "Mandelbox" (SkyTopia 2010) : alternating box-fold
//	(clamp on [-1,1] then reflect), sphere-fold (inverse-radius scale
//	below `min_r2`, identity above), and scale by Power. c102's
//	Power=-1.77 inverts orientation (negative scales are Mandelbox
//	stylistic convention). Reference : Tom Lowe original post +
//	Iquilez "Mandelbox" follow-up.
static inline float de_mandelbox_( float3 p, float Power )
{
    constexpr float fold_lim = 1.0;
    constexpr float min_r2   = 0.25;
    constexpr float fixed_r2 = 1.0;

    float3 z   = p;
    float  dr  = 1.0;
    for( int i = 0; i < DE_ITER_; ++i )
    {
        //	Box-fold : reflect components outside [-fold_lim, fold_lim].
        z = clamp( z, -fold_lim, fold_lim ) * 2.0 - z;
        //	Sphere-fold : inverse-square scale inside a small sphere.
        float r2 = dot( z, z );
        if(      r2 < min_r2  ) { float t = fixed_r2 / min_r2; z *= t; dr *= t; }
        else if( r2 < fixed_r2 ) { float t = fixed_r2 / r2;    z *= t; dr *= t; }
        //	Scale + translate (Power applied here ; negative flips).
        z  = z * Power + p;
        dr = dr * fabs( Power ) + 1.0;
    }
    return length( z ) / fabs( dr );
}

//	---------------- DE 5 : Dodecahedron ------------------------------
//	Icosahedral kaleidoscopic IFS : reflect against the 3 great-circle
//	planes of the icosahedron whose normals encode the golden ratio
//	phi = 1.61803399 (c102's Power). After scaling, the union of
//	reflections approximates a dodecahedron's surface. Reference :
//	Knighty's "icosahedral kIFS" recipes, Fractal Forums circa 2010.
static inline float de_dodecahedron_( float3 p, float Power )
{
    //	Plane normals derived from phi.
    float3 n1 = normalize( float3(  Power, 1.0, 0.0 ) );
    float3 n2 = normalize( float3( -Power, 1.0, 0.0 ) );
    float3 n3 = normalize( float3(  0.0, Power, 1.0 ) );
    for( int i = 0; i < DE_ITER_; ++i )
    {
        p  = fabs( p );
        p -= 2.0 * min( 0.0, dot( p, n1 ) ) * n1;
        p -= 2.0 * min( 0.0, dot( p, n2 ) ) * n2;
        p -= 2.0 * min( 0.0, dot( p, n3 ) ) * n3;
        p  = p * 2.0 - float3( Power, Power - 1.0, Power - 1.0 );
    }
    return ( length( p ) - 1.5 ) * pow( 2.0, -float( DE_ITER_ ) );
}

//	---------------- DE 6 : Knot --------------------------------------
//	Trefoil-like distance estimate : take p, fold into a torus around
//	the z-axis (radius from xy plane to circle of radius R), then
//	wind the parametric angle by Power and re-displace -- gives the
//	characteristic interlocking-tube knot signature. Reference :
//	Iquilez `sdTorus` + angle-wind technique used in the c102 source.
static inline float de_knot_( float3 p, float Power )
{
    constexpr float R = 1.0;     // major radius
    constexpr float r = 0.25;    // minor radius
    float ang  = atan2( p.y, p.x ) * Power;
    float rad  = length( p.xy ) - R;
    //	After winding, fold the (rad, p.z) plane by the wound angle.
    float c    = cos( ang );
    float s    = sin( ang );
    float2 q   = float2( rad * c - p.z * s, rad * s + p.z * c );
    return length( q ) - r;
}

//	---------------- DE 7 : Quaternion --------------------------------
//	Quaternion Julia : lift p to a 4D quaternion (zw component fixed),
//	iterate q = q*q + c with Hamilton product, escape-distance via
//	Hubbard-Douady. Power treated as the Julia constant scale
//	(c102 default 0.16 places the Julia in a recognizable basin).
//	Reference : Mandelbulber `Quaternion4D` formula + Hart/Sandin/Kauffman
//	original 1989 SIGGRAPH paper "Ray tracing deterministic 3-D fractals".
static inline float de_quaternion_( float3 p, float Power )
{
    float4 z = float4( p, 0.0 );
    float4 c = float4( Power, Power, Power, Power );
    float  md2 = 1.0;
    float  mz2 = dot( z, z );
    for( int i = 0; i < DE_ITER_; ++i )
    {
        md2 *= 4.0 * mz2;
        //	Hamilton-square : (a + bi + cj + dk)^2 expanded.
        z = float4( z.x * z.x - dot( z.yzw, z.yzw ),
                    2.0 * z.x * z.y,
                    2.0 * z.x * z.z,
                    2.0 * z.x * z.w ) + c;
        mz2 = dot( z, z );
        if( mz2 > 4.0 ) break;
    }
    return 0.25 * sqrt( mz2 / max( md2, 1e-6 ) ) * log( max( mz2, 1e-6 ) );
}

//	---------------- scene_() : 8-way DE dispatch ----------------------
//	Replaces the c102 stub's `length(p) - 1.0`. Each de_type routes to
//	the matching variant above ; default falls back to the sphere DE
//	(keeps the regression #34 stub-shape addressable if de_type is OOB).
static inline float scene_( float3 p, int de_type, float Power )
{
    switch( de_type )
    {
        case 0: return de_cubefield_(   p, Power );
        case 1: return de_sponge_(      p, Power );
        case 2: return de_sierpinski_(  p, Power );
        case 3: return de_mandelbulb_(  p, Power );
        case 4: return de_mandelbox_(   p, Power );
        case 5: return de_dodecahedron_( p, Power );
        case 6: return de_knot_(        p, Power );
        case 7: return de_quaternion_(  p, Power );
        default: return length( p ) - 1.0;   // sphere fallback
    }
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

    //	Per-de_type parameter set (P5 demotion of file-scope `Power`/`Offset` ;
    //	values lifted verbatim from c102 `ifs.metal` switch -- see preserved
    //	stub for the matching dispatch).
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
