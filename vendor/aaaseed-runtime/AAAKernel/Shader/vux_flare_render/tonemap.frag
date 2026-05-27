

#if USE_AAA_UNIFORM == 1
	#define tone_shoulder_strength 			aaa_fu_float[ 0]		// default to 0.23
	#define tone_linear_strength 			aaa_fu_float[ 1]		// default to 0.33
	#define tone_linear_angle 				aaa_fu_float[ 2]		// default to 0.1
	#define tone_toe_strength 				aaa_fu_float[ 3]		// default to 0.2
	#define tone_toe_numerator 				aaa_fu_float[ 4]		// default to 0.01
	#define tone_toe_denominator 			aaa_fu_float[ 5]		// default to 0.3
	#define tone_linear_white_point_value 	aaa_fu_float[ 6]		// default to 2.0
	#define tone_exposure_bias 				aaa_fu_float[ 7]		// default to 2.0
	#define tone_inverse_gamma 				aaa_fu_float[ 8]		// default to 0.45
	#define tone_keying 					aaa_fu_float[ 9]		// default to 0.25
	#define tone_average 					aaa_fu_float[10]		// default to 0.5

	#define bypass							aaa_vu_int[ 0]

#else
	layout(binding = 0) uniform tonemap_settings_block
	{
		float 	shoulder_strength;									// default to 0.23
		float 	linear_strength;									// default to 0.33
		float 	linear_angle;                                       // default to 0.1
		float 	toe_strength;                                       // default to 0.2
		float 	toe_numerator;                                      // default to 0.01
		float 	toe_denominator;                                    // default to 0.3
		float 	linear_white_point_value;                           // default to 2.0
		float 	exposure_bias;                                      // default to 2.0
		float 	inverse_gamma;                                      // default to 0.45
		float 	keying;                                             // default to 0.25
		float 	average;                                            // default to 0.5
		int 	bypass;
	} u_tonemap_settings;

	#define tone_shoulder_strength 			u_tonemap_settings.shoulder_strength
	#define tone_linear_strength 			u_tonemap_settings.linear_strength
	#define tone_linear_angle 				u_tonemap_settings.linear_angle
	#define tone_toe_strength 				u_tonemap_settings.toe_strength
	#define tone_toe_numerator 				u_tonemap_settings.toe_numerator
	#define tone_toe_denominator 			u_tonemap_settings.toe_denominator
	#define tone_linear_white_point_value 	u_tonemap_settings.linear_white_point_value
	#define tone_exposure_bias 				u_tonemap_settings.exposure_bias
	#define tone_inverse_gamma 				u_tonemap_settings.inverse_gamma
	#define tone_keying 					u_tonemap_settings.keying
	#define tone_average 					u_tonemap_settings.average
	#define bypass							u_tonemap_settings.bypass

#endif


vec3 Uncharted2Tonemap(vec3 x)
{
   return ( ( x * ( tone_shoulder_strength * x
    + tone_linear_angle * tone_linear_strength)
    + tone_toe_strength* tone_toe_numerator)
    / ( x* ( tone_shoulder_strength * x + tone_linear_strength)
    + tone_toe_strength * tone_toe_denominator))
    - tone_toe_numerator / tone_toe_denominator;
}


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
		vec3 base_color = texture(g_input_texture, in_texcoord).rgb;

		float a_over_bar = tone_keying * (1.0 / tone_average);

		base_color *= a_over_bar;
		vec3 curr = Uncharted2Tonemap(tone_exposure_bias * base_color);
		vec3 white_scale = vec3(1.0) / Uncharted2Tonemap(vec3(tone_linear_white_point_value));
		vec3 color = curr * white_scale;
		vec3 retColor = pow(color,vec3(tone_inverse_gamma));

		out_result = vec4(retColor, 1.0);
	}
	else
	{
		out_result = vec4(texture(g_input_texture, in_texcoord).rgb, 1.0);
	}
}


