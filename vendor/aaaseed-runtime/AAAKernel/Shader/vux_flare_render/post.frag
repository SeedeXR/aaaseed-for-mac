

//-----------------------------------------------------------------------------

#if USE_AAA_UNIFORM == 1
	#define Kelvins 			aaa_fu_float[ 0]
	#define Gamma 				aaa_fu_float[ 1]
	#define FStops 				aaa_fu_float[ 2]
	#define fade_bottom 		aaa_fu_float[ 8]
	#define fade_top 			aaa_fu_float[ 9]
	#define bypass				aaa_vu_int[ 0]
	#define color_factor		aaa_fu_vec4[0]
	#define fade_color			aaa_fu_vec4[1]
#else
	layout(binding = 0) uniform post_settings_block
	{
		float 	kelvins;
		float 	gamma;
		float 	fstops;
		int 	bypass;
	} u_post_settings;

	#define Kelvins 			u_post_settings.kelvins
	#define Gamma 				u_post_settings.gamma
	#define FStops 				u_post_settings.fstops
	#define bypass				u_post_settings.bypass

#endif

//-----------------------------------------------------------------------------

//pipeline inputs
layout(location = 0) in vec2 in_texcoord;
//outputs
layout(location = 0) out vec4 out_result;
//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;

//-----------------------------------------------------------------------------
float apply_gamma( float color, float gamma )
{
	return 1.0f - pow( 1.0f-color, gamma );
}

vec3 apply_gamma( vec3 color, vec3 gamma )
{
	return vec3(1) - pow( vec3(1)-color, gamma );
}

//-----------------------------------------------------------------------------

// Color Temperature is the color due to black body radiation at a given
// temperature. The temperature is given in Kelvin. The concept is widely used
// in photography and in tools such as f.lux.
//
// The function here converts a given color temperature into a near equivalent
// in the RGB colorspace. The function is based on a curve fit on standard sparse
// set of Kelvin to RGB mappings.
//
// The approximations used are suitable for photo-mainpulation and other non-critical uses.
// They are not suitable for medical or other high accuracy
// use cases.
//
// Accuracy is best between 1000K and 40000K.

//float saturate(float v) { return clamp(v, 0.0f, 1.0f); }
//vec2  saturate(vec2  v) { return clamp(v, vec2(0.0f, 0.0f), 			vec2(1.0f, 1.0f)); }
//vec3  saturate(vec3  v) { return clamp(v, vec3(0.0f, 0.0f, 0.0f), 	    vec3(1.0f, 1.0f, 1.0f)); }
//vec4  saturate(vec4  v) { return clamp(v, vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f)); }

vec3 kelvins_to_rgb( float p_kelvins )
{
	vec3 res;
    float temperature = clamp(p_kelvins, 1000.0, 40000.0) * 0.01;

    if (temperature <= 66.0)
    {
        res.r = 1.0;
        res.g = saturate(0.39008157876901960784 * log(temperature) - 0.63184144378862745098);
    }
    else
    {
    	float t = temperature - 60.0;
        res.r = saturate(1.29293618606274509804 * pow(t, -0.1332047592));
        res.g = saturate(1.12989086089529411765 * pow(t, -0.0755148492));
    }

    if (temperature >= 66.0f)
        res.b = 1.0f;
    else if(temperature <= 19.0f)
        res.b = 0.0f;
    else
        res.b = saturate(0.54320678911019607843f * log(temperature - 10.0f) - 1.19625408914f);

    return res;
}

//-----------------------------------------------------------------------------

void main()
{
	if (bypass == 0)
	{
		vec3 col = texture(g_input_texture, in_texcoord).rgb;

		col.rgb = clamp_01( col.rgb * color_factor.rgb );

		// To linear space.
		col.r = srgb_to_linear(col.r);
		col.g = srgb_to_linear(col.g);
		col.b = srgb_to_linear(col.b);

		// Temperature.
		vec3 bbt = kelvins_to_rgb(Kelvins);
		col.rgb *= bbt.rgb;

		// Gamma.
		float gam = clamp( Gamma, 0.0000001, 3.0 );

		col.rgb = apply_gamma( col.rgb, vec3(gam) );

		// Exposure.
		col.rgb = col.rgb * pow(2.0, FStops);

		// Back from linear space.
		col.r = linear_to_srgb(col.r);
		col.g = linear_to_srgb(col.g);
		col.b = linear_to_srgb(col.b);


		float fade = smoothstep( fade_top, fade_bottom, in_texcoord.y );
		// Debug: test burned out pixel (>1.0).
		//if (col.r > 1.0 || col.g > 1.0 || col.b > 1.0)
		//	out_result = vec4(1.0, 0.0, 0.0, 1.0);

		out_result.rgb = clamp( mix( col.rgb, fade_color.rgb, fade ), 0.0, 1.0 );
		out_result.a = 1.0;
	}
	else
	{
		out_result = vec4(texture(g_input_texture, in_texcoord).rgb, 1.0);
	}
}

//-----------------------------------------------------------------------------





















