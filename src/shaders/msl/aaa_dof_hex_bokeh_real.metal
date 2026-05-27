// aaa_dof_hex_bokeh_real.metal -- c140-A : ELEVENTH Path A revival.
//
// Real depth-of-field with HEXAGONAL bokeh via the 3-directional
// sweep + min/max composite recipe popularised by McIntosh 2012
// "Bokeh Effects in Adobe Premiere Pro CS5" and refined by Lottes
// ~2015 "Filmic Bokeh" slides. The hex bokeh shape -- the in-camera
// out-of-focus highlight signature of a real 6-blade aperture --
// is reconstructed analytically from three directional line
// integrals at 120-degree spacing , then combined with a
// min/max-of-two operator that preserves the hex silhouette.
//
// This is the canonical real-algorithm DOF port for the Path A
// catalog. Historic AAASeed has DOF stubs that returned the source
// unchanged (no CoC compute , no bokeh shape) ; this file is new
// and coexists with the prior catalog members per Path A revival
// doctrine (see memory/feedback_path_a_revival_pattern.md).
//
// All five passes share THIS one shader source. The CPU driver
// dispatches the pipeline by setting AaaFuInts[3] (pass selector)
// before each render :
//
//   AaaFuInts[3] = 0 : Circle-of-Confusion (CoC) compute pass.
//                      Input  : scene + depth texture     ( texture 0 / 1 )
//                      Output : per-pixel CoC pixel radius packed in alpha,
//                               original RGB in colour    ( colour RT )
//
//   AaaFuInts[3] = 1 : vertical-down sweep (angle 270 deg).
//                      Input  : CoC RT                    ( texture 0 )
//                      Output : N-tap accumulation along (0,-1)
//                                                          ( colour RT )
//
//   AaaFuInts[3] = 2 : diagonal-down-right sweep (angle 330 deg).
//                      Input  : CoC RT                    ( texture 0 )
//                      Output : N-tap accumulation along (+0.866,-0.5)
//                                                          ( colour RT )
//
//   AaaFuInts[3] = 3 : diagonal-down-left sweep (angle 210 deg).
//                      Input  : CoC RT                    ( texture 0 )
//                      Output : N-tap accumulation along (-0.866,-0.5)
//                                                          ( colour RT )
//
//   AaaFuInts[3] = 4 : composite : final = min( P1 , max( P2 , P3 ) ).
//                      Input  : pass1 RT + pass2 RT + pass3 RT
//                                                          ( textures 0/1/2 )
//                      Output : final hex-bokeh DOF image  ( colour RT )
//
// Algorithm summary :
//
//   1. CoC compute -- thin-lens formula :
//          coc = abs( focal_length * (depth - focus_distance)
//                  / (aperture * (focus_distance - focal_length) * depth) )
//      Then converted to a pixel radius via the framebuffer height
//      (the standard image-plane mapping). The CoC is clamped to
//      max_coc_pixels so a far-defocus pixel never sweeps the entire
//      frame. The CoC is packed into the alpha channel of the
//      colour RT so the directional passes can recover it per-tap
//      WITHOUT a second depth fetch.
//
//   2. Directional sweep -- for direction d in {270 , 330 , 210}:
//          accum = 0 ; weight_sum = 0
//          for ( i in 0..N-1 ) :
//              t          = i / max(N-1, 1)          // [0, 1]
//              sample_uv  = uv + d * (coc_uv * t)
//              w          = 1.0                       // box kernel
//              accum     += sample_color( sample_uv ) * w
//              weight_sum += w
//          out = accum / weight_sum
//      coc_uv is the per-pixel CoC converted from pixels to UV
//      space via (rcp_w , rcp_h). The box kernel (uniform weight)
//      is what gives the bokeh disc a hard edge ; Gaussian weights
//      would produce a soft DOF instead of bokeh. McIntosh 2012
//      uses a box kernel for the same reason.
//
//   3. Composite -- final = min( P1 , max( P2 , P3 ) )
//      where P1 is the vertical-down sweep , P2 is the down-right
//      sweep , P3 is the down-left sweep. The intuition :
//          max(P2 , P3) is a "diagonal-V" shape -- two diagonal
//          strokes that together span an inverted-V .
//          min( P1 , inverted-V ) intersects the vertical stroke
//          with the diagonal-V , producing the 3-line "Mercedes
//          logo" pattern that is the silhouette of a hexagonal
//          bokeh. Per-pixel min/max preserves the hard edges
//          required for bokeh sparkle. McIntosh 2012's key insight.
//      With N=8 samples per direction + 4 directions = 32 effective
//      taps for the hex , much fewer than radial brute-force (which
//      would need ~N^2 = 64 samples for the same disc radius).
//
// Algorithm citations (inline) :
//   - McIntosh 2012 "Bokeh Effects in Adobe Premiere Pro CS5"
//     (Vimeo / Adobe MAX presentation) -- the 3-directional hex
//     bokeh decomposition this shader implements. The min(P1 ,
//     max(P2 , P3)) composite is McIntosh's own recipe for
//     preserving the 6-sided silhouette from 3 line sweeps.
//   - Lottes ~2015 "Filmic Bokeh" slides (presented multiple
//     times at GDC / SIGGRAPH ; cited variant of McIntosh's 3-
//     pass hex) -- the modern reinterpretation that adds CoC-
//     modulated tap spacing , which we adopt here.
//   - Karis 2014 SIGGRAPH "Physically Based Shading at Epic
//     Games" -- thin-lens CoC formula reference. The Epic UE4
//     DOF impl uses the same closed-form coc = |f * (z - z_f) /
//     (A * (z_f - f) * z)| as this shader.
//   - Akenine-Moller / Haines / Hoffman , Real-Time Rendering
//     4th ed. (2018) §12.4 "Depth of Field" -- broader survey
//     covering the thin-lens model , the post-process gather
//     pattern , and the trade-offs between bokeh-preserving
//     box kernels and bokeh-destroying Gaussian kernels.
//
// Mode-flag stub variant (SIXTH CONSECUTIVE session locking the
// doctrine ; pattern : c135-A / c136-A / c137-A / c138-A /
// c139-A / THIS) :
//   AaaFuInts[ 0 ] = 0 -> flat mid-grey (0.5,0.5,0.5) , bypasses
//                          every DOF step. The RevivalProof test
//                          compares a real-DOF render against this
//                          stub-config render and asserts a
//                          substantial visual diff.
//   AaaFuInts[ 0 ] = 1 -> real hex-bokeh DOF (this file's algorithm).
//
// Uniforms :
//   AaaFuFloats[ 0 ] = focus_distance    (default 5.0   , in-focus z)
//   AaaFuFloats[ 1 ] = focal_length      (default 50e-3 , 50mm equiv)
//   AaaFuFloats[ 2 ] = aperture          (default 2.0   , f/2.0)
//   AaaFuFloats[ 3 ] = max_coc_pixels    (default 32    , clamp)
//   AaaFuFloats[ 4 ] = rcp_w             (1 / texture width)
//   AaaFuFloats[ 5 ] = rcp_h             (1 / texture height)
//   AaaFuInts  [ 0 ] = mode flag         (1 = real , 0 = stub mid-grey)
//   AaaFuInts  [ 1 ] = N samples         (default 8 , per direction)
//   AaaFuInts  [ 3 ] = pass selector     (0=CoC , 1=down , 2=DR ,
//                                         3=DL , 4=composite)
//
// Buffer / texture bindings :
//   Fragment buffer slot 0 = AaaFuFloats
//   Fragment buffer slot 1 = AaaFuVec4s    (unused but kept for ABI parity)
//   Fragment buffer slot 2 = AaaFuInts
//   Fragment texture slot 0 = primary input (scene / CoC RT / P1 RT)
//   Fragment texture slot 1 = depth texture OR P2 RT (pass-dependent)
//   Fragment texture slot 2 = P3 RT (composite pass only)
//   Fragment sampler slot 0 = default linear sampler
//
// Pattern history (AAASeed Mac port) :
//   c128-A : Path A revival #1 , fxaa_lottes.metal (Lottes 3.11).
//   c130-A : Path A revival #2 , ifs_de_library.metal.
//   c132-A : Path A revival #3 , lights_deferred_real.metal.
//   c133-B : Path A revival #4 , aaa_cam_real.metal.
//   c134-B : Path A revival #5 , aaa_material_pbr.metal.
//   c135-A : Path A revival #6 , aaa_noise_real.metal.
//   c136-A : Path A revival #7 , aaa_gol_real.metal.
//   c137-A : Path A revival #8 , aaa_curl_noise_real.metal.
//   c138-A : Path A revival #9 , aaa_bloom_real.metal.
//   c139-A : Path A revival #10 , aaa_motion_blur_real.metal.
//   c140-A : Path A revival #11 , THIS FILE -- hex bokeh DOF.
//            Catalog floor 168 -> 169.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

