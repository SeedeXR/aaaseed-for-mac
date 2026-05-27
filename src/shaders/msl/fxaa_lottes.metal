// fxaa_lottes.metal -- Path A REVIVAL of FXAA, real Timothy Lottes 3.11 algorithm.
//
// c128 Path A revival #1 : the FIRST stubbed shader in the Phase 3 regression
// baseline (c127-B's fxaa.metal passthrough) gets a real native algorithm.
// fxaa.metal is preserved verbatim (its golden anchors regression #27) ;
// THIS file is a SEPARATE shader so adding it doesn't perturb the existing
// baseline.
//
// Reference  : NVIDIA FXAA 3.11 by Timothy Lottes, "FXAA WhitePaper.pdf"
//              (NVIDIA Developer, 2011) -- public domain.
//              Lottes, FXAA_311.h , `FxaaPixelShader()` reference implementation.
//              GLSL port : https://github.com/mattdesl/glsl-fxaa (also public domain).
//
// Algorithm summary (Lottes 3.11 PC_QUALITY preset, simplified to the
// quality-medium edge-search loop with 8 steps, no sub-pixel jitter
// extra-quality tail) :
//
//   1. Sample 5-tap luma cross : NW, NE, SW, SE (corner taps) and M (centre).
//   2. Local contrast : luma_range = max(...) - min(...).
//      Early-out if luma_range < max(edge_threshold_min,
//                                    luma_max * edge_threshold).
//   3. Sub-pixel blend factor from corner luma average vs M.
//   4. Edge direction : detect HORIZONTAL or VERTICAL by comparing
//      (NW+NE) - (SW+SE) vs (NW+SW) - (NE+SE) magnitudes.
//   5. March along edge in both directions up to 8 steps with quality
//      offsets {1, 1.5, 2, 4, 12} (Lottes Q__PS preset trimmed).
//   6. Determine which side of the edge needs blending toward the
//      opposite-luma neighbour ; compute final blend offset.
//   7. Mix the final tap with the centre by max(sub_pixel_blend,
//      edge_blend).
//
// Lottes paper section -> uniform mapping :
//   section 4.1 "Quality Preset"  -> AaaFuFloats[0..1] = rcpFrame.xy
//                                     (= 1/W, 1/H of the source texture).
//                                  -> AaaFuFloats[2] = subpix_blend    (Lottes
//                                     `fxaaQualitySubpix`, default 0.75).
//                                  -> AaaFuFloats[3] = edge_threshold  (default 0.166).
//                                  -> AaaFuFloats[4] = edge_threshold_min (default 0.0833).
//   section 3.2 "Luma"            -> hardcoded rec.709 luma weights (0.299, 0.587, 0.114).
//   section 4.3 "Edge Search"     -> hardcoded 8-step march offsets (Q__PS).
//   AaaFuInts[0]                  -> bypass flag (mirrors fxaa.metal convention).
//
// MSL conventions  (c102 Path A doctrine) :
//   [[buffer(0)]] AaaFuFloats
//   [[buffer(2)]] AaaFuInts
//   [[texture(0)]] _aaa_tex_0     (the source colour buffer)
//   [[sampler(0)]] _aaa_samp      (linear clamp ; required for the offset taps)
//
// Determinism : no iTime, no random, no iFrame, no framebuffer feedback.
// Bit-exact same output for the same input + uniforms on every run.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };
struct AaaFuInts   { int   values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

//  Lottes section 3.2 : rec.709 luma. The constant matches Lottes
//  `FxaaLuma()` exactly so the threshold values from the white paper
//  retain their intended sensitivity.
static inline float FxaaLuma( float3 rgb )
{
    return dot( rgb, float3( 0.299, 0.587, 0.114 ) );
}

//  The full Lottes FxaaPixelShader medium-quality body.
//  Translated tap-for-tap from the reference, with the Q__PS 8-step
//  search loop expanded inline so MSL can fully unroll it.
static inline float3 FxaaPixelShader_Lottes(
    float2                  uv,
    texture2d< float >      tex,
    sampler                 samp,
    float2                  rcp_frame,
    float                   subpix_blend,
    float                   edge_threshold,
    float                   edge_threshold_min )
{
    //  --- 5-tap luma cross : center + 4 diagonal corners (Lottes 4.2). ----
    float3 const rgb_m  = tex.sample( samp, uv ).rgb;
    float3 const rgb_nw = tex.sample( samp, uv + float2( -1.0, -1.0 ) * rcp_frame ).rgb;
    float3 const rgb_ne = tex.sample( samp, uv + float2(  1.0, -1.0 ) * rcp_frame ).rgb;
    float3 const rgb_sw = tex.sample( samp, uv + float2( -1.0,  1.0 ) * rcp_frame ).rgb;
    float3 const rgb_se = tex.sample( samp, uv + float2(  1.0,  1.0 ) * rcp_frame ).rgb;

    float const luma_m  = FxaaLuma( rgb_m  );
    float const luma_nw = FxaaLuma( rgb_nw );
    float const luma_ne = FxaaLuma( rgb_ne );
    float const luma_sw = FxaaLuma( rgb_sw );
    float const luma_se = FxaaLuma( rgb_se );

    //  --- Local contrast + early-out (Lottes 4.2 "Local Contrast Check"). -
    float const luma_min = min( luma_m, min( min( luma_nw, luma_ne ), min( luma_sw, luma_se ) ) );
    float const luma_max = max( luma_m, max( max( luma_nw, luma_ne ), max( luma_sw, luma_se ) ) );
    float const luma_range = luma_max - luma_min;

    if( luma_range < max( edge_threshold_min, luma_max * edge_threshold ) )
    {
        //  Below threshold -> return the centre unchanged (no AA needed).
        return rgb_m;
    }

    //  --- Edge direction : horizontal vs vertical (Lottes 4.3). -----------
    //  Compute the 2D gradient from the 4 corner taps.
    float const edge_horz = abs( ( luma_nw + luma_ne ) - ( luma_sw + luma_se ) );
    float const edge_vert = abs( ( luma_nw + luma_sw ) - ( luma_ne + luma_se ) );
    bool  const horz_span = edge_horz >= edge_vert;

    //  --- Sub-pixel blend factor (Lottes 4.4). ----------------------------
    //  Average the corners, take |avg - center|/range -> [0,1] ratio.
    //  Smoothstep'd via the quadratic curve from the reference.
    float const luma_avg = ( luma_nw + luma_ne + luma_sw + luma_se ) * 0.25;
    float const sub_pix_a = abs( luma_avg - luma_m );
    float const sub_pix_b = clamp( sub_pix_a / luma_range, 0.0, 1.0 );
    float const sub_pix_c = ( -2.0 * sub_pix_b + 3.0 ) * sub_pix_b * sub_pix_b;
    float const sub_pix_blend_amount = sub_pix_c * sub_pix_c * subpix_blend;

    //  --- Edge endpoints : pick the two taps that straddle the edge. ------
    //  For a horizontal edge we step in y ; for vertical in x. Use the
    //  brighter side to decide which way to push the final sample.
    float2 const dir = horz_span ? float2( 0.0, rcp_frame.y )
                                 : float2( rcp_frame.x, 0.0 );
    float2 const perp = horz_span ? float2( rcp_frame.x, 0.0 )
                                  : float2( 0.0, rcp_frame.y );

    float const luma_pos = horz_span ? ( ( luma_nw + luma_ne ) * 0.5 )
                                     : ( ( luma_nw + luma_sw ) * 0.5 );
    float const luma_neg = horz_span ? ( ( luma_sw + luma_se ) * 0.5 )
                                     : ( ( luma_ne + luma_se ) * 0.5 );
    bool const is_pos_side = abs( luma_pos - luma_m ) >= abs( luma_neg - luma_m );
    float2 const edge_step = is_pos_side ? -dir : dir;

    //  --- 8-step edge march (Lottes 4.5 Q__PS preset). --------------------
    //  Quality offsets : {1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 12.0}.
    //  March from the centre along the edge collecting luma until we
    //  hit a tap whose luma exits the [luma_min, luma_max] band.
    float const quality[ 8 ] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 12.0 };

