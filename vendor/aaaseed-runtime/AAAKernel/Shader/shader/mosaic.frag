/*mosaic - as known from tv */

uniform sampler2D texUnit;
uniform float mxw_vertslider_scale_0x0_200x0_0x5_mxw;
/*
the one and only main function
*/
void main(void)
{	float tiles = mxw_vertslider_scale_0x0_200x0_0x5_mxw;
	/*
	look up lower left corner of each tile
	tiles: 10
	texV = 0.75 ()
	lower = floor(texV)*tiles) / tiles
	*/
	float itiles = 1.0/tiles;

	vec2 texCoord = gl_TexCoord[0].xy;
	texCoord = (floor(texCoord*tiles)*itiles) + (itiles*0.5);

	vec4 c = texture2D(texUnit, texCoord);

	gl_FragColor = c;
}
