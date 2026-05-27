

/*
dot effekt
*/
uniform float mxw_maxV;uniform float mxw_maxU;
uniform sampler2D texUnit;
uniform float mxw_vertslider_scale_0x0_100x0_1x0_mxw;
/*
the one and only main function
*/
void main(void)
{	float tiles = mxw_vertslider_scale_0x0_100x0_1x0_mxw;
	/*
	want rect textures one day ...
	*/
	vec2 fac;
	fac.x = 1.0/mxw_maxU;
	fac.y = 1.0/mxw_maxV;


	/*
	look up middle of each tile
	*/
	float itiles = 1.0/tiles;
	vec2 txCrdtilemid;
	txCrdtilemid.x = floor(gl_TexCoord[0].x*tiles/mxw_maxU)*itiles + (itiles*0.5);
	txCrdtilemid.y = floor(gl_TexCoord[0].y*tiles/mxw_maxV)*itiles + (itiles*0.5);
	vec4 basecolor = texture2D(texUnit, txCrdtilemid/fac);

	/*
	now, for every tile calc dot radius and decide if inside
	*/


	float radius = length(basecolor.rgb); // 0..1/tiles
	float dist = length(abs(gl_TexCoord[0].xy*fac - txCrdtilemid)) * tiles * 2.0;

	vec4 c = vec4(1.0);
	if(dist>radius)
	{
		c = vec4(0.0);
	}

	gl_FragColor = c;
}