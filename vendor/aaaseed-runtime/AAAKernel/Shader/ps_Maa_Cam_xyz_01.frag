
//Maa 9 Jan 2008 z calculation have to be redone

// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];
//uniform float		aaa_fu_float[6];
//	2	:	offset for z
//	3	:	scale for z

uniform int			aaa_fu_int[6];
//	0	:	type
//	1	:	do mod

void main()
{
vec4	src;
vec4	dst;
float	s;
int		type;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);
	if( aaa_fu_int[2]!=0 )
		src = blend(src);
//	s = gl_FragCoord.z;
	s = aaa_fu_float[2] + aaa_fu_float[3]*gl_FragCoord.z;

	if( aaa_fu_int[1]!=0 )
		s = mod( s, 1.0 );

	type = aaa_fu_int[0];
	if( type==0 )	//normal
	{
		dst = src;
		dst.a = dst.a*s;
	}
	else if( type==1 )	// see it as white
	{
		dst.r = dst.g = dst.b = s;
		dst.a = 1.;
	}
	else if( type==2 )	// see it as alpha
	{
		dst.r = dst.g = dst.b = 1.;
		dst.a = s;
	}
	else if( type==-1 )	// check texture
	{
		dst = src;
	}
	else if( type==-2 )	// check texture
	{
		dst = gl_Color;
	}
	else if( type==-3 )	// check texture
	{
		dst = src*gl_Color;
		dst.a = 1.;
	}
	else if( type==-4 )	// check texture
	{
		dst = src*gl_Color;
	}
	else if( type==4 )	// check texture
	{
		dst.r = dFdx( texture2D( aaa_tex2d[0], gl_FragCoord.x*aaa_fu_float[4]).r )*32.;
		dst.g = dFdy( texture2D( aaa_tex2d[0], gl_FragCoord.y*aaa_fu_float[5]).g )*32.;
		dst.b = 1.0;
		dst.a = 1;
	}
	else
	{
		dst.r = mod( dFdx(gl_FragCoord.x*aaa_fu_float[4]), 1.0 );
		dst.g = mod( dFdy(gl_FragCoord.y*aaa_fu_float[5]), 1.0 );
		dst.b = src.b;
		dst.a = s;
	}
	gl_FragColor = dst;
}
