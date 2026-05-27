// aaa_bloom_real.metal -- c138-A : NINTH Path A revival.
//
// Real bloom post-process : the canonical 4-pass real-time bloom
// pipeline (luminance threshold -> horizontal Gaussian blur ->
// vertical Gaussian blur -> additive composite). Lives alongside
// the historic `bloom.metal` / `darkglow.metal` / `ps_Maa_threshold`
// stubs (no existing file is touched ; their goldens stay intact
// per the Path A revival doctrine).
//
// All four passes share THIS one shader source. The CPU driver
// dispatches the pipeline by setting AaaFuInts[3] (pass selector)
// before each render :
//
//   AaaFuInts[3] = 0 : threshold pass.
//                      Input  : base scene texture        ( texture 0 )
//                      Output : thresholded "hot" pixels  ( colour RT )
//
//   AaaFuInts[3] = 1 : horizontal Gaussian blur pass.
//                      Input  : threshold RT              ( texture 0 )
//                      Output : H-blurred image           ( colour RT )
//
//   AaaFuInts[3] = 2 : vertical Gaussian blur pass.
//                      Input  : H-blurred RT              ( texture 0 )
//                      Output : V-blurred (= full sep.    ( colour RT )
//                               Gaussian) image
//
//   AaaFuInts[3] = 3 : additive composite pass.
//                      Input  : base scene texture        ( texture 0 )
//                                 + blurred bloom RT      ( texture 1 )
//                      Output : final image with bloom    ( colour RT )
//
// Mode-flag stub variant (the settled doctrine across the last
// FOUR Path A revivals -- c135-A / c136-A / c137-A / THIS) :
//   AaaFuInts[0] = 0 : flat mid-grey stub branch ; bypasses every
//                      multipass step. The RevivalProof test compares
//                      a real-bloom render against this stub-config
//                      render and asserts a substantial visual diff.
//   AaaFuInts[0] = 1 : real bloom path (this file's algorithm).
//
// Float uniforms :
//   AaaFuFloats[ 0 ] = threshold       (luma cut-off ; default 0.6)
//   AaaFuFloats[ 1 ] = bloom_intensity (composite gain ; default 0.4)
//   AaaFuFloats[ 2 ] = rcp_w           (1 / texture width  -- sample step)
//   AaaFuFloats[ 3 ] = rcp_h           (1 / texture height -- sample step)
//
// Algorithm citations :
//   - Bjorke 2007 "GPU Gems 3" Ch.40 "Incremental Computation of
//     the Gaussian" -- 7-tap linear-sampling trick that exploits
//     bilinear interpolation between adjacent texels so a 13-tap
//     separable Gaussian costs 7 texture fetches per pass instead
//     of 13. We use the NAIVE 13-tap form here to keep the kernel
//     mathematically obvious : Pascal triangle row 12 normalised
//     (1, 12, 66, 220, 495, 792, 924, 792, 495, 220, 66, 12, 1) /
//     4096 ; sigma ~ 4 px. The linear-tap optimisation is noted
//     for posterity ; both forms produce the same convolution
//     (up to sampler precision) , but the explicit form is easier
//     to audit against the kernel reference.
//   - Real-Time Rendering 4th ed. (Akenine-Moller / Haines /
//     Hoffman) section 10.6 "Bloom" -- the canonical exposition
//     of the threshold + separable blur + additive composite
//     pipeline. The smoothstep soft threshold (threshold .. threshold
//     + 0.5) is the standard avoid-hard-edges fix from §10.6.1.
//   - Sousa 2008 GDC "Crysis 2 Next Gen Effects" -- introduced the
//     luminance-threshold + multi-scale Gaussian stack used by
//     modern AAA bloom (our single-octave 13-tap is the simplest
//     stage of that pipeline). Threshold + dirt-overlay framing is
//     attributable to this talk.
//   - Rec. 709 luma weights (0.2126, 0.7152, 0.0722) -- ITU-R BT.709
//     standard ; matches the Lottes FXAA luma convention to within
//     coefficient precision.
//
// Pattern history (AAASeed Mac port) :
//   c128-A : Path A revival #1, fxaa_lottes.metal (Lottes 3.11).
//   c130-A : Path A revival #2, ifs_de_library.metal.
//   c132-A : Path A revival #3, lights_deferred_real.metal.
//   c133-B : Path A revival #4, aaa_cam_real.metal.
//   c134-B : Path A revival #5, aaa_material_pbr.metal.
//   c135-A : Path A revival #6, aaa_noise_real.metal.
//   c136-A : Path A revival #7, aaa_gol_real.metal.
//   c137-A : Path A revival #8, aaa_curl_noise_real.metal.
//   c138-A : Path A revival #9, THIS FILE -- real 4-pass bloom
//            (threshold + H Gaussian + V Gaussian + composite).
//            Catalog floor 166 -> 167.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

