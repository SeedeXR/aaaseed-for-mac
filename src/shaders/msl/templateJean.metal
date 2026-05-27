// templateJean.metal -- Path A port from WebGL/3D/templateJean.frag (c93, sebastien durand 2016).
// Sphere-distortion raymarcher with diffuse + specular lighting.
// Patches :
//   - `in VS_out { pos_world, pos_ec, nor_ec, alpha, color, tex_coor[N] } vs_out;`
//     -> stage_in stub : only `vs_out.tex_coor[0].xy` -> in.uv.xy read in live path.
//   - `aaa_tex2d[4]` declared but **unused** in live path ; dropped.
//   - `aaa_fu_float[3..6]` -> `_aaa_floats.values[3..6]`
//   - `aaa_fu_vec4[3]`     -> `_aaa_vec4s.values[3]`
//   - `mat3` GLSL row-major -> `float3x3` MSL column-major (c87 lesson) ;
//     Rot3X / Rot3Y construction flipped accordingly.
//   - Rot3X / Rot3Y declared but unused in live path -> dropped (only `march`,
//     `calcNormal`, `camera`, `sdSphere`, `scene` are reached).
//   - `gl_FragColor` -> return value
//   - file-scope `vec2 uv;` (P5) -- never assigned globally in live path -> dropped

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };

constant int   I_MAX_ = 100;
constant float E_     = 0.002;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float sdSphere_( float3 p, float r )
{
    return length( p ) - r;
}

static inline float scene_( float3 p, float t3 )
{
    float const ct = cos( t3 );
    float const st = sin( t3 );
    float3 const q = ( -0.9 + 0.1 * ct ) * 0.9 * p
                   + ( 0.35 * ct )
                   * cos( 5.0 * p.x * st )
                   * sin( 5.0 * p.y * ct )
                   * sin( 5.0 * p.z * -st );
    return sdSphere_( q, 1.0 );
}

static inline float4 march_( float3 pos, float3 dir, float t3 )
{
    float2 dist = float2( 0.0 );
    float3 p    = float3( 0.0 );
    float4 step_ = float4( 0.0 );
    for( int i = -1; i < I_MAX_; ++i )
    {
        p = pos + dir * dist.y;
        dist.x = scene_( p, t3 );
        dist.y += dist.x;
        if( dist.x < E_ || dist.y > 20.0 ) break;
        step_.x++;
    }
    step_.y = dist.x;
    step_.w = dist.y;
    return step_;
}

static inline float3 calcNormal_( float3 pos, float e, float3 dir, float t3 )
{
    float3 const eps = float3( e, 0.0, 0.0 );
    return normalize( float3(
        march_( pos + eps.xyy, dir, t3 ).w - march_( pos - eps.xyy, dir, t3 ).w,
        march_( pos + eps.yxy, dir, t3 ).w - march_( pos - eps.yxy, dir, t3 ).w,
        march_( pos + eps.yyx, dir, t3 ).w - march_( pos - eps.yyx, dir, t3 ).w ) );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]] )
{
    //	VS_out.tex_coor[0].xy stub
    float2 const frag_coord = in.uv.xy;
    float  const t3         = _aaa_floats.values[ 3 ];

    float4 col = float4( 1.0 );

    float3 const pos = float3( 0.0, 0.0, -10.0 );
    float3 const dir = normalize( float3( ( frag_coord - 0.5 ) * 2.0, _aaa_floats.values[ 5 ] ) );

    float4 const inter = march_( pos, dir, t3 );
    float3 const v = pos + inter.w * dir;
    if( inter.w <= 17.0 )
    {
        float3 const n      = calcNormal_( pos, E_, dir, t3 );
        float3 const ev     = normalize( v - pos );
        float3 const ref_ev = reflect( ev, n );
        float3 const light_pos = float3( -20.0, 10.0, -25.0 );

        float3 const vl = normalize( light_pos - v );
        float  const diffuse  = max( 0.0, dot( vl, n ) );
        float  const specular = pow( max( 0.0, dot( vl, ref_ev ) ), 42.0 );
        col.xyz = col.xyz * ( diffuse + specular );
    }

    return float4( _aaa_floats.values[ 6 ] * col.xyz + _aaa_vec4s.values[ 3 ].xyz, 1.0 );
}
