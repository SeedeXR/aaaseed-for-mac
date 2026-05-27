
/*
null (0) shader ohne effekt
*/

uniform sampler2D texUnit;uniform float mxw_vertslider_scale_0x0_20x0_0x5_mxw;
void main(void)
{
   vec2 texCoord = gl_TexCoord[0].xy;
   vec4 c = texture2D(texUnit, texCoord);

   float v = mxw_vertslider_scale_0x0_20x0_0x5_mxw;

   c = floor(c*v)/v;

   gl_FragColor = c;
}