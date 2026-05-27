



/*

receives the maximum vertical coordinate the shader has
( imagine a rectangular image inside an sqare image )
uniform float mxw_maxV;

*/



uniform float mxw_maxV;
uniform float mxw_maxU;

uniform sampler2D texUnit;
uniform float mxw_vertslider_scaleV_0x0_10x0_0x5_mxw;
uniform float mxw_vertslider_scaleH_0x0_10x0_0x5_mxw;

/*
the one and only main function
*/
void main(void)
{
	float v = mxw_vertslider_scaleV_0x0_10x0_0x5_mxw;
	float h = mxw_vertslider_scaleH_0x0_10x0_0x5_mxw;

   vec2 texCoord = gl_TexCoord[0].xy;

   texCoord.y = mod(v*texCoord.y,mxw_maxV);
   texCoord.x = mod(h*texCoord.x,mxw_maxU);

   vec4 c = texture2D(texUnit, texCoord);

   gl_FragColor = c;

}