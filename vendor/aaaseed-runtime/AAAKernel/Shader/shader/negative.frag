

uniform sampler2D texUnit;
void main(void)
{
   vec2 texCoord = gl_TexCoord[0].xy;
   vec4 c = texture2D(texUnit, texCoord);

	c.r = 1.0-c.r;
	c.g = 1.0-c.g;
	c.b = 1.0-c.b;

   gl_FragColor = c;
}