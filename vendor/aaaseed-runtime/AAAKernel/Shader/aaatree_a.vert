//#version 330 compatibility

out VS_out
{
	vec2	uv;
	vec4	color;
} vs_out;


// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	gl_Position		= gl_ModelViewProjectionMatrix * gl_Vertex;
	vs_out.color	= gl_Color;
	vs_out.uv		= gl_MultiTexCoord0.xy;
}