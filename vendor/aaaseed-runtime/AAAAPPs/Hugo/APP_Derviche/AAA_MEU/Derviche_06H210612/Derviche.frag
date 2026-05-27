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
} vs_out;

void main (void)
{
	gl_FragColor = vs_out.color ;
	//gl_FragColor.r = vs_out.pos_world.y;
	//gl_FragColor = vec4( 1. );
	//gl_FragColor = vec4( .4 );
	//gl_FragColor = vec4( 1., 1., 0., 1 );
}
