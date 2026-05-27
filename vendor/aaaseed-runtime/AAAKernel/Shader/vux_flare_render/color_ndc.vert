
// Base vertex shader to draw a quad in normalized device coordinates (NDC).

layout(location = 0) in vec3 in_position;
//layout(location = 1) in vec2 in_texcoord;

layout(location = 0) out vec4 out_color;

void main(void)
{
//todo missing du model matrix
	gl_Position = vec4( in_position, 1.0 );
//    out_texcoord = in_texcoord;
//Maa is still old school ...
//	out_texcoord = gl_MultiTexCoord0.xy;
//Maa is still old school ...
	out_color = gl_Color;
}