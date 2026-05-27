
// --------------------------------------------------------------------------
// IN, OUT, INITIALIZATION

//	output data
out		vec4		fragColor;
//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{
	vec4 col = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	gl_FragColor.xyz = aaa_fu_float[1] * ( col.xyz - aaa_fu_float[0] );
	gl_FragColor.a = col.a;
}
