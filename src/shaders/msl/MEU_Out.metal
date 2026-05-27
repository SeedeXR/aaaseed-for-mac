// MEU_Out.metal -- Path A port from MEU/MEU_Out.frag (c92, Maa Jul 2012).
// MEU "Output" block : ndc/uv-mode switch + s_type case-table + gamma + fuzzy mask.
// Patches :
//   - `layout(location = 0) in ST_AAA_BV BV;` -> stage_in stub : BV.tex_coor[0].st -> in.uv.xy
//   - file-scope `CONST` engine macro (UBO-init pseudo-const) -> P5, moved into fs_main
//   - `aaa_vu_int[0..3]` -> AaaFuInts[0..3] (vu/fu merge convention)
//   - `aaa_fu_float[14..15]` -> AaaFuFloats[14..15] (size offsets)
//   - `aaa_fu_vec4[0..5]` -> AaaFuVec4s[0..5]
//   - `aaa_fu_int[0..1]` -> AaaFuInts[8..9] (engine has both vu/fu int blocks ;
//     mac maps fu_int past vu_int range to avoid collision)
//   - `TEX_0` macro alias -> `_aaa_tex_0`
//   - `textureSize(tex, 0)` -> `int2( tex.get_width(), tex.get_height() )`
//   - `gl_FragCoord.xy` -> `in.position.xy`
//   - `clamp_01( x )` engine helper -> `saturate(x)` MSL builtin
//   - `discard` -> `discard_fragment()`
//   - `get_fuzzy` helper preserved as static inline

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float3 get_fuzzy_( float2 uv, float4 fuzzy_lrbt, float4 fuzzy_gamma )
{
    float4 v = float4( uv.x, 1.0 - uv.x, uv.y, 1.0 - uv.y ) * fuzzy_lrbt;
    v = pow( saturate( v ), fuzzy_gamma );
    return float3( min( v.x, v.y ) * min( v.z, v.w ) );
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    int   const draw_sx        = _aaa_ints.values[ 0 ];
    int   const draw_sy        = _aaa_ints.values[ 1 ];
    int   const draw_ox        = _aaa_ints.values[ 2 ];
    int   const draw_oy        = _aaa_ints.values[ 3 ];
    bool  const b_ndc          = ( draw_sx != 0 );
    int   const size_offset_u  = int( _aaa_floats.values[ 14 ] );
    int   const size_offset_v  = int( _aaa_floats.values[ 15 ] );
    float3 const color_gamma   = _aaa_vec4s.values[ 0 ].rgb;
    float4 const color_asked   = _aaa_vec4s.values[ 1 ];
    int    const s_type        = _aaa_ints.values[ 8 ];
    float4 const fuzzy_lrbt    = _aaa_vec4s.values[ 4 ];
    float4 const fuzzy_gamma   = _aaa_vec4s.values[ 5 ];

    int2  const tex_0_size = int2( _aaa_tex_0.get_width(), _aaa_tex_0.get_height() );

    float2 uv;
    if( b_ndc )
    {
        uv = in.position.xy;
        float2 const tex_size = float2( tex_0_size ) + float2( float( size_offset_u ), float( size_offset_v ) );

        uv.y -= ( float( draw_sy ) - tex_size.y ) + float( draw_oy );
        if( uv.y < 0.0 || tex_size.y < uv.y )
            discard_fragment();

        uv.x -= float( draw_ox );
        if( uv.x < 0.0 || tex_size.x < uv.x )
            discard_fragment();
        uv /= tex_size;
    }
    else
    {
        uv = in.uv.xy;   // BV.tex_coor[0].st stub
    }

    float4 dst = float4( 0.0 );
    if( s_type == 1 )
    {
        float2 const uvt = mix( _aaa_vec4s.values[ 2 ].xy, _aaa_vec4s.values[ 2 ].zw, uv );
        dst = _aaa_tex_0.sample( _aaa_samp, uvt ) * color_asked;
    }
    else if( s_type == 0 )
    {
        dst = _aaa_tex_0.sample( _aaa_samp, uv ) * color_asked;
    }
    else
    {
        switch( s_type )
        {
            case 512:
            case 513:   dst = color_asked;        break;
            case -768:
            case -769:  dst = float4( 1.0 );      break;
            case -1024: dst = float4( uv, 0.0, 1.0 ); break;
            case -1025:
            {
                float2 const uvt = mix( _aaa_vec4s.values[ 2 ].xy, _aaa_vec4s.values[ 2 ].zw, uv );
                dst = float4( uvt, 0.0, 1.0 );
                break;
            }
            default:    dst = float4( 0.0 );      break;
        }
    }

    dst.rgb = pow( dst.rgb, color_gamma );

    if( _aaa_ints.values[ 9 ] == 1 )
        dst.rgb *= get_fuzzy_( uv, fuzzy_lrbt, fuzzy_gamma );

    return dst;
}
