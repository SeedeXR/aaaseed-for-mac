// aaa_gol_real.metal -- c136-A : SEVENTH Path A revival.
//
// Two real 2D cellular automata sharing one shader, switchable via
// uniform : Conway's Game of Life (B3/S23 ; 2 states) and Brian's
// Brain (3 states ; alive -> dying -> dead -> alive iff 2 neighbours).
// Both use the 8-neighbour Moore neighbourhood with toroidal (wrap-
// around) topology. The historic GOL stub family in this catalog
// renders flat / passthrough textures with no neighbour-evaluation
// step ; this file is the canonical replacement with real
// cellular-automaton rules running per-fragment over a CPU-side
// ping-pong texture pair.
//
// Per Path A revival doctrine (memory/feedback_path_a_revival_pattern.md) :
//   - No existing catalog shader is modified. This file lives
//     alongside the catalog ; the c80 / c81 goldens of every
//     historic shader are preserved bit-exactly.
//   - The new file pairs with two new regression TESTs in
//     tests/regression/phase3_golden_frame_regression_test.cpp
//     (one golden-pixel-compare, one revival-vs-stub diff proof).
//   - Catalog floor in tests/unit/path_a_catalog_test.cpp bumps
//     164 -> 165 with a c136-A comment.
//
// Algorithm citations (inline at each rule) :
//   - Conway 1970 Scientific American October "Mathematical Games --
//     The fantastic combinations of John Conway's new solitaire game
//     LIFE" -- the original B3/S23 2-state automaton.
//   - Silverman 1991 "The Phantom Fish Tank -- Brian's Brain" Logo
//     Foundation -- 3-state automaton ; alive cell becomes dying ;
//     dying cell becomes dead ; dead cell becomes alive iff exactly
//     2 alive neighbours.
//   - Wolfram 2002 "A New Kind of Science" Ch. 4 -- CA classification
//     (class 1 fixed point, class 2 periodic, class 3 chaotic, class
//     4 complex). Conway is class 4 ; Brian's Brain is class 3.
//   - R-pentomino + glider seed patterns : Gardner 1970 / 1983
//     "Wheels, Life, and Other Mathematical Amusements" Ch. 20-22.
//
// Pattern history (AAASeed Mac port) :
//   c128-A : Path A revival #1, fxaa_lottes.metal.
//   c130-A : Path A revival #2, ifs_de_library.metal.
//   c132-A : Path A revival #3, lights_deferred_real.metal.
//   c133-B : Path A revival #4, aaa_cam_real.metal.
//   c134-B : Path A revival #5, aaa_material_pbr.metal.
//   c135-A : Path A revival #6, aaa_noise_real.metal.
//   c136-A : Path A revival #7, THIS FILE -- real Conway + Brian's
//            Brain cellular automata. Catalog floor 164 -> 165.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

//	Engine-ABI uniform structs. Same trio used by every Path A port.
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//	Fullscreen-triangle VS, identical in shape to aaa_noise_real / pbr.
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
//	Deterministic 16x16 seed stencil. Bit 0 = alive (Conway state 1 or
//	Brian's-Brain "alive"). Tiled 16x16 across the 256x256 output so
//	each macro tile starts identical -- determinism without any
//	per-frame randomness. The stencil packs three canonical seeds :
//	  - R-pentomino  (Gardner 1970) : the 5-cell methuselah that runs
//	    1103 steps in classic Life before stabilising.
//	  - Glider       (Gardner 1970) : the 5-cell c/4 diagonal
//	    spaceship -- the simplest non-stationary Life pattern.
//	  - Brian's-Brain "spaceship" (Silverman 1991) : 5-cell pattern
//	    that translates by (2,0) every 4 steps.
//	The three shapes are spatially separated within the 16x16 tile
//	so they evolve independently for at least a few iterations before
//	their light cones intersect under toroidal wrap.
//	-------------------------------------------------------------------
constant uchar pattern[ 16 * 16 ] = {
    //	Row 0..3 : R-pentomino in upper-left quadrant.
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    0,0,1,1,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    0,1,1,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    0,0,1,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    //	Row 4..7 : Glider in upper-right quadrant.
    0,0,0,0,  0,0,0,0,  0,0,1,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,  0,0,0,1,  0,0,0,0,
    0,0,0,0,  0,0,0,0,  0,1,1,1,  0,0,0,0,
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    //	Row 8..11 : Brian's-Brain spaceship in lower-left quadrant.
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    0,1,0,1,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    0,0,1,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    0,1,0,1,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    //	Row 12..15 : Two-cell "blinker seed" in lower-right quadrant.
    //	(Blinker oscillates period-2 under Conway, drifts under Brian.)
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,  0,0,0,0,  1,1,1,0,
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0
};

