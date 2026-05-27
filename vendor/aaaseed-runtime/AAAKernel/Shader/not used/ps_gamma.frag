// GLSL fragment shader
// Gamma

uniform float fu_float[6];

void main (void)
{
	float exposure = fu_float[1];
	float gamma = fu_float[0];

	vec4 color;
	color.xyz = texture2D( tex0, gl_TexCoord[0].st).xyz;

	color = color * exposure;
	color.xyz = pow( color.xyz, gamma );
	color.a = texture2D( tex0, gl_TexCoord[0].st ).a;

	gl_FragColor = blend( color );
}
