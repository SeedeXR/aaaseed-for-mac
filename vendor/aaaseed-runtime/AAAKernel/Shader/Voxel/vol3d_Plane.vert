//#version 330 compatibility

//#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable

out vec4 ecPosition;
out vec3 transformedNormal;

out VS_out
{
	vec4	pos_world;
	vec4	color;
} vs_out;


void main(void)
{
	vs_out.color		= gl_Color;
	vs_out.pos_world	= gl_Vertex;

	// Do fixed functionality vertex transform
	gl_Position = ftransform();

	// Eye-coordinate position of vertex, needed in various calculations
//	ecPosition = gl_ModelViewMatrix * gl_Vertex;

//	vs_out.nor = gl_Normal;

//	vs_out.uvw = (gl_TextureMatrix[0] * gl_MultiTexCoord0 ).stp;
}

