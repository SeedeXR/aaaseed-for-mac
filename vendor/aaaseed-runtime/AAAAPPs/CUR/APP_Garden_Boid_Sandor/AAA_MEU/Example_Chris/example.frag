
#if USE_AAA_UNIFORM == 1
#else
#endif

#define MAX_DISTANCE 20.
#define MAX_STEPS    100
#define EPSILON        0.0001

#define time 		aaa_fu_float[0]
#define hue_shift 	aaa_fu_float[1]
#define sat_offset 	aaa_fu_float[2]

layout(binding = 0) uniform sampler2D g_input_texture;

layout(location = 0) in vec2 in_texcoord;	
layout(location = 1) in vec4 layer_color;

layout(location = 0) out vec4 out_result;

uniform sampler2D   aaa_tex2d[4];

void main(void)
{

	vec4	color = texture( aaa_tex2d[0], in_texcoord );   // * aaa_fu_float[0];
	//color = vec4( 1,1,1, .5 );

#if 0
    vec3    hsv = rgb2hsv( color.rgb );
    hsv.x += hue_shift;
   
 //   hsv.y += sat_offset;
    hsv.y = clamp_01( hsv.y *sat_offset);

    hsv.z *= 1.2;
    hsv.z = clamp_01( hsv.z );

	color.rgb = hsv2rgb( hsv );
#endif
	out_result = color * layer_color;
 // out_result.a = layer_color.a;

	// out_result = vec4(in_texcoord, 0, 1);
	// out_result = layer_color;
}
