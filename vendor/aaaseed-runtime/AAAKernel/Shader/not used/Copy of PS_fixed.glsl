uniform sampler2D tex0;

void main (void)
{
//       vec4 color;
//       vec4 texture;

//       color = gl_Color;

//       color = texture2D(tex0, gl_TexCoord[0].xy);

//       gl_FragColor = 1 - color;
	vec4 color = texture2D(tex0,gl_TexCoord[0].st);
	gl_FragColor = 1 - color;
}
