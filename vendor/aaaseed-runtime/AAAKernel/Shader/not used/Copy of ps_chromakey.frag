// GLSL fragment shader
// Chromakeying


uniform	sampler2D	aaa_tex[4];

void main (void)
{
	vec3	chroma_color = aaa_fu_vec4[0].rgb;
	float	threshold_low = aaa_fu_float[0];
	float	threshold_high = aaa_fu_float[1];
	vec3	color_tex = texture2D( aaa_tex[0], gl_TexCoord[0].st ).rgb;
	float	dist = distance( color_tex, chroma_color );
	float	alpha = smoothstep( threshold_low, threshold_high, dist );
//	if( dist > threshold_high )
//		discard;
	gl_FragColor = vec4(color_tex, alpha);
}
