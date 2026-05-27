

#if USE_AAA_UNIFORM == 1
	#define dof_input_size 			vec2(aaa_fu_float[0], aaa_fu_float[1])		// input texture size in pixels
	#define dof_blur_size 			aaa_fu_float[2]								// default to 9.0

#else
	layout(binding = 0) uniform dof_settings_block
	{
		vec2 	input_size;	// input texture size in pixels
		float 	blur_size;	// default to 9.0
		float 	pad;
	} u_dof_settings;

	#define dof_input_size 	u_dof_settings.input_size
	#define dof_blur_size 	u_dof_settings.blur_size

#endif

//shader pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//gbuffer outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;
layout(binding = 1) uniform sampler2D g_depth_coc_texture;



vec2 read_depth_and_coc(vec2 uv)
{
	return texture(g_depth_coc_texture, in_texcoord).rg;
}

void main()
{
	vec2 sample_offset = 0.5f / dof_input_size;

	vec3 center_color = texture(g_input_texture, in_texcoord).rgb;
	vec3 color_sum = center_color;
	float total_contribution = 1.0;

	vec2 center_depth_blur = read_depth_and_coc(in_texcoord);
	float center_depth = center_depth_blur.x;
	float center_blur = center_depth_blur.y;

	if( center_blur > 0 )
	{
		const float coc_size = center_blur * dof_blur_size;

		for( uint i = 0; i < disc_sample_count; i++ )
		{
			vec2 kernel_value = disc_kernel[i];
			vec2 offset = kernel_value * coc_size;
			vec2 tap_coord = in_texcoord + offset * sample_offset;

			vec3 tap_color = texture(g_input_texture, tap_coord).rgb;
			vec2 tap_depth_blur = read_depth_and_coc(tap_coord);

			float tap_depth = tap_depth_blur.x;
			float tap_blur = tap_depth_blur.y;

			float tap_contribution = (tap_depth > center_depth) ? 1.0 : tap_blur;

			color_sum += tap_color * tap_contribution;
			total_contribution += tap_contribution;
		}
	}


	color_sum /= total_contribution;

	out_result = vec4(color_sum, 1.0);
}