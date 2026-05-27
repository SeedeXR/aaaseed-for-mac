// GLSL fragment shader
// Negative Image

uniform sampler2D	aaa_tex2d[4];

void main (void)
{
	vec4 color = texture2D( aaa_tex2d[0],gl_TexCoord[0].st);
	float alpha = color.a;
	color = 1.0 - color;
	color.a = alpha;
	gl_FragColor = blend( color );
}
