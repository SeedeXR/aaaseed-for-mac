//#version 330

// GLSL fragment shader
// Apply texture

// --------------------------------------------------------------------------
// IN, OUT, INITIALIZATION

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


in VS_out
{
	vec2	uv;
	vec4	color;
} fs_in;

// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{
	vec4 col = texture2D( aaa_tex2d[0], fs_in.uv );

	vec3 color = aaa_fu_float[1] * (col.xyz - aaa_fu_float[0]);
	color *= vec3(aaa_fu_float[2], aaa_fu_float[3], aaa_fu_float[4] );
	color = clamp( color, vec3(0.,0.,0.), vec3(1.,1.,1.) );
	gl_FragColor.xyz = pow( color, vec3( aaa_fu_float[5], aaa_fu_float[6], aaa_fu_float[7] ) );

//	gl_FragColor.r = 1.;
//	gl_FragColor.g = 1.;
//	gl_FragColor.b = 1.;
	gl_FragColor.a = 1.;

}