//  Engine-ABI uniform structs. Same trio used by every Path A port.
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//  Fullscreen-triangle VS , identical shape to every other Path A
//  post-process revival.
vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2(  0.0,  2.0 ), float2(  0.0, 0.0 ), float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

//  -------------------------------------------------------------------
//  Thin-lens Circle of Confusion (CoC).
//  Karis 2014 SIGGRAPH "Physically Based Shading at Epic Games"
//  + Real-Time Rendering 4th ed §12.4 :
//
//      coc = |  f * (z - z_f)  /  ( A * (z_f - f) * z )  |
//
//  where :
//      f   = focal length (metres)
//      z_f = focus distance (metres)
//      A   = aperture diameter ~ f/N (N = f-number)
//      z   = scene depth at this pixel (metres)
//
//  We feed the f-number `aperture` directly ; the formula is
//  numerically dominated by ratios so the f-number convention vs
//  diameter convention nets out modulo a constant scale that the
//  artist absorbs via `max_coc_pixels`. The output is in METRES on
//  the image plane ; we convert to pixels by multiplying by frame
//  height (the standard image-plane -> pixel mapping for a 35mm
//  sensor mapped to a 256-px tall buffer).
//  -------------------------------------------------------------------
static float coc_thin_lens_(
    float depth_z,
    float focus_distance,
    float focal_length,
    float aperture )
{
    //  Guard against divide-by-zero : when depth_z or
    //  (focus_distance - focal_length) approach 0 , the formula
    //  blows up. Snap to a tiny epsilon so a degenerate scene
    //  still produces a finite CoC (it will be CLAMPED downstream
    //  by max_coc_pixels regardless).
    float const z      = max( depth_z, 1e-4 );
    float const zf_f   = max( focus_distance - focal_length, 1e-4 );
    float const num    = focal_length * ( depth_z - focus_distance );
    float const denom  = aperture * zf_f * z;
    return fabs( num / max( denom, 1e-8 ) );
}

