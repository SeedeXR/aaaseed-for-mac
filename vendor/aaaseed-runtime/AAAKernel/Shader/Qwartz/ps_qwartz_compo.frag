
//Maa july 2012
uniform sampler2D	aaa_tex2d[4];

// Uniform variables for texturing

//	aaa_fu_vec4[0]
//	aaa_fu_vec4[1,2]


void main (void)
{
	vec2 co = gl_TexCoord[0].st;
	vec4 p1 = texture2D( aaa_tex2d[0], co );
	co.x = aaa_fu_float[0] + co.x * aaa_fu_float[1];
	co.y = aaa_fu_float[2] + co.y * aaa_fu_float[3];
	vec4 p2 = texture2D( aaa_tex2d[1], co );
	//p2.a = p2.r;
	//p2.r = 1.;
	//p2.g = 1.;
	//p2.b = 1.;
	float alpha = p2.r;
	p1.a = alpha;
	#if 0
		p1.g *= 1 + (alpha - 1.) * aaa_fu_float[4] *.4	;
	#else
		p1.rgb *= 1 + (alpha - 1.) * aaa_fu_float[4];
	#endif
	gl_FragColor = p1;
}
