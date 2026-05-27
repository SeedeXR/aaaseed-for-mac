

//layout(early_fragment_tests) in;



layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec3 in_position_world;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_color;
layout(location = 4) in vec4 in_rnd;

#define hardness		aaa_fu_float[0]
#define roundness		aaa_fu_float[1]
#define	noise			aaa_fu_float[2]

#define color_a			aaa_fu_vec4[0]
#define color_b			aaa_fu_vec4[1]
#define uv_noise_factor	aaa_fu_vec4[2].xy

#define u_factor		aaa_fu_float[4]

#define	s_frag_test		aaa_fu_int[0]

#define	s_prim			aaa_vu_int[1]

//outputs
layout(location = 0) out vec4 out_result;

uniform sampler2D	aaa_samp0;
#define TEX 		aaa_samp0

#define NOISE simplex3d

float do_roundness( in vec2 uv )
{
	uv = uv * 2. - 1.;
	float d = dot( uv, uv );
	if( roundness != 1. )
	{
		uv = abs(uv);
		d = mix( max(uv.s,uv.t)*.5, d, roundness );
	}
	return d;
}
void main()		
{
#if 1
	switch( s_frag_test )
	{
	case 0:	// SQUARE TO ROUND with noise
		{
		vec2  uv;
		if( s_prim == 1 )	//GL_POINTS
			uv = gl_TexCoord[0].st;
		else
			uv = in_texcoord.st;

		float d = do_roundness( uv );
		if( noise != 0. )
		{
			vec2 ce = (uv + in_position_world.xy * 10.) * uv_noise_factor;
			d += ( NOISE( vec3( ce, 	in_rnd.x * 123.) ) + 1.) * noise;
			d += ( NOISE( vec3( ce * 2,	in_rnd.x * 829.) ) + 1.) * noise * .5;
		}
		if( d > .999 )
			discard;
		// if( d < .8 )
		// 	discard;

		//d = 1. - pow( d, hardness );
		//d = smoothstep( hardness, 1., d );	//this do circle
		d = 1. - smoothstep( hardness, 1., d );	//better than previous when hardness is 1.
		//d = smoothstep( 1., hardness, d );
	//	out_result = vec4( d, d, d, 1 ); // * color_a;
		out_result = vec4( 1,1,1, d );
		out_result *= mix( color_a, color_b, in_rnd.b );
		out_result *= in_color;

	//	out_result.a *= in_rnd.a;	//mix( color_a, color_b, in_rnd );
	//	out_result.a *= d;
	//	out_result *= d;
		}
		break;
	case 1:	// WHITE
		out_result = vec4( 1 );
		break;
	case 2:	// MAP TEXTURE
		{
			vec2  uv;
			if( s_prim == 1 )	//GL_POINTS
				uv = gl_TexCoord[0].st;
			else
				uv = in_texcoord.st;
			out_result = texture( TEX, uv );	//.rgb
			out_result *= mix( color_a, color_b, in_rnd.b );
			out_result *= in_color;
		}
		break;
	default:
	case 3:	// ROUND
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
			out_result *= mix( color_a, color_b, in_rnd.b );
		}
		break;
	}
#else
	vec2  uv;
	if( s_prim == 1 )	//GL_POINTS
		uv = gl_TexCoord[0].st;
	else
		uv = in_texcoord.st;
	out_result = texture( TEX, uv );	//.rgb
//	out_result = vec4(1);
//	out_result = in_color * mix( color_a, color_b, in_rnd );
#endif

}
	
	