////#version 330 compatibility
// we don't want to use include here
//	this basic shader need to work everywhere
////#extension GL_EXT_gpu_shader4 : enable

// GLSL vertex shader - Gabu Monitor
// for AAASeed
// Author : Mâa

out VS_out
{
	vec4			color;
	vec2			tex_coor;
} vs_out;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex ;
	vs_out.color = gl_Color;
	vs_out.tex_coor = gl_MultiTexCoord0.st;
}

