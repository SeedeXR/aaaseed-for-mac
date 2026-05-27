
uniform sampler2D   aaa_tex2d[4];


CONST vec2 strenght = aaa_fu_vec4[0].xy;
//CONST vec2 duv   = aaa_fu_vec4[0].zw;

CONST int what		= aaa_fu_int[0];
CONST int ite_nb	= aaa_fu_int[1];

CONST float th      = aaa_fu_float[0];

// use for access to neightboor pixel     du,  dv,            -du,            0 
CONST vec4 duv = vec4( aaa_fu_vec4[0].zw, -aaa_fu_vec4[0].z, 0 ) * 1;   

#if 1 // Ndc method
	layout(location = 0) in vec2 in_texcoord;
#else
	in ST_AAA_BV BV;
#	define in_texcoord BV.tex_coor[0].st
#endif

// sucessive rectangles wrapping around using trigonometric direction (Counter Clock Wise)
//todo should do it with circles
CONST  ivec2 move_spiral[240] =
{
	{1,0},
	{0,1},	//2
	{-1,0},	{-1,0},
	{0,-1},	{0,-1},	//6
	{1,0},	{1,0},	{1,0},
	{0,1},	{0,1},	{0,1},	//12
	{-1,0},	{-1,0},	{-1,0},	{-1,0},
	{0,-1},	{0,-1},	{0,-1},	{0,-1},	//20
	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},
	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	//30
	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},
	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	//42
	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},
	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	//56
	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},
	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	//72
	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},
	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	//90
	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},
	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	//110
	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},
	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	//132
	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},
	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	//156
	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},
	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	//182
	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},	{-1,0},
	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	{0,-1},	//210
	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},	{1,0},
	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	{0,1},	//240
};
void test_spiral( sampler2D tex, vec2 uv, float th, int nb )
{
	for( int i=0; i<nb; i++ )
	{
		uv += move_spiral[i] * duv.xy;
		if( texture2D( tex, uv ).r <= th )
			discard;
	}
}	


void test_3x3( sampler2D tex, vec2 uv, float th )
{
	if(      texture2D( tex, uv + duv.xw ).r <= th
		||   texture2D( tex, uv - duv.xw ).r <= th
		||   texture2D( tex, uv + duv.wy ).r <= th
		||   texture2D( tex, uv - duv.wy ).r <= th
		||   texture2D( tex, uv + duv.xy ).r <= th
		||   texture2D( tex, uv - duv.xy ).r <= th
		||   texture2D( tex, uv + duv.zy ).r <= th
		||   texture2D( tex, uv - duv.zy ).r <= th
	)
		discard;
}
void test_nxn_star( sampler2D tex, vec2 uv, float th, int nb )
{
	vec4 d = vec4(0);
	for( int i=1; i<=nb; i++ )
	{
		d += duv;
		if(      texture2D( tex, uv + d.xw ).r <= th
			||   texture2D( tex, uv - d.xw ).r <= th
			||   texture2D( tex, uv + d.wy ).r <= th
			||   texture2D( tex, uv - d.wy ).r <= th
			||   texture2D( tex, uv + d.xy ).r <= th
			||   texture2D( tex, uv - d.xy ).r <= th
			||   texture2D( tex, uv + d.zy ).r <= th
			||   texture2D( tex, uv - d.zy ).r <= th
		)
		discard;
	}
}
void test_nx1( sampler2D tex, vec2 uv, float th, int nb )
{
	vec4 d = vec4(0);
	for( int i=1; i<=nb; i++ )
	{
		d += duv;
		if(      texture2D( tex, uv + d.xw ).r <= th
			||   texture2D( tex, uv - d.xw ).r <= th
		)
		discard;
	}
}
void test_1xn( sampler2D tex, vec2 uv, float th, int nb )
{
	vec4 d = vec4(0);
	for( int i=1; i<=nb; i++ )
	{
		d += duv;
		if(      texture2D( tex, uv + d.wy ).r <= th
			||   texture2D( tex, uv - d.wy ).r <= th
		)
		discard;
	}
}
void main()
{
    	vec2 uv = in_texcoord;

	vec4 col = texture2D( aaa_tex2d[0], uv );

	if( col.r < th )
		discard;

	// float th_a = .999;
	// if( a.a < th_a )
	//      discard;
	// float th = .1;
	// if( a.r < th && a.g < th && a.b < th )
	//      discard;

	switch( what )
	{
	case 1:
		test_spiral( aaa_tex2d[0], uv, th, ite_nb );
		break;
	case 2:	//U
		test_nx1( aaa_tex2d[0], uv, th, ite_nb );
		break;
	case 3:	//V
		test_1xn( aaa_tex2d[0], uv, th, ite_nb );
		break;
	case 4:
		test_3x3( aaa_tex2d[0], uv, th );
		break;
	default:	//5 to 11
		test_nxn_star( aaa_tex2d[0], uv, th, what-3 );
		break;
	}

	//Apply slider overlay
	gl_FragColor = col;
}