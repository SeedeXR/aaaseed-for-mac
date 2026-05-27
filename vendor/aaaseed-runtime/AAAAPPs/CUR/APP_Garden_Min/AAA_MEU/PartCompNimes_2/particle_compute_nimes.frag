

//layout(early_fragment_tests) in;



layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec3 in_position_world;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_color;
layout(location = 4) in vec4 in_rnd;

#define hardness		aaa_fu_float[0]
#define roundness		aaa_fu_float[1]
#define	noise			aaa_fu_float[2]
#define noise_freq		aaa_fu_float[3]

#define	s_test			aaa_fu_int[0]

#define	s_prim			aaa_vu_int[1]

//outputs
layout(location = 0) out vec4 out_result;

uniform sampler2D	aaa_samp0;
#define TEX 		aaa_samp0

#define NOISE simplex3d
void main()
{
#if 0
	switch( s_test )
	{
	case 0:
		{
		vec2  uv;
		if( s_prim == 1 )	//GL_POINTS
			uv = gl_TexCoord[0].st;
		else
			uv = in_texcoord.st;

		uv = uv * 2. - 1.;
	//	tex.s *= 4.;
		float d = dot(uv.st, uv.st);
		if( roundness != 1. )
		{
			vec2 uvb = abs(uv);
			d = mix( max(uvb.s,uvb.t), d, roundness );
		}
		if( noise != 0. )
		{
			d += ( NOISE( vec3( vec2(uv * noise_freq), 		in_rnd.x * 123.) ) + 1.) * noise;
			d += ( NOISE( vec3( vec2(uv * noise_freq * 2),	in_rnd.x * 829.) ) + 1.) * noise * .5;
		}

		if( d >= 1. )
			discard;
		// if( d < .8 )
		// 	discard;

		//d = 1. - pow( d, hardness );
		//d = smoothstep( hardness, 1., d );	//this do circle
		d = 1. - smoothstep( hardness, 1., d );	//better than previous when hardness is 1.

		//if( d > .4 )
		//	discard;
	
	//	out_result = vec4( d, d, d, 1 ); // * aaa_fu_vec4[0];
	//	out_result = mix( vec4(1,0,0,1), vec4(0,1,0,1), d );
		out_result = vec4( 1,1,1, d );
		out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );
		out_result *= in_color;

	//	out_result.a *= in_rnd.a;	//mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd );
	//	out_result.a *= d;
	//	out_result *= d;
		}
		break;
	case 1:
		out_result = vec4( 1. );
		break;
	case 2:
		{
			vec2  uv;
			if( s_prim == 1 )	//GL_POINTS
				uv = gl_TexCoord[0].st;
			else
				uv = in_texcoord.st;
			out_result = texture( TEX, uv );	//.rgb
			out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );
			out_result *= in_color;
		}
		break;
	default:
	case 3:
		{
			vec2  uv;
			if( s_prim == 1 )	//GL_POINTS
				uv = gl_TexCoord[0].st;
			else
				uv = in_texcoord.st;

			uv = uv * 2. - 1.;
			float d = dot(uv.st, uv.st);
			if( d > 1. )
				discard;
			d = 1. - pow( d, hardness );
			out_result = vec4( 1,1,1, d );
			out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );
		}
		break;
	}
#else
	vec2  uv;
	if( s_prim == 1 )	//GL_POINTS
		uv = gl_TexCoord[0].st;
	else
		uv = in_texcoord.st;
	out_result = texture( TEX, uv ) * in_color;	//.rgb
//	out_result = vec4(1);
//	out_result = in_color * mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd );
#endif

}
	
	