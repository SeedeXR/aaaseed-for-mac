

#if USE_AAA_UNIFORM == 1
	#define bypass				aaa_vu_int[0]
	#define mode				aaa_vu_int[1]

#else
	layout(binding = 0) uniform settings_block
	{
		int 	mode; //0 = world, 1 = view
		int 	bypass;
	} u_settings;

	#define bypass				u_settings.bypass
	#define mode				u_settings.mode

#endif


//pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;
layout(binding = 1) uniform sampler2D g_depth_texture;

#define ITERATION_COUNT 5


void main()
{
	if (bypass == 0)
	{
		vec2 screen_clip = in_texcoord;
		screen_clip = screen_clip * vec2(2.0) - vec2(1.0);
		//screen_clip.y *= -1.0;
		float depth_base = texture(g_depth_texture, in_texcoord).r;
		float depth =depth_base;
		depth = depth * 2.0 - 1.0;

		vec4 screen_coord = vec4(screen_clip, depth, 1.0);

		mat4 mat = aaa_cam.view_projection_inverse;

		if (mode == 1)
		{
			mat = aaa_cam.projection_inverse;
		}

		vec4 world_position= mat * screen_coord;
		world_position.xyz /= world_position.w;

		if (depth_base < 1.0)
		{
			out_result = vec4(world_position.xyz, 1.0);
		}
		else
		{
			out_result = vec4(0.0,0.0,0.0, 1.0);
		}
	}
	else
	{
		out_result = vec4( texture(g_input_texture, in_texcoord).rgb, 1.0 );
	}
}