    float2 uv_pos = uv + edge_step * 0.5 + perp;
    float2 uv_neg = uv + edge_step * 0.5 - perp;

    float luma_end_pos = 0.0;
    float luma_end_neg = 0.0;
    bool  done_pos = false;
    bool  done_neg = false;
    float2 step_pos = perp;
    float2 step_neg = perp;

    for( int i = 0; i < 8; ++i )
    {
        if( !done_pos )
        {
            luma_end_pos = FxaaLuma( tex.sample( samp, uv_pos ).rgb ) - luma_m;
            if( abs( luma_end_pos ) > edge_threshold )
                done_pos = true;
            else
                uv_pos += step_pos * quality[ i ];
        }
        if( !done_neg )
        {
            luma_end_neg = FxaaLuma( tex.sample( samp, uv_neg ).rgb ) - luma_m;
            if( abs( luma_end_neg ) > edge_threshold )
                done_neg = true;
            else
                uv_neg -= step_neg * quality[ i ];
        }
    }

    //  --- Final offset / blend (Lottes 4.6). ------------------------------
    float const dist_pos = horz_span ? ( uv_pos.x - uv.x ) : ( uv_pos.y - uv.y );
    float const dist_neg = horz_span ? ( uv.x - uv_neg.x ) : ( uv.y - uv_neg.y );
    float const dist_total = dist_pos + dist_neg;
    float const dist_near  = min( abs( dist_pos ), abs( dist_neg ) );

    //  Edge blend factor : how close are we to the nearer endpoint ?
    float const edge_blend = clamp( 0.5 - dist_near / max( dist_total, 1e-6 ), 0.0, 0.5 );

    //  Final blend amount = max( sub_pixel_blend, edge_blend ).
    float const final_blend = max( sub_pix_blend_amount, edge_blend );

    //  Final tap : push the centre uv along edge_step by final_blend.
    float2 const final_uv = uv + edge_step * final_blend;
    return tex.sample( samp, final_uv ).rgb;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int const bypass = _aaa_ints.values[ 0 ];

    if( bypass != 0 )
        return float4( _aaa_tex_0.sample( _aaa_samp, in.uv ).rgb, 1.0 );

    float2 const rcp_frame          = float2( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ] );
    float  const subpix_blend       = _aaa_floats.values[ 2 ];
    float  const edge_threshold     = _aaa_floats.values[ 3 ];
    float  const edge_threshold_min = _aaa_floats.values[ 4 ];

    float3 const r = FxaaPixelShader_Lottes(
        in.uv, _aaa_tex_0, _aaa_samp, rcp_frame,
        subpix_blend, edge_threshold, edge_threshold_min );

    return float4( r, 1.0 );
}
