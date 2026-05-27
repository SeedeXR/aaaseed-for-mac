
// Uniform variables for texturing
uniform sampler2D  aaa_tex2d[4];
CONST int mode = aaa_fu_int[0];

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
	vec4	mask = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	float grey;
//todo
	if( aaa_fu_int[1] == 1 )
	 	grey = mask.a ;
	else
				grey = gray( mask.xyz );
	grey = smoothstep( aaa_fu_float[0], aaa_fu_float[1], grey );

	vec4 src1 = texture2D( aaa_tex2d[1], gl_TexCoord[0].st );
	src1 *= aaa_fu_vec4[1];

	vec4 src2 = texture2D( aaa_tex2d[2], gl_TexCoord[0].st );
	src2 *= aaa_fu_vec4[2];

	if( mode <= 3 )
	{
		vec4 col;
		if(mode==1)
			col = src1;
		else if(mode==2)
			col = src2;
		else if(mode==3)
			col = mask;
		gl_FragColor = col;
		return;
	}

	if(mode==4)
		src2 = vec4(0);
	else if(mode==5)
	{
		src1 = vec4(0);
		grey = 1. - grey;
	}


	// if( aaa_fu_int[1] == -1 )
	// {
	// 	gl_FragColor = vec4( grey, grey, grey, 1 );
	// }
	// else
	// {

		if( grey > 1. )
			grey = 1.;
		else if( grey < 0. )
			grey = 0.;
		if( aaa_fu_int[1] == 1 )
			grey = float(1.) - grey;

		vec4	dst;
	//	dst.xyz =  src.xyz * aaa_fu_float[0];
	//	dst.xyz += doit( src ).xyz * aaa_fu_float[1];

		dst.rgba =  src1.rgba * grey + src2.rgba * (float(1.) - grey);

	//	dst.a = self:get_text_color_table()
		dst *= aaa_fu_vec4[0];

	//	dst.r = aaa_fu_int[0] * .2;
		gl_FragColor = dst;
//	}
}