//	Seed-lookup : returns 1 (alive) or 0 (dead) at integer cell (cx, cy)
//	in the 256x256 grid, modulo the 16x16 tile. Identical across all
//	tiles -- deterministic and texture-free.
static inline int seed_alive_( int cx, int cy )
{
    int const tx = cx & 15;   // mod 16
    int const ty = cy & 15;
    return int( pattern[ ty * 16 + tx ] );
}

//	-------------------------------------------------------------------
//	Toroidal wrap for floating UVs. The texture sampler is configured
//	with the backend's default clamp-to-edge ; we wrap MANUALLY here
//	via fract(uv + 1) which is bit-equivalent to (uv mod 1) for both
//	positive and slightly-negative neighbour offsets at the grid edge.
//	The +1 guard absorbs UVs in [-1, 0) before the fract pass.
//	-------------------------------------------------------------------
static inline float2 wrap_uv_( float2 uv )
{
    return fract( uv + float2( 1.0, 1.0 ) );
}

//	Sample the previous-state texture at the neighbour cell offset
//	(dx, dy) in cell units. dx, dy are in [-1, +1]. Returns the
//	previous-state RGBA where R = alive (Conway), G = dying (Brian).
static inline float4 sample_neighbour_( texture2d< float > prev,
                                        sampler            samp,
                                        float2             uv,
                                        float2             cell_size,
                                        int                dx,
                                        int                dy )
{
    float2 const offset = float2( float( dx ), float( dy ) ) * cell_size;
    return prev.sample( samp, wrap_uv_( uv + offset ) );
}

