//#version 330 compatibility
////#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable


uniform	sampler2D	aaa_tex2d[4];

in VS_out
{
	vec4	pos_world;
	vec4	pos_ec;
	vec4	color;
	vec2	uv;
} vs_out;

void main (void)
{
	gl_FragColor = vs_out.color;
	float fa = 1./(abs(aaa_fu_float[0]) + .00001);
	float u = 1 - abs( 2 * vs_out.uv.x - 1. );
	float f =  u * fa;
//	gl_FragColor.rgb = vec3(f);
	gl_FragColor.a = f;

}
