
uniform vec4	aaa_fu_vec4[6];
uniform float	fu_float[6];

uniform sampler2D tex0;


void main (void)
{
	vec3	chroma_color = aaa_fu_vec4[0].rgb;
	float	orig_per = fu_float[0];
	float	threshold = fu_float[1];
	float	threshold_delta = fu_float[2] + threshold;
//    	float	delta = clamp(fu_float[3], 0.0, 1.0 );

	vec4	color_tex = texture2D(tex0,gl_TexCoord[0].st).rgb;
	float	dist = distance( normalize(color_tex.rgb), normalize(chroma_color) );
//		if( dist > threshold )
//			discard;
	float	alpha;
	if( dist > ( threshold_delta ) )
		discard;
	if( dist < threshold )
		alpha = 1.0;
	else
		alpha = 1.0 - (dist - threshold) / threshold_delta;

//		float	alpha = 1.0 - smoothstep( threshold, threshold + delta, dist );
//		gl_FragColor = vec4(color_tex, alpha);
	vec4	final_color = vec4( color_tex.rgb, alpha );
	gl_FragColor = final_color* orig_per + ( 1.0 - orig_per ) * color_tex ;
}
