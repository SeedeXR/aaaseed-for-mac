

//Maa july 2012
uniform sampler2D	aaa_tex2d[4];

//uniform float	aaa_fu_float[6];
//	0	:	left	( 0 nothing / 1 all texture )
//	1	:	right	idem
//	2	:	bottom	idem
//	3	:	top	idem
//	5	:	multiplication factor to help adjust interactivly

void main (void)
{
	vec4 texture;
	int dim = aaa_tex_dim[0];
	if( dim == 2 )
	{
		texture = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	}
	else if( dim == 0 )
	{
		texture = vec4( 1, 1, 1, 1 );
	}
	else if( dim == 1 )
	{
		texture = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	}
	else
	{
		texture = vec4( 0, 0, 1, 1 );
	}

	gl_FragColor.a = pow( texture.a * aaa_fu_float[1] + aaa_fu_float[0], aaa_fu_float[2] ) ;
	gl_FragColor.rgb = gl_Color.rgb;
}
