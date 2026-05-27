
/*
der "null-shader"
hier wird eine textur eingelesen, und wieder ausgegeben.
simple, oder?
*/


/*
dies bedeutet:
- einen vertikalen slider
- deskriptor "brightness"
- minimal 0.0
- maximal 1.0
- initial 0.5
*/

uniform float mxw_vertslider_brightness_0x0_10x0_0x7_mxw;
uniform float mxw_vertslider_color_0x0_10x0_0x05_mxw;

uniform sampler2D texUnit;

void main(void)
{
   vec2 texCoord = gl_TexCoord[0].xy;
   vec4 c = texture2D(texUnit, texCoord);
   c.r *= mxw_vertslider_brightness_0x0_10x0_0x7_mxw*5.0;
   c.b *= mxw_vertslider_color_0x0_10x0_0x05_mxw*8.0;
   gl_FragColor = c;
}