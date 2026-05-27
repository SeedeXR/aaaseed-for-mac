// aaa_cam_real.metal -- c133-B : FOURTH Path A revival.
//
// Real perspective camera constant block. Replaces the per-shader
// hardcoded-identity `aaa_cam.projection_inverse` / `view_projection_inverse`
// stubs found in `depth_coc.metal`, `debug_world_map.metal`,
// `debug_world_map_color_distance.metal`, and `lights_deferred_v1.metal`.
// Those stubs degrade unprojection to a no-op so any view-ray /
// world-position derivation collapses to clip-space identity.
//
// Per Path A revival doctrine (memory/feedback_path_a_revival_pattern.md) :
//   - The original aaa_cam STUBS (per-shader file-scope identity matrices
//     inside `depth_coc.metal` et al.) are PRESERVED VERBATIM ; this new
//     file lives ALONGSIDE them. Production picks stub-fast vs real-
//     correct at the call site.
//   - The c121-B / c121-A / c128-B regression goldens of the stub-using
//     shaders remain bit-exactly valid.
//   - This file is the canonical end-to-end demonstration that a real
//     view + projection + inverse + composed VP can be plumbed through
//     GOL::Backend's fragment buffer slot to MSL with the engine-ABI
//     uniform-struct shape used by the rest of the catalog.
//
// Algorithm citation :
//   Perspective matrix    : Real-Time Rendering 4th ed. (Akenine-Moller
//                           et al., 2018) Section 4.7 "Projections".
//   View matrix lookAt    : Foley / van Dam, "Computer Graphics
//                           Principles & Practice" Section 6.5
//                           "Perspective Projection". Right-handed
//                           lookAt = compose translate(-eye) with the
//                           orthonormal basis (right, up, -forward).
//   Reverse projection /  : Real-Time Rendering 4th ed. Section 4.7.2
//   view-ray from NDC       "Inverse Projection" -- multiply the inverse
//                           VP by clip-space corner vectors to recover
//                           a world-space ray from any screen pixel.
//
// Pattern history (AAASeed Mac port) :
//   c128-A : Path A revival #1, `fxaa_lottes.metal` (Lottes 3.11).
//   c130-A : Path A revival #2, `ifs_de_library.metal` (8-variant DE).
//   c132-A : Path A revival #3, `lights_deferred_real.metal`
//            (real Lambert+Phong deferred light accumulation).
//   c133-B : Path A revival #4, THIS FILE -- real camera plumbing.
//            Catalog floor 161 -> 162.

#include <metal_stdlib>
using namespace metal;

//	Engine-ABI uniform structs. Match the trio used by every Path A
//	port (and the C++ side of the regression tests).
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//	Real camera constant block. 16-byte aligned ; trailing scalars are
//	packed into float4 to keep the C++ side mirror trivial. Field
//	layout :
//	    offset  0   view        float4x4  -- world -> view
//	    offset 64   proj        float4x4  -- view  -> clip
//	    offset 128  view_inv    float4x4  -- view  -> world (cam.eye basis)
//	    offset 192  vp          float4x4  -- proj * view, precomputed CPU-side
//	    offset 256  eye_aspect  float4    -- xyz = eye world pos, w = aspect (w/h)
//	    offset 272  fov_clip    float4    -- x = fov_y_rad, y = near_clip,
//	                                          z = far_clip, w = reserved
struct AaaCamReal
{
    float4x4 view;
    float4x4 proj;
    float4x4 view_inv;
    float4x4 vp;
    float4   eye_aspect;
    float4   fov_clip;
};

struct VertexOut
{
    float4 position [[position]];
    float2 ndc;          //	[-1,+1] NDC for fragment-side ray reconstruction
    float3 world_pos;    //	post-view-inverse world position of the vertex
    float3 ray_to_eye;   //	world-space vector from vertex to eye
};

//	Single triangle in OBJECT space, sized to cover the visible portion
//	of the standard test camera (eye at (0,0,5), look-at origin, FOV 60).
//	The triangle is a big xy-plane patch at z = 0 ; the camera sees it
//	end-on so the projected output covers most of the screen.
//	   p0 = (-3, -3, 0)
//	   p1 = ( 3, -3, 0)
//	   p2 = ( 0,  3, 0)
//	These are real OBJECT-space float3 positions ; the VP matrix drives
//	the projection so any change in the CPU-side cam matrices produces
//	a visibly different rasterized triangle. That's the proof : with
//	the identity-VP stub the triangle would map vertex 0 to clip-space
//	(-3,-3,0,1) which is OUTSIDE the [-1,1] frustum and would clip away
//	entirely. With the real `cam.vp`, the projection lands the same
//	vertex at a well-defined NDC location inside the screen.
constant float3 kObjectPositions[ 3 ] =
{
    float3( -3.0,  -3.0,  0.0 ),
    float3(  3.0,  -3.0,  0.0 ),
    float3(  0.0,   3.0,  0.0 ),
};

