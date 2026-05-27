// ps_fontsdf.metal -- Path A port from ps_fontsdf.frag (c79).
// MSDF / SDF font rendering with screen-space anti-aliasing.
// Patches :
//   - `layout(location=0) in vec2 out_uv` -> `in.uv.xy`
//   - `aaa_samp0` -> `_aaa_tex_0`
//   - `aaa_fu_vec4[0]` -> `_aaa_vec4s.values[0]`
//   - `dFdx` / `dFdy` / `fwidth` map 1:1 to MSL equivalents
//   - `textureSize( tex, 0 )` -> `int2( tex.get_width(), tex.get_height() )`
//   - `inversesqrt` -> MSL `rsqrt`
//   - `gl_FragColor` -> return value

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuVec4s { float4 values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float median( float r, float g, float b )
{
    return max( min( r, g ), min( max( r, g ), b ) );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    float4 const uColor = _aaa_vec4s.values[ 0 ];
    float2 const pos    = in.uv.xy;
    float3 const samp   = _aaa_tex_0.sample( _aaa_samp, pos ).rgb;
    int2   const sz     = int2( _aaa_tex_0.get_width(), _aaa_tex_0.get_height() );

    float const dx       = dfdx( pos.x ) * float( sz.x );
    float const dy       = dfdy( pos.y ) * float( sz.y );
    float const toPixels = 8.0 * rsqrt( dx * dx + dy * dy );
    (void) toPixels;   // referenced by name but not used past this assignment
    float const sigDist  = median( samp.r, samp.g, samp.b );
    float const w        = fwidth( sigDist );
    float const opacity  = smoothstep( 0.5 - w, 0.5 + w, sigDist );

    return float4( uColor.rgb, opacity * uColor.a );
}
