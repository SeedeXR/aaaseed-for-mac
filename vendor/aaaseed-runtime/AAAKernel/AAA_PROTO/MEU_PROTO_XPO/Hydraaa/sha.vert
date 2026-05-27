//	HYDRAAA vertex shader
//	Uses an interface block instead of separate "layout(location = N) out"
//	declarations because the latter triggers transform-feedback validation
//	("invalid xfb_buffer ... larger than MAX_TRANSFORM_FEEDBACK_BUFFERS - 1")
//	on Apple GL where xfb is not supported. The GaBu_Monitor.vert shader
//	uses the same pattern and is the only MEU-shader that compiles on Mac.

out VS_out
{
	vec4	color;
	vec2	tex_coor;
} vs_out;

void main(void)
{
	gl_Position		= gl_ModelViewProjectionMatrix * gl_Vertex;
	vs_out.color	= gl_Color;
	vs_out.tex_coor	= gl_MultiTexCoord0.st;
}
