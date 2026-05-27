

#if USE_AAA_UNIFORM == 1
	#define input_size_over 	vec2( aaa_fu_float[0], aaa_fu_float[1] )	// input texture size in pixels
	#define blur_size_asked 	aaa_fu_float[2]								// default to 9.0
	#define near_out			aaa_fu_float[3]
	#define near_in 			aaa_fu_float[4]
	#define far_in				aaa_fu_float[5]
	#define far_out 			aaa_fu_float[6]
	#define use_vdebug			aaa_fu_int[0]
	#define use_blur_texture 	aaa_fu_int[1]

#else
	layout(binding = 0) uniform dof_settings_block
	{
		vec2 	input_size;		// input texture size in pixels
		float 	blur_size;		// default to 9.0
		float 	pad;
		float 	coc_near_out
		float 	coc_near_in
		float 	coc_far_in
		float 	coc_far_out
		int 	use_blur_texture;
		float 	pad;
	} u_dof_settings;

	#define input_size_over 	vec2( 1./u_dof_settings.input_size.x, 1./u_dof_settings.input_size.y )	//todo not tested
	#define blur_size_asked 	u_dof_settings.blur_size
	#define near_out			u_dof_settings.coc_near_out
	#define near_in 			u_dof_settings.coc_near_in
	#define far_in				u_dof_settings.coc_far_in
	#define far_out 			u_dof_settings.coc_far_out
	#define use_blur_texture 	u_dof_settings.use_blur_texture

#endif



//pipeline inputs
layout(location = 0) in vec2 in_texcoord;
//outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;
layout(binding = 1) uniform sampler2D g_depth_texture;
layout(binding = 2) uniform sampler2D g_blur_texture;


float calc_coc_factor( in float depth )
{
    float f0 = (depth + near_in) / ( -near_out + near_in );
	float f1 = (depth + far_in) / ( -far_out + far_in );
	return max( saturate(f0), saturate(f1) );
}

void compute_coc( in vec2 p_uv, out float p_depth, out float p_factor )
{
	vec2 screen_clip = p_uv;
	screen_clip = screen_clip * 2.0 - 1.0;

	float depth = texture(g_depth_texture, p_uv).r;
		  depth = depth * 2.0 - 1.0;

	vec4 screen_coord = vec4(screen_clip, depth, 1.0);
	vec4 view_position = aaa_cam.projection_inverse * screen_coord;
	view_position.xyz /= view_position.w;

	p_depth  = view_position.z;
	p_factor = calc_coc_factor(p_depth);
}

void main()
{
	float blur_size = blur_size_asked;
	if( use_blur_texture == 1 )
	{	//todo add a offset and a factor here
		blur_size = texture(g_blur_texture, in_texcoord).r * blur_size;
	}


	vec3 color = texture( g_input_texture, in_texcoord ).rgb;
	// Disc DOF.
	if( blur_size > 0. )
	{
		float center_depth;
		float center_blur;
		compute_coc( in_texcoord, center_depth, center_blur );

		if( use_vdebug == 1 )
		{
			color.r = center_blur;
		}
		else
		{
			if( center_blur > 0 )
			{
				vec2 sample_offset = 0.5f * input_size_over;
				float total_contribution = 1.0;
				const float coc_size = center_blur * blur_size;
				for( uint i = 0; i < disc_sample_count; i++ )
				{
					vec2 kernel_value = disc_kernel[i];
					vec2 offset = kernel_value * coc_size;
					vec2 tap_coord = in_texcoord + offset * sample_offset;

					vec3 tap_color = texture( g_input_texture, tap_coord ).rgb;

					float tap_depth;
					float tap_blur;
					compute_coc( tap_coord, tap_depth, tap_blur );

					float tap_contribution = (tap_depth > center_depth) ? 1.0 : tap_blur;

					color += tap_color * tap_contribution;
					total_contribution += tap_contribution;
				}
				color /= total_contribution;
			}
	//		out_result = vec4( 1., 0., 0., 1. );
		}
	}
//	else
//		out_result = vec4( 1. );
	out_result = vec4( color, 1. );
}















