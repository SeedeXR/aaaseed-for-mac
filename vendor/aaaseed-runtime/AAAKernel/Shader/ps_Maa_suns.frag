// GLSL fragment shader - Apply Texture
// for Aaaseed
// Author : Maa

//#version 330 compatibility

//#extension GL_ARB_shading_language_include : enable

CONST float sun_radius	= aaa_fu_float[4]/2.;
CONST float ray_radius	= aaa_fu_float[5]/2.;
CONST float ray			= aaa_fu_float[6];
CONST float rot_offset	= aaa_fu_float[7] * PI2;

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
	vec2 v;
	//v.x = gl_FragCoord.x * aaa_fu_float[0] ;
	//v.y = gl_FragCoord.y * aaa_fu_float[1] ;
	v.x = ( gl_TexCoord[0].x - .5 - aaa_fu_float[2] ) * aaa_fu_float[0] ;
	v.y = ( gl_TexCoord[0].y - .5 - aaa_fu_float[3] ) * aaa_fu_float[1] ;
//	v.x = (v.x - .5) * 8. ;
//	v.y = (v.y - .5) * 8. * aaa_fu_float[0] / aaa_fu_float[1] ;
//	v.x -= aaa_fu_float[2];
//	v.y -= aaa_fu_float[3];

	float d = length( v ) ;

	float f;
	if( d < sun_radius )	//in central
	{
		f = 0.;
	}
	else if( d > ray_radius )	//out ray
	{
		f = 1.;
	}
	else
	{
		float a = atan( v.y, v.x ) + rot_offset;

		f = mod( a * aaa_fu_int[0] * .15915, 1. );
		if( f < ray )
			f = 0.;
		else
			f = 1.;
	}
	color = mix( aaa_fu_vec4[0], aaa_fu_vec4[1], f );

//	color.r = v.x ;
//	color.g = v.y ;

//	color.r = vertex.x * .01;
//	color.g = vertex.y * .01;
//	color.b = 0.;
/*
	color = gl_Color;

	color += gl_SecondaryColor;
	color = clamp(color, 0.0, 1.0);
*/
//	color.a = 1.;
	gl_FragColor = color;
}

