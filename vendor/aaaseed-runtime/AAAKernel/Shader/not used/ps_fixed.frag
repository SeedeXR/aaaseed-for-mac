// GLSL fragment shader
// Apply texture

uniform sampler2D	aaa_tex0;

void main (void)
{
	vec4 color;
	color = gl_Color;

	vec4 texture = texture2D(tex0,gl_TexCoord[0].st);

	int tex_env_mode = aaa_tex_env_mode[0];

	gl_FragColor = blend( texture, tex_env_mode );
}
