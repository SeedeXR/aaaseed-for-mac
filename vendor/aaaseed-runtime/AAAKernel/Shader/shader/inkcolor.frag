

/*
ink line effekt
*/
uniform float mxw_maxV;uniform float mxw_maxU;
uniform sampler2D texUnit;
uniform float mxw_vertslider_scale_0x0_100x0_0x2_mxw;
/*
the one and only main function
*/
void main(void)
{	float tiles = mxw_vertslider_scale_0x0_100x0_0x2_mxw;
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
	vec4 basecolor = texture2D(texUnit, txCrdtilemid/fac);

	/*
	now, for every tile calc brightness
	*/
	float brightness = length(basecolor.rgb); // 0..1/tiles
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
		c = dist > 0.95 ? vec4( 0.0 ) : vec4( pow(1.0 - (dist-brightness), 6.0) );
	}

	gl_FragColor = c*basecolor;
}