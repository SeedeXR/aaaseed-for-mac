//#extension GL_EXT_gpu_shader4 : enable

//	v 0.1		9 Sept 2011		maa

// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];

uniform float		aaa_fu_src;	//	quantity of the fixed path texture
uniform float		aaa_fu_out;	//	quantity of the computed texture from the shader

void main()
{
	vec2 ori	= gl_TexCoord[0].st;
	vec4 src	= texture2D( aaa_tex2d[0], ori );

	//	we get the mask from the center
	ori.s	= (ori.s - 0.5) * aaa_fu_float[3] + .5;
	ori.t	= (ori.t - 0.5) * aaa_fu_float[4] + .5;
	vec4 mask	= texture2D( aaa_tex2d[1], ori );
	//mask.g = 1.;
	//	this is a test

/*
	float test_val = 1.;
	src.r = test_val;
	src.g = test_val;
	src.b = test_val;
	src.a = 1.;
*/

	//	grey from image
	float	g = src.x * 0.2989 * 2. + src.y * 0.5870 * 2. + src.z * 0.1140 * 2. ;
	//	g = aaa_fu_float[0] + g * ( aaa_fu_float[1] - aaa_fu_float[0] );
	//	g = g * ( aaa_fu_float[1] );
	//	g *= aaa_fu_out;

	//g = g * 2.;

	src.rgb *= ( 1.-aaa_fu_float[2] );
	vec4	dst;
	dst.r = dst.g = dst.b = g * aaa_fu_float[2];
	dst.a = src.a;
	src.rgb +=  dst.rgb;
	src.rgb *= 1. + aaa_fu_float[2] * (mask.g * aaa_fu_float[1] - 1.);
/*
	float f = 1.;
	src.r = mask.g * f;
	src.g = mask.g * f;
	src.b = mask.g * f;
	src.a = 1.;
	*/

	gl_FragColor = src * gl_Color;
}

