

#if USE_AAA_UNIFORM == 1
	#define fog_color 				vec3(aaa_fu_float[0], aaa_fu_float[1], aaa_fu_float[2])
	#define fog_start 				aaa_fu_float[3]
	#define fog_density 			aaa_fu_float[4]
	#define noise_frequency 		vec3(aaa_fu_float[5], aaa_fu_float[5], aaa_fu_float[5])
	#define noise_offset 			vec3(aaa_fu_float[6], aaa_fu_float[7], aaa_fu_float[8])
	#define noise_strength 			aaa_fu_float[9]
	#define noise_density_factor 	aaa_fu_float[10]
	//#define sun_color 				vec3(aaa_fu_float[13], aaa_fu_float[14], aaa_fu_float[15])
	//#define sun_direction 			vec3(aaa_fu_float[16], aaa_fu_float[17], aaa_fu_float[18])

	//#define sun_amount 				aaa_fu_float[19]
	#define sun_color vec3(0.0)
	#define sun_direction vec3(0.0)
	#define sun_amount 0.0
	#define bypass					aaa_vu_int[ 0]

#else
	layout(binding = 0) uniform fog_settings_block
	{
		vec3 	fog_color;
		float 	fog_start;
		float 	fog_density;
		float   noise_frequency;
		vec3    noise_offset;
		float   noise_strength;
		float   noise_density_factor;
		vec3    sun_color;
		vec3    sun_direction;
		float 	sun_amount;
		int 	bypass;
	} u_fog_settings;

	#define fog_color 				u_fog_settings.fog_color
	#define fog_start 				u_fog_settings.fog_start
	#define fog_density 			u_fog_settings.fog_density
	#define noise_frequency 		u_fog_settings.noise_frequency
	#define noise_offset 			u_fog_settings.noise_offset
	#define noise_strength 			u_fog_settings.noise_strength
	#define noise_density_factor 	u_fog_settings.noise_density_factor
	#define sun_color 				u_fog_settings.sun_color
	#define sun_direction		 	u_fog_settings.sun_direction
	#define sun_amount				u_fog_settings.sun_amount
	#define bypass					u_fog_settings.bypass

#endif

#ifndef PI
#define PI acos(-1)
#endif

#define NB_STEPS 16


float GetFogDensity(float depth, float beginFogDistance, float fogDensity)
{
	float d = max(depth - beginFogDistance,0);
	float amount = 1.0 - exp(-d * fogDensity * 0.1);
	return amount;
}


//pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;
layout(binding = 1) uniform sampler2D g_depth_texture;



void main()
{
	if (bypass == 0)
	{
		vec2 screen_clip = in_texcoord;
		screen_clip.y = 1.0 -screen_clip.y;
		screen_clip = screen_clip * 2.0 - 1.0;

		float depth_base = texture(g_depth_texture, in_texcoord).r;
		float depth = depth_base * 2.0 - 1.0;

		vec4 screen_coord = vec4(screen_clip, depth, 1.0);
		vec4 base_color = texture(g_input_texture, in_texcoord);

		vec4 world_position = aaa_cam.view_projection_inverse * screen_coord;
		world_position.xyz /= world_position.w;

		//we accumulate additively, so ray direction does not matter
		vec3 ray_begin = aaa_cam.camera_position;
		vec3 ray_end = world_position.xyz;

		vec3 ray_vector = ray_end- ray_begin;
		float ray_length = length(ray_vector);
		vec3 ray_direction = ray_vector / ray_length;

		float ray_step_length = ray_length / NB_STEPS;

		float accumulated_fog = 0.0;

		vec3 ray_position = ray_begin;

		for (int i = 0; i < NB_STEPS; i++)
		{
			float noise_density = iqFbm3d(ray_position * noise_frequency.xxx + noise_offset) * noise_strength;

			//clamp only to avoid negative values
			noise_density = max(noise_density, 0.0);

			//note: could maybe optimize a little by accumulating step length too?
			// needs profiling
			float ray_camera_distance = distance(aaa_cam.camera_position, ray_position);

			float ray_fog_density = mix(fog_density, fog_density + noise_density, noise_density_factor);

			accumulated_fog += GetFogDensity(ray_camera_distance, fog_start, ray_fog_density) ;
			ray_position += ray_step_length;
		}

		accumulated_fog /= NB_STEPS;
		accumulated_fog = clamp(accumulated_fog, 0.0, 1.0);

		float sun_dot_ray = clamp(dot(sun_direction, ray_direction), 0.0, 1.0);
		sun_dot_ray *= sun_amount;

		vec3 final_fog_color = mix(fog_color, sun_color, sun_dot_ray);

		base_color.rgb = mix(base_color.rgb, final_fog_color, accumulated_fog);

		out_result = vec4(base_color.xyz,1.0);

		//vec3 p = vec3(in_texcoord, 0.0);

		//float noise_density = iqFbm3d(p * noise_frequency + noise_offset) * noise_strength;

		/*if (depth_base < 1.0)
		{
			//out_result = vec4(noise_density,noise_density,noise_density,1.0);
			out_result = vec4(world_position.xyz, 1.0);
		}
		else
		{
			out_result = vec4(0.0,0.0,0.0, 1.0);
		}*/
	}
	else
	{
		out_result = vec4( texture(g_input_texture, in_texcoord).rgb, 1.0 );
	}
}


