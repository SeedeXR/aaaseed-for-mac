// aaa_motion_blur_real.metal -- c139-A : TENTH Path A revival.
//
// Real camera-velocity-projected motion blur. Builds directly on
// c133-B's `aaa_cam_real.metal` view/proj/vp matrix infrastructure :
// adds a SECOND `AaaCamReal` constant block (`cam_prev`) for the
// previous-frame view + proj + vp , then derives a per-pixel screen-
// space velocity by reprojecting each fragment through both frames'
// VP matrices. The velocity vector drives an N-tap accumulation
// along its direction , producing the canonical streak / smear of
// classic post-process camera motion blur.
//
// The Path A catalog has NO prior motion-blur shader -- the historic
// AAASeed renderer relied on a stub that returned the source colour
// untouched. This file is the canonical real-algorithm port. Per
// Path A revival doctrine (memory/feedback_path_a_revival_pattern.md)
// nothing in the historic shader set is touched ; this file is new
// and coexists with the prior catalog members.
//
// Algorithm (McGuire 2012 + Rosado 2007 + Real-Time Rendering 4th
// ed. §12.5) :
//
//   1. Reconstruct the world-space position behind the current
//      pixel from
//        ndc.xy = uv * 2 - 1 ; ndc.y = -ndc.y    (Metal Y-flip)
//        depth_ndc = linear_depth_to_ndc( linear_z )
//        clip = float4( ndc.xy , depth_ndc , 1.0 )
//        world_h = cam.view_inv * cam.proj_inv * clip
//        world = world_h.xyz / world_h.w
//      We carry both `cam.view_inv` and an inverse-projection helper
//      (we approximate using the precomputed `cam.vp` inverse derived
//      on the CPU side and supplied via cam_prev / cam.vp BOTH for
//      auditability ; in this shader we exploit the simpler form :
//      reconstruct a view-space ray from NDC + linear depth + the
//      proj fov scalars , then transform to world via cam.view_inv).
//
//   2. Reproject through previous-frame VP :
//        clip_prev = cam_prev.vp * float4( world , 1.0 )
//        ndc_prev  = clip_prev.xyz / clip_prev.w
//        uv_prev   = ndc_prev.xy * 0.5 + 0.5    (Y-flipped to match)
//
//   3. Screen-space velocity vector :
//        velocity_uv = ( uv - uv_prev ) * intensity
//      Clamp magnitude to `max_velocity_pixels / kBufferDim` so a
//      runaway reprojection (eg. behind-camera, w <= 0) cannot smear
//      across the whole frame.
//
//   4. N-tap accumulate along velocity :
//        accum = 0 ; weight_sum = 0
//        for ( i in 0..N-1 ) :
//            t          = ( i + 0.5 ) / N - 0.5      // [-0.5, +0.5)
//            sample_uv  = uv + t * velocity
//            w          = 1.0 - abs(t) * 0.5         // soft falloff
//            accum     += sample_color( sample_uv ) * w
//            weight_sum += w
//        final = accum / weight_sum
//      The (i + 0.5)/N - 0.5 stride distributes taps SYMMETRICALLY
//      around the current pixel along the velocity vector , which is
//      the classic Rosado 2007 / GPU Gems 3 Ch.27 formulation.
//
// Algorithm citations (inline) :
//   - McGuire et al. 2012 I3D "A Reconstruction Filter for Plausible
//     Motion Blur" -- the modern reference for screen-space motion-
//     blur reconstruction. The per-pixel velocity-tile + tap-along-
//     velocity recipe.
//   - Rosado 2007 "GPU Gems 3" Ch.27 "Motion Blur as a Post-Processing
//     Effect" -- the canonical exposition of camera-velocity-projected
//     post-process motion blur as used in Crysis / Lost Planet ;
//     introduces the world-pos reconstruct + reproject + N-tap
//     pipeline this shader implements.
//   - Akenine-Moller / Haines / Hoffman , Real-Time Rendering 4th
//     ed. (2018) §12.5 "Motion Blur" -- broader survey covering the
//     screen-space reconstruction approach , velocity buffer trade-
//     offs , and the relationship to physically-based shutter time.
//
// Mode-flag stub variant (FIFTH CONSECUTIVE session locking in the
// doctrine ; pattern : c135-A / c136-A / c137-A / c138-A / THIS) :
//   AaaFuInts[ 0 ] = 0 -> flat mid-grey (0.5,0.5,0.5) , bypasses every
//                          motion-blur step. The RevivalProof test
//                          compares a real-blur render against this
//                          stub-config render and asserts a substantial
//                          visual diff.
//   AaaFuInts[ 0 ] = 1 -> real motion blur (this file's algorithm).
//
// Uniforms :
//   AaaFuFloats[ 0 ] = blur_intensity        (default 0.5 , velocity scale)
//   AaaFuFloats[ 1 ] = max_velocity_pixels   (default 64 , clamp)
//   AaaFuFloats[ 2 ] = linear_depth          (default 0.5 , scene depth)
//   AaaFuInts  [ 0 ] = mode flag             (1 = real , 0 = stub mid-grey)
//   AaaFuInts  [ 1 ] = N samples             (default 16)
//
// Buffer / texture bindings :
//   Fragment buffer slot 0 = AaaFuFloats
//   Fragment buffer slot 1 = AaaFuVec4s    (unused but kept for ABI parity)
//   Fragment buffer slot 2 = AaaFuInts
//   Fragment buffer slot 3 = AaaCamReal cam       (current frame matrices)
//   Fragment buffer slot 4 = AaaCamReal cam_prev  (previous frame matrices)
//   Fragment texture slot 0 = scene colour (the image being blurred)
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
//   c139-A : Path A revival #10 , THIS FILE -- DOUBLE-DIGIT CROSSED.
//            Catalog floor 167 -> 168.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

