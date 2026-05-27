// aaa_material_pbr.metal -- c134-B : FIFTH Path A revival.
//
// Real Cook-Torrance microfacet BRDF for the `aaa_material` engine
// global. The historic stub behavior (per the c102 / c103 / c128-B
// notes on gbuffer.metal and friends) is "flat albedo with at most a
// constant ambient lift" : `aaa_material.material` collapses to a
// pass-through colour. This file is the physically-based replacement :
// GGX NDF + Smith Schlick-GGX geometry + Schlick Fresnel combined into
// the Cook-Torrance specular term, plus a Lambert diffuse lobe and a
// single directional light. Production picks stub-fast vs real-correct
// at the call site.
//
// Per Path A revival doctrine (memory/feedback_path_a_revival_pattern.md) :
//   - The original "flat albedo" stub behaviour (engine-global identity
//     plumbed through gbuffer.metal et al.) is PRESERVED. This new file
//     lives alongside the existing catalog ; it does NOT modify any
//     existing shader source.
//   - The c121-A / c121-B / c128-B regression goldens of every shader
//     that consumes `aaa_material.material` remain bit-exactly valid.
//   - This file is the canonical end-to-end demonstration of a real
//     physically-based BRDF flowing through GOL::Backend's fragment-
//     uniform slots to MSL with the engine-ABI struct shape used by
//     the rest of the Path A catalog.
//
// Algorithm citations (inline below at each term) :
//   - Cook + Torrance 1982 ACM ToG, "A Reflectance Model for
//     Computer Graphics" -- the original microfacet BRDF formulation.
//   - Walter et al. 2007 EGSR, "Microfacet Models for Refraction
//     through Rough Surfaces" -- GGX (Trowbridge-Reitz) NDF.
//   - Schlick 1994 CGF, "An Inexpensive BRDF Model for Physically-
//     based Rendering" -- Fresnel approximation F0 + (1-F0)(1-cosT)^5.
//   - Smith 1967 IEEE TAP, "Geometrical shadowing of a random rough
//     surface" -- the Smith G geometry function.
//   - Karis 2013 SIGGRAPH "Real Shading in Unreal Engine 4" --
//     remapping k = (alpha+1)^2 / 8 for analytic (direct) lights ;
//     diffuse / specular energy split via (1 - F)*(1 - metallic).
//   - Real-Time Rendering 4th ed. (Akenine-Moller et al., 2018)
//     Sections 9.5 to 9.8 -- canonical recap of the entire pipeline.
//
// Pattern history (AAASeed Mac port) :
//   c128-A : Path A revival #1, fxaa_lottes.metal (Lottes 3.11).
//   c130-A : Path A revival #2, ifs_de_library.metal (8-variant DE).
//   c132-A : Path A revival #3, lights_deferred_real.metal
//            (real Lambert + Phong deferred light accumulation).
//   c133-B : Path A revival #4, aaa_cam_real.metal (real camera
//            constant block).
//   c134-B : Path A revival #5, THIS FILE -- real Cook-Torrance PBR.
//            Catalog floor 162 -> 163.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

//	Engine-ABI uniform structs. Match the trio used by every Path A
//	port (and the C++ side of the regression tests).
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//	Mathematical constant. M_PI_F from metal_stdlib is also available
//	but defining locally keeps the citation comments self-contained.
constant float kPi = 3.14159265358979323846;

//	Fullscreen-triangle VS, identical in shape to lights_deferred_real.
//	The fragment stage owns the procedural albedo + normal derivation
//	from `uv`, so the regression test only has to supply uniforms.
vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2(  0.0,  2.0 ), float2(  0.0, 0.0 ), float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

//	---- GGX Normal Distribution Function -----------------------------------
//	Walter et al. 2007 EGSR, "Microfacet Models for Refraction through
//	Rough Surfaces", Equation 33 :
//	    D_GGX( n, h ) = alpha^2 / ( pi * ( (n.h)^2 * (alpha^2 - 1) + 1 )^2 )
//	with alpha = roughness^2 (the perceptually-linear roughness mapping
//	from Burley 2012 / Karis 2013).
static inline float D_GGX_( float n_dot_h, float roughness )
{
    float const alpha   = roughness * roughness;
    float const alpha2  = alpha * alpha;
    float const denom_a = n_dot_h * n_dot_h * ( alpha2 - 1.0 ) + 1.0;
    float const denom   = kPi * denom_a * denom_a;
    return alpha2 / max( denom, 1e-7 );
}