//	-------------------------------------------------------------------
//	Fragment : one CA step against the previous-state texture, or seed
//	emission when iteration_count == 0.
//
//	Uniform slot layout (c136-A spec) :
//	   AaaFuInts[ 0 ] = mode  : 0 = stub (flat mid-grey), 1 = real CA.
//	   AaaFuInts[ 1 ] = variant : 0 = Conway B3/S23, 1 = Brian's Brain.
//	   AaaFuInts[ 2 ] = iteration index : 0 = emit seed pattern,
//	                    >0 = step previous-state texture once.
//
//	Output channels :
//	   R  : Conway alive flag (or Brian's-Brain alive state). 255 = alive.
//	   G  : Brian's-Brain "dying" state. 255 = dying. Always 0 in Conway.
//	   B  : generation counter mod 256 -- visual age indicator, also
//	        makes the stub-vs-real diff visible even where the cell
//	        states accidentally match the mid-grey of the stub branch.
//	   A  : opaque 255.
//	-------------------------------------------------------------------
fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    prev_state    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    (void) _aaa_floats;
    (void) _aaa_vec4s;

    int const mode      = _aaa_ints.values[ 0 ];
    int const variant   = _aaa_ints.values[ 1 ];
    int const iter_idx  = _aaa_ints.values[ 2 ];

    //	---------- Stub branch : flat mid-grey -----------------------
    //	The historic GOL stub family writes a passthrough / flat
    //	colour with no neighbour evaluation. Mid-grey 0.5 is the
    //	regression-comparable stand-in -- the RevivalProof test
    //	relies on the real CA diverging from this flat field.
    if( mode == 0 )
        return float4( 0.5, 0.5, 0.5, 1.0 );

    //	---------- Real CA branch ------------------------------------

    //	Cell coordinate from fragment [[position]]. The viewport is
    //	256x256 ; cell pitch in UV space is 1/256.
    float2 const uv         = in.uv;
    float2 const cell_size  = float2( 1.0 / 256.0, 1.0 / 256.0 );
    int    const cx         = int( floor( uv.x * 256.0 ) );
    int    const cy         = int( floor( uv.y * 256.0 ) );

    //	-------- Iteration 0 : emit the deterministic seed -----------
    //	The seed stencil (R-pentomino + glider + Brian's spaceship)
    //	tiles 16x16 across the 256x256 grid. Stub-vs-real divergence
    //	is already large at iter 0 because the stub is flat mid-grey
    //	while the seed is mostly dead (R=0) with sparse alive cells.
    if( iter_idx == 0 )
    {
        int const alive = seed_alive_( cx, cy );
        return float4(
            float( alive ),     // R = alive in Conway / Brian
            0.0,                // G = dying (always 0 at seed time)
            0.0,                // B = generation = 0
            1.0 );
    }

    //	-------- Iteration N (N >= 1) : step the rule ----------------
    //	Sample the previous-state texture at the 9 cells (self + 8
    //	Moore neighbours) and count alive neighbours. UV wrap is
    //	manual via wrap_uv_ -> bit-equivalent toroidal topology even
    //	though the sampler is clamp-to-edge.
    float4 const self = prev_state.sample( _aaa_samp, uv );

    //	Count of alive (R > 0.5) neighbours. Branch-free sum.
    int n_alive = 0;
    for( int dy = -1; dy <= 1; ++dy )
    {
        for( int dx = -1; dx <= 1; ++dx )
        {
            if( dx == 0 && dy == 0 ) continue;
            float4 const nb = sample_neighbour_( prev_state, _aaa_samp,
                                                 uv, cell_size, dx, dy );
            if( nb.r > 0.5 ) n_alive++;
        }
    }

    int const self_alive = ( self.r > 0.5 ) ? 1 : 0;
    int const self_dying = ( self.g > 0.5 ) ? 1 : 0;

    //	Generation counter : previous B channel + 1, mod 256.
    //	The +1/255 wrap keeps the channel byte-precise after RGBA8
    //	round-tripping (B is 0..255 in storage ; we encode as 0..1).
    float const gen_prev = self.b;
    float const gen_next = fract( gen_prev + ( 1.0 / 255.0 ) );

    int next_alive = 0;
    int next_dying = 0;

    if( variant == 0 )
    {
        //	------- Conway B3/S23 (Conway 1970 Scientific American) --
        //	Born when exactly 3 alive neighbours.
        //	Survives when alive and (2 or 3) alive neighbours.
        //	Dies otherwise.
        if( self_alive == 1 )
            next_alive = ( n_alive == 2 || n_alive == 3 ) ? 1 : 0;
        else
            next_alive = ( n_alive == 3 ) ? 1 : 0;
        next_dying = 0;   // Conway is 2-state ; G channel always 0.
    }
    else
    {
        //	------- Brian's Brain (Silverman 1991) -------------------
        //	Three states encoded as : alive (R=1, G=0),
        //	dying (R=0, G=1), dead (R=0, G=0).
        //	Rules :
        //	  alive  -> dying  (unconditional)
        //	  dying  -> dead   (unconditional)
        //	  dead   -> alive  iff exactly 2 alive neighbours
        //	                   (Silverman's "B2" birth rule).
        if( self_alive == 1 )
        {
            next_alive = 0;
            next_dying = 1;
        }
        else if( self_dying == 1 )
        {
            next_alive = 0;
            next_dying = 0;
        }
        else
        {
            //	dead -> alive iff exactly 2 alive neighbours.
            next_alive = ( n_alive == 2 ) ? 1 : 0;
            next_dying = 0;
        }
    }

    return float4(
        float( next_alive ),    // R
        float( next_dying ),    // G
        gen_next,               // B = generation mod 256
        1.0 );                  // A = opaque
}
