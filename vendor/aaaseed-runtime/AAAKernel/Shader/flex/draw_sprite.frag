
struct st_part_custom
{
	flat	uint  _active;
			float _age;
			float _emitter_sel;
	flat	int   _hit_test;
};

//layout(early_fragment_tests) in;

#define hardness		aaa_fu_float[0]
#define roundness		aaa_fu_float[1]
#define	s_test			aaa_fu_int[0]

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
		vec2  uv = in_uv_dens.xy;
		uv = uv * 2. - 1.;
	//	tex.s *= 4.;
		float d = mix( max(abs(uv.s),abs(uv.t)), dot(uv.st, uv.st), roundness );
		if( d > 1. )
			discard;
		d = 1. - pow( d, hardness );

	//	out_result = vec4( d, d, d, 1 ); // * aaa_fu_vec4[0];
		out_result = vec4( 1,1,1, d ) * in_color;
	//	out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );
		
		
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


	