//	---- Smith Schlick-GGX geometry function --------------------------------
//	Smith 1967 IEEE TAP, "Geometrical shadowing of a random rough surface"
//	for the separable G = G1(n.v) * G1(n.l). Karis 2013 SIGGRAPH "Real
//	Shading in Unreal Engine 4" gives the analytic-light remap :
//	    k = ( alpha + 1 )^2 / 8       (alpha = roughness here, NOT roughness^2)
//	    G1( x ) = x / ( x * ( 1 - k ) + k )
//	Karis uses `roughness` directly in the k formula (not roughness^2) ;
//	this is the standard UE4 / Filament path.
static inline float G1_SchlickGGX_( float n_dot_x, float k )
{
    return n_dot_x / max( n_dot_x * ( 1.0 - k ) + k, 1e-7 );
}

static inline float G_Smith_( float n_dot_v, float n_dot_l, float roughness )
{
    float const k = ( roughness + 1.0 ) * ( roughness + 1.0 ) / 8.0;
    return G1_SchlickGGX_( n_dot_v, k ) * G1_SchlickGGX_( n_dot_l, k );
}

//	---- Schlick Fresnel ----------------------------------------------------
//	Schlick 1994 CGF, "An Inexpensive BRDF Model for Physically-based
//	Rendering" :  F(v,h) = F0 + ( 1 - F0 ) * ( 1 - v.h )^5
//	For metals F0 == albedo (the tint of the metal) ; for dielectrics F0
//	is a constant ~0.04 ; mix() lerps between them by metallic.
static inline float3 F_Schlick_( float v_dot_h, float3 F0 )
{
    float const x  = saturate( 1.0 - v_dot_h );
    float const x2 = x * x;
    return F0 + ( float3( 1.0 ) - F0 ) * ( x2 * x2 * x );
}

//	---- Procedural g-buffer derived from screen UV -------------------------
//	The c134-B regression doesn't carry a real texture g-buffer ; instead
//	the fragment generates a deterministic "scene" from `uv` :
//	  - Albedo : 4x4 checkerboard of red / mid-grey -- mirrors c132-A
//	             checker fixture so the eye can compare regressions
//	             at a glance.
//	  - Normal : outward-tilted ( (u,v,1) normalized ) so the n.l and
//	             n.v dot products vary smoothly across the frame.
//	  - World position : ( ndc.x, ndc.y, 0 ) -- the surface is the
//	             rasterized fullscreen quad lying at z = 0 in a
//	             trivial view space whose camera lives on +z.
static inline float3 checker_albedo_( float2 uv, float3 albedo_uniform )
{
    int const tx = int( uv.x * 4.0 );
    int const ty = int( uv.y * 4.0 );
    bool const  light = ( ( tx ^ ty ) & 1 ) == 0;
    float3 const dark = float3( 0.6, 0.05, 0.05 );    // red-tinged dark
    float3 const grey = float3( 0.5, 0.5, 0.5 );
    //	uniform albedo channel modulates the checker so a stub-config
    //	(albedo = pure base) is still distinguishable from the PBR-config
    //	in the RevivalProof test.
    return mix( light ? grey : dark, albedo_uniform, 0.5 );
}

static inline float3 procedural_normal_( float2 uv )
{
    float const u = uv.x * 2.0 - 1.0;
    float const v = uv.y * 2.0 - 1.0;
    float const inv_len = rsqrt( u * u + v * v + 1.0 );
    return float3( u * inv_len, v * inv_len, 1.0 * inv_len );
}

//	---- Fragment : Cook-Torrance PBR ---------------------------------------
//	Combine :
//	    BRDF_spec = D * G * F / ( 4 * (n.l) * (n.v) )
//	    BRDF_diff = ( 1 - F ) * ( 1 - metallic ) * albedo / pi
//	    L_o       = ambient + ( BRDF_diff + BRDF_spec ) * (n.l) * light_color
//	                                                    * light_intensity
//	                + emissive_color * emissive_intensity
//	Then exposure scale + gamma encode.
//
//	Uniform slot layout (c134-B spec) :
//	   AaaFuVec4s[ 0 ] = albedo.rgb + metallic.w
//	   AaaFuVec4s[ 1 ] = ( roughness, ambient_occlusion, emissive_intensity, ior )
//	   AaaFuVec4s[ 2 ] = emissive_color.rgb + pad
//	   AaaFuVec4s[ 3 ] = light_dir.xyz + light_intensity.w
//	   AaaFuVec4s[ 4 ] = light_color.rgb + pad
//	   AaaFuVec4s[ 5 ] = view_pos.xyz + exposure.w
//	   AaaFuFloats[ 0 ] = gamma
fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]] )
{
    (void) _aaa_ints;  //	reserved -- not used in c134-B layout.

    //	---- Unpack material UBO -------------------------------------------
    float4 const mat0       = _aaa_vec4s.values[ 0 ];
    float4 const mat1       = _aaa_vec4s.values[ 1 ];
    float4 const mat2       = _aaa_vec4s.values[ 2 ];
    float4 const light_di   = _aaa_vec4s.values[ 3 ];
    float4 const light_col4 = _aaa_vec4s.values[ 4 ];
    float4 const view_exp   = _aaa_vec4s.values[ 5 ];
    float  const gamma      = max( _aaa_floats.values[ 0 ], 1e-6 );

    float3 const albedo_u   = mat0.xyz;
    float  const metallic   = saturate( mat0.w );
    float  const roughness  = clamp( mat1.x, 0.04, 1.0 );   //	Disney floor 0.04
    float  const ao         = saturate( mat1.y );
    float  const emi_inten  = max( mat1.z, 0.0 );
    //	mat1.w = ior -- reserved for a later dielectric-F0 derivation
    //	(F0 = ((ior-1)/(ior+1))^2). Per c134-B spec we use the standard
    //	0.04 dielectric F0 ; the ior field is uniform-slot-reserved so a
    //	follow-up session can swap in F0_from_ior without churning the
    //	uniform layout.
    (void) mat1.w;

    float3 const emissive_color  = mat2.xyz;
    float3 const light_dir       = normalize( light_di.xyz + float3( 0.0, 0.0, 1e-6 ) );
    float  const light_intensity = max( light_di.w, 0.0 );
    float3 const light_color     = light_col4.xyz;
    float3 const view_pos        = view_exp.xyz;
    float  const exposure        = max( view_exp.w, 1e-6 );

    //	---- Procedural surface --------------------------------------------
    float3 const albedo    = checker_albedo_( in.uv, albedo_u );
    float3 const N         = procedural_normal_( in.uv );
    //	Surface world position : the fullscreen quad lives at z = 0 in
    //	an arbitrary view ; ndc.xy maps to world.xy.
    float3 const world_pos = float3( in.uv * 2.0 - 1.0, 0.0 );

    //	---- View direction + half vector ----------------------------------
    float3 const V         = normalize( view_pos - world_pos );
    float3 const L         = light_dir;
    float3 const H         = normalize( V + L );

    float  const n_dot_v   = max( dot( N, V ), 0.0 );
    float  const n_dot_l   = max( dot( N, L ), 0.0 );
    float  const n_dot_h   = max( dot( N, H ), 0.0 );
    float  const v_dot_h   = max( dot( V, H ), 0.0 );

    //	---- F0 : dielectric 0.04 mix into albedo for metals ---------------
    //	Real-Time Rendering 4th ed. Sec 9.5.3 -- the standard PBR F0 split.
    float3 const F0 = mix( float3( 0.04 ), albedo, metallic );

    //	---- BRDF terms ----------------------------------------------------
    float  const D = D_GGX_( n_dot_h, roughness );
    float  const G = G_Smith_( n_dot_v, n_dot_l, roughness );
    float3 const F = F_Schlick_( v_dot_h, F0 );

    //	Cook-Torrance specular : numerator D*G*F , denominator 4*(n.l)*(n.v).
    //	The 1e-4 floor on the denominator protects against horizon-grazing
    //	rays where either dot product vanishes.
    float3 const spec_num   = D * G * F;
    float  const spec_denom = 4.0 * n_dot_v * n_dot_l + 1e-4;
    float3 const specular   = spec_num / spec_denom;

    //	Energy-conserving Lambert diffuse : kD = (1 - F) , then suppress
    //	for metals (which have no diffuse lobe). Karis 2013 / Real-Time
    //	Rendering 4th ed. Sec 9.5.2.
    float3 const kD       = ( float3( 1.0 ) - F ) * ( 1.0 - metallic );
    float3 const diffuse  = kD * albedo / kPi;

    //	---- Light accumulation -------------------------------------------
    //	Single directional light per c134-B spec. Extension to N lights
    //	is a uniform-slot trivially -- gate `light_intensity > 0` for the
    //	stub-comparable path.
    float3 const direct = ( diffuse + specular ) * light_color * ( light_intensity * n_dot_l );

    //	Ambient floor : ao * albedo * 0.05 . Captures the indirect-light
    //	floor without a full IBL ; Real-Time Rendering 4th ed. Sec 11.3
    //	"Ambient and Environment Lighting" treats this as an acceptable
    //	approximation for direct-lighting test rigs.
    float3 const ambient = ao * albedo * 0.05;

    //	Emissive add : pure additive, no Fresnel modulation.
    float3 const emissive = emissive_color * emi_inten;

    float3 color = ambient + direct + emissive;

    //	---- Tone mapping + gamma encode ----------------------------------
    color *= exposure;
    color  = pow( max( color, float3( 0.0 ) ), float3( 1.0 / gamma ) );

    return float4( color, 1.0 );
}
