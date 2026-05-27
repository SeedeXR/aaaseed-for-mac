

#if USE_AAA_UNIFORM == 1
#	define bypass				aaa_fu_int[0]
#	define sprite_intensity		aaa_fu_float[0]
#	define time	    		    aaa_fu_float[1]
#	define intensity_range	    vec2(aaa_fu_float[2], aaa_fu_float[3])
#	define color_in				aaa_fu_vec4[0]
#else
	layout(binding = 0) uniform settings_block
	{
		int 	mode;		//0 = texture, 1 = procedural
		int 	bypass;
	} u_settings;

	#define bypass				u_settings.bypass
	#define mode				u_settings.mode

#endif

layout(early_fragment_tests) in;
layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec3 in_world_position;

//outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;
layout(binding = 1) uniform sampler2D g_depth_texture;

void main()
{
	out_result = vec4(in_texcoord, 0.0, 0.5);
	if( bypass == 0 )
	{
		// Begin: Previous //
		vec4 color = texture( g_input_texture, in_texcoord );
		color.a *= sprite_intensity;

		float animated_intensity = sin(in_world_position.x * 20.0 + time) * cos(in_world_position.y * 5.0 + time) + sin(in_world_position.z *1.0 +time);
		animated_intensity = saturate(animated_intensity);
		animated_intensity = intensity_range.x + (animated_intensity * (intensity_range.y - intensity_range.x));

		color.a *= animated_intensity;

		//get final intensity
		//float test = color.r * color.a + color.g * color.a + color.b * color.a;
		float test = dot( color.rgb, color.aaa );
		if (test <= 0.0)
			discard;
		out_result = color * color_in;
		// End: Previous //



// 		// Begin: New //
// 		vec2 screen_clip = in_texcoord;
// //		screen_clip.y = 1.0 - screen_clip.y;
// 		screen_clip = screen_clip * 2.0 - 1.0;

// 		float depth = texture( g_depth_texture, in_texcoord ).r;
// 		depth = depth * 2.0 - 1.0;

// 		vec4 screen_coord = vec4( screen_clip, depth, 1.0 );

// 		vec4 view_position = aaa_cam.projection_inverse * screen_coord;
// 		view_position.xyz /= view_position.w;

// 		vec4 world_position = aaa_cam.view_projection_inverse * screen_coord;
// 		world_position.xyz /= world_position.w;

// 		out_result = vec4(world_position.z, 0.0, 0.0, 1.0);



// 		// vec4 color = texture( g_input_texture, in_texcoord );
// 		// color.a *= sprite_intensity;

// 		// float animated_intensity = sin(in_world_position.x * 20.0 + time) * cos(in_world_position.y * 5.0 + time) + sin(in_world_position.z *1.0 +time);
// 		// animated_intensity = saturate(animated_intensity);
// 		// animated_intensity = intensity_range.x + (animated_intensity * (intensity_range.y - intensity_range.x));

// 		// color.a *= animated_intensity;

// 		// //get final intensity
// 		// //float test = color.r * color.a + color.g * color.a + color.b * color.a;
// 		// float test = dot( color.rgb, color.aaa );
// 		// if (test <= 0.0)
// 		// 	discard;
// 		// out_result = color * color_in;
// 		// End: New //
	}
	else
	{
		out_result = vec4(1);
	}
}