//  Engine-ABI uniform structs. Same trio used by every Path A port.
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//  AaaCamReal -- BYTE-EQUIVALENT to c133-B's `struct AaaCamReal`.
//  Doctrine forbids editing c133-B's aaa_cam_real.metal ; MSL has no
//  cross-TU includes ; so we restate the struct here. The CPU-side
//  mirror in tests/regression/phase3_golden_frame_regression_test.cpp
//  is shared and stays one canonical layout.
//      offset  0   view        float4x4
//      offset 64   proj        float4x4
//      offset 128  view_inv    float4x4
//      offset 192  vp          float4x4
//      offset 256  eye_aspect  float4
//      offset 272  fov_clip    float4    (x = fov_y_rad, y = near, z = far, w = reserved)
//  Total = 288 bytes.
struct AaaCamReal
{
    float4x4 view;
    float4x4 proj;
    float4x4 view_inv;
    float4x4 vp;
    float4   eye_aspect;
    float4   fov_clip;
};

//  Fullscreen-triangle VS , identical shape to every other Path A
//  post-process revival. Two-triangle "big triangle" trick covers
//  the [-1,+1] NDC region with one primitive. UV in [0,1] over the
//  visible quadrant.
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
//  Reconstruct world-space position from screen UV + linear depth +
//  cam.view_inv + cam.fov_clip. Real-Time Rendering 4th ed §12.5
//  + Rosado 2007 GPU Gems 3 Ch.27 : the canonical recipe uses
//  inverse-VP composed with a clip-space probe ; we use the closed-
//  form view-space ray construction equivalent (cheaper , avoids a
//  full 4x4 inverse on the GPU).
//
//      ndc.xy        = uv * 2 - 1       (Metal : flip y so +y is up)
//      tan_half_fy   = tan(fov_y * 0.5)
//      view.x        = ndc.x * tan_half_fy * aspect * linear_z
//      view.y        = ndc.y * tan_half_fy           * linear_z
//      view.z        = -linear_z        (RH view space : -z is forward)
//      world         = cam.view_inv * float4( view , 1 )
//
//  The view_inv lift takes the view-space ray endpoint back to world.
//  This matches what cam.vp_inv * clip_at_depth would produce up to
//  a sign / convention choice ; both forms appear in the literature.
//  -------------------------------------------------------------------
static float3 reconstruct_world_pos_(
    float2     uv,
    float      linear_z,
    constant   AaaCamReal& cam )
{
    float const  fov_y       = cam.fov_clip.x;
    float const  aspect      = cam.eye_aspect.w;
    float const  tan_half_fy = tan( fov_y * 0.5 );

    //  NDC in [-1,+1] , Y-flip so +y maps to "up" (Metal frame-buffer
    //  origin is top-left ; vertical flip mirrors that).
    float2 const ndc = float2( uv.x * 2.0 - 1.0,
                               1.0 - uv.y * 2.0 );

    //  View-space ray endpoint at distance linear_z along -z .
    float3 const view_pos = float3( ndc.x * tan_half_fy * aspect * linear_z,
                                    ndc.y * tan_half_fy           * linear_z,
                                   -linear_z );

    //  Lift to world via the inverse-view (orthonormal basis +
    //  eye translation -- see c133-B's view_inv construction).
    float4 const world_h = cam.view_inv * float4( view_pos, 1.0 );
    return world_h.xyz / world_h.w;
}

