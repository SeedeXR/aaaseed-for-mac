// aaa_noise_real.metal -- c135-A : SIXTH Path A revival.
//
// Real Perlin (1985 / 2002) classic 3D noise + Simplex (Perlin 2001)
// 3D noise for the `aaa_noise` engine globals. The historic stubs
// (ps_Maa_noise.metal, ps_Maa_noise_cheap.metal) collapse to either
// a 3D-texture sample with a stubbed model-coordinate input (c80
// P4 stub : `MCposition = float3(0)`) or to a near-passthrough
// trivial-frac construction. Neither computes a real gradient noise
// function. This file is the canonical replacement : two classic
// gradient noises sharing Ken Perlin's permutation table, both at
// scalar AND fbm scales, in a 4-quadrant 256x256 image.
//
// Per Path A revival doctrine (memory/feedback_path_a_revival_pattern.md) :
//   - The original ps_Maa_noise{,_cheap}.metal stubs are PRESERVED
//     verbatim. This new file lives alongside the existing catalog ;
//     it does NOT modify any existing shader source.
//   - The c80 / c81 regression goldens of every shader that consumes
//     the noise stubs remain bit-exactly valid.
//   - This file is the canonical end-to-end demonstration of a real
//     Perlin + Simplex 3D gradient noise flowing through GOL::Backend
//     to MSL with the engine-ABI struct shape used by the rest of
//     the Path A catalog.
//
// Algorithm citations (inline below at each term) :
//   - Perlin 1985 SIGGRAPH "An Image Synthesizer" -- original 3D
//     gradient noise with integer-lattice hashing.
//   - Perlin 2002 SIGGRAPH "Improving Noise" -- the quintic
//     fade(t) = 6t^5 - 15t^4 + 10t^3 interpolant + 16-vector grad
//     table (replaces the random-unit-sphere of the 1985 paper).
//   - Perlin 2001 SIGGRAPH Course Notes "Noise Hardware" -- Simplex
//     noise (skewed simplicial lattice, 4 corners in 3D, radial
//     contribution kernel).
//   - Gustavson 2005 "Simplex noise demystified" -- clean reference
//     impl of Simplex 3D ; the F3 = 1/3 skew + G3 = 1/6 unskew +
//     0.6 - distSq radial kernel + grad12 table appear here.
//
// Pattern history (AAASeed Mac port) :
//   c128-A : Path A revival #1, fxaa_lottes.metal (Lottes 3.11).
//   c130-A : Path A revival #2, ifs_de_library.metal (8-variant DE).
//   c132-A : Path A revival #3, lights_deferred_real.metal
//            (real Lambert + Phong deferred light accumulation).
//   c133-B : Path A revival #4, aaa_cam_real.metal (real camera
//            constant block).
//   c134-B : Path A revival #5, aaa_material_pbr.metal (real
//            Cook-Torrance PBR).
//   c135-A : Path A revival #6, THIS FILE -- real Perlin + Simplex
//            3D gradient noise. Catalog floor 163 -> 164.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

//	Engine-ABI uniform structs. Match the trio used by every Path A
//	port (and the C++ side of the regression tests).
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//	Fullscreen-triangle VS, identical in shape to aaa_material_pbr.
//	The fragment stage owns the procedural noise from `uv`, so the
//	regression test only has to supply uniforms.
vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2(  0.0,  2.0 ), float2(  0.0, 0.0 ), float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

//	-------------------------------------------------------------------
//	Ken Perlin's canonical 256-entry permutation table.
//	Source : the appendix of Perlin 2002 SIGGRAPH "Improving Noise"
//	(and the widely-mirrored reference Java impl on Ken Perlin's NYU
//	page http://mrl.cs.nyu.edu/~perlin/noise/). Duplicated to 512
//	entries so `perm[i]` is wrap-free for any i in [0, 511] -- the
//	standard "doubled table" optimisation also from Perlin 2002.
//	-------------------------------------------------------------------
constant int perm[ 512 ] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
    140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,
    247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,
    57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
    74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,
    60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,
    65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,
    200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,
    52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,
    207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,
    119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
    129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,
    218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
    81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,
    184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,
    222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
    //	---- Duplicated copy ----
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
    140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,
    247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,
    57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
    74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,
    60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,
    65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,
    200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,
    52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,
    207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,
    119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
    129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,
    218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
    81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,
    184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,
    222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

