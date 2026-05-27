// HypnoLord.metal -- Path A port from Voxel/HypnoLord.frag (c88, ~280 LOC).
// Volumetric voxel raymarch with LFSR-noise + xfer-function colour lookup
// + per-fragment 6-tap gradient + Phong lighting.
// Patches :
//   - `in vec3 pv` / `in vec3 dv` (per-vertex ray origin/direction) -> P4 stubs
//     `pv` -> `float3( 0.0, 0.0, -1.0 )`, `dv` -> `float3( in.uv.x * 2.0 - 1.0,
//      in.uv.y * 2.0 - 1.0, 1.0 )`. Pipeline shape valid ; real per-vertex
//     plumbing pending.
//   - `voltex` (sampler3D) -> `texture3d< float > _aaa_tex_3d_0 [[texture(0)]]`
//   - `xfer_func` (sampler2D) -> `_aaa_tex_1 [[texture(1)]]`
//   - `time` / `v1` / `v2` / `noise` individual uniforms -> AaaFuFloats[0..3]
//   - `layout(location = 0) out vec4 o0` -> return value
//   - `discard` -> `discard_fragment()`
//   - file-scope `vec3 lightdir = vec3(...);` etc. (P5) -> moved into fs_main
//   - integer-bitwise ops (LFSR_Rand_Gen) -> MSL supports int<<, ^, &, 0x7fffffff

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float values[ 16 ]; };

constant float EPSILON_ = 0.0001;
constant float PP_      = 1.0;
constant float PX_      = PP_ / 256.0;
constant float PY_      = PP_ / 256.0;
constant float PZ_      = PP_ / 256.0;
constant int   STEPS_   = 800;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline int LFSR_Rand_Gen_( int n )
{
    n = ( n << 13 ) ^ n;
    return ( n * ( n * n * 15731 + 789221 ) + 1376312589 ) & 0x7fffffff;
}

static inline float noise3f_( float3 p )
{
    int3 ip = int3( floor( p ) );
    int  n  = ip.x + ip.y * 57 + ip.z * 113;
    return float( LFSR_Rand_Gen_( n ) ) / 1073741824.0 - 0.5;
}

static inline float2 box_( float3 p, float3 d, float3 c, float3 s )
{
    float3 bl = c - s, bh = c + s;
    float3 ol = ( bl - p ) / d, oh = ( bh - p ) / d;
    float3 l = min( ol, oh );
    float3 h = max( ol, oh );
    float  ff = min( h.x, min( h.y, h.z ) );
    float  fn = max( max( l.x, 0.0 ), max( l.y, l.z ) );
    return float2( step( fn, ff ), fn );
}

fragment float4 fs_main(
    VertexOut             in              [[stage_in]],
    texture3d< float >    _aaa_tex_3d_0   [[texture(0)]],
    texture2d< float >    _aaa_tex_1      [[texture(1)]],
    sampler               _aaa_samp       [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats     [[buffer(0)]] )
{
    float const time_  = _aaa_floats.values[ 0 ];
    float const v1     = _aaa_floats.values[ 1 ];
    float const v2     = _aaa_floats.values[ 2 ];
    float const noise_ = _aaa_floats.values[ 3 ];
    (void) time_;   // engine uniform unused in live raymarch ; kept for parity

    //	P4 stubs for per-vertex ray inputs
    float3 const pv = float3( 0.0, 0.0, -1.0 );
    float3 const dv = float3( in.uv.x * 2.0 - 1.0, in.uv.y * 2.0 - 1.0, 1.0 );

    float3 const lightdir       = float3( 0.0, -1.0, 0.0 );
    float  const ambientParam   = 0.2;
    float  const diffuseParam   = 0.25;
    float  const specularParam  = 0.25;
    float  const shininessParam = 128.0;

    float3 const p = pv;
    float3 const d = normalize( dv );
    float2 const h = box_( p, d, float3( 0.0 ), float3( 1.0 ) );
    if( h.x == 0.0 )
        discard_fragment();

    float3 const t = ( d * h.y + p ) * 0.5 + 0.5;

    float3 const raydir = d * 0.001;
    float4 sum = float4( 0.0 );

    float  const nnn = 0.25 * ( 2.0 + noise3f_( float3( in.position.xy, 1.0 ) ) );
    float3 sample_position  = t + raydir * ( noise_ * nnn );
    float3 sample_position1 = sample_position + float3( PX_, 0.0, 0.0 );
    float3 sample_position2 = sample_position - float3( PX_, 0.0, 0.0 );
    float3 sample_position3 = sample_position + float3( 0.0, PY_, 0.0 );
    float3 sample_position4 = sample_position - float3( 0.0, PY_, 0.0 );
    float3 sample_position5 = sample_position + float3( 0.0, 0.0, PZ_ );
    float3 sample_position6 = sample_position - float3( 0.0, 0.0, PZ_ );

    for( int i = 0; i < STEPS_; i++ )
    {
        sample_position += raydir;

        float4 samp = _aaa_tex_3d_0.sample( _aaa_samp, sample_position );

        sample_position1 += raydir;
        sample_position2 += raydir;
        sample_position3 += raydir;
        sample_position4 += raydir;
        sample_position5 += raydir;
        sample_position6 += raydir;

        if( samp.x < 0.15 ) samp.x = 0.0;

        float3 backward;
        backward.x = _aaa_tex_3d_0.sample( _aaa_samp, sample_position2 ).x;
        backward.y = _aaa_tex_3d_0.sample( _aaa_samp, sample_position4 ).x;
        backward.z = _aaa_tex_3d_0.sample( _aaa_samp, sample_position6 ).x;
        float3 fwd;
        fwd.x = _aaa_tex_3d_0.sample( _aaa_samp, sample_position1 ).x;
        fwd.y = _aaa_tex_3d_0.sample( _aaa_samp, sample_position3 ).x;
        fwd.z = _aaa_tex_3d_0.sample( _aaa_samp, sample_position5 ).x;

        float3 gradient = backward - fwd;
        float  l = length( gradient );
        (void) l;
        gradient = normalize( gradient );

        float4 color = _aaa_tex_1.sample( _aaa_samp, float2( backward.x + v1, fwd.x + v2 ) );
        float const LdotG    = clamp( dot( lightdir, gradient ), 0.0, 1.0 );
        float const diffuse  = LdotG;
        float const specular = pow( LdotG, shininessParam );

        color.rgb = color.rgb * ( ambientParam + diffuseParam * diffuse )
                  + specularParam * specular * color.a;

        sum = ( 1.0 - sum.a ) * clamp( color, 0.0, 1.0 ) + sum;
        if( sum.a > 0.9 ) break;

        sample_position  += raydir;
        sample_position1 += raydir;
        sample_position2 += raydir;
        sample_position3 += raydir;
        sample_position4 += raydir;
        sample_position5 += raydir;
        sample_position6 += raydir;

        if( sample_position.x > 1.0 || sample_position.y > 1.0 || sample_position.z > 1.0 ) break;
        if( sample_position.x < 0.0 || sample_position.y < 0.0 || sample_position.z < 0.0 ) break;
    }

    return sum;
}