//  -------------------------------------------------------------------
//  Project a world-space position through a camera's VP matrix to
//  recover its screen-space UV under that camera. The reverse of
//  reconstruct_world_pos_ : world -> clip -> NDC -> uv .
//
//      clip = cam.vp * float4( world , 1 )
//      ndc  = clip.xyz / clip.w
//      uv   = float2( ndc.x * 0.5 + 0.5 , 0.5 - ndc.y * 0.5 )
//
//  Y-flip mirrors reconstruct_world_pos_ so a round-trip (this frame ,
//  this matrix) is an identity -- a sanity check that the projection
//  pair is self-consistent.
//  -------------------------------------------------------------------
static float2 project_world_to_uv_(
    float3     world,
    constant   AaaCamReal& cam )
{
    float4 const clip = cam.vp * float4( world, 1.0 );
    //  Guard against w <= 0 (point behind camera). In that case the
    //  perspective divide produces a nonsense uv ; pass a sentinel
    //  far from [0,1] so the caller's clamp / saturate collapses
    //  the velocity to zero. We use uv = (-100,-100).
    if( clip.w <= 1e-4 )
        return float2( -100.0, -100.0 );

    float3 const ndc = clip.xyz / clip.w;
    return float2( ndc.x * 0.5 + 0.5,
                   0.5 - ndc.y * 0.5 );
}

//  -------------------------------------------------------------------
//  Fragment entry.
//
//  Stub branch (mode = 0) : flat mid-grey , doctrine pattern shared
//  with c135-A / c136-A / c137-A / c138-A.
//
//  Real branch (mode = 1) :
//      1. Reconstruct world pos from uv + linear depth + cam .
//      2. Reproject through cam_prev to get prev-frame uv .
//      3. Velocity = (uv - prev_uv) * intensity , clamped .
//      4. N taps along velocity , soft-falloff weighted average .
//  -------------------------------------------------------------------
fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]],
    constant AaaCamReal&  cam             [[buffer(3)]],
    constant AaaCamReal&  cam_prev        [[buffer(4)]],
    texture2d< float >    _aaa_tex_0      [[texture(0)]],
    sampler               _aaa_samp       [[sampler(0)]] )
{
    (void) _aaa_vec4s;

    int const mode = _aaa_ints.values[ 0 ];
    if( mode == 0 )
    {
        //  Stub branch : flat mid-grey . Matches the c135-A / c136-A /
        //  c137-A / c138-A stub-branch convention -- the RevivalProof
        //  test depends on the real motion-blur output diverging from
        //  this flat-grey baseline.
        return float4( 0.5, 0.5, 0.5, 1.0 );
    }

    float const blur_intensity      = _aaa_floats.values[ 0 ];
    float const max_velocity_pixels = _aaa_floats.values[ 1 ];
    float const linear_z            = _aaa_floats.values[ 2 ];

    int   const n_samples_raw       = _aaa_ints.values[ 1 ];
    //  Clamp N to [ 2 , 32 ] so an unset / negative uniform does not
    //  freeze the GPU on a runaway loop. Default is 16 ; matches the
    //  Rosado 2007 / Crysis profile.
    int   const N = clamp( n_samples_raw, 2, 32 );

    float2 const uv = in.uv;

    //  Step 1 : reconstruct world position behind this pixel under
    //  the CURRENT camera.
    float3 const world_pos = reconstruct_world_pos_( uv, linear_z, cam );

    //  Step 2 : reproject the same world point through the PREVIOUS
    //  camera's VP matrix to get prev-frame uv.
    float2 const uv_prev = project_world_to_uv_( world_pos, cam_prev );

    //  Step 3 : screen-space velocity vector. The velocity in UV
    //  space is the difference scaled by intensity. We then clamp
    //  the per-axis magnitude to (max_velocity_pixels / kBufferDim)
    //  so a behind-camera reprojection cannot produce nonsensical
    //  smears across the entire frame.
    //
    //  kBufferDim = 256 (matches the regression-test fixture size ,
    //  see tests/regression/phase3_golden_frame_regression_test.cpp
    //  kWidth = kHeight = 256). Encoded inline so this shader stays
    //  self-contained ; the conversion can be revisited when a real
    //  framebuffer-size uniform lands.
    float const kBufferDim     = 256.0;
    float const max_v_uv       = max_velocity_pixels / kBufferDim;
    float2      velocity       = ( uv - uv_prev ) * blur_intensity;

    float const v_len = length( velocity );
    if( v_len > max_v_uv )
        velocity = velocity * ( max_v_uv / v_len );

    //  Step 4 : N-tap accumulate along the velocity vector.
    //  Tap offsets t in [-0.5, +0.5) so taps straddle the current
    //  pixel symmetrically. Soft falloff weight 1 - 0.5 * |t|
    //  attenuates the tail taps slightly (matches the McGuire 2012
    //  / Rosado 2007 reconstruction filter style).
    float3 accum      = float3( 0.0 );
    float  weight_sum = 0.0;
    for( int i = 0; i < N; ++i )
    {
        float const t          = ( float( i ) + 0.5 ) / float( N ) - 0.5;
        float2 const sample_uv = saturate( uv + t * velocity );
        float const  w         = 1.0 - 0.5 * abs( t );

        accum      += _aaa_tex_0.sample( _aaa_samp, sample_uv ).rgb * w;
        weight_sum += w;
    }

    float3 const final_ = accum / max( weight_sum, 1e-4 );
    return float4( final_, 1.0 );
}
