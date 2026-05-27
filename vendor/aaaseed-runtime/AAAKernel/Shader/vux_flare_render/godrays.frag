

//todo pass this number as uniform
#ifndef NUM_SAMPLES
#define NUM_SAMPLES 64
#endif


#if USE_AAA_UNIFORM == 1
	#define godrays_screen_light_position 		vec2(aaa_fu_float[0], aaa_fu_float[1])	// light position (screen space [-1,1], default to [0,0] (center))
	#define godrays_illumination_decay 			aaa_fu_float[2]                         // default to 0.5
	#define godrays_density 					aaa_fu_float[3]                         // default to 0.1
	#define godrays_weight 						aaa_fu_float[4]                         // default to 0.2
	#define godrays_decay 						aaa_fu_float[5]                         // default to 1.0
	#define bypass								aaa_vu_int[ 0]

#else
	layout(binding = 0) uniform godrays_settings_block
	{
		vec2  	screen_light_position;		// light position (screen space [-1,1], default to [0,0] (center))
		float 	illumination_decay;			// default to 0.5
		float 	density;					// default to 0.1
		float 	weight;						// default to 0.2
		float 	decay;						// default to 1.0
		int 	bypass;
		float 	pad;
	} u_godrays_settings;

	#define godrays_screen_light_position 		u_godrays_settings.screen_light_position
	#define godrays_illumination_decay 			u_godrays_settings.illumination_decay
	#define godrays_density 					u_godrays_settings.density
	#define godrays_weight 						u_godrays_settings.weight
	#define godrays_decay 						u_godrays_settings.decay
	#define bypass								u_godrays_settings.bypass

#endif


//pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//outputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;



void main()
{
	if( bypass == 0 )
	{
		vec2 delta_text_coord = in_texcoord - godrays_screen_light_position;

		delta_text_coord *= (1.0 / NUM_SAMPLES) * godrays_density;

		vec3 color = texture(g_input_texture, in_texcoord).rgb;

		float base_decay = godrays_illumination_decay;

		vec2 uv = in_texcoord;

		for (int i = 0 ; i < NUM_SAMPLES; i++)
		{
			uv -= delta_text_coord;
			vec3 color_sample = texture(g_input_texture, uv).rgb;

			color_sample *= (base_decay * godrays_weight);

			color += color_sample;
			base_decay *= godrays_decay;
		}

		out_result = vec4(color, 1.0);
		out_result.rgb = clamp( out_result.rgb, 0.0, 1.0 );
	}
	else
	{
		out_result = vec4(texture(g_input_texture, in_texcoord).rgb, 1.0);
	}
}













