//#version 330 compatibility
////#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable


#define F1 aaa_fu_float[0]
#define F2 aaa_fu_float[1]
#define F3 aaa_fu_float[2]
#define F4 aaa_fu_float[3]

in VS_out
{
	vec4	pos_world;
	vec4	pos_ec;
	vec4	color;
	float	hardness;
} vs_out;

#define roundness	1
//#define hardness	.1

//outputs
layout(location = 0) out vec4 out_result;

void main (void)
{
#if 1
	gl_FragColor = vs_out.color ;
#else
	vec2 uv = gl_TexCoord[0].st;
	uv = uv * 2. - 1.;
	//	tex.s *= 4.;
	float d = mix( max(abs(uv.s),abs(uv.t)), dot(uv.st, uv.st), roundness );
	if( d > 1. )
		discard;

	float hardness = 1. - pow( d, vs_out.hardness );
	//	out_result = vec4( d, d, d, 1 ); // * aaa_fu_vec4[0];

#if 0
	out_result = vec4( 1,1,1, d ) * vs_out.color;
//		out_result = vs_out.color;
//	out_result = vec4( uv, 0, d);
#else
	//float g = max(uv.r, uv.g) * 2. + 1.;
	//float g = (max(uv.r, uv.g) + .5) * 1.5;
	//float g = smoothstep( -.5, .5, max(uv.r, uv.g) ) * 1.; // * 2. + 1.;
	float g = (atan(	uv.g, uv.r ) / 3.14159 + 1.) * .5  - .25;
 	g = smoothstep( 0., .25, abs(g) ); // * 2. + 1.;
	g = 1.-smoothstep( 0., .8, d ) * g; 
//	out_result = vec4( g,g,g, 1 );
	out_result = vec4( g,g,g, hardness ) * vec4( pow( vs_out.color.rgb, vec3(2.,2.,2.) ), vs_out.color.a );
#endif
//	gl_FragColor =r ;
	//gl_FragColor.r = vs_out.pos_world.y;
	//gl_FragColor = vec4( 1. );
	//gl_FragColor = vec4( .4 );
	//gl_FragColor = vec4( 1., 1., 0., 1 );
#endif
}