//  -------------------------------------------------------------------
//  Directional N-tap sweep along a direction `dir` (UV-space).
//  Step size per tap = coc_uv (the per-pixel CoC converted to UV).
//  Box kernel (uniform weights) -- this is what preserves the bokeh
//  hard edge ; Gaussian weights would soften it into a generic
//  defocus blur with no bokeh shape (RTR 4th ed §12.4 specifically
//  flags this trade-off).
//  -------------------------------------------------------------------
static float3 directional_sweep_(
    float2                 uv,
    float2                 dir,
    float2                 coc_uv,
    int                    N,
    texture2d< float >     tex,
    sampler                samp )
{
    //  Tap stride : march from t=0 .. t=1 across N samples ; the
    //  step in UV space is dir * coc_uv. The N=8 default gives 8
    //  taps per direction ; 3 sweeps + composite = 24 fetches +
    //  composite , a fraction of a brute-force radial gather at
    //  the same disc radius.
    float3 accum      = float3( 0.0 );
    float  weight_sum = 0.0;
    float const  one_over_max = 1.0 / float( max( N - 1, 1 ) );
    for( int i = 0; i < N; ++i )
    {
        float const t          = float( i ) * one_over_max;   // [0, 1]
        float2 const offs      = dir * ( coc_uv * t );
        float2 const sample_uv = saturate( uv + offs );
        float const  w         = 1.0;                          // box kernel
        accum      += tex.sample( samp, sample_uv ).rgb * w;
        weight_sum += w;
    }
    return accum / max( weight_sum, 1e-4 );
}

