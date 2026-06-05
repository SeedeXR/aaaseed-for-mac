// aaa_particle_portal.metal -- c157 : procedural GPU particle portal.
//
// 40,960 small white particles (40 radial bands x 128 angular cells x 8
// particles per cell) forming a circular portal / energy-field flow on a
// black background. Authored for the MEU runner's Lua surface : a script
// drives weight / push / pull / acceleration / origin every frame (see
// bundle/macos/meu/Samples/particle_portal/particle_portal.lua).
//
// Why procedural (no particle buffer) : the MEU runner draws fullscreen
// triangles only, and 40k stateful particles cannot be stepped in vanilla
// Lua at 60 fps. Instead every particle's position is a CLOSED-FORM
// function of (id, t) evaluated in the fragment stage, using the classic
// spatial-hash trick : particles are owned by polar grid cells, each
// pixel evaluates only its neighbouring cells (5 bands x 3 cells x 8 =
// 120 gaussian sprites/pixel), so total particle count is decoupled from
// per-pixel cost. Per-band rotating frames keep fast swirl motion LOCAL
// to the owning cell (the pixel un-rotates before the cell lookup).
//
// Technique citations :
//   - Reeves 1983 SIGGRAPH "Particle Systems -- A Technique for Modeling
//     a Class of Fuzzy Objects" : stochastic per-particle attributes
//     (weight, phase) from a seed ; gaussian point sprites.
//   - The cell-hash procedural particle field is the standard
//     fragment-only formulation popularised on Shadertoy (e.g. iq's
//     "Voronoise"-style hashed grids) : derive particle state from
//     hash(cell_id) instead of streaming a buffer.
//
// Engine ABI (catalog trio -- floats@0 / vec4s@1 / ints@2) :
//   ints[0]    : mode. 1 = particle portal ; 0 = dim fallback ring.
//   floats[0]  : time (seconds).
//   vec4s[0]   : ( pull, push, accel, weight )    -- the four forces, 0..1
//                (accel usefully up to ~3). pull gathers the annulus into
//                a tight ring ; push is the radial breathing amplitude ;
//                accel multiplies band spin rate ; weight scales particle
//                mass (heavier = bigger/brighter but slower spin).
//   vec4s[1]   : ( origin_x, origin_y, portal_radius, particle_size )
//                in normalized uv units (size in uv units, ~0.002-0.01).
//   vec4s[2]   : ( brightness, aspect, spin_dir, unused )

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2(  0.0,  2.0 ), float2(  0.0, 0.0 ), float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

//	Grid dimensions : RINGS x SEGS x PER_CELL = 40 x 128 x 8 = 40,960.
constant int   RINGS    = 40;
constant int   SEGS     = 128;
constant int   PER_CELL = 8;
constant float TWO_PI   = 6.28318530718;

//	Classic fract-sin hash (one float in [0,1) per 3 integer ids + salt).
//	Quality is ample for sprite attributes.
static float hash3( float a, float b, float c, float salt )
{
    return fract( sin( a * 127.1 + b * 311.7 + c * 74.7 + salt * 53.13 )
                  * 43758.5453 );
}

fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]] )
{
    float const t      = _aaa_floats.values[ 0 ];
    int   const mode   = _aaa_ints.values[ 0 ];

    float const pull   = clamp( _aaa_vec4s.values[ 0 ].x, 0.0, 0.92 );
    float const push   = clamp( _aaa_vec4s.values[ 0 ].y, 0.0, 1.0 );
    float const accel  =        _aaa_vec4s.values[ 0 ].z;
    float const weight = clamp( _aaa_vec4s.values[ 0 ].w, 0.0, 1.0 );

    float2 const origin = _aaa_vec4s.values[ 1 ].xy;
    float  const R      = max( _aaa_vec4s.values[ 1 ].z, 1e-3 );
    float  const psize  = max( _aaa_vec4s.values[ 1 ].w, 5e-4 );

    float const brightness = max( _aaa_vec4s.values[ 2 ].x, 0.0 );
    float const aspect     = max( _aaa_vec4s.values[ 2 ].y, 1e-3 );
    float const spin_dir   = ( _aaa_vec4s.values[ 2 ].z < 0.0 ) ? -1.0 : 1.0;

    //	Pixel position relative to the portal origin, aspect-corrected so
    //	the portal is circular regardless of window shape.
    float2 p = in.uv - origin;
    p.x *= aspect;
    float const pr     = length( p );
    float const ptheta = atan2( p.y, p.x );

    if( mode == 0 )
    {
        //	Fallback : black with a faint static ring (proves the shader
        //	compiled + the ints binding works without the full field).
        float const d = fabs( pr - R );
        float const v = 0.15 * exp( -d * d / ( 2.0 * 0.01 * 0.01 ) );
        return float4( v, v, v, 1.0 );
    }

    //	Annulus the particles live in (before pull-gathering).
    float const r_min = R * 0.45;
    float const r_max = R * 1.55;
    float const band_w = ( r_max - r_min ) / float( RINGS );

    //	Pull gathers every home radius toward the portal ring R. To keep
    //	the pixel -> band lookup consistent, invert the same mapping for
    //	the pixel radius : a pixel at radius pr "came from" the un-pulled
    //	radius (pr - pull*R) / (1 - pull).
    float const inv_pr = ( pr - pull * R ) / ( 1.0 - pull );
    int   const center_band =
        int( floor( ( inv_pr - r_min ) / band_w ) );

    float acc = 0.0;

    //	5 radial bands x 3 angular cells x 8 particles = 120 sprites.
    for( int db = -2; db <= 2; ++db )
    {
        int const band = center_band + db;
        if( band < 0 || band >= RINGS ) continue;
        float const fb = float( band );

        //	Per-band spin : differential rotation (inner bands faster)
        //	with weight-scaled acceleration -- heavier fields spin up
        //	more slowly. Closed form ; angle wraps naturally.
        float const band_mass = mix( 0.55, 1.45, hash3( fb, 3.0, 7.0, 0.17 ) )
                              * mix( 0.6, 1.6, weight );
        float const omega = spin_dir
                          * ( 0.25 + 1.3 * ( 1.0 - fb / float( RINGS ) ) )
                          * ( 0.4 + accel * 1.6 ) / band_mass;
        float const spin = omega * t;

        //	Pixel angle in this band's rotating frame.
        float const theta_rot = ptheta - spin;
        float const cell_f    = ( theta_rot / TWO_PI ) * float( SEGS );
        int   const center_cell = int( floor( cell_f ) );

        for( int dc = -1; dc <= 1; ++dc )
        {
            //	Wrap the angular cell index.
            int cell = ( center_cell + dc ) % SEGS;
            if( cell < 0 ) cell += SEGS;
            float const fc = float( cell );

            for( int k = 0; k < PER_CELL; ++k )
            {
                float const fk = float( k );
                float const ha = hash3( fb, fc, fk, 0.31 );  //	angle jitter
                float const hr = hash3( fb, fc, fk, 0.53 );  //	radius jitter
                float const hw = hash3( fb, fc, fk, 0.71 );  //	per-particle mass
                float const hp = hash3( fb, fc, fk, 0.93 );  //	phase

                //	Home position (rotating frame, pre-pull).
                float const ang0   = ( ( fc + ha ) / float( SEGS ) ) * TWO_PI;
                float const base_r = r_min + ( fb + hr ) * band_w;

                //	Forces, closed form :
                //	pull  : gather toward the ring radius R.
                //	push  : radial breathing, per-particle phase.
                float const w_k    = mix( 0.4, 1.0, hw );
                float       r_home = mix( base_r, R, pull );
                r_home += push * 0.10 * R
                        * sin( t * ( 0.8 + 1.4 * w_k ) + hp * TWO_PI );
                //	Small tangential shimmer (energy-field crackle), kept
                //	below one cell width so locality holds.
                float const ang = ang0
                    + ( 0.35 / float( SEGS ) ) * TWO_PI
                      * sin( t * 1.7 + hp * 9.42 );

                //	Compare in the rotating frame.
                float2 const q  = r_home * float2( cos( ang ), sin( ang ) );
                float2 const px = pr * float2( cos( theta_rot ),
                                               sin( theta_rot ) );
                float const d2 = distance_squared( px, q );

                //	Gaussian point sprite ; heavier particles are larger
                //	and brighter (weight slider scales the whole field).
                float const sigma = psize * ( 0.55 + 0.9 * w_k )
                                  * ( 0.7 + 0.6 * weight );
                acc += w_k * exp( -d2 / ( 2.0 * sigma * sigma ) );
            }
        }
    }

    //	Soft additive tonemap : keeps single sprites crisp, lets dense
    //	regions bloom toward white without hard clipping.
    float const v = 1.0 - exp( -acc * brightness );
    return float4( v, v, v, 1.0 );
}
