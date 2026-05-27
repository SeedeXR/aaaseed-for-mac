
/*
PeteMetaImage - some freeframe effect
*/


/*
dies bedeutet:
- einen vertikalen slider
- deskriptor "brightness"
- minimal 0.0
- maximal 1.0
- initial 0.5
*/


uniform float mxw_vertslider_red_0x0_1x0_0x95_mxw;
uniform float mxw_vertslider_blue_0x0_1x0_0x95_mxw;
uniform float mxw_vertslider_green_0x0_1x0_0x95_mxw;
uniform float mxw_vertslider_alpha_0x0_1x0_0x95_mxw;

uniform sampler2D texUnit;

void main(void)
{
  float a = mxw_vertslider_red_0x0_1x0_0x95_mxw;
  float b = mxw_vertslider_blue_0x0_1x0_0x95_mxw;
  float c = mxw_vertslider_green_0x0_1x0_0x95_mxw;
  float alpha = mxw_vertslider_alpha_0x0_1x0_0x95_mxw;

   vec2 texCoord = gl_TexCoord[0].xy;
   vec4 col = texture2D(texUnit, texCoord);
   col.r *= a;
   col.g *= b;
   col.b *= c;
   col.a *= alpha;
   gl_FragColor = col;
}





