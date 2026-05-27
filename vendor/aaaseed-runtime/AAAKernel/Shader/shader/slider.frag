
/*
slider: beschraenkt texturkoordinatenbereiche
*/

uniform float mxw_vertslider_v1coord_0x0_1x0_1x0_mxw;
uniform float mxw_vertslider_u1coord_0x0_1x0_1x0_mxw;
uniform float mxw_vertslider_v2coord_0x0_1x0_0x0_mxw;
uniform float mxw_vertslider_u2coord_0x0_1x0_0x0_mxw;

uniform sampler2D texUnit;

void main(void)
{
   float v1 = mxw_vertslider_v1coord_0x0_1x0_1x0_mxw;
   float u1 = mxw_vertslider_u1coord_0x0_1x0_1x0_mxw;
   float v2 = mxw_vertslider_v2coord_0x0_1x0_0x0_mxw;
   float u2 = mxw_vertslider_u2coord_0x0_1x0_0x0_mxw;

   vec2 texCoord = gl_TexCoord[0].xy;
   texCoord.y = min(texCoord.y,v1);
   texCoord.x = min(texCoord.x,u1);
   texCoord.y = max(texCoord.y,v2);
   texCoord.x = max(texCoord.x,u2);

   vec4 c = texture2D(texUnit, texCoord);
   gl_FragColor = c;
}