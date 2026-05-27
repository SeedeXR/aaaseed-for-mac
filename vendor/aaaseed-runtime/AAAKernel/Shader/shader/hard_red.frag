

uniform sampler2D texUnit;
void main(void)
{
   vec2 texCoord = gl_TexCoord[0].xy;
   vec4 c = texture2D(texUnit, texCoord);
	float b = c.x+c.y+c.z;
	b= pow(b,8.0);
   vec4 vec4col = vec4(0);
   vec4col.r = b;
   vec4col.a = 1.0;
   gl_FragColor = vec4col;
}