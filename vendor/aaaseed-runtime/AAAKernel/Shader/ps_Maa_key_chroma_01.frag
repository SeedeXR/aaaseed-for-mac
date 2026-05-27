// GLSL fragment shader
// Chromakeying

//uniform vec4	aaa_fu_vec4[6];
//	0 chroma color
//	1 Incrust color
//uniform float	aaa_fu_float[6];
//	0	threshold_min
//	1	threshold_max


uniform int		aaa_fu_int[6];

uniform float	aaa_fu_src;	//	quantity of the fixed path texture
uniform float	aaa_fu_out;	//	quantity of the computed texture from the shader

uniform sampler2D	aaa_tex2d[4];

void main (void)
{
vec3	chroma_color = aaa_fu_vec4[0].rgb;

float	threshold_max = aaa_fu_float[0];
float	threshold_min = threshold_max - aaa_fu_float[1];

vec3	color_red;

	vec4	color_tex = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	vec4	color_dst;

	float	alpha;

	float grey = gray( color_tex.rgb );

	if( grey < aaa_fu_float[2] )
		alpha = 1;
	else
	{
		float	dist = distance( normalize(color_tex.rgb), normalize(chroma_color) );
//		float	dist = distance( color_tex.rgb, chroma_color );

//	if( dist < threshold_min )
//		alpha = 1.0;
//	else if( dist > threshold_max )
//		alpha = 0.0;
//	else
		alpha = smoothstep( threshold_min, threshold_max, dist );
	}
//	gl_FragColor = vec4( color_tex.rgb, alpha ) * aaa_fu_out + color_tex * aaa_fu_src;

//	gl_FragColor = mix( color_tex, aaa_fu_vec4[1], alpha ) * aaa_fu_out + color_tex * aaa_fu_src;
	gl_FragColor = vec4( color_tex.rgb, alpha ) * aaa_fu_out + color_tex * aaa_fu_src;
}
