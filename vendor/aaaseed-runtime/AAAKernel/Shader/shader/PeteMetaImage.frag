/*
PeteMetaImage - some freeframe effect
*/

uniform float mxw_maxV;
uniform float mxw_maxU;
uniform sampler2D texUnit;
uniform float mxw_vertslider_scale_0x0_100x0_0x05_mxw;

/*
the one and only main function
*/
void main(void)
{
	float tiles = mxw_vertslider_scale_0x0_100x0_0x05_mxw;

	vec2 texCoord = gl_TexCoord[0].xy;

	/*
	look up middle each tile
	extract color
	*/
	float itiles = 1.0/tiles;
	vec2 basetexCoord = (floor(texCoord*tiles)*itiles) + (itiles*0.5);
	

	/*
	now take col like multiply.mxf
	*/
	texCoord.y = mod(tiles*texCoord.y,mxw_maxV);
	texCoord.x = mod(tiles*texCoord.x,mxw_maxU);
	vec4 c = texture2D(texUnit, texCoord);

	gl_FragColor = c * basecolorc;

}