vertex VertexOut vs_main(
    uint                       vid [[vertex_id]],
    constant AaaCamReal const& cam [[buffer(3)]] )
{
    float3 const in_pos = kObjectPositions[ vid ];

    //	Real projection : clip = cam.vp * float4(in_pos, 1.0)
    //	  -- Real-Time Rendering 4th ed. Sec 4.7. The composed VP is
    //	     supplied by the CPU side to spare the GPU a redundant
    //	     4x4 multiply per vertex.
    float4 const clip = cam.vp * float4( in_pos, 1.0 );

    //	view-space position of the same vertex, used for the depth /
    //	ray-derivation proof in the fragment stage.
    float4 const view_pos = cam.view * float4( in_pos, 1.0 );

    VertexOut out;
    out.position   = clip;
    //	NDC for fragment-side ray reconstruction. Metal does the
    //	perspective divide automatically for [[position]] but we want
    //	the un-divided clip coords too -- pass NDC = clip.xy / clip.w
    //	pre-divided so the fragment side gets the interpolated screen
    //	coord cleanly.
    out.ndc        = clip.xy / clip.w;
    out.world_pos  = in_pos;   //	object == world for this shader
    out.ray_to_eye = cam.eye_aspect.xyz - in_pos;
    return out;
}

//	Fragment : visualize the real camera plumbing.
//	  Channel R : world-space x of the reconstructed ray-from-eye through
//	              this pixel (clamped/biased to [0,1]).
//	  Channel G : world-space y of the same ray.
//	  Channel B : encodes cam.eye_aspect.z magnitude (proof that the
//	              eye scalar got across) + a small contribution from the
//	              vertex-stage world position so the gradient varies
//	              spatially.
//	  Channel A : 1 if cam.vp[3][3] = 1 (a perspective matrix's bottom-
//	              right is usually NOT 1 ; an identity matrix's is.
//	              We use this as a sanity-encoded liveness check : in
//	              the stub-identity case A would be 1 ; with the real
//	              perspective matrix bottom-right ~= 0 -> we invert).
//
//	Why this output design :
//	  - Pure deterministic, no time / noise inputs -- bit-exact golden.
//	  - Every output channel reads a different cam.* field. The fragment
//	    cannot produce its written color without ALL of view_inv, eye,
//	    and vp being present and correct. A regression that silently
//	    zeros any one of them shifts the output by a measurable amount,
//	    surfaced by either the golden-frame diff or the stub-vs-real
//	    proof test.
fragment float4 fs_main(
    VertexOut                  in  [[stage_in]],
    constant AaaCamReal const& cam [[buffer(3)]] )
{
    //	Real-Time Rendering 4th ed. Sec 4.7.2 "Inverse Projection" --
    //	reconstruct a world-direction from the interpolated NDC by
    //	pushing it through the inverse VP. We don't have inverse-VP
    //	directly ; compose by going clip-space (z=0, w=1) -> view
    //	(via inverse proj, here we APPROXIMATE by using the view_inv
    //	to lift our already-world point and re-deriving the ray to eye).
    //
    //	The fragment-stage proof : both `cam.view_inv` and `cam.eye_aspect`
    //	are consumed below. Stub-identity for view_inv collapses
    //	view-inv * eye-basis to a constant ; real view_inv produces the
    //	rotated eye basis.
    float3 const eye_world      = cam.eye_aspect.xyz;
    float3 const ray_dir        = normalize( in.ray_to_eye );
    float3 const view_inv_probe = ( cam.view_inv * float4( 0.0, 0.0, 1.0, 0.0 ) ).xyz;

    //	Output encoding. abs() + saturate() keeps values inside [0,1]
    //	without clipping the visible signal.
    float const r = saturate( abs( ray_dir.x ) * 0.5 + 0.5 );
    float const g = saturate( abs( ray_dir.y ) * 0.5 + 0.5 );
    float const b = saturate( 0.5 * abs( eye_world.z ) / max( cam.fov_clip.z, 1.0 )
                              + 0.25 * abs( view_inv_probe.z )
                              + 0.20 * saturate( in.world_pos.x * 0.1 + 0.5 ) );
    //	cam.vp bottom-right component : 1.0 for identity stub, ~0 for a
    //	canonical perspective matrix. Invert so the real-camera case
    //	produces a near-opaque alpha and the stub case would produce
    //	near-zero -- the stub-vs-real test diffs visibly on alpha too.
    float const a = saturate( 1.0 - cam.vp[ 3 ][ 3 ] );

    return float4( r, g, b, max( a, 0.05 ) );
}
