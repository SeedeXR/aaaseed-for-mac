// fxaa.metal -- Path A port from vux_flare_render/fxaa.frag (c102, ~631 LOC source).
// NVIDIA FXAA 3.11 by Timothy Lottes -- canonical screen-space anti-aliasing.
// **Compile-shape minimal port** : engine source's `FxaaPixelShader()` is ~500 LOC
// of luminance edge detection + sub-pixel jitter + 8-step edge march. Mac stubs
// the body as a passthrough (returns the centre tap) so the catalog compile-shape
// passes ; the real FXAA can be revived later by porting the FxaaPixelShader
// helper following the Lottes whitepaper.
// Patches :
//   - `g_input_texture` -> `_aaa_tex_0`
//   - `settings_rcpFrame` -> `AaaFuFloats[0..1]`
//   - `aaa_vu_int[0]` (bypass) -> AaaFuInts[0]
//   - `layout(location = 0) in vec2 in_texcoord` -> `in.uv`
//   - FxaaPixelShader -> passthrough stub returning centre RGB

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

//	FxaaPixelShader passthrough stub. Real port deferred to a dedicated FXAA
//	session ; the algorithm is the canonical Lottes 3.11 with ~25 helper
//	defines (FxaaLuma, FxaaToFloat3, FxaaTexLuma, etc.) and an 8-step edge
//	march in two directions. Catalog-compile shape preserved by returning
//	the centre tap.
static inline float3 FxaaPixelShader_(
    float2 uv, texture2d< float > tex, sampler samp, float2 rcpFrame )
{
    (void) rcpFrame;
    return tex.sample( samp, uv ).rgb;
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

    float2 const settings_rcpFrame = float2( _aaa_floats.values[ 0 ], _aaa_floats.values[ 1 ] );
    float3 const r = FxaaPixelShader_( in.uv, _aaa_tex_0, _aaa_samp, settings_rcpFrame );
    return float4( r, 1.0 );
}
