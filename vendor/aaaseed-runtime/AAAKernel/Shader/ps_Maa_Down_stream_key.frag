


uniform sampler2D	aaa_samp0;


void main (void)
{

	vec4 color;

	vec2 uv;
	uv = gl_TexCoord[0].xy;
	color = texture( aaa_samp0, uv );


	float grey = gray( color.rgb );

	if( aaa_fu_float[0] < grey && grey < aaa_fu_float[1] )
		color = vec4( 1, 1, 1, aaa_fu_float[2] );

/*
	float d = length( v ) ;

	float f;
	if( d < aaa_fu_float[4] )
	{
		f = 0.;
	}
	else if( d > aaa_fu_float[5] )
	{
		f = 1.;
	}
	else
	{
		float a = atan( v.y, v.x ) + aaa_fu_float[6] ;

		f = mod( a * aaa_fu_int[0] * .15915 * 2.0 , 1. );
		if( f < aaa_fu_float[7] )
			f = 0.;
		else
			f = 1.;
	}

	color = mix( aaa_fu_vec4[0], aaa_fu_vec4[1], f );

	color = vec4( 1. );

//	color.r = v.x ;
//	color.g = v.y ;
*/
/*
	color.r = uv.x;
	color.g = uv.y;
	color.b = 0.;
	color.a = 1.;
*/
	gl_FragColor = color;
}

