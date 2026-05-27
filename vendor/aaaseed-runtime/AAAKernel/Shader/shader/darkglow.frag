/*
dark glow ...
basically this means: accelerate colors by pow(x,y)
like contrast does but do 1:1 above limit
*/


uniform sampler2D texUnit;
uniform float mxw_vertslider_limit_0x0_1x0_0x2_mxw;
uniform float mxw_vertslider_factor_0x0_10x0_0x4_mxw;

void main(void)

{

   vec4 c = texture2D(texUnit, gl_TexCoord[0].xy);
   float limit = mxw_vertslider_limit_0x0_1x0_0x2_mxw;
   float factor = mxw_vertslider_factor_0x0_10x0_0x4_mxw;

   /*
   check if at least one of the colors is above limit,
   then tune color
   */

   c.rgb = max(max(c.r,c.g),c.b)<limit?pow(c.rgb,vec3(factor)):c.rgb;

   gl_FragColor = c;

} 