//	-------------------------------------------------------------------
//	Perlin 2002 "Improving Noise" quintic fade.
//	    fade(t) = 6t^5 - 15t^4 + 10t^3
//	Strictly C2-continuous (first and second derivatives vanish at
//	t = 0 and t = 1). Replaces the C1-only cubic (3t^2 - 2t^3) of the
//	1985 paper -- the visible "directional artefacts" along lattice
//	axes that plagued Perlin 1985 noise come from the C1 cubic.
//	-------------------------------------------------------------------
static inline float fade_( float t )
{
    return t * t * t * ( t * ( t * 6.0 - 15.0 ) + 10.0 );
}

//	-------------------------------------------------------------------
//	Perlin 2002 "Improving Noise" 16-direction gradient.
//	Hash low 4 bits of h to select one of 16 unit-ish vectors over
//	the cube's edges + center face directions. (The actual set is 12
//	cube-edge directions plus 4 repeats -- Perlin's clever choice that
//	yields no diagonal bias while being branch-free on hardware.)
//	-------------------------------------------------------------------
static inline float grad_( int h, float x, float y, float z )
{
    int  const   hh = h & 15;
    float const  u  = ( hh < 8 ) ? x : y;
    float const  v  = ( hh < 4 ) ? y : ( ( hh == 12 || hh == 14 ) ? x : z );
    float const  uu = ( ( hh & 1 ) == 0 ) ? u : -u;
    float const  vv = ( ( hh & 2 ) == 0 ) ? v : -v;
    return uu + vv;
}

//	-------------------------------------------------------------------
//	Perlin classic 3D noise, output in [-1, +1] (typical).
//	    1. Find lattice cube containing (x, y, z) and offsets.
//	    2. Compute fade(u), fade(v), fade(w).
//	    3. Hash 8 corners via the doubled permutation table.
//	    4. Trilinear-blend grad . offset with the fade weights.
//	Perlin 1985 + 2002.
//	-------------------------------------------------------------------
static float perlin3_( float3 p )
{
    int const X = int( floor( p.x ) ) & 255;
    int const Y = int( floor( p.y ) ) & 255;
    int const Z = int( floor( p.z ) ) & 255;
    float const x = p.x - floor( p.x );
    float const y = p.y - floor( p.y );
    float const z = p.z - floor( p.z );

    float const u = fade_( x );
    float const v = fade_( y );
    float const w = fade_( z );

    int const A  = perm[ X     ] + Y;
    int const AA = perm[ A     ] + Z;
    int const AB = perm[ A + 1 ] + Z;
    int const B  = perm[ X + 1 ] + Y;
    int const BA = perm[ B     ] + Z;
    int const BB = perm[ B + 1 ] + Z;

    float const n000 = grad_( perm[ AA     ], x      , y      , z       );
    float const n100 = grad_( perm[ BA     ], x - 1.0, y      , z       );
    float const n010 = grad_( perm[ AB     ], x      , y - 1.0, z       );
    float const n110 = grad_( perm[ BB     ], x - 1.0, y - 1.0, z       );
    float const n001 = grad_( perm[ AA + 1 ], x      , y      , z - 1.0 );
    float const n101 = grad_( perm[ BA + 1 ], x - 1.0, y      , z - 1.0 );
    float const n011 = grad_( perm[ AB + 1 ], x      , y - 1.0, z - 1.0 );
    float const n111 = grad_( perm[ BB + 1 ], x - 1.0, y - 1.0, z - 1.0 );

    float const ix0 = mix( n000, n100, u );
    float const ix1 = mix( n010, n110, u );
    float const ix2 = mix( n001, n101, u );
    float const ix3 = mix( n011, n111, u );
    float const iy0 = mix( ix0, ix1, v );
    float const iy1 = mix( ix2, ix3, v );
    return mix( iy0, iy1, w );
}

