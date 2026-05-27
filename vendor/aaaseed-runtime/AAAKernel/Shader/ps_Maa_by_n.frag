

//	AAAseed uniform vertex shader
uniform sampler2D	aaa_samp0;


void main (void)
{
//	vec4	vertex = gl_Vertex;
//	vec2 v = vertex.xy;

//		dst.r = mod( dFdx(gl_FragCoord.x*aaa_fu_float[4]), 1.0 );
//		dst.g = mod( dFdy(gl_FragCoord.y*aaa_fu_float[5]), 1.0 );
//		dst.b = src.b;
//		dst.a = s;

	vec4 color;
//	color.r = mod( dFdx(gl_FragCoord.x * aaa_fu_float[0]), 1.0 );
//	color.g = mod( dFdy(gl_FragCoord.y * aaa_fu_float[1]), 1.0 );
	vec2 uv;
	//v.x = gl_FragCoord.x * aaa_fu_float[0] ;
	//v.y = gl_FragCoord.y * aaa_fu_float[1] ;
//	v.x = ( gl_TexCoord[0].x - .5 - aaa_fu_float[2] ) * aaa_fu_float[0] ;
//	v.y = ( gl_TexCoord[0].y - .5 - aaa_fu_float[3] ) * aaa_fu_float[1] ;
	uv = gl_TexCoord[0].xy;

	uv = (uv - .5) * 2.;

	// go to polar coor
	float angle = atan( uv.y, uv.x );
	float d		= length( uv );



//	f = 1.;


	angle = angle / ( 3.14159 * 2. );
	if( angle < 0. )
		angle += 1.;

	angle -= aaa_fu_float[1];
	angle *= aaa_fu_float[0];

	if( aaa_fu_int[1] == 0 )
	{
		angle = mod( abs(angle), 2. );
		if( angle >= 1 )
			angle = 2. - angle;
	}
	else
	{
		angle = mod( abs(angle), 1. );
	}

	//angle = mod( , 1. );
	float a = angle;

	float f;
	f = mod( angle * aaa_fu_float[0] * 2. / 3.14159, 1. ) * 2.;
	if( f > 1. )
		f = 2. - f;
	f = tan(f * 3.14159 * .25);
	f = sqrt( 1 + f*f );

	angle *= 3.14159 * .5;

	uv.x = cos(angle) * d;
	uv.y = sin(angle) * d;

	uv -= aaa_fu_vec4[0].xy;
	uv *= aaa_fu_vec4[0].zw;
	color = texture( aaa_samp0, uv );
	if( aaa_fu_int[0] == -1 )
		color = vec4( a, a, a, 1 );
	if( uv != min( max( uv, 0. ), 1. ) )
		color *= aaa_fu_float[2];

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

