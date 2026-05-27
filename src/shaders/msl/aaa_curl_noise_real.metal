// aaa_curl_noise_real.metal -- c137-A : EIGHTH Path A revival.
//
// Real curl-noise velocity field : a divergence-free 3D flow derived
// from the curl of a 3D vector potential whose components are
// independent samples of c135-A's Perlin 3D gradient noise. Standard
// in fluid-simulation literature ; the curl operator guarantees
// div(curl(psi)) == 0 identically, so the resulting velocity field
// is incompressible without any pressure-projection step.
//
// Per Path A revival doctrine (memory/feedback_path_a_revival_pattern.md) :
//   - No existing catalog shader is modified. This file lives
//     alongside the catalog ; the c80 / c81 / c135-A goldens of
//     every historic shader (including aaa_noise_real.metal which
//     this file's noise math is COPIED from, not refactored to
//     share) remain bit-exactly valid.
//   - MSL does not share translation units the way GLSL's #include
//     model does ; each .metal is a self-contained module. So the
//     canonical Perlin permutation table + perlin3_ helpers below
//     are byte-equivalent COPIES of the c135-A originals, not
//     references. This preserves c135-A's golden by leaving its
//     source file untouched.
//   - The new file pairs with two new regression TESTs in
//     tests/regression/phase3_golden_frame_regression_test.cpp.
//   - Catalog floor in tests/unit/path_a_catalog_test.cpp bumps
//     165 -> 166 with a c137-A comment.
//
// Algorithm citations :
//   - Bridson, Hourihan, Nordenstam 2007 ACM SCA "Curl-Noise for
//     Procedural Fluid Flow" -- canonical reference ; defines the
//     vector-potential -> curl -> velocity construction with
//     finite-difference derivatives on 3D noise samples.
//   - Bridson + Mueller-Fischer 2007 SIGGRAPH Course Notes "Fluid
//     Simulation" Ch. 14 -- exposition of the curl-noise idea in
//     the broader fluids course.
//   - Perlin 2001 SIGGRAPH Course Notes "Noise Hardware" + Perlin
//     2002 SIGGRAPH "Improving Noise" -- the underlying 3D gradient
//     noise sampled to build the potential field psi.
//   - Stam 1999 SIGGRAPH "Stable Fluids" -- RK2 (mid-point) semi-
//     Lagrangian advection used in the bottom-half particle quadrants.
//
// Pattern history (AAASeed Mac port) :
//   c128-A : Path A revival #1, fxaa_lottes.metal (Lottes 3.11).
//   c130-A : Path A revival #2, ifs_de_library.metal (8-variant DE).
//   c132-A : Path A revival #3, lights_deferred_real.metal.
//   c133-B : Path A revival #4, aaa_cam_real.metal.
//   c134-B : Path A revival #5, aaa_material_pbr.metal.
//   c135-A : Path A revival #6, aaa_noise_real.metal.
//   c136-A : Path A revival #7, aaa_gol_real.metal.
//   c137-A : Path A revival #8, THIS FILE -- real curl noise
//            (divergence-free velocity field from curl of Perlin
//            noise) plus 50/100-step RK2 advection visualisation.
//            Catalog floor 165 -> 166.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

//	Engine-ABI uniform structs. Same trio used by every Path A port.
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//	Fullscreen-triangle VS, identical in shape to aaa_noise_real /
//	aaa_gol_real / aaa_material_pbr.
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
//	BYTE-EQUIVALENT COPY of the table in aaa_noise_real.metal (c135-A).
//	MSL does not share translation units, and the doctrine says don't
//	touch the c135-A source to preserve its golden -- so we duplicate
//	the verbatim Perlin 2002 appendix table. Same permutation, doubled
//	to 512 entries for wrap-free indexing.
//	Source : Perlin 2002 SIGGRAPH "Improving Noise" appendix +
//	         http://mrl.cs.nyu.edu/~perlin/noise/
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
//	Perlin 2002 quintic fade : fade(t) = 6t^5 - 15t^4 + 10t^3.
//	C2-continuous interpolant. Copy of c135-A fade_ .
//	-------------------------------------------------------------------
static inline float fade_( float t )
{
    return t * t * t * ( t * ( t * 6.0 - 15.0 ) + 10.0 );
}

