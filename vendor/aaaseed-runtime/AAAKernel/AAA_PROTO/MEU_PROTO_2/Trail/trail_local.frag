
in ST_AAA_BV BV;

// Uniform variables for texturing
//uniform sampler2D		aaa_tex2d[4];
uniform sampler2D	aaa_samp0;
#if 0
#	define TEX_A aaa_samp0
#else
	layout(binding = 0) uniform sampler2D TEX_A;
#endif

CONST vec4 col_factor	= aaa_fu_vec4[0];
CONST vec4 col_offset	= aaa_fu_vec4[1];
CONST vec4 col_gamma	= aaa_fu_vec4[2];

CONST bool b_inverse	= (aaa_fu_int[0]==1);
CONST bool b_clamp		= (aaa_fu_int[1]==1);
CONST bool b_tint		= (aaa_fu_int[2]==1);


vec4 get_color( in int unit )
{
	vec4	tex;
	int dim = aaa_tex_dim[unit];
	if( dim == 2 )
		tex = texture2D( TEX_A, BV.tex_coor[0].st );
	else if( dim == 0 )
		tex = vec4( 1, 1, 1, 1 );
	else if( dim == 1 )
		tex = vec4( 1, 0, 0, 1 );
	else
		tex = vec4( 0, 0, 1, 1 );

	return tex;
}

void main()
{
	vec4 color = get_color( 0 );

	if( b_inverse )
		color.rgb = vec3(1) - color.rgb;

//	color.rgb *= BV.color.rgb;

	color *= col_factor;

	//offset
	color += col_offset;
	//color.rgb - clamp( color.rgb, 0., 1. );
	//gamma
	color = pow( color, col_gamma );

//	if( b_clamp )
//		color = clamp_01( color );

#if 1
	gl_FragColor = color;
#else
	//gl_FragColor = vec4(aaa_fu_float[0]);
	//gl_FragColor.r = src1_color.r;
//	float v = BV.tex_coor[0].s * BV.tex_coor[0].t;
	float v = texture2D( TEX_A, BV.tex_coor[0].st ).r * .5;
	//gl_FragColor.r = v==0. ? 0 : 1.;
	gl_FragColor.r = v;
	//gl_FragColor.a = 1.;
#endif
}
