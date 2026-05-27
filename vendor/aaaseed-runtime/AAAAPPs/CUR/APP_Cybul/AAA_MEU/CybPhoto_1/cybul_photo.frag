
#if USE_AAA_UNIFORM == 1

#	define u_contrast			aaa_fu_float[0]
#	define u_brightness			aaa_fu_float[1]
#	define u_saturation			aaa_fu_float[2]

#else

#	define u_contrast			0
#	define u_brightness			0
#	define u_saturation			0

#endif


layout(binding = 0) uniform sampler2D g_input_texture;

layout(location = 0) in vec2 in_texcoord;	

layout(location = 0) out vec4 out_result;

CONST float f1 = aaa_fu_float[0];

void main(void)
{
	vec4 color = texture( g_input_texture, in_texcoord );
	float grey = compute_gray( color.rgb );
	out_result = vec4( mix( color.rgb, vec3(grey), f1), 1. );


	// brightness
	float brightness = u_brightness * .5;
	color.rgb += brightness;
	clamp_01(color);

	// contrast
	float contrast = u_contrast * 128.;
	float factor = (259. * (contrast + 255.)) / (255. * (259. - contrast));
	color.rgb = (factor * (255. * color.rgb - 128.) + 128.) / 255.; 
	clamp_01(color);

	// saturation
	float saturation = 0.8 * u_saturation;
	float rgb_max = max(max(color.r, color.g), color.b);
	float rgb_min = min(min(color.r, color.g), color.b);
	float delta = rgb_max - rgb_min;
	if( delta != 0. )
	{ 
		float value = rgb_max + rgb_min;
		float l = value / 2.;
		float s = (l < 0.5 ? (delta / value) : delta / (2. - value));

		float alpha = 0.;
		if( saturation >= 0. )
		{
			if( saturation + s >= 1. )	alpha = s;
			else 						alpha = 1. - saturation;
		
			alpha = 1. / alpha - 1.;
			color.rgb += (color.rgb - l) * alpha;
			clamp_01(color);
		}
		else
		{
			alpha = saturation;
			color.rgb = l + (color.rgb - l) * (1. + alpha);
			clamp_01(color);
		}
	}

	// result
	out_result = color;
}
