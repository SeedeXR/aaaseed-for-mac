
uniform sampler2D	tex0;

void main (void)
{
	vec4 color;
	color = gl_Color;

	vec4 texture = texture2D(tex0,gl_TexCoord[0].st);

	gl_FragColor = blend( texture );
}
