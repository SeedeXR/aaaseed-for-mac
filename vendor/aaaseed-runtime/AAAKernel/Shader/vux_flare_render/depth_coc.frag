

//pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//outputs
layout(location = 0) out vec2 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_depth_texture;


float calc_coc_factor(in float depth)
{
  	vec4 coc = aaa_cam.coc_factors;

    float f0 = 1.0 - saturate((depth - coc.x) / max(coc.y - coc.x, 0.01f));
    float f1 = saturate((depth - coc.z) / max(coc.w - coc.z, 0.01f));
    float blur = saturate(f0 + f1);
    return blur;
}

void main()
{
	vec2 screen_clip = in_texcoord;
	screen_clip.y = 1.0 - screen_clip.y;
	screen_clip = screen_clip * 2.0 - 1.0;

	float depth = texture(g_depth_texture, in_texcoord).r;
	depth = depth * 2.0 - 1.0;

	vec4 screen_coord = vec4(screen_clip, depth, 1.0);

	vec4 view_position = aaa_cam.projection_inverse * screen_coord;
	view_position.xyz /= view_position.w;

	out_result = vec2(view_position.z, calc_coc_factor(view_position.z));
}