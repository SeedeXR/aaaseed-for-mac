
in ST_AAA_BV BV;

layout(location = 0) out vec4 out_color_0;
layout(location = 1) out vec4 out_color_1;

//uniform vec4        aaa_fu_vec4[]; // color de 0 a 3, active ds 4.xyzw et 5.xy
uniform sampler2D   aaa_tex2d[4];

/* Todo
    - uniform for fade in - out lv logos / kinect (value1 vs 2)
        - 1 int for trig
        - 1 float for phase / time
    - time or phase for heatmap background
    - heatmap color gradient
*/
CONST bool	b_colormap_vert	=	(aaa_fu_int[0]!=0);

CONST float	uvy             =   aaa_fu_float[7];

CONST float	disp_factor_1	=   aaa_fu_float[0];
CONST float	disp_factor_2	=   aaa_fu_float[1];
CONST float	disp_gamma_1	=   aaa_fu_float[2];
CONST float	disp_gamma_2	=   aaa_fu_float[3];
CONST float	disp_v_offset_1	=   aaa_fu_float[4];
CONST float	disp_v_offset_2	=   aaa_fu_float[5];

//float   fade            =   aaa_fu_float[5];

CONST float	f1             	=   aaa_fu_float[8];
CONST float	f2             	=   aaa_fu_float[9];

CONST float	heat_begin		=   aaa_fu_float[10];
CONST float	heat_end		=   aaa_fu_float[11];
CONST float	heat_gamma		=   aaa_fu_float[12];
CONST float	th				=   aaa_fu_float[13];

CONST vec4	color_below			=	aaa_fu_vec4[0];
CONST vec4	fade_u_vec			=	aaa_fu_vec4[1];

CONST float fade_len			=	fade_u_vec.y ;

CONST float	fade_u_alpha_begin	=	fade_u_vec.x;
CONST float	fade_u_alpha_end	=	fade_u_alpha_begin + fade_len/9.;

CONST float	fade_u_d2_begin		=	fade_u_alpha_begin + 1.*fade_len/9.;
CONST float	fade_u_d2_end		=	fade_u_alpha_begin + 3.*fade_len/9.;

CONST float	fade_u_d1_begin		=	fade_u_alpha_begin + 5.*fade_len/9.;
CONST float	fade_u_d1_end		=	fade_u_alpha_begin + 9.*fade_len/9.;


#define TEX_FIELD		aaa_tex2d[0]
#define TEX_MONO		aaa_tex2d[1]
#define TEX_LINE		aaa_tex2d[2]
#define TEX_COLORMAP 	aaa_tex2d[3]

void main()
{
    //float   fade        =   clamp( time_since_trig, 0, 1 );
    //    uvy = uvy * .9;
    vec2    frag_coord  =   BV.tex_coor[0].xy;
//	cut by the side : not interesting
//	if(  frag_coord.x < aaa_fu_float[8] )
//		discard;

	float d1_f = linearstep( fade_u_d1_begin, fade_u_d1_end, frag_coord.x );
	float d2_f = linearstep( fade_u_d2_begin, fade_u_d2_end, frag_coord.x );

    //      First displace texture coords
    vec2    uv_field	= vec2( (frag_coord.x + 10./9. * .05 ) * 9./10., uvy + disp_v_offset_1 );
	float	disp_1		= compute_gray( texture( TEX_FIELD, uv_field ).rgb );
			disp_1		= pow( disp_1, disp_gamma_1 ) * disp_factor_1 * d1_f;
    //      Second displace texture coords
    vec2    uv_mono		= vec2( frag_coord.x, uvy + disp_v_offset_2 );
    float   disp_2   	= compute_gray( texture( TEX_MONO, uv_mono ).rgb );// * fade;
			disp_2		= pow( disp_2, disp_gamma_2 ) * disp_factor_2 * d2_f;
 //   float   tmp     =   disp;

    float disp 	= max( disp_1, disp_2 );

    //      Line texture coords
    float    v_line =   frag_coord.y;
	v_line -= f2 + disp * (1.-f2-f1*.5) ;
    v_line /= f1;
    v_line += .5;
    v_line = clamp(  v_line, .08, .9 );	//unclear but .999999 causing troubles sometimes with CLAMP_TO_EDGE
  // v_line -= f2;

	vec4 col = texture( TEX_LINE, vec2( .5, v_line ) );
    if( col.a < .01 )
        discard;

#if 0
	float v =  heatmap_offset - disp * heatmap_factor;
	vec3 col_map = 0.5 + 0.5 * cos( - length( v ) * 2.5 + vec3( 0, 2, 4 ) );
#else

//	float v = (disp - heat_begin) / (heat_end - heat_begin);
	float v = disp;
	float v01 = clamp_01( v );
	v01 = pow( v01, heat_gamma );
	v01 = heat_begin + v01 * (heat_end - heat_begin);
//todo deal with direction used from texture even an angle 
	vec4 col_map = texture( TEX_COLORMAP, b_colormap_vert ? vec2(.5,v01) : vec2(v01,.5) );
#endif

	if( col.a < 1. )
	{
		col.rgb = col_map.rgb;
	}
	else
	{
		//todo alternative appear disappear
		// if( frag_coord.y < th )
		// 	col = vec4( 0,0,0, 0);
		// else
			col = mix( color_below, col_map,  pow(col.r,.5) );
	}
	//todo alternative appear disappear
	if( frag_coord.y < th )
		discard;

	col.a *= smoothstep( fade_u_alpha_begin, fade_u_alpha_end, frag_coord.x );
	col *= BV.color; 
	out_color_0 = col;
//	out_color_0 =  texture( aaa_tex2d[3], frag_coord );
//	out_color_0 = vec4( BV.color );
//	out_color_1 = vec4( vec3( frag_coord2.y ), .25 );
	// out_color_0.r = linearstep( fade_u_d1_begin, fade_u_d1_end, frag_coord.x );
	// out_color_0.g = linearstep( fade_u_d2_begin, fade_u_d2_end, frag_coord.x );
	// out_color_0.b = 0.;
	// out_color_0.a =  1.;
}