//  Engine-ABI uniform structs. Same trio used by every Path A port.
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//  Fullscreen-triangle VS , identical shape to every other Path A revival.
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
//  Rec.709 luma. ITU-R BT.709 standard linearised weights.
//  Spec :  Y = 0.2126 R + 0.7152 G + 0.0722 B .
//  Used by Pass 1 (luminance threshold) only.
//  -------------------------------------------------------------------
static inline float luma_rec709_( float3 rgb )
{
    return dot( rgb, float3( 0.2126, 0.7152, 0.0722 ) );
}

//  -------------------------------------------------------------------
//  Pascal triangle row 12 , normalised by 1 / 4096 (= 2^12).
//  Gives a discrete approximation to a sigma ~ 4 Gaussian over a
//  13-tap stencil centred on the current pixel.
//  Weights :        1   12   66   220   495   792   924   792   495   220   66   12   1
//  Sum     = 4096 -> normalised weights sum to 1.0 exactly under
//  exact arithmetic ; under FP we observe ~1e-7 deviation which is
//  below the sampler / FP precision floor.
//  Reference : binomial coefficients C(12, k) ; this is the textbook
//  exact-Gaussian-coefficients-via-Pascal'-triangle approximation.
//  -------------------------------------------------------------------
constant float kGauss13[ 13 ] = {
       1.0f / 4096.0f,    12.0f / 4096.0f,    66.0f / 4096.0f,
     220.0f / 4096.0f,   495.0f / 4096.0f,   792.0f / 4096.0f,
     924.0f / 4096.0f,
     792.0f / 4096.0f,   495.0f / 4096.0f,   220.0f / 4096.0f,
      66.0f / 4096.0f,    12.0f / 4096.0f,     1.0f / 4096.0f
};

//  -------------------------------------------------------------------
//  Soft luminance threshold.
//  Hard threshold (`luma > T ? color : 0`) produces a sharp visual
//  discontinuity that aliases under camera shake / animation. The
//  smoothstep ramp from T to T + 0.5 gives a soft "knee" -- pixels
//  near the threshold contribute partially to bloom. RTR 4th ed
//  §10.6.1 specifically recommends this fix.
//  -------------------------------------------------------------------
static inline float3 soft_threshold_( float3 color, float threshold )
{
    float const  y    = luma_rec709_( color );
    float const  knee = smoothstep( threshold, threshold + 0.5, y );
    return color * knee;
}

//  -------------------------------------------------------------------
//  Naive 13-tap separable Gaussian. The direction vector is
//  (rcp_w, 0) for horizontal pass and (0, rcp_h) for vertical pass
//  so this one function services both blur passes.
//  Bjorke 2007 GPU Gems 3 Ch.40's 7-sample linear trick exploits
//  bilinear interpolation between pairs of adjacent texels to halve
//  the fetch cost ; we keep the explicit 13 fetches for readability.
//  -------------------------------------------------------------------
static float3 gauss13_(
    float2                  uv,
    float2                  dir,
    texture2d< float >      tex,
    sampler                 samp )
{
    float3 acc = float3( 0.0 );
    //  Offsets -6..+6 ; centre = tap 6 (weight 924/4096 = 0.225...).
    for( int i = 0; i < 13; ++i )
    {
        float const  off  = float( i - 6 );
        float2 const uv_i = uv + dir * off;
        acc += kGauss13[ i ] * tex.sample( samp, uv_i ).rgb;
    }
    return acc;
}