//	-------------------------------------------------------------------
//	Simplex 3D : 12-direction gradient.
//	Gustavson 2005 "Simplex noise demystified" : the 12 mid-edges of
//	a unit cube. Hash mod 12 to pick the gradient. Avoids the
//	axis-aligned bias of an all-corner hash.
//	-------------------------------------------------------------------
constant float3 grad12_table[ 12 ] = {
    float3(  1, 1, 0 ), float3( -1,  1, 0 ), float3(  1, -1,  0 ), float3( -1, -1,  0 ),
    float3(  1, 0, 1 ), float3( -1,  0, 1 ), float3(  1,  0, -1 ), float3( -1,  0, -1 ),
    float3(  0, 1, 1 ), float3(  0, -1, 1 ), float3(  0,  1, -1 ), float3(  0, -1, -1 )
};

static inline float grad12_dot_( int h, float3 d )
{
    return dot( grad12_table[ h % 12 ], d );
}

//	-------------------------------------------------------------------
//	Simplex 3D noise, output range nominally [-1, +1].
//	Perlin 2001 SIGGRAPH Course Notes ; Gustavson 2005 reference impl.
//	    Skew factor : F3 = 1 / 3   (skews input to unit-simplex space).
//	    Unskew     : G3 = 1 / 6   (unskews back to unit-cube space).
//	    Corner kernel : t = 0.6 - dot(d, d) ; if t > 0 then t^4 * grad.d.
//	    Scale factor 32 empirically chosen to bring output into [-1,+1].
//	-------------------------------------------------------------------
static float simplex3_( float3 p )
{
    float const F3 = 1.0 / 3.0;
    float const G3 = 1.0 / 6.0;

    float const s   = ( p.x + p.y + p.z ) * F3;
    int   const i   = int( floor( p.x + s ) );
    int   const j   = int( floor( p.y + s ) );
    int   const k   = int( floor( p.z + s ) );

    float const tt  = float( i + j + k ) * G3;
    float const X0  = float( i ) - tt;
    float const Y0  = float( j ) - tt;
    float const Z0  = float( k ) - tt;
    float3 const d0 = float3( p.x - X0, p.y - Y0, p.z - Z0 );

    //	Determine which simplex (tetrahedron) we are in. There are
    //	six orderings of (d0.x, d0.y, d0.z) ; each picks a different
    //	pair of (i1, j1, k1) , (i2, j2, k2) corner offsets.
    int i1, j1, k1, i2, j2, k2;
    if( d0.x >= d0.y )
    {
        if     ( d0.y >= d0.z ) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; }
        else if( d0.x >= d0.z ) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; }
        else                    { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; }
    }
    else
    {
        if     ( d0.y <  d0.z ) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; }
        else if( d0.x <  d0.z ) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; }
        else                    { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; }
    }

    float3 const d1 = d0 - float3( float( i1 ), float( j1 ), float( k1 ) ) + float3( G3 );
    float3 const d2 = d0 - float3( float( i2 ), float( j2 ), float( k2 ) ) + float3( 2.0 * G3 );
    float3 const d3 = d0 - float3( 1.0, 1.0, 1.0 )                          + float3( 3.0 * G3 );

    int const ii = i & 255;
    int const jj = j & 255;
    int const kk = k & 255;
    int const gi0 = perm[ ii      + perm[ jj      + perm[ kk      ] ] ];
    int const gi1 = perm[ ii + i1 + perm[ jj + j1 + perm[ kk + k1 ] ] ];
    int const gi2 = perm[ ii + i2 + perm[ jj + j2 + perm[ kk + k2 ] ] ];
    int const gi3 = perm[ ii + 1  + perm[ jj + 1  + perm[ kk + 1  ] ] ];

    float n0 = 0.0, n1 = 0.0, n2 = 0.0, n3 = 0.0;
    float t0 = 0.6 - dot( d0, d0 );
    if( t0 > 0.0 ) { t0 *= t0; n0 = t0 * t0 * grad12_dot_( gi0, d0 ); }
    float t1 = 0.6 - dot( d1, d1 );
    if( t1 > 0.0 ) { t1 *= t1; n1 = t1 * t1 * grad12_dot_( gi1, d1 ); }
    float t2 = 0.6 - dot( d2, d2 );
    if( t2 > 0.0 ) { t2 *= t2; n2 = t2 * t2 * grad12_dot_( gi2, d2 ); }
    float t3 = 0.6 - dot( d3, d3 );
    if( t3 > 0.0 ) { t3 *= t3; n3 = t3 * t3 * grad12_dot_( gi3, d3 ); }

    return 32.0 * ( n0 + n1 + n2 + n3 );
}