//	-------------------------------------------------------------------
//	Perlin 2002 16-direction gradient. Copy of c135-A grad_ .
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
//	Perlin classic 3D noise. Byte-equivalent COPY of c135-A perlin3_ .
//	Output nominally in [-1, +1] (typical Perlin range).
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
//	Vector potential psi : float3 whose components are INDEPENDENT
//	Perlin 3D noise samples offset by large vectors so the three
//	scalar fields are uncorrelated. Bridson 2007 SCA s. 3 :
//	"each component of psi is a separate scalar Perlin noise function
//	 (offset in input space so they are independent samples of the
//	  same noise)."
//	Offsets are arbitrary large irrationals chosen to push the input
//	domain across many lattice cells -- ensures the three components
//	sample uncorrelated regions of the noise.
//	-------------------------------------------------------------------
static inline float3 potential_( float3 p )
{
    float3 const off_y = float3( 31.416, 47.853, 17.293 );
    float3 const off_z = float3( 83.155, 67.219, 29.671 );
    float const psi_x = perlin3_( p              );
    float const psi_y = perlin3_( p + off_y      );
    float const psi_z = perlin3_( p + off_z      );
    return float3( psi_x, psi_y, psi_z );
}

//	-------------------------------------------------------------------
//	Curl of the vector potential via central finite differences.
//	Bridson 2007 SCA eq. (4) :
//	    vel = curl(psi) = ( dpsi_z/dy - dpsi_y/dz ,
//	                        dpsi_x/dz - dpsi_z/dx ,
//	                        dpsi_y/dx - dpsi_x/dy )
//	With eps = 1e-3 the central-difference 2-sample stencil
//	    f'(p) ~= ( f(p + eps) - f(p - eps) ) / ( 2 eps )
//	gives smooth velocities that are divergence-free to within
//	the FP truncation of the finite-difference derivative.
//	Total : 12 noise samples (6 component fns * 2 eps offsets).
//	The composition is identically incompressible because
//	div(curl(F)) == 0 for any C2 vector field F .
//	-------------------------------------------------------------------
static float3 curl_( float3 p )
{
    float const eps   = 0.001;
    float const inv2e = 1.0 / ( 2.0 * eps );

    //	vel.x = (psi.z(p + eps_y) - psi.z(p - eps_y) - psi.y(p + eps_z) + psi.y(p - eps_z)) / (2 eps)
    float const psi_z_yp = potential_( p + float3( 0.0, eps, 0.0 ) ).z;
    float const psi_z_yn = potential_( p - float3( 0.0, eps, 0.0 ) ).z;
    float const psi_y_zp = potential_( p + float3( 0.0, 0.0, eps ) ).y;
    float const psi_y_zn = potential_( p - float3( 0.0, 0.0, eps ) ).y;
    float const vx       = ( psi_z_yp - psi_z_yn - psi_y_zp + psi_y_zn ) * inv2e;

    //	vel.y = (psi.x(p + eps_z) - psi.x(p - eps_z) - psi.z(p + eps_x) + psi.z(p - eps_x)) / (2 eps)
    float const psi_x_zp = potential_( p + float3( 0.0, 0.0, eps ) ).x;
    float const psi_x_zn = potential_( p - float3( 0.0, 0.0, eps ) ).x;
    float const psi_z_xp = potential_( p + float3( eps, 0.0, 0.0 ) ).z;
    float const psi_z_xn = potential_( p - float3( eps, 0.0, 0.0 ) ).z;
    float const vy       = ( psi_x_zp - psi_x_zn - psi_z_xp + psi_z_xn ) * inv2e;

    //	vel.z = (psi.y(p + eps_x) - psi.y(p - eps_x) - psi.x(p + eps_y) + psi.x(p - eps_y)) / (2 eps)
    float const psi_y_xp = potential_( p + float3( eps, 0.0, 0.0 ) ).y;
    float const psi_y_xn = potential_( p - float3( eps, 0.0, 0.0 ) ).y;
    float const psi_x_yp = potential_( p + float3( 0.0, eps, 0.0 ) ).x;
    float const psi_x_yn = potential_( p - float3( 0.0, eps, 0.0 ) ).x;
    float const vz       = ( psi_y_xp - psi_y_xn - psi_x_yp + psi_x_yn ) * inv2e;

    return float3( vx, vy, vz );
}

