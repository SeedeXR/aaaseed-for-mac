
// Uniform variables for texturing
uniform sampler2D  aaa_tex2d[4];



/*
vec4 doit( vec4 a )
{
	vec4	ret;
	float	g = compute_gray(a.xyz);

	if( g <= aaa_fu_float[2] )
		ret.x = ret.y = ret.z = 0.;
	else
	{
		g -= aaa_fu_float[2];
		g /= 1. - aaa_fu_float[2];
		g *= aaa_fu_float[3] ;
		ret.x = ret.y = ret.z = g ;
	}

	ret.a = a.a;
	return ret;
}
*/

void main()
{
	vec4	src1 = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	src1 *= aaa_fu_vec4[1];

	vec4	src2 = texture2D( aaa_tex2d[1], gl_TexCoord[0].st );
	src2 *= aaa_fu_vec4[2];

/*
	float grey;
	if( aaa_fu_int[0] == 1 )
		grey = mask.a ;
	else
		grey = gray( mask.xyz );
	grey = smoothstep( aaa_fu_float[0], aaa_fu_float[1], grey );
*/
	if( aaa_fu_int[0] == -2 )
	{
		gl_FragColor = src2;
	}
	else if( aaa_fu_int[0] == -1 )
	{
		gl_FragColor.xyz = src1.xyz;
		gl_FragColor.a = 1.;
	}
	else
	{
		float grey;
		grey = gray( src2.xyz );
		grey = smoothstep( aaa_fu_float[0], aaa_fu_float[1], grey );

/*
		if( grey > 1. )
			grey = 1.;
		else if( grey < 0. )
			grey = 0.;
		if( aaa_fu_int[1] == 1 )
			grey = float(1.) - grey;
*/
		vec4	dst;
	//	dst.xyz =  src.xyz * aaa_fu_float[0];
	//	dst.xyz += doit( src ).xyz * aaa_fu_float[1];

		dst.rgb =  src1.rgb * grey;
		dst.a = 1.;

	//	dst.a = self:get_text_color_table()
		dst *= aaa_fu_vec4[0];

	//	dst.r = aaa_fu_int[0] * .2;
		gl_FragColor = dst;
	}
	//gl_FragColor = vec4( -aaa_fu_int[0]*.5, aaa_fu_float[0], 0, 1 );
}