//	-------------------------------------------------------------------
//	Five-octave fbm sums for both noises. Standard 2x lacunarity +
//	0.5 gain per octave (Musgrave 1989 ; recap in Real-Time Rendering
//	4th ed. Sec 14.6.2). Output rescaled to roughly [-1, +1] by the
//	geometric series 1 + 1/2 + 1/4 + 1/8 + 1/16 ~= 1.9375.
//	-------------------------------------------------------------------
static float perlin_fbm5_( float3 p )
{
    float sum = 0.0, amp = 1.0, freq = 1.0;
    for( int o = 0; o < 5; ++o )
    {
        sum  += amp * perlin3_( p * freq );
        freq *= 2.0;
        amp  *= 0.5;
    }
    return sum / 1.9375;
}

static float simplex_fbm5_( float3 p )
{
    float sum = 0.0, amp = 1.0, freq = 1.0;
    for( int o = 0; o < 5; ++o )
    {
        sum  += amp * simplex3_( p * freq );
        freq *= 2.0;
        amp  *= 0.5;
    }
    return sum / 1.9375;
}

//	-------------------------------------------------------------------
//	Fragment : 4-quadrant 256x256 layout, deterministic across runs.
//	  Top-left  ( u in [0,0.5] , v in [0,0.5] ) : Perlin scalar @ 4.0
//	  Top-right ( u in [0.5,1] , v in [0,0.5] ) : Simplex scalar @ 4.0
//	  Bot-left  ( u in [0,0.5] , v in [0.5,1] ) : Perlin fbm5  @ 2.0
//	  Bot-right ( u in [0.5,1] , v in [0.5,1] ) : Simplex fbm5 @ 2.0
//	Greyscale ; each channel = noise mapped from [-1,+1] to [0,1].
//
//	Uniform slot layout (c135-A spec) :
//	   AaaFuInts[ 0 ] = mode (0 = stub-passthrough , 1 = real noise)
//	The stub-passthrough path returns a flat mid-grey so the
//	RevivalProof_DiffersFromStub test sees the 4-quadrant noise
//	signal as a strong divergence.
//	-------------------------------------------------------------------
fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]] )
{
    (void) _aaa_floats;
    (void) _aaa_vec4s;

    int const mode = _aaa_ints.values[ 0 ];
    if( mode == 0 )
    {
        //	Stub-passthrough : the historic ps_Maa_noise.metal with its
        //	c80 stub `MCposition = float3(0)` collapses to a constant
        //	noise sample -- effectively a flat colour across the frame.
        //	Mid-grey 0.5 is the regression-comparable stand-in.
        return float4( 0.5, 0.5, 0.5, 1.0 );
    }

    //	Real noise. Split the unit square into the 4 quadrants.
    float2 const uv = in.uv;
    float        scalar = 0.0;

    if( uv.y < 0.5 )
    {
        //	Top half = scalar noise. Scale 4.0 -> ~4 cells across the
        //	half-frame. Use a z-slice driven by quadrant index so
        //	Perlin and Simplex sample different slices and don't
        //	accidentally line up.
        if( uv.x < 0.5 )
        {
            float3 const p = float3( uv * 4.0, 0.25 );
            scalar = perlin3_( p );
        }
        else
        {
            float3 const p = float3( ( uv - float2( 0.5, 0.0 ) ) * 4.0, 0.75 );
            scalar = simplex3_( p );
        }
    }
    else
    {
        //	Bottom half = 5-octave fbm. Scale 2.0 base -> wider
        //	cells but the 5-octave sum brings back high-frequency
        //	detail. Different z-slices again.
        if( uv.x < 0.5 )
        {
            float3 const p = float3( float2( uv.x, uv.y - 0.5 ) * 2.0, 1.25 );
            scalar = perlin_fbm5_( p );
        }
        else
        {
            float3 const p = float3( ( uv - float2( 0.5, 0.5 ) ) * 2.0, 1.75 );
            scalar = simplex_fbm5_( p );
        }
    }

    //	Map [-1, +1] -> [0, 1] for display. Clamp catches the
    //	rare Simplex out-of-range pixel near the 0.6-radius kernel
    //	boundary.
    float const g = saturate( scalar * 0.5 + 0.5 );
    return float4( g, g, g, 1.0 );
}
