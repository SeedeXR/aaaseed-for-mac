
/*
kommentar
*/


uniform sampler2D texUnit;
void main(void)
{
   vec2 texCoord = gl_TexCoord[0].xy;
   vec4 c = texture2D(texUnit, texCoord);
   vec4 vec4col = vec4(c);
   vec4col.g = c.b;
   vec4col.b = c.g;
   gl_FragColor = vec4col;
}