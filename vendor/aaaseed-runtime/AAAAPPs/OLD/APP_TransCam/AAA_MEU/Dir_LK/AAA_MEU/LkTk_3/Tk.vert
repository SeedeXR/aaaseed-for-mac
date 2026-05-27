//#version 330 compatibility
////#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable




out VS_out
{
	vec4	pos_world;
	vec4	pos_ec;
	vec4	color;
	vec2	uv;
} vs_out;


void main(void)
{
	vec4	pos;
	vec4	color;

	pos = gl_Vertex;
	vs_out.pos_world = pos;
	// Eye-coordinate position of vertex, needed in various calculations
	pos = gl_ModelViewMatrix * pos;
	vs_out.pos_ec = pos;

	vs_out.uv = gl_MultiTexCoord0.xy;
	vs_out.color = gl_Color;
	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;
}
