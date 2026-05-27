

#if USE_AAA_UNIFORM == 1
#	define bloom_spread 	aaa_fu_float[0]		// default to 1.0
#	define bloom_intensity 	aaa_fu_float[1]		// default to 2.0
#	define bypass			aaa_vu_int[0]

#else
	layout(binding = 0) uniform fog_settings_block
	{
		float 	bloom_spread;		// default to 1.0
		float 	bloom_intensity;	// default to 2.0
		int 	bypass;
		float 	pad;
	} u_settings;
#	define bloom_spread 	u_settings.bloom_spread
#	define bloom_intensity 	u_settings.bloom_intensity
#	define bypass			u_settings.bypass

#endif

//pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;



void main()
{
	if (bypass == 0)
	{
		ivec2 size = textureSize(g_input_texture, 0);

		float uv_x = in_texcoord.x * size.x;
		float uv_y = in_texcoord.y * size.y;

		vec3 sum = vec3(0.0);
		for (int n = 0; n < 9; ++n) {
			uv_y = (in_texcoord.y * size.y) + (bloom_spread * float(n - 4));
			vec4 h_sum = vec4(0.0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x - (4.0 * bloom_spread), uv_y), 0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x - (3.0 * bloom_spread), uv_y), 0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x - (2.0 * bloom_spread), uv_y), 0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x - bloom_spread, uv_y), 0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x, uv_y), 0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x + bloom_spread, uv_y), 0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x + (2.0 * bloom_spread), uv_y), 0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x + (3.0 * bloom_spread), uv_y), 0);
			h_sum += texelFetch(g_input_texture, ivec2(uv_x + (4.0 * bloom_spread), uv_y), 0);
			sum += h_sum.rgb / 9.0;
		}

		vec3 color = texture(g_input_texture, in_texcoord).rgb;
		out_result = vec4(color + ((sum / 9.0) * bloom_intensity), 1.0 );
	}
	else
	{
		out_result = vec4( texture(g_input_texture, in_texcoord).rgb, 1.0 );
	}
}