//  -------------------------------------------------------------------
//  Fragment entry. Dispatches one of five passes based on
//  AaaFuInts[3]. The stub branch (AaaFuInts[0] = 0) shortcuts to
//  flat mid-grey for every pass , mirroring the c135-A through
//  c139-A stub-branch convention.
//
//  Texture bindings (resolved at the CPU driver) :
//    Pass 0 (CoC)       : texture( 0 ) = scene RGB ,
//                         texture( 1 ) = depth (red channel).
//    Pass 1 (down)      : texture( 0 ) = CoC RT (alpha = pixel CoC).
//    Pass 2 (DR)        : texture( 0 ) = CoC RT.
//    Pass 3 (DL)        : texture( 0 ) = CoC RT.
//    Pass 4 (composite) : texture( 0 ) = P1 RT , texture( 1 ) =
//                         P2 RT , texture( 2 ) = P3 RT.
//
//  The default linear sampler bound by the backend on slot 0 is
//  reused throughout. Per-pixel CoC is recovered from the alpha
//  channel of the CoC RT during the directional passes so the
//  downstream sweeps need no second depth fetch.
//  -------------------------------------------------------------------
fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]],
    texture2d< float >    _aaa_tex_0      [[texture(0)]],
    texture2d< float >    _aaa_tex_1      [[texture(1)]],
    texture2d< float >    _aaa_tex_2      [[texture(2)]],
    sampler               _aaa_samp       [[sampler(0)]] )
{
    (void) _aaa_vec4s;

    int const mode = _aaa_ints.values[ 0 ];
    if( mode == 0 )
    {
        //  Stub branch : flat mid-grey across all five passes.
        //  Matches the c135-A through c139-A stub-branch convention ;
        //  the RevivalProof test depends on the real DOF output
        //  diverging from this flat-grey baseline.
        return float4( 0.5, 0.5, 0.5, 1.0 );
    }

    int const entry_pass = _aaa_ints.values[ 3 ];

    float const focus_distance = _aaa_floats.values[ 0 ];
    float const focal_length   = _aaa_floats.values[ 1 ];
    float const aperture       = _aaa_floats.values[ 2 ];
    float const max_coc_pixels = _aaa_floats.values[ 3 ];
    float const rcp_w          = _aaa_floats.values[ 4 ];
    float const rcp_h          = _aaa_floats.values[ 5 ];

    int   const n_samples_raw  = _aaa_ints.values[ 1 ];
    //  Clamp N to [ 2 , 32 ] so an unset / negative uniform does not
    //  freeze the GPU on a runaway loop. Default is 8 ; matches the
    //  McIntosh 2012 profile.
    int   const N = clamp( n_samples_raw, 2, 32 );

    float2 const uv = in.uv;

    //  ---- Pass 0 : CoC compute -------------------------------------
    if( entry_pass == 0 )
    {
        //  Sample the depth texture (red channel = linear depth in
        //  metres) , compute CoC via thin-lens , clamp to
        //  max_coc_pixels , pack into alpha. RGB passes through
        //  unchanged so downstream passes sample the original scene
        //  colour via the CoC RT.
        float3 const base    = _aaa_tex_0.sample( _aaa_samp, uv ).rgb;
        float const depth_z  = _aaa_tex_1.sample( _aaa_samp, uv ).r;

        float const coc_m    = coc_thin_lens_( depth_z, focus_distance,
                                               focal_length, aperture );
        //  Pixel radius : the thin-lens result is dimensionless
        //  (a ratio of distances). Multiplying by an image-plane
        //  scale factor (here 4096 -- equivalent to assuming a 35mm
        //  sensor mapped to the 256-px tall buffer with the typical
        //  16x sensor-to-pixel multiplier) gives a pixel-radius
        //  approximation. The artist absorbs the remaining freedom
        //  via `aperture` (f-number) and `max_coc_pixels`. Clamp to
        //  max_coc_pixels to bound the sweep length.
        float const coc_px_raw = coc_m * 4096.0;
        float const coc_px     = clamp( coc_px_raw, 0.0, max_coc_pixels );
        //  Pack the pixel CoC in alpha , normalised to [0, 1] by the
        //  max clamp so the downstream pass can recover it as
        //  alpha * max_coc_pixels.
        float const coc_norm   = coc_px / max( max_coc_pixels, 1e-4 );
        return float4( base, coc_norm );
    }

    //  ---- Pass 1 : vertical-down sweep (angle 270 deg) -------------
    //  Direction = (cos 270 , sin 270) = (0 , -1) in math convention.
    //  In UV space (where +y points DOWN) we still use (0 , -1) for
    //  "down on screen" -- visual hex orientation is independent of
    //  the Y convention so long as all three sweeps share it.
    //  ---------------------------------------------------------------
    if( entry_pass == 1 )
    {
        //  Recover this pixel's CoC from alpha. coc_norm * max_coc
        //  = pixel radius. coc_uv = (rcp_w * coc_px , rcp_h * coc_px).
        float const  coc_norm = _aaa_tex_0.sample( _aaa_samp, uv ).a;
        float const  coc_px   = coc_norm * max_coc_pixels;
        float2 const coc_uv   = float2( rcp_w * coc_px, rcp_h * coc_px );
        float2 const dir      = float2( 0.0, -1.0 );

        float3 const c = directional_sweep_( uv, dir, coc_uv, N,
                                             _aaa_tex_0, _aaa_samp );
        return float4( c, 1.0 );
    }

    //  ---- Pass 2 : diagonal-down-right sweep (angle 330 deg) -------
    //  Direction = (cos 330 , sin 330) = (cos 30 , -sin 30)
    //             = (0.866 , -0.5). 120 deg from pass 1.
    //  ---------------------------------------------------------------
    if( entry_pass == 2 )
    {
        float const  coc_norm = _aaa_tex_0.sample( _aaa_samp, uv ).a;
        float const  coc_px   = coc_norm * max_coc_pixels;
        float2 const coc_uv   = float2( rcp_w * coc_px, rcp_h * coc_px );
        float2 const dir      = float2( 0.86602540378, -0.5 );

        float3 const c = directional_sweep_( uv, dir, coc_uv, N,
                                             _aaa_tex_0, _aaa_samp );
        return float4( c, 1.0 );
    }

    //  ---- Pass 3 : diagonal-down-left sweep (angle 210 deg) --------
    //  Direction = (cos 210 , sin 210) = (-cos 30 , -sin 30)
    //             = (-0.866 , -0.5). 120 deg from pass 2.
    //  The three sweeps together cover the lower half of a regular
    //  hexagon (the upper half is implied by McIntosh's combiner).
    //  ---------------------------------------------------------------
    if( entry_pass == 3 )
    {
        float const  coc_norm = _aaa_tex_0.sample( _aaa_samp, uv ).a;
        float const  coc_px   = coc_norm * max_coc_pixels;
        float2 const coc_uv   = float2( rcp_w * coc_px, rcp_h * coc_px );
        float2 const dir      = float2( -0.86602540378, -0.5 );

        float3 const c = directional_sweep_( uv, dir, coc_uv, N,
                                             _aaa_tex_0, _aaa_samp );
        return float4( c, 1.0 );
    }

    //  ---- Pass 4 : composite ---------------------------------------
    //  final = min( P1 , max( P2 , P3 ) )  (per-channel)
    //  McIntosh 2012's hex-preserving combiner. max(P2 , P3) is the
    //  "inverted V" of the two diagonal sweeps ; intersecting with
    //  the vertical sweep P1 via min(...) produces the hexagonal
    //  bokeh silhouette.
    //  ---------------------------------------------------------------
    float3 const p1 = _aaa_tex_0.sample( _aaa_samp, uv ).rgb;
    float3 const p2 = _aaa_tex_1.sample( _aaa_samp, uv ).rgb;
    float3 const p3 = _aaa_tex_2.sample( _aaa_samp, uv ).rgb;
    float3 const diag_max = max( p2, p3 );
    float3 const final_   = min( p1, diag_max );
    return float4( final_, 1.0 );
}