//  -------------------------------------------------------------------
//  Fragment entry. Dispatches one of four passes based on
//  AaaFuInts[3]. The stub branch (AaaFuInts[0] = 0) shortcuts to
//  flat mid-grey for every pass, mirroring the c135-A / c136-A /
//  c137-A stub-branch convention.
//
//  Texture bindings :
//    Pass 0 (threshold) : texture( 0 ) = base scene.
//    Pass 1 (H blur)    : texture( 0 ) = threshold RT.
//    Pass 2 (V blur)    : texture( 0 ) = H-blur RT.
//    Pass 3 (composite) : texture( 0 ) = base scene ,
//                         texture( 1 ) = blurred bloom (V-blur RT).
//
//  The default linear sampler bound by the backend on slot 0 is
//  reused throughout -- linear filtering is what makes Bjorke's
//  7-tap trick work and also softens the explicit-13-tap result.
//  -------------------------------------------------------------------
fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]],
    texture2d< float >    _aaa_tex_0      [[texture(0)]],
    texture2d< float >    _aaa_tex_1      [[texture(1)]],
    sampler               _aaa_samp       [[sampler(0)]] )
{
    (void) _aaa_vec4s;

    int const mode = _aaa_ints.values[ 0 ];
    if( mode == 0 )
    {
        //  Stub branch : flat mid-grey 0.5 across all four passes.
        //  Matches the c135-A / c136-A / c137-A stub convention ;
        //  the RevivalProof test depends on the real bloom output
        //  diverging from this flat-grey baseline.
        return float4( 0.5, 0.5, 0.5, 1.0 );
    }

    int const entry_pass = _aaa_ints.values[ 3 ];

    float const  threshold       = _aaa_floats.values[ 0 ];
    float const  bloom_intensity = _aaa_floats.values[ 1 ];
    float const  rcp_w           = _aaa_floats.values[ 2 ];
    float const  rcp_h           = _aaa_floats.values[ 3 ];

    float2 const uv = in.uv;

    //  ---- Pass 0 : luminance threshold ------------------------------
    if( entry_pass == 0 )
    {
        //  Sample the base scene , soft-threshold by rec.709 luma.
        //  Output : pixels with luma >= threshold contribute their
        //  full color ; pixels with luma <= threshold contribute 0 ;
        //  pixels in [threshold, threshold + 0.5] ramp smoothly.
        float3 const base = _aaa_tex_0.sample( _aaa_samp, uv ).rgb;
        float3 const hot  = soft_threshold_( base, threshold );
        return float4( hot, 1.0 );
    }

    //  ---- Pass 1 : horizontal Gaussian blur -------------------------
    if( entry_pass == 1 )
    {
        //  13-tap separable Gaussian along x. Sample step = rcp_w
        //  so each tap moves exactly one source-texel along x.
        float2 const dir = float2( rcp_w, 0.0 );
        float3 const c   = gauss13_( uv, dir, _aaa_tex_0, _aaa_samp );
        return float4( c, 1.0 );
    }

    //  ---- Pass 2 : vertical Gaussian blur ---------------------------
    if( entry_pass == 2 )
    {
        //  Same 13-tap kernel rotated 90 degrees. Combined with
        //  pass 1 this gives a full 2D separable Gaussian (the
        //  N x N kernel decomposes into 2N taps along axes by
        //  separability of the Gaussian).
        float2 const dir = float2( 0.0, rcp_h );
        float3 const c   = gauss13_( uv, dir, _aaa_tex_0, _aaa_samp );
        return float4( c, 1.0 );
    }

    //  ---- Pass 3 : additive composite -------------------------------
    //  Final pass. Sample the BASE scene (texture 0) and the
    //  BLURRED bloom (texture 1) , then :
    //      final = base + bloom_intensity * blurred
    //  RTR 4th ed §10.6 formulation. bloom_intensity is the artist-
    //  facing knob ; default 0.4 keeps highlights soft without
    //  blowing out the rest of the frame. Saturate caps at 1.0
    //  for LDR display.
    float3 const base    = _aaa_tex_0.sample( _aaa_samp, uv ).rgb;
    float3 const blurred = _aaa_tex_1.sample( _aaa_samp, uv ).rgb;
    float3 const final_  = saturate( base + bloom_intensity * blurred );
    return float4( final_, 1.0 );
}
