
//Maa 14 Jan 2008 test on gradient

// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

//uniform float	aaa_fu_float[6];
//	2	:	offset for z
//	3	:	scale for z

//uniform int		aaa_fu_int[6];
//	0	:	type
//	1	:	do mod

void main()
{
	vec4	src;
	vec4	dst;
	//float	s;
	int		type;
	ivec2	tex_suv = textureSize( aaa_tex2d[0], 0 );
	float dx = 1./tex_suv.x;
	float dy = 1./tex_suv.y;

//	s = aaa_fu_float[2] + aaa_fu_float[3]*gl_FragCoord.z;

//	if( aaa_fu_int[1]!=0 )
//	s = mod( s, 1.0 );

	type = aaa_fu_int[0];
	if( type==0 )	//normal
	{
		vec2	co;
		float	g;
		float	d;
		vec4	pixel;
 //todoopt use swizzle
		co.x = gl_TexCoord[0].x*aaa_fu_float[2];
		co.y = gl_TexCoord[0].y*aaa_fu_float[3];
		pixel = texture2D( aaa_tex2d[1], co );
		g = gray( pixel );

		vec4	px;
		co.x += dx*aaa_fu_float[4];
		pixel = texture2D( aaa_tex2d[1], co );
		co.x -= dx*aaa_fu_float[4];
		d = abs( g - gray( pixel.xyz ) );

		co.y += dy*aaa_fu_float[5];
		pixel = texture2D( aaa_tex2d[1], co );
		d += abs( g - gray( pixel.xyz ) );

		vec2	po;
		po = gl_TexCoord[0].st;
		po.y += d*aaa_fu_float[1];
		dst = texture2D( aaa_tex2d[1], po );
		dst *= (1.-d*aaa_fu_float[0]);
		if( aaa_fu_int[2]!=0 )
			dst = blend(dst);

	//	dst.rgb = d;
	//	dst.a = 1;
	}
	else if( type==1 )	// check texture
	{
		dst.r = gl_FragCoord.x*aaa_fu_float[2];
		dst.g = gl_FragCoord.y*aaa_fu_float[3];
		dst.b = 1.0;
		dst.a = 1;
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
	else
	{
		dst.r = mod( dFdy( texture2D( aaa_tex2d[0], gl_FragCoord.x*aaa_fu_float[4] ).r ) *aaa_fu_float[1], 1.0 );
		dst.g = mod( dFdy(gl_FragCoord.y*aaa_fu_float[5])*aaa_fu_float[1], 1.0 );
	//	dst.b = src.b;
		dst.a = 1;
	}
//	dst *= aaa_fu_float[0];
	gl_FragColor = dst;
}
