// ps_toon2.metal -- Path A port from ps_toon2.frag (c90).
// Toon shader (silhouette + specular threshold + diffuse step).
// Patches :
//   - `in float specular` / `in float diffuse` / `in float sil` (P4 per-vertex)
//     -> const stubs : diffuse=0.7, sil=0.8 (mid-tone-bright defaults).
//   - **Upstream-stub recovery** : engine source references `spec` inside the
//     `else` branch but never declares it in the live path (the original
//     `float spec = pow(...)` is commented out). Stub `spec = 0.0` (no-specular
//     identity for the `spec < 0.2` threshold).
//   - `fu_float[0]` (non-array) -> `_aaa_floats.values[0]` (silhouetteThreshold)
//   - `aaa_fu_vec4[0..2]` -> `_aaa_vec4s.values[0..2]` (material / silhouette / specular)
//   - `gl_FragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    (void) in;

    //	P4 stubs (per-vertex inputs unplumbed on Mac) :
    float const diffuse = 0.7;   // mid-bright
    float const sil     = 0.8;   // non-silhouette default
    //	Upstream-stub : engine source uses `spec` undeclared in live path.
    float const spec    = 0.0;   // no-specular identity for `spec < 0.2`

    float const silhouetteThreshold = _aaa_floats.values[ 0 ];

    if( sil < silhouetteThreshold )
        return _aaa_vec4s.values[ 1 ];   // silhouetteColor

    float4 result = _aaa_vec4s.values[ 0 ];   // materialColor
    if( spec < 0.2 )
        result *= 0.98;
    else
        result = _aaa_vec4s.values[ 2 ];     // specularColor

    if( diffuse < 0.5 )
        result *= 0.9;

    return result;
}
