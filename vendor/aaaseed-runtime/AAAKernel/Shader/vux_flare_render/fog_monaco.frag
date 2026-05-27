

#ifndef ENABLE_2D_DENSITY_MAP
#define ENABLE_2D_DENSITY_MAP 0
#endif


#if USE_AAA_UNIFORM == 1
	#define dist_begin				aaa_fu_vec4[0].xyz
	#define dist_factor				aaa_fu_vec4[1].xyz
	#define fog_color_top			aaa_fu_vec4[2]
	#define fog_color_bottom		aaa_fu_vec4[3]

//	#define fog_start 		 		aaa_fu_float[0]
//	#define fog_density	 			aaa_fu_float[0]

	#define sun_pos_world	 		aaa_fu_vec4[4].xyz
	#define sun_r2					aaa_fu_vec4[4].w
	#define sun_color 		 		aaa_fu_vec4[5].xyz
	#define sun_amount 		 		aaa_fu_float[7]
	#define dist_gamma 				aaa_fu_float[8]
	#define fog_y_top 				aaa_fu_float[9]
	#define fog_y_bottom 			aaa_fu_float[10]
	#define fog_y_gamma		 		aaa_fu_float[11]
	#define vdebug 			 		aaa_fu_int[0]

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
	if( vdebug > 0 )
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

		vec4 sun_dir = aaa_cam.view * vec4( sun_pos_world.xyz, 0. );
		//sun_pos.xyz = sun_dir.xyz/sun_dir.w;
		sun_dir.xyz = normalize(sun_dir.xyz);

		vec4 base_color = texture( g_input_texture, in_texcoord );

		//float d = mix( view_position.z, distance(world_position.xyz, aaa_cam.camera_position), fog_depth_factor );

		vec3 v = world_position.xyz;
		v.z = abs(v.z);
		v.y = -v.y;
		v -= vec3( 21, 0, 4.4 ) + dist_begin;
		v *= dist_factor;
		float d = max( max( v.x, v.y) , v.z );

//		vec3 vec_sun = sun_pos - world_position.xyz;
//		float dist_sun_2 = dot( vec_sun, vec_sun );

		if( vdebug >= 2 )
		{
			if( d<0 )
				out_result = vec4( 1,1,1,1 );
			else if ( d>1. )
				out_result = vec4( 0,0,1,1 );
			else
				out_result = mix( vec4(1,0,0,1), vec4(0,1,0,1), d );
//			if( dist_sun_2 < sun_r2 )
//				out_result = vec4( 1,1,0,1);
			if( vdebug == 3 )
				out_result *= base_color;
		}
		else
		{
//			d = max( -d-fog_start, 0 );

	#if ENABLE_2D_DENSITY_MAP == 1
			d *= texture( g_densitymap_texture, in_texcoord ).r;
	#endif

			float fog_y_range = fog_y_top - fog_y_bottom;
			float fog_norm_y = ( world_position.y - fog_y_bottom) / fog_y_range;
			fog_norm_y = clamp_01( fog_norm_y );
			fog_norm_y = pow( fog_norm_y, fog_y_gamma );

			vec3 fog_color = fog_color_top.rgb;
			float sun_dot_ray = clamp_01( dot(sun_dir.xyz, normalize(view_position.xyz) ) );
			sun_dot_ray = smoothstep( aaa_fu_float[5], aaa_fu_float[6], sun_dot_ray );
			sun_dot_ray *= sun_amount;

			fog_color = mix( fog_color_bottom.rgb, fog_color, fog_norm_y );
			fog_color = mix( fog_color, sun_color, sun_dot_ray );

			//fog_color = vec3( sun_dot_ray, sun_dot_ray, sun_dot_ray );

			float fog_amount = clamp_01(d); //exp( -d * fog_color_density.w * 0.1 );
			fog_amount = pow( fog_amount, dist_gamma );

//			if( dist_sun_2 < sun_r2 )
//				fog_color.rgb = mix( fog_color.rgb, sun_color, 1 );

//			float sun_dot_ray = clamp_01( dot(sun_direction, normalize(view_position.xyz) ) );
//			sun_dot_ray *= sun_amount;

//			vec3 final_fog_color = mix( fog_color_density.rgb, sun_color, sun_dot_ray );
			vec3 final_fog_color = fog_color.rgb;

			base_color.rgb = mix( base_color.rgb, final_fog_color, fog_amount );
			base_color.a = 1.0;

			out_result = base_color;
		}
	}
	else
	{
		out_result = vec4( texture( g_input_texture, in_texcoord ).rgb, 1.0 );
	}
}


