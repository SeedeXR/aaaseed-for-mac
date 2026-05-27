

uniform sampler2D tex0;

void main (void)
{
	int i;
	vec4 sum = vec4(0.0);
 //todoopt use swizzle and make it resolution independant
	sum = texture2D(tex0, gl_TexCoord[0].st) * (-4.0);
	sum += texture2D(tex0, gl_TexCoord[0].st + vec2(-1/640, 0.0));
	sum += texture2D(tex0, gl_TexCoord[0].st + vec2(1.0/640, 0.0));
	sum += texture2D(tex0, gl_TexCoord[0].st + vec2(0.0, 1.0/352));
	sum += texture2D(tex0, gl_TexCoord[0].st + vec2(0.0, -1.0/352));

	vec4 color = texture2D(tex0,gl_TexCoord[0].st);

	gl_FragColor = blend( color + sum );
}
