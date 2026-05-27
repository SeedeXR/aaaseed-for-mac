

layout(early_fragment_tests) in;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec3 in_position_world;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 in_color;
layout(location = 4) in vec4 in_rnd;

//outputs
layout(location = 0) out vec4 out_result;

void main()
{
#if 1
	vec2  tex = gl_TexCoord[0].st;
	tex = tex * 2. - 1.;
	float d = dot( tex.st, tex.st );
	if( d > 1. )
		discard;
	d = 1. - pow( d, aaa_fu_float[0] );


//	out_result = vec4( d, d, d, 1 ); // * aaa_fu_vec4[0];
	out_result = vec4( 1, 1, 1, d );
	out_result *= mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd.b );
//	out_result.a *= in_rnd.a;	//mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd );
//	out_result.a *= d;
//	out_result *= d;
#else
	out_result = in_color * mix( aaa_fu_vec4[0], aaa_fu_vec4[1], in_rnd );
#endif
}


	