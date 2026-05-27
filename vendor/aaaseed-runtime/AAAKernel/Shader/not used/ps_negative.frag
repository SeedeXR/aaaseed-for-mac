// GLSL fragment shader
// Negative Image

uniform sampler2D tex0;

void main (void)
{
	vec4 color = texture2D(tex0,gl_TexCoord[0].st);
	float alpha = color.a;
	color = 1.0 - color;
	color.a = alpha;
	gl_FragColor = blend( color );
}
