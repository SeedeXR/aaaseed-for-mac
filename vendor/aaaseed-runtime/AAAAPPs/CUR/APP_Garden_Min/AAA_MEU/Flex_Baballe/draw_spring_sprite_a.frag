
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

uniform sampler2D	aaa_samp4;
#define TEX_ID0 	aaa_samp4

uniform sampler2D	aaa_samp5;
#define TEX_ID1 	aaa_samp5

uniform sampler2D	aaa_samp6;
#define TEX_ID2 	aaa_samp6

//layout(location = 0) in vec3			in_normal;
//layout(location = 1) in vec3			in_position_world;
layout(location = 0) in vec4			in_uv_dens_factor;
layout(location = 1) in vec4			in_color;
layout(location = 2) in vec3			in_normal;
layout(location = 3) in st_part_custom	in_custom;
layout(location = 7) in flat int 		in_tag;

//outputs
layout(location = 0) out vec4			out_result;
layout(location = 1) out vec4			out_normal;

void main()
{
	out_normal = vec4( in_normal, 1 ); 
	if( in_tag == 0 )
	{
		vec2  uv = in_uv_dens_factor.xy;
		uv = uv * 2. - 1.;
		float d = mix( max(abs(uv.s),abs(uv.t)), dot(uv.st, uv.st), roundness );
		if( d > 1. )
			discard;
		d = 1. - pow( d, hardness );
		out_result = vec4( 1,1,1, d ) * in_color;

		if( out_result.a < .01 )
			discard;
	}
	else
	{
		vec4 obj0 = texture( TEX_ID0, in_uv_dens_factor.xy );
		vec4 obj1 = texture( TEX_ID1, in_uv_dens_factor.xy );
		vec4 obj2 = texture( TEX_ID2, in_uv_dens_factor.xy );

		if( in_tag == 1 )
			out_result = obj0;
		if( in_tag == 2 )
			out_result = obj1;
		if( in_tag == 3 )
			out_result = obj2;			
	}
}


	