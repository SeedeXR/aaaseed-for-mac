// aaatree_a.metal -- Path A port from aaatree_a.frag (c97, ~206 LOC).
// Multi-color-band visualisation : 16 individual samplers + UV-driven dispatcher
// + interpolated mix across two indexed samplers.
// Patches :
//   - `aaa_samp0..15` individual samplers -> `_aaa_tex_0..15` (16 [[texture(N)]] bindings)
//   - `in VS_out { uv, color } fs_in;` -> stage_in stubs : fs_in.uv -> in.uv.xy ;
//     fs_in.color -> float4(1)
//   - `aaa_fu_int[0..2]` -> AaaFuInts[0..2] (nb / sel / see)
//   - `aaa_fu_float[0]` -> AaaFuFloats[0] (fac)
//   - `get_color( uv, sel )` 16-way ternary dispatcher -> preserved as static inline
//     using MSL switch (cleaner than the engine ternary tree but semantically identical)
//   - `gl_FragColor` -> return value

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

static inline float4 get_color_(
    float2 uv, int sel,
    texture2d< float > t0, texture2d< float > t1, texture2d< float > t2, texture2d< float > t3,
    texture2d< float > t4, texture2d< float > t5, texture2d< float > t6, texture2d< float > t7,
    texture2d< float > t8, texture2d< float > t9, texture2d< float > t10, texture2d< float > t11,
    texture2d< float > t12, texture2d< float > t13, texture2d< float > t14, texture2d< float > t15,
    sampler samp )
{
    switch( sel )
    {
        case  0: return t0.sample(  samp, uv );
        case  1: return t1.sample(  samp, uv );
        case  2: return t2.sample(  samp, uv );
        case  3: return t3.sample(  samp, uv );
        case  4: return t4.sample(  samp, uv );
        case  5: return t5.sample(  samp, uv );
        case  6: return t6.sample(  samp, uv );
        case  7: return t7.sample(  samp, uv );
        case  8: return t8.sample(  samp, uv );
        case  9: return t9.sample(  samp, uv );
        case 10: return t10.sample( samp, uv );
        case 11: return t11.sample( samp, uv );
        case 12: return t12.sample( samp, uv );
        case 13: return t13.sample( samp, uv );
        case 14: return t14.sample( samp, uv );
        default: return t15.sample( samp, uv );
    }
}

fragment float4 fs_main(
    VertexOut             in            [[stage_in]],
    texture2d< float >    _aaa_tex_0    [[texture(0)]],
    texture2d< float >    _aaa_tex_1    [[texture(1)]],
    texture2d< float >    _aaa_tex_2    [[texture(2)]],
    texture2d< float >    _aaa_tex_3    [[texture(3)]],
    texture2d< float >    _aaa_tex_4    [[texture(4)]],
    texture2d< float >    _aaa_tex_5    [[texture(5)]],
    texture2d< float >    _aaa_tex_6    [[texture(6)]],
    texture2d< float >    _aaa_tex_7    [[texture(7)]],
    texture2d< float >    _aaa_tex_8    [[texture(8)]],
    texture2d< float >    _aaa_tex_9    [[texture(9)]],
    texture2d< float >    _aaa_tex_10   [[texture(10)]],
    texture2d< float >    _aaa_tex_11   [[texture(11)]],
    texture2d< float >    _aaa_tex_12   [[texture(12)]],
    texture2d< float >    _aaa_tex_13   [[texture(13)]],
    texture2d< float >    _aaa_tex_14   [[texture(14)]],
    texture2d< float >    _aaa_tex_15   [[texture(15)]],
    sampler               _aaa_samp     [[sampler(0)]],
    constant AaaFuFloats& _aaa_floats   [[buffer(0)]],
    constant AaaFuInts&   _aaa_ints     [[buffer(2)]] )
{
    //	VS_out stubs
    float2 uv = in.uv.xy;
    float4 const fs_color = float4( 1.0 );

    float4 const mask = _aaa_tex_0.sample( _aaa_samp, uv );

    int   const nb  = _aaa_ints.values[ 0 ];
    int         sel = _aaa_ints.values[ 1 ];
    int         see = _aaa_ints.values[ 2 ];
    float const fac = clamp( _aaa_floats.values[ 0 ], 0.0, 1.0 );

    float4 result;
    if( sel >= 5 )
    {
        if( sel == 5 || see == 15 )
        {
            result = mask;
        }
        else if( sel == 6 )
        {
            result = get_color_( uv, see,
                _aaa_tex_0, _aaa_tex_1, _aaa_tex_2, _aaa_tex_3,
                _aaa_tex_4, _aaa_tex_5, _aaa_tex_6, _aaa_tex_7,
                _aaa_tex_8, _aaa_tex_9, _aaa_tex_10, _aaa_tex_11,
                _aaa_tex_12, _aaa_tex_13, _aaa_tex_14, _aaa_tex_15,
                _aaa_samp );
        }
        else
        {
            if( sel == 7 )      sel = 2;
            else if( sel == 8 ) sel = 3;
            else                sel = 4;
            uv *= float( sel );
            see = int( floor( uv.y ) ) * sel;
            see += int( floor( uv.x ) );
            result = get_color_( uv, see,
                _aaa_tex_0, _aaa_tex_1, _aaa_tex_2, _aaa_tex_3,
                _aaa_tex_4, _aaa_tex_5, _aaa_tex_6, _aaa_tex_7,
                _aaa_tex_8, _aaa_tex_9, _aaa_tex_10, _aaa_tex_11,
                _aaa_tex_12, _aaa_tex_13, _aaa_tex_14, _aaa_tex_15,
                _aaa_samp );
        }
    }
    else
    {
        float val;
        if( 1 <= sel && sel <= 3 )
            val = mask.r;
        else
            val = 0.0;

        if(     sel <= 1 ) val = fract( val + uv.x );
        else if( 3 <= sel ) val = fract( val + uv.y );

        val = ( val - 0.5 ) * float( nb - int( fac ) ) + 0.5 * float( nb );
        val -= fac * 0.5;
        int const ind = int( val );
        val -= float( ind );
        val = clamp( 1.0 - ( 1.0 - val ) / fac, 0.0, 1.0 );

        float3 const c0 = get_color_( uv, ind + 1,
            _aaa_tex_0, _aaa_tex_1, _aaa_tex_2, _aaa_tex_3,
            _aaa_tex_4, _aaa_tex_5, _aaa_tex_6, _aaa_tex_7,
            _aaa_tex_8, _aaa_tex_9, _aaa_tex_10, _aaa_tex_11,
            _aaa_tex_12, _aaa_tex_13, _aaa_tex_14, _aaa_tex_15,
            _aaa_samp ).rgb;
        float3 const c1 = get_color_( uv, ind + 2,
            _aaa_tex_0, _aaa_tex_1, _aaa_tex_2, _aaa_tex_3,
            _aaa_tex_4, _aaa_tex_5, _aaa_tex_6, _aaa_tex_7,
            _aaa_tex_8, _aaa_tex_9, _aaa_tex_10, _aaa_tex_11,
            _aaa_tex_12, _aaa_tex_13, _aaa_tex_14, _aaa_tex_15,
            _aaa_samp ).rgb;
        result = float4( mix( c0, c1, val ), 1.0 );
    }

    return result * fs_color;
}
