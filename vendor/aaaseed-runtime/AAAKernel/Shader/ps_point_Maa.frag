
//	v 0.1		Mai 2016		maa

// Uniform variables for texturing
//uniform sampler2D	aaa_tex2d[4];



in GS_out
{
	vec2 uv;
	vec4 color;
//	vec3 normal;
} gs_in;

void main()
{
	//vec2 ori	= gl_TexCoord[0].st;
	//src	= texture2D( aaa_tex2d[0], gs_in.uv );

	float d;
	if( aaa_fu_int[0] == 0 )
		d = max( abs( gs_in.uv.s - .5), abs( gs_in.uv.t - .5) ) * 2.;
	else
	{
		vec2 uv = gs_in.uv - .5;
		d = dot( uv, uv );
		if( d > .25  )
			discard;
		d *= 4;
	}
//	d = pow( d, aaa_fu_float[0] );
//	d = 1 - d;
	d = smoothstep( aaa_fu_float[0], aaa_fu_float[1], d );
	vec4 src = vec4( 1, 1, 1, 1 - d );

	src.rgb *= ( 1.-aaa_fu_float[2] );

	//gl_FragColor = src * gl_Color;
	gl_FragColor = src * gs_in.color;
//	gl_FragColor = vec4( 1. );
}