//	-------------------------------------------------------------------
//	RK2 (mid-point method) semi-Lagrangian advection step.
//	    k1   = vel(p)
//	    p_m  = p + dt/2 * k1
//	    k2   = vel(p_m)
//	    p'   = p + dt * k2
//	Stam 1999 "Stable Fluids" : RK2 is the cheapest stable choice ;
//	improves over forward-Euler by one order. dt is small (0.01) so
//	even pure forward-Euler would be stable, but RK2 reduces phase
//	error which makes the trail "stretch" look correct under longer
//	step counts (the bottom-right quadrant at 100 steps).
//	-------------------------------------------------------------------
static inline float3 advect_step_( float3 p, float dt )
{
    float3 const k1 = curl_( p );
    float3 const pm = p + 0.5 * dt * k1;
    float3 const k2 = curl_( pm );
    return p + dt * k2;
}

//	Advect a starting position N steps and return the final position.
//	Used by the bottom-half quadrants which render the *displacement*
//	from start as brightness -- a longer trail means a stronger
//	transport by the curl field. Bridson 2007 mentions advected-
//	particle visualisation as the canonical way to communicate
//	"this is a real, structured velocity field, not random noise".
static float3 advect_n_( float3 p0, int N, float dt )
{
    float3 p = p0;
    for( int i = 0; i < N; ++i )
        p = advect_step_( p, dt );
    return p;
}

