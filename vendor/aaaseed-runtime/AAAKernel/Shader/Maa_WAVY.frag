
in ST_AAA_BV BV;

// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];


void main()
{
	vec2	uv = BV.tex_coor[0].st + aaa_fu_float[3] * BV.nor_ec.xy ;
	vec4	src1_color;
	vec4	src2_color;

	src1_color = texture2D( aaa_tex2d[0], uv );	//gl_TexCoord[0].st );
	src2_color = texture2D( aaa_tex2d[1], uv );

	if( aaa_fu_int[0]==1 )
	{
		src1_color.rgb = vec3(1) - src1_color.rgb;
		src2_color.rgb = vec3(1) - src2_color.rgb;
	}
	float f = aaa_fu_float[0];
	vec4 color = src1_color * aaa_fu_vec4[1] * f + src2_color * aaa_fu_vec4[2] * (1.-f);
	if( aaa_fu_float[1]>0. && aaa_fu_float[2]>0. )
	{
		color.a = smoothstep( aaa_fu_float[1], aaa_fu_float[2], gray(color.xyz) );
	}
	gl_FragColor = color * BV.color;
//	gl_FragColor.b = 0.;
//	gl_FragColor.a = 1.;

//	gl_FragColor.xyz = BV.nor_ec.xyz;s
//	gl_FragColor.a = 1.;
}
