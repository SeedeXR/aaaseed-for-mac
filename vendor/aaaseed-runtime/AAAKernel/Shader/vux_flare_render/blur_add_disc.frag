

#if USE_AAA_UNIFORM == 1
#	define blur_add_input_size 		vec2(aaa_fu_float[0], aaa_fu_float[1])		// input texture size in pixels
#	define blur_add_size 			aaa_fu_float[2]								// default to 9.0
#	define blur_add_factor 			aaa_fu_float[3]								// default to 0.5
#	define blur_add_intensity		aaa_fu_float[4]								// default to 1.0
#else
	layout(binding = 0) uniform blur_add_settings_block
	{
		vec2 input_size;	// input texture size in pixels
		float blur_size;	// default to 9.0
		float add_factor;
    	float add_intensity;
	} u_blur_add_settings;

#	define blur_add_input_size	u_blur_add_settings.input_size
#	define blur_add_size		u_blur_add_settings.blur_size
#	define blur_add_factor 		u_blur_add_settings.add_factor
#	define blur_add_intensity	u_blur_add_settings.add_intensity
#endif

//shader pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//gbuffer outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;
layout(binding = 1) uniform sampler2D g_add_texture;


void main()
{
	vec2 sample_offset = 0.5f * blur_add_input_size;
	sample_offset *= blur_add_size;

	vec3 base_color =     texture(g_input_texture, in_texcoord).rgb;
	vec3 base_color_add = texture(g_add_texture, in_texcoord).rgb;

	vec3 color_sum = base_color_add;
	float total_contribution = 1.0;

	for( uint i = 0; i < disc_sample_count; i++ )
	{
		vec2 tap_coord = in_texcoord + disc_kernel[i] * sample_offset;
		color_sum += texture(g_add_texture, tap_coord).rgb;
		tap_coord = in_texcoord + disc_kernel[i].yx * sample_offset * 2.;
		color_sum += texture(g_add_texture, tap_coord).rgb;
	}
	color_sum *= blur_add_intensity / float(disc_sample_count * 2);

	vec3 result = mix(base_color, base_color + color_sum, blur_add_factor);

	out_result = vec4(result, 1.0);
}