//	-------------------------------------------------------------------
//	Fragment : 4-quadrant 256x256 layout, deterministic across runs.
//
//	Per c137-A spec :
//	  Top-left  (u in [0,0.5], v in [0,0.5]) :
//	      velocity magnitude |vel| as greyscale. Visualises flow
//	      strength : bright bands = fast streams, dark = stagnation.
//	  Top-right (u in [0.5,1], v in [0,0.5]) :
//	      velocity direction as RGB (vel * 0.5 + 0.5). Visualises
//	      flow orientation : R/G/B channels = x/y/z velocity components
//	      mapped to [0,1] for display.
//	  Bot-left  (u in [0,0.5], v in [0.5,1]) :
//	      50-step RK2 advection from a uniform start grid ; brightness
//	      proportional to |p_final - p_start| (transport magnitude).
//	  Bot-right (u in [0.5,1], v in [0.5,1]) :
//	      100-step RK2 advection ; longer trails. Same brightness
//	      mapping, so the diff between BL and BR shows how the field
//	      stretches particles over time.
//
//	Uniform slot layout (c137-A spec) :
//	   AaaFuInts[ 0 ] = mode (0 = stub-passthrough , 1 = real curl noise)
//	   AaaFuInts[ 1 ] = overlay mode :
//	                       0 = 4-quadrant composite (default for golden)
//	                       1 = velocity magnitude across whole frame
//	                       2 = velocity direction RGB across whole frame
//	   AaaFuInts[ 2 ] = advection step count override :
//	                       0 = use defaults (50 for BL, 100 for BR)
//	                       N (>0) = use N steps for the advected quadrants
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
        //	Stub branch : flat mid-grey 0.5, matching the c135-A
        //	stub-branch convention. The historic GOL-stub family also
        //	collapses to a passthrough flat field with no real velocity
        //	computation ; mid-grey is the regression-comparable stand-in.
        //	The RevivalProof test depends on the real curl field
        //	diverging from this flat colour.
        return float4( 0.5, 0.5, 0.5, 1.0 );
    }

    int const overlay  = _aaa_ints.values[ 1 ];
    int const adv_over = _aaa_ints.values[ 2 ];

    //	Step counts for the bottom-half advected quadrants. If the
    //	caller sets AaaFuInts[2] > 0, BOTH quadrants use that count.
    //	Default (override = 0) : BL = 50 , BR = 100 (per c137-A spec).
    int const adv_n_left  = ( adv_over > 0 ) ? adv_over : 50;
    int const adv_n_right = ( adv_over > 0 ) ? adv_over : 100;
    float const dt        = 0.01;

    float2 const uv = in.uv;

    //	Whole-frame overlays (overlay 1, 2) skip the quadrant split
    //	and visualise a single channel of the curl field across the
    //	entire 256x256 frame. These are convenience modes for callers
    //	that want a single-purpose view (e.g., debug velocity arrows
    //	in CPU-side overlay layer).
    if( overlay == 1 )
    {
        //	Whole frame = velocity magnitude.
        float3 const p   = float3( uv * 3.0, 0.5 );
        float3 const vel = curl_( p );
        float const  g   = saturate( length( vel ) * 0.35 );
        return float4( g, g, g, 1.0 );
    }
    if( overlay == 2 )
    {
        //	Whole frame = velocity direction (vel * 0.5 + 0.5).
        float3 const p   = float3( uv * 3.0, 0.5 );
        float3 const vel = curl_( p );
        float3 const rgb = saturate( vel * 0.5 + float3( 0.5 ) );
        return float4( rgb, 1.0 );
    }

    //	Default overlay 0 : 4-quadrant composite.
    //	Each quadrant maps its local UV (uv_q in [0,1]^2) to a 3D
    //	sample point with its own z-slice so the four quadrants
    //	view independent slabs of the volumetric noise.
    if( uv.y < 0.5 )
    {
        //	---- Top half : direct curl-field visualisation -----------
        if( uv.x < 0.5 )
        {
            //	Top-left : velocity magnitude as greyscale.
            //	Scale 3.0 -> ~3 noise cells across the quadrant ;
            //	z-slice 0.25 distinguishes from the other 3 quadrants.
            //	0.35 multiplier on |vel| empirically maps the curl
            //	field's typical magnitude (~1..3) into a visible
            //	[0..1] greyscale range without saturating.
            float2 const uv_q = uv * 2.0;                       // [0,1] x [0,1]
            float3 const p    = float3( uv_q * 3.0, 0.25 );
            float3 const vel  = curl_( p );
            float const  g    = saturate( length( vel ) * 0.35 );
            return float4( g, g, g, 1.0 );
        }
        else
        {
            //	Top-right : velocity direction as RGB.
            //	Each component of vel is remapped from [-1,+1] to
            //	[0,1] (the `* 0.5 + 0.5` standard direction-to-colour
            //	convention used across the literature, e.g., normals).
            float2 const uv_q = ( uv - float2( 0.5, 0.0 ) ) * 2.0;
            float3 const p    = float3( uv_q * 3.0, 0.5 );
            float3 const vel  = curl_( p );
            float3 const rgb  = saturate( vel * 0.5 + float3( 0.5 ) );
            return float4( rgb, 1.0 );
        }
    }
    else
    {
        //	---- Bottom half : advected particle displacement --------
        //	The "particle" is the fragment's start position. We
        //	advect it N steps through the curl field via RK2 and
        //	colour by displacement magnitude. Long trails -> bright ;
        //	stagnation -> dark. Bridson 2007 SCA recommends this
        //	visualisation specifically because it makes the structure
        //	of the velocity field legible to the eye in a way that
        //	raw |vel| does not.
        if( uv.x < 0.5 )
        {
            //	Bottom-left : 50-step advection (medium trails).
            float2 const uv_q = ( uv - float2( 0.0, 0.5 ) ) * 2.0;
            float3 const p0   = float3( uv_q * 3.0, 0.75 );
            float3 const p1   = advect_n_( p0, adv_n_left, dt );
            float  const disp = length( p1 - p0 );
            //	Empirical scaling 1.5 brings 50-step displacement
            //	into a visible [0,1] greyscale range. Saturate
            //	clamps the brightest pixels.
            float const g     = saturate( disp * 1.5 );
            return float4( g, g, g, 1.0 );
        }
        else
        {
            //	Bottom-right : 100-step advection (longer trails).
            //	Same scaling family : disp * 0.85 because 100 steps
            //	tend to produce ~2x the displacement of 50 steps so
            //	we tone it down to maintain visible contrast across
            //	the quadrant rather than mostly-white saturation.
            float2 const uv_q = ( uv - float2( 0.5, 0.5 ) ) * 2.0;
            float3 const p0   = float3( uv_q * 3.0, 1.0 );
            float3 const p1   = advect_n_( p0, adv_n_right, dt );
            float  const disp = length( p1 - p0 );
            float const g     = saturate( disp * 0.85 );
            return float4( g, g, g, 1.0 );
        }
    }
}
