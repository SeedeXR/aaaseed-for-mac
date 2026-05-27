// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];


//hack dpool
void main (void)
{
vec4	color;

	color =		aaa_fu_float[0] * texture2D( aaa_tex2d[0], gl_TexCoord[0].xy );
	color +=	aaa_fu_float[1] * texture2D( aaa_tex2d[1], gl_TexCoord[0].xy );
//	color +=	aaa_fu_float[2] * texture2D( aaa_tex2d[2], gl_TexCoord[0].xy);
//	color +=	aaa_fu_float[3] * texture2D( aaa_tex2d[3], gl_TexCoord[0].xy);

	gl_FragColor = color;
}
