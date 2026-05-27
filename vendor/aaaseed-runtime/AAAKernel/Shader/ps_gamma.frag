// GLSL fragment shader
// Gamma

uniform sampler2D	aaa_tex2d[4];


void main (void)
{
	float exposure = aaa_fu_float[1];
	float gamma = aaa_fu_float[0];

	vec4 color;
	color.xyz = texture2D( aaa_tex2d[0], gl_TexCoord[0].st).xyz;

	color = color * exposure;
	color.xyz = pow( color.xyz, gamma );
	color.a = texture2D( aaa_tex2d[0], gl_TexCoord[0].st ).a;

	gl_FragColor = blend( color );
}
