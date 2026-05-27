// ps_Maa_Max_01.metal -- Path A port from ps_Maa_Max_01.frag (c85).
// Angular-kernel max-of-green search across nb_x * nb_y samples, 3 modes.
// Patches :
//   - `aaa_tex2d[0]`           -> `_aaa_tex_0`
//   - `aaa_fu_float[0..5]`     -> `_aaa_floats.values[0..5]`
//   - `aaa_fu_int[0..2]`       -> `_aaa_ints.values[0..2]`
//   - `aaa_fu_vec4[0..1]`      -> `_aaa_vec4s.values[0..1]`
//   - `gl_TexCoord[0].x/y/st`  -> `in.uv.xy`
//   - `textureSize(tex, 0)`    -> `int2( tex.get_width(), tex.get_height() )`
//   - Original source initialises `dst` without a constructor and writes
//     `dst.xyz += texture * (1.-dst.a)` ; Mac initialises to opaque-black ;
//     behaviour-preserving guard for deterministic compile.

#include <metal_stdlib>
using namespace metal;

struct VertexOut { float4 position [[position]]; float2 uv; };

struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };

constant float PI_HALF_ = 1.5707963267948966192313216916398;
constant float PI_      = 3.1415926535897932384626433832795;

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ), float2( -1.0, 1.0 ), float2( 3.0, 1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),   float2( 0.0, 0.0 ),  float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}

static inline float doit_(
    float2 uv,
    texture2d< float > tex, sampler samp,
    constant AaaFuFloats& fl, constant AaaFuInts& ii )
{
    int const nb_x = ii.values[ 0 ];
    int const nb_y = ii.values[ 1 ];

    int2  const tex_suv = int2( tex.get_width(), tex.get_height() );
    float       dx = fl.values[ 4 ] / float( tex_suv.x );
    float       dy = fl.values[ 5 ] / float( tex_suv.y );
    float const sx = uv.x - dx * 0.5;
    float const sy = uv.y - dy * 0.5;
    float const dsx = dx * 0.5;
    float const dsy = dy * 0.5;
    dx /= float( nb_x - 1 );
    dy /= float( nb_y - 1 );

    float const dax = PI_ / float( nb_x + 1 );
    float const day = PI_ / float( nb_y + 1 );

    float g = 0.0;
    float2 pos;

    if( ii.values[ 2 ] == 0 )
    {
        pos.x = sx;
        float ax = -PI_HALF_ + dax;
        for( int i = nb_x; i > 0; i-- )
        {
            float const cos_x = cos( ax ) - 1.0;
            pos.y = sy;
            float ay = -PI_HALF_ + day;
            for( int j = nb_y; j > 0; j-- )
            {
                float const tmp = cos_x + cos( ay );
                if( tmp > 0.0 )
                {
                    float const v = tex.sample( samp, pos ).g;
                    if( v > fl.values[ 2 ] )
                    {
                        float const vv = v * tmp;
                        if( vv > g ) g = vv;
                    }
                }
                pos.y += dy;
                ay    += day;
            }
            pos.x += dx;
            ax    += dax;
        }
    }
    else if( ii.values[ 2 ] == 1 )
    {
        pos.x = sx;
        for( int i = nb_x; i > 0; i-- )
        {
            pos.y = sy;
            for( int j = nb_y; j > 0; j-- )
            {
                float const v = tex.sample( samp, pos ).g;
                if( v > fl.values[ 2 ] && v > g ) g = v;
                pos.y += dy;
            }
            pos.x += dx;
        }
    }
    else if( ii.values[ 2 ] == 2 )
    {
        //	4 edge scans (top, bottom, left, right)
        pos = float2( sx, sy );
        for( int i = nb_x; i > 0; i-- )
        {
            float const v = tex.sample( samp, pos ).g;
            if( v > fl.values[ 2 ] && v > g ) g = v;
            pos.x += dx;
        }
        pos = float2( sx, sy + 2.0 * dsy );
        for( int i = nb_x; i > 0; i-- )
        {
            float const v = tex.sample( samp, pos ).g;
            if( v > fl.values[ 2 ] && v > g ) g = v;
            pos.x += dx;
        }
        pos = float2( sx, sy );
        for( int j = nb_y; j > 0; j-- )
        {
            float const v = tex.sample( samp, pos ).g;
            if( v > fl.values[ 2 ] && v > g ) g = v;
            pos.y += dy;
        }
        pos = float2( sx + 2.0 * dsx, sy );
        for( int j = nb_y; j > 0; j-- )
        {
            float const v = tex.sample( samp, pos ).g;
            if( v > fl.values[ 2 ] && v > g ) g = v;
            pos.y += dy;
        }
    }

    float const f = 1.0 / ( 1.0 - fl.values[ 3 ] );
    return ( g < fl.values[ 3 ] ) ? 0.0 : ( g - fl.values[ 3 ] ) * f;
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuVec4s&  _aaa_vec4s    [[buffer(1)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    float const s = doit_( in.uv.xy, _aaa_tex_0, _aaa_samp, _aaa_floats, _aaa_ints );

    float4 dst = mix( _aaa_vec4s.values[ 0 ], _aaa_vec4s.values[ 1 ], s ) * _aaa_floats.values[ 1 ];
    dst.xyz += _aaa_tex_0.sample( _aaa_samp, in.uv.xy ).xyz * ( 1.0 - dst.a );
    dst.a   += _aaa_floats.values[ 0 ];
    return dst;
}
