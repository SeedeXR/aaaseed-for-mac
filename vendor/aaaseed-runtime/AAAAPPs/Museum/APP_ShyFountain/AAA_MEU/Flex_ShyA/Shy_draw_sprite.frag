
struct st_part_custom
{
	flat	uint  _active;
			float _age;
			float _emitter_sel;
	flat	int   _hit_test;
};

//layout(early_fragment_tests) in;

#define hardness	aaa_fu_float[0]
#define roundness	aaa_fu_float[1]
#define	s_test		aaa_fu_int[0]

uniform sampler2D	aaa_samp0;
#define TEX 		aaa_samp0
//layout(location = 0) in vec3 in_normal;
//layout(location = 1) in vec3 in_position_world;
layout(location = 0) in vec3			in_uv_dens;
layout(location = 1) in vec4			in_color;
layout(location = 2) in vec3			in_normal;
layout(location = 3) in st_part_custom	in_custom;
//layout(location = 4) in vec4			in_rnd;*/

//outputs
layout(location = 0) out vec4 out_result;
layout(location = 1) out vec4 out_normal;


void main()
{
#if 1
	if( s_test == 0 )
	{
		//do texture round/square hardness
		vec2  uv = in_uv_dens.xy - .5;
		float d;
		if( roundness >= 1. )
		{
			d = dot(uv.st, uv.st);
			if( d > .25 )
				discard;
			d *= 4.;
			d = sqrt( d );
		}
		else if( roundness <= 0. )
		{
			d = max( abs(uv.s), abs(uv.t) );
			if( d > .5 )
				discard;
			d *= 2.;
		}
		else
		{
			d = mix( max(abs(uv.s),abs(uv.t) ) * 2., sqrt( dot(uv.st, uv.st)*4. ), roundness );
			if( d > 1.)
				discard;
		}
#if 0
		d = 1. - d;
		d = linearstep( 0., 1.-hardness, d );
		//d = 1. - d;
		//d = 1. - pow( d, hardness );
#else
		d = linearstep( clamp_01(hardness), 1.0001, d );
		d = 1. - d;
#endif
		
#if 1
		float g = 1; //.5 + 1. - in_custom._age;
		g = pow( g, 4. );
		out_result = vec4( vec3(g), d*g );
#else
		float g = 1. - in_custom._age / 8.;
		if( g <= 0. )
		{
			out_result = vec4( vec3(0,0,1), d );
		}
		else
		{
			//float g = in_custom._emitter_sel;
			//	out_result = vec4( d, d, d, 1 ); // * aaa_fu_vec4[0];
			out_result = vec4( vec3(g,1-g,0), d );
			//	out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );
		}
#endif
		out_result *= in_color;
		
	//	d = in_uv_dens.z;
	//	out_result.r = d * .5;

	//	out_result.a *= in_rnd.a;	//mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd );
	//	out_result.a *= d;
	//	out_result *= d;
	}
	else //if( s_test == 1 )
	{
		out_result = texture( TEX, in_uv_dens.xy )  * in_color;
		if( out_result.a < .1 )
			discard;
		//out_result = vec4( 1,1,1, 0 );
	}

	//out_result.rg *= 1. - (in_custom._age-2)/2.;
//	out_result.gb *= in_custom._age/8.;
//	out_result.gb = vec2( in_custom._hit_test );
//	out_result = texture( aaa_samp0,	in_texcoord );
#else
	out_result = vec4(1);
#endif

//	out_normal = vec4( 0,1,0, 1 );
	out_normal = vec4( in_normal, 1 ); 
}


	