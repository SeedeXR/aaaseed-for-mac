

uniform sampler2D texUnit;
void main(void)
{
   vec2 texCoord = gl_TexCoord[0].xy;
   vec4 c = texture2D(texUnit, texCoord);
	float b = (	c.x*4.0/21.0+
				c.y*16.0/21.0+
				c.z*1.0/21.0);
   vec4 vec4col = vec4(b);
   vec4col.a = 1.0;
   gl_FragColor = vec4col;
}