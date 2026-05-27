
in ST_AAA_BV BV;

// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

void main()
{
	vec2 uv = BV.tex_coor[0].st;

	vec4 color;
	vec4 mask;
	int dim = aaa_tex_dim[0];
	if( dim != 2 )
	{
		if( dim == 0 )
			mask = vec4( 1,1,1,1 );
		else if( dim == 1 )
			mask = vec4( 1,0,0,1 );
		else if( dim == 2 )
			mask = vec4( 0,1,0,1 );
		else if( dim == 3 )
			mask = vec4( 0,0,1,1 );
		gl_FragColor = mask;
		return;
	}

//	mask = texture2D( aaa_tex2d[1], uv*.97 + .015 );
	mask = texture2D( aaa_tex2d[1], uv );
		//color = vec4( uv.s, uv.t, 0, 1 );

	uv *= vec2( aaa_fu_float[1], aaa_fu_float[2] );
	uv += vec2( aaa_fu_float[0], - aaa_fu_float[2]*.5);
	float a = aaa_fu_float[3] * PI2;
	float c = cos( a );
	float s = sin( a );
	uv *= mat2( c, -s, s, c );
	uv += .5;
//	vec4 mask	= texture2D( aaa_tex2d[1], uv );	//gl_TexCoord[0].st );
	color		= texture2D( aaa_tex2d[0], uv );	//gl_TexCoord[0].st );
	color.rgb	*= texture2D( aaa_tex2d[2], uv ).rgb;

	gl_FragColor.rgb = color.rgb * BV.color.rgb * mask.r;
	gl_FragColor.a = mask.r;
//	gl_FragColor = color;
//	gl_FragColor *= BV.color;
}
