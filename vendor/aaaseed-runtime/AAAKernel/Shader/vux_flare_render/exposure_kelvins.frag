

#if USE_AAA_UNIFORM == 1
	#define kelvin_degrees 					aaa_fu_float[0]		// default to 5500 - range from 1000 to 6500
	#define gamma_factor 					aaa_fu_float[1]		// default to 2.2  - range from 0.0000001 to 3.0
	#define exposure_fstops 				aaa_fu_float[2]		// default to 0.0  - range from -5.0 to 5.0

#else
	layout(binding = 0) uniform tonemap_settings_block
	{
		float kelvins;						// default to 5500 - range from 1000 to 6500
		float gamma;						// default to 2.2  - range from 0.0000001 to 3.0
		float exposure_fstops;				// default to 0.0  - range from -5.0 to 5.0
		float pad;
	} u_settings;

	#define kelvin_degrees			u_settings.kelvins
	#define gamma_factor 			u_settings.gamma
	#define exposure_fstops 		u_settings.exposure_fstops


#endif


//pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//goutputs
layout(location = 0) out vec4 out_result;

//texture inputs
layout(binding = 0) uniform sampler2D g_input_texture;


// from sRGB to Linear Electro-Optical Transfer Function (EOTF)
// sRGB / 709
float srgb_to_linear(in float v)
{
	if (v < 0.04045f)
	{
		if (v < 0.0)
			return 0.0;
		else
			return v * (1.0 / 12.92f);
	}
	else
	{
		return pow( (v + 0.055f) * (1.0 / 1.055), 2.4 );
	}
}

// to sRGB from Linear Opto-Electronic Transfer Function (OETF)
// sRGB / 709
float linear_to_srgb(in float v)
{
	if (v < 0.0031308f)
	{
		if (v < 0.0)
			return 0.0;
		else
			return v * 12.92;
	}
	else
	{
		return 1.055f * pow(v, 1.0 / 2.4) - 0.055;
	}
}

float gamma(float color, float gamma)
{
	return (1.0 - pow(1.0-color, gamma));
}

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

vec3 kelvins_to_rgb(float p_kelvins)
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

    if (temperature >= 66.0)
        res.b = 1.0;
    else if(temperature <= 19.0)
        res.b = 0.0;
    else
        res.b = saturate(0.54320678911019607843f * log(temperature - 10.0) - 1.19625408914f);

    return res;
}


void main()
{
	vec3 col = texture(g_input_texture, in_texcoord).rgb;

	// to linear space.
	col.r = srgb_to_linear(col.r);
	col.g = srgb_to_linear(col.g);
	col.b = srgb_to_linear(col.b);

	// temperature.
	vec3 bbt = kelvins_to_rgb(kelvin_degrees);
	col.r *= bbt.r;
	col.g *= bbt.g;
	col.b *= bbt.b;

	// gamma.
	col.r = gamma(col.r, clamp(gamma_factor, 0.0000001, 3.0));
	col.g = gamma(col.g, clamp(gamma_factor, 0.0000001, 3.0));
	col.b = gamma(col.b, clamp(gamma_factor, 0.0000001, 3.0));

	// exposure.
	col.rgb = col.rgb * pow(2.0, exposure_fstops);

	// back from linear space.
	col.r = linear_to_srgb(col.r);
	col.g = linear_to_srgb(col.g);
	col.b = linear_to_srgb(col.b);

	out_result = vec4(col, 1.0);
}











