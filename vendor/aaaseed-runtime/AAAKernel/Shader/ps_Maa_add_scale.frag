
// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];


void main()
{
vec4	src;

	src = texture2D( aaa_tex2d[0], gl_TexCoord[0].st);

	gl_FragColor = aaa_fu_vec4[0] + src * aaa_fu_vec4[1];
}

