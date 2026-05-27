
layout(location = 0) out vec4 color_out_0;
layout(location = 1) out vec4 color_out_1;

in ST_AAA_BV BV;

//uniform vec4        aaa_fu_vec4[]; // color de 0 a 3, active ds 4.xyzw et 5.xy
uniform sampler2D   aaa_tex2d[4];

#define SEL     aaa_fu_int[0]
#define ALPHA   aaa_fu_float[0]
#define MIX     aaa_fu_float[1]
void main()
{
    vec2    uv  = BV.tex_coor[0].xy;
//    vec2    ts          = textureSize( aaa_tex2d[0], 0 );
    vec4    col = BV.color;

    if( aaa_fu_int[1]!=0 )
    {
        switch( SEL )
        {
        case 0:     col = vec4( texture2D( aaa_tex2d[0], uv ).rgb, 1.); break;
        case 1:
            col.a = ALPHA;
            break;
        case 2:
            col.a = ALPHA;
            col.rgb = mix( col.rgb, texture2D(aaa_tex2d[0], uv).rgb, MIX );
            break;
        case 3: break;
        default:
            break;
        }
    }

    color_out_0 = col;
    float id = aaa_fu_int[1];
    //float r = id <<
    //color_out_1 = vec4( vec3( float( aaa_fu_int[1] )  * .09  ), 1.);
    if( id != 0. && MIX > 0. )
        color_out_1 = vec4( vec3( id / 64 ), 1. );
//   frag_color_buf1.xyz = nor *.5 +.5;
}
