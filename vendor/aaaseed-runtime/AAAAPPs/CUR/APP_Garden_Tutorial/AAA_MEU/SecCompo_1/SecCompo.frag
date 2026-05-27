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
	vec2 uv = vs_out.uv;
	vec4 col = texture2D(	aaa_tex2d[0],	uv );
	uv.y = (uv.y + 1.2	) * .3;
	vec4 mask = texture2D(	aaa_tex2d[1],	uv );
	float f = mask.g;
	//col.xyz = vec3(1.)-col.xyz;
	col.xyz = mix( col.xyz, vec3(1.)-col.xyz, f );
	//gl_FragColor = vs_out.color * col;
	//col.r = 1. -  col.r;

	gl_FragColor = col;

}
