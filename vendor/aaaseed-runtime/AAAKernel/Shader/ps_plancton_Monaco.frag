// GLSL fragment shader
// Apply texture

//uniform float		aaa_fu_float[6];
in		float		sel_x;		//	this to pass to the fragment shader
in		float		sel_y;		//	this to pass to the fragment shader
in		float	aaa_att_w;		//	this to pass to the fragment shader

void main()
{
	vec2  tex = gl_TexCoord[0].st;
	tex = tex * 2. - 1.;
	float d = dot( tex.st, tex.st );
	if( d > 1. )
		discard;

	d = 1. - pow( d, aaa_fu_float[0] );
	gl_FragColor = vec4( gl_Color.xyz, aaa_att_w * d ) * aaa_fu_vec4[0];

//	gl_FragColor = vec4(1.);
}
