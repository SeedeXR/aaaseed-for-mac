

#if USE_AAA_UNIFORM == 1
	#define bypass				aaa_vu_int[0]
	#define mode				aaa_vu_int[1]
	#define multiplier 	        aaa_fu_float[0]
	
#else
	layout(binding = 0) uniform settings_block
	{
		float   multiplier;
		int 	mode;
		int 	bypass;
	} u_settings;

	#define bypass				u_settings.bypass
	#define mode				u_settings.mode
	#define multiplier			u_settings.multiplier
#endif

//pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;
layout(binding = 1) uniform sampler2D g_depth_texture;
layout(binding = 2) uniform sampler2D g_reference_texture;


void main()
{
	if( bypass == 0 )
	{
		vec2 screen_clip = in_texcoord;
		screen_clip = screen_clip * vec2(2.0) - vec2(1.0);
		//screen_clip.y *= -1.0;
		float depth_base = texture(g_depth_texture, in_texcoord).r;
		float depth =depth_base;
		depth = depth * 2.0 - 1.0;

		vec4 screen_coord = vec4(screen_clip, depth, 1.0);

		vec4 world_position= aaa_cam.view_projection_inverse * screen_coord;
		///vec4 world_position= aaa_cam.view_inverse * aaa_cam.projection_inverse * screen_coord;
		world_position.xyz /= world_position.w;

		vec3 world_position_ref = texture(g_reference_texture, in_texcoord).xyz;

		vec3 diff = abs(world_position.xyz - world_position_ref);

		if( mode == 1 )
		{
			float d = dot(diff, diff);
			diff = vec3(d);
		}

		diff *= multiplier;

		if( depth_base < 1.0 )
			out_result = vec4( diff.xyz, 1. );
		else
			out_result = vec4( 0.,0.,0., 1. );
	}
	else
	{
		out_result = vec4( texture( g_input_texture, in_texcoord ).rgb, 1.0 );
	}
}


