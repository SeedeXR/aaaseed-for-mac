

layout(binding = 0) uniform sampler2D g_input_texture;

layout(location = 0) in vec2 in_texcoord;

layout(location = 0) out vec4 out_result;

void main(void)
{
	out_result = texture(g_input_texture, in_texcoord);
}