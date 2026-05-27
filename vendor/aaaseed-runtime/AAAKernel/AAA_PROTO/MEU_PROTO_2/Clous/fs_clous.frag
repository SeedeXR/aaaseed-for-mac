
in ST_AAA_BV BV;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];

void main (void)
{
	vec4 texture;

	//gl_FrontColor = blend_fs( texture );
	//texture = texture2D( aaa_tex2d[0], BV.tex_coor[0].st );
	//texture = vec4( 1. );
	//texture = vec4(1.);

	vec2 uv;// = aaa_fu_vec4[0].xy;
	//uv += BV.tex_coor[0].st * aaa_fu_vec4[0].zw;
	uv = BV.pos_world.xz * .125 + .5;
	uv.y = 1. - uv.y;
	if( aaa_fu_int[0] == 0 )
		texture = texture2D( aaa_tex2d[1], uv );// * BV.color;
	else
	{
		uv.x = uv.x*.835+.06;
		uv.y = uv.y*.83+.0625;
		texture = texture2D( aaa_tex2d[2], uv );
	}

	gl_FragColor = texture;
//	gl_FragColor = BV.color;
//	gl_FragColor.a = 1;
//	gl_FragColor = vec4(0,1,0,.5);

}
