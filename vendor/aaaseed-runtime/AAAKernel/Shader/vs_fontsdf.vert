


layout(location = 0) in vec3 in_position_world;
layout(location = 1) in vec2 in_texcoord;

layout(location = 0) out vec2 out_uv;


void main(void)
{
	vec4	pos;
	pos = gl_Vertex;
	pos = gl_ModelViewMatrix * pos;
	out_uv = in_texcoord.xy;
	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;
}
