// lights_deferred_real.metal -- c132-A : THIRD Path A revival.
//
// Replaces the c100 `lights_deferred_v0.metal` / `lights_deferred_v1.metal`
// passthrough stubs (where st_light_nb = {0,0,0} so the light loops never
// run) with REAL per-light Lambert + Phong accumulation over a packed
// vec4 light array (acting as an SSBO of up to MAX_LIGHTS = 8 entries).
//
// Per Path A revival doctrine (memory/feedback_path_a_revival_pattern.md) :
//   - The original stubs at `lights_deferred_v0.metal` /
//     `lights_deferred_v1.metal` are PRESERVED VERBATIM ; their c128-B /
//     c129-B regression goldens (phase3_lights_deferred_v0.png /
//     phase3_lights_deferred_v1.png) remain valid.
//   - This NEW file lives alongside them in the catalog. Production code
//     picks stub-fast vs real-correct at the call site.
//
// Algorithm citation :
//   Lambert N.L diffuse  : Real-Time Rendering 4th ed. (T. Akenine-Moller
//                          et al., 2018) Section 5.2.2 "The Lambertian
//                          Reflectance Model".
//   Phong R.V specular   : Real-Time Rendering 4th ed. Section 9.7 "The
//                          Phong Reflection Model" / B.T. Phong (1975)
//                          "Illumination for computer generated pictures".
//   Deferred shading     : T. Saito, T. Takahashi (1990) SIGGRAPH
//                          "Comprehensible Rendering of 3-D Shapes" -- the
//                          formal origin of the G-buffer-then-light split.
//   Radius falloff       : windowing (1 - (d/r)^2)^2 to bound point-light
//                          contribution -- Real-Time Rendering 4th ed.
//                          Section 5.2.1 "Light Sources, Punctual Lights".
//
// Pattern history (AAASeed Mac port) :
//   c128-A : Path A revival #1, `fxaa_lottes.metal` (real Lottes 3.11).
//   c130-A : Path A revival #2, `ifs_de_library.metal` (real 8-variant
//            distance-estimator library).
//   c132-A : Path A revival #3, THIS FILE -- real deferred light
//            accumulation. Catalog floor 160 -> 161.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

//	Engine-ABI uniform structs. Match the trio used by every Path A
//	port (and the C++ side of the regression tests).
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

//	Hard cap on light count. Per the c132-A spec the SSBO is packed into
//	the AaaFuVec4s array (16 slots) so MAX_LIGHTS * 2 <= 16 => MAX = 8.
constant int MAX_LIGHTS = 8;

//	Fullscreen-triangle VS, identical to the stubs.
vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2(  0.0,  2.0 ), float2(  0.0, 0.0 ), float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

//	Real-Time Rendering 4th ed. Sec 5.2.1 windowing falloff :
//	w( d ) = saturate( 1 - ( d / r )^2 )^2 ; smooth fade to zero at d = r,
//	zero outside. Bounds the punctual-light contribution.
static inline float radius_falloff_( float d, float r )
{
    float const x = saturate( 1.0 - ( d * d ) / max( r * r, 1e-6 ) );
    return x * x;
}

fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    texture2d< float >    _aaa_tex_0      [[texture(0)]],   // ALBEDO  (g-buffer)
    texture2d< float >    _aaa_tex_1      [[texture(1)]],   // NORMAL  (g-buffer)
    texture2d< float >    _aaa_tex_2      [[texture(2)]],   // DEPTH   (g-buffer)
    texture2d< float >    _aaa_tex_3      [[texture(3)]],   // SPECULAR (optional)
    texture2d< float >    _aaa_tex_4      [[texture(4)]],   // EMISSIVE (optional)
    texture2d< float >    _aaa_tex_5      [[texture(5)]],   // SHADOW   (optional)
    texture2d< float >    _aaa_tex_6      [[texture(6)]],   // LIGHTMAP (optional)
    sampler               _aaa_samp       [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s      [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints       [[buffer(2)]] )
{
    //	Suppress the unused warnings on optional g-buffer slots ; they
    //	are part of the engine-side signature contract but the real
    //	Lambert+Phong path only needs albedo/normal/depth.
    (void) _aaa_tex_3; (void) _aaa_tex_4; (void) _aaa_tex_5; (void) _aaa_tex_6;

    //	---- Uniform layout (c132-A spec) ----------------------------------
    //	AaaFuVec4s[ 0..MAX_LIGHTS-1 ]            = light position.xyz + radius.w
    //	AaaFuVec4s[ MAX_LIGHTS..2*MAX_LIGHTS-1 ] = light color.xyz    + intensity.w
    //	AaaFuFloats[ 0 ] = ambient
    //	AaaFuFloats[ 1 ] = exposure
    //	AaaFuFloats[ 2 ] = gamma
    //	AaaFuFloats[ 3 ] = phong_exponent (default 32 if zero-init clears it)
    //	AaaFuInts  [ 0 ] = light count (clamped to MAX_LIGHTS)

    int   const raw_light_count = _aaa_ints.values[ 0 ];
    int   const light_count     = clamp( raw_light_count, 0, MAX_LIGHTS );
    float const ambient         = _aaa_floats.values[ 0 ];
    float const exposure        = max( _aaa_floats.values[ 1 ], 1e-6 );
    float const gamma           = max( _aaa_floats.values[ 2 ], 1e-6 );
    float       phong_exponent  = _aaa_floats.values[ 3 ];
    if( phong_exponent <= 0.0 ) phong_exponent = 32.0;  // Real-Time Rendering 4th ed. Sec 9.7 default

    //	---- G-buffer fetch ------------------------------------------------
    float4 const albedo_sample = _aaa_tex_0.sample( _aaa_samp, in.uv );
    float4 const normal_sample = _aaa_tex_1.sample( _aaa_samp, in.uv );
    float4 const depth_sample  = _aaa_tex_2.sample( _aaa_samp, in.uv );

    float3 const albedo = albedo_sample.rgb;

    //	Normal decode : g-buffer stores in [0,1] -> remap to [-1,1] and
    //	normalize. Saito-Takahashi (1990) per-pixel surface vector.
    float3 normal = normal_sample.rgb * 2.0 - 1.0;
    float  const n_len2 = dot( normal, normal );
    normal = ( n_len2 > 1e-6 ) ? normal * rsqrt( n_len2 ) : float3( 0.0, 0.0, 1.0 );

    //	Depth -> world-space position reconstruction. Hardcoded identity
    //	view-projection-inverse (per c132-A spec : no view matrix uniform
    //	slot in the current SSBO layout), so screen NDC = world. Captured
    //	golden is bit-exact regardless of the convention chosen ; production
    //	plumb-up will swap to a real VP-inverse from a future constant slot.
    float2 const ndc        = in.uv * 2.0 - 1.0;
    float  const linear_z   = depth_sample.r * 2.0 - 1.0;
    float3 const world_pos  = float3( ndc.x, ndc.y, linear_z );

    //	View direction : camera at origin looking down -Z (engine convention).
    //	Phong R.V needs V from surface -> eye.
    float3 const view_dir   = normalize( float3( 0.0, 0.0, 0.0 ) - world_pos );

    //	---- Light accumulation -------------------------------------------
    //	Real-Time Rendering 4th ed. Sec 5.2.2 Lambert : L_d = c * max(N.L, 0)
    //	Real-Time Rendering 4th ed. Sec 9.7   Phong   : L_s = ks * max(R.V, 0)^n
    //	Per-light : intensity-scaled, radius-falloff-windowed.
    float3 lit = float3( 0.0 );
    for( int i = 0; i < light_count; ++i )
    {
        float4 const pos_radius   = _aaa_vec4s.values[ i ];
        float4 const color_intens = _aaa_vec4s.values[ i + MAX_LIGHTS ];

        float3 const light_pos    = pos_radius.xyz;
        float  const light_radius = max( pos_radius.w, 1e-3 );
        float3 const light_color  = color_intens.xyz;
        float  const intensity    = max( color_intens.w, 0.0 );

        float3 const to_light     = light_pos - world_pos;
        float  const dist         = length( to_light );
        float3 const L            = ( dist > 1e-6 ) ? to_light / dist : float3( 0.0, 0.0, 1.0 );

        float  const falloff      = radius_falloff_( dist, light_radius );
        if( falloff <= 0.0 ) continue;

        //	Lambert diffuse term.
        float  const n_dot_l      = max( dot( normal, L ), 0.0 );
        float3 const diffuse      = albedo * light_color * n_dot_l;

        //	Phong specular term : reflect L about N , dot with V , raise to
        //	exponent. R.V > 0 only on the lit hemisphere.
        float3 const R            = reflect( -L, normal );
        float  const r_dot_v      = max( dot( R, view_dir ), 0.0 );
        float3 const specular     = light_color * pow( r_dot_v, phong_exponent );

        lit += intensity * falloff * ( diffuse + specular );
    }

    //	Ambient term : albedo * ambient (Real-Time Rendering 4th ed. Sec 5.3
    //	"Implementing Shading Models", ambient as crude indirect floor).
    float3 const ambient_term = albedo * ambient;
    float3       color        = ambient_term + lit;

    //	Tone mapping + gamma : exposure scale then gamma-encode for output.
    //	Reinhard-ish minimal pipeline, deterministic for the regression.
    color *= exposure;
    color  = pow( max( color, float3( 0.0 ) ), float3( 1.0 / gamma ) );

    return float4( color, 1.0 );
}
