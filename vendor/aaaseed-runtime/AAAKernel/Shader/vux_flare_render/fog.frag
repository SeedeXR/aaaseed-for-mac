

#ifndef ENABLE_2D_DENSITY_MAP
#define ENABLE_2D_DENSITY_MAP 0
#endif


#if USE_AAA_UNIFORM == 1
	#define fog_color_density_low	aaa_fu_vec4[0]
	#define fog_color_density_high	aaa_fu_vec4[1]
	#define fog_start 		 		aaa_fu_float[0]
	#define sun_color 		 		vec3(aaa_fu_float[1], aaa_fu_float[2], aaa_fu_float[3])
	#define sun_direction 	 		vec3(aaa_fu_float[4], aaa_fu_float[5], aaa_fu_float[6])
	#define sun_amount 		 		aaa_fu_float[7]
	#define fog_depth_factor 		aaa_fu_float[8]
	#define fog_y_top 				aaa_fu_float[9]
	#define fog_y_bottom 			aaa_fu_float[10]
	#define fog_y_gamma		 		aaa_fu_float[11]
	#define bypass			 		aaa_vu_int[0]

#else
	layout(binding = 0) uniform fog_settings_block
	{
		vec3 	color;
		float 	start;
		float 	density;
		vec3    sun_color;
		vec3    sun_direction;
		float 	sun_amount;
		float 	fog_depth_factor;
		int 	bypass;
	} u_fog_settings;

	#define fog_color 		    u_fog_settings.color
	#define fog_start 		    u_fog_settings.start
	#define fog_density 	    u_fog_settings.density
	#define sun_color 		    u_fog_settings.sun_color
	#define sun_direction	    u_fog_settings.sun_direction
	#define sun_amount		    u_fog_settings.sun_amount
	#define fog_depth_factor	fog_depth_factor.fog_depth_factor
	#define bypass			    u_fog_settings.bypass

#endif


//shader pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//gbuffer outputs
layout(location = 0) out vec4 out_result;

//texture inputs
	layout(binding = 0) uniform sampler2D g_input_texture;
	layout(binding = 1) uniform sampler2D g_depth_texture;

#if ENABLE_2D_DENSITY_MAP == 1
	layout(binding = 2) uniform sampler2D g_densitymap_texture;
#endif

void main()
{
	if( bypass == 0 )
	{
		vec2 screen_clip = in_texcoord;
//		screen_clip.y = 1.0 -screen_clip.y;
		screen_clip = screen_clip * 2.0 - 1.0;

		float depth = texture( g_depth_texture, in_texcoord ).r;
		depth = depth * 2.0 - 1.0;

		vec4 screen_coord = vec4( screen_clip, depth, 1.0 );

		vec4 view_position = aaa_cam.projection_inverse * screen_coord;
		view_position.xyz /= view_position.w;

		vec4 world_position = aaa_cam.view_projection_inverse * screen_coord;
		world_position.xyz /= world_position.w;

		vec4 base_color = texture( g_input_texture, in_texcoord );

		float d = mix( view_position.z, distance(world_position.xyz, aaa_cam.camera_position), fog_depth_factor );

		d = max( -d-fog_start, 0 );

	#if ENABLE_2D_DENSITY_MAP == 1
		d *= texture( g_densitymap_texture, in_texcoord ).r;
	#endif

		float fog_y_range = fog_y_bottom - fog_y_top;
		float fog_norm_y = ( world_position.y - fog_y_top) / fog_y_range;
		fog_norm_y = clamp_01( fog_norm_y );
		fog_norm_y = pow( fog_norm_y, fog_y_gamma );

		vec4 fog_color_density = mix( fog_color_density_high, fog_color_density_low, fog_norm_y );

		float fog_amount = exp( -d * fog_color_density.w * 0.1 );

		float sun_dot_ray = clamp_01( dot(sun_direction, normalize(view_position.xyz) ) );
		sun_dot_ray *= sun_amount;

		vec3 final_fog_color = mix( fog_color_density.rgb, sun_color, sun_dot_ray );

		base_color.rgb = mix( final_fog_color, base_color.rgb, fog_amount );
		base_color.a = 1.0;

		out_result = base_color;
	}
	else
	{
		out_result = vec4( texture( g_input_texture, in_texcoord ).rgb, 1.0 );
	}
//	out_result = vec4( 1.0 );
}


