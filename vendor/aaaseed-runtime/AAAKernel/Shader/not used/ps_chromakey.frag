// GLSL fragment shader
// Chromakeying

uniform vec4	aaa_fu_vec4[6];
uniform float	fu_float[6];

uniform sampler2D tex0;


void main (void)
{
	vec3	chroma_color = aaa_fu_vec4[0].rgb;
	float	threshold = fu_float[0];
	float	delta = clamp(fu_float[1], 0.0, 1.0 );
	vec3	color_tex = texture2D(tex0,gl_TexCoord[0].st).rgb;
	float	dist = distance( color_tex, chroma_color );
	float	alpha = 1.0 - smoothstep( threshold, threshold + delta, dist );
	gl_FragColor = blend( vec4(color_tex, alpha) );
}
