

/*
ink line effekt
*/
uniform float mxw_maxV;uniform float mxw_maxU;
uniform sampler2D texUnit;

uniform float mxw_vertslider_scale_0x0_100x0_0x4_mxw;
uniform float mxw_vertslider_scale2_0x0_20x0_0x05_mxw;

/*
the one and only main function
*/
void main(void)
{	/*	test	*/	float tiles = mxw_vertslider_scale_0x0_100x0_0x4_mxw;	float line = mxw_vertslider_scale2_0x0_20x0_0x05_mxw;

	/*
	want rect textures one day ...
	*/
	vec2 fac;
	fac.x = 1.0/mxw_maxU;
	fac.y = 1.0/mxw_maxV;

	/*
	look up middle of each tile
	*/
	vec2 txCrdtilemid;
	txCrdtilemid.x = gl_TexCoord[0].x/mxw_maxU;
	txCrdtilemid.y = (floor(gl_TexCoord[0].y*tiles/mxw_maxV) + 0.5)/tiles;

	/*
	now, for every tile calc brightness
	*/
	float brightness = length(texture2D(texUnit, txCrdtilemid/fac).rgb); // 0..1/tiles
	float dist = abs(gl_TexCoord[0].y*fac.y - txCrdtilemid.y) * tiles * 2.0;

	/*
	init with white
	turn black if dist > brightness
	*/
	vec4 c = vec4( 0.999 );
	if( dist > brightness)
	{
		/*
		should degrade from 0.x to 0.1 from 1..0 then 0
		always zero if > 0.95
		*/
		c = dist > 0.95 ? vec4( 0.0 ) : vec4( pow(1.0 - (dist-brightness), line) );
	}

	gl_FragColor = c;
}