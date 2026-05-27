
in VS_out
{
	vec4			color;
	vec2			tex_coor;
} fs_in;

uniform sampler2D  aaa_samp0;

CONST int flags = aaa_fu_int[0];
CONST int channel_nb = aaa_fu_int[1];
CONST float level_min = aaa_fu_float[0];
CONST float level_max = aaa_fu_float[1];
#if 0
	// opengl4 version
	CONST int s_mode		= bitfieldExtract( flags, 0, 6 );
	CONST int s_alpha		= bitfieldExtract( flags, 6, 6 );
	CONST int s_out			= bitfieldExtract( flags, 12, 6 );
	CONST int s_level_use	= bitfieldExtract( flags, 18, 6 );
	CONST bool b_abs		= bitfieldExtract( flags, 24, 1 ) != 0;
#else
	CONST int s_mode		=   flags 			& 0x3f;
	CONST int s_alpha		= ( flags >> 6  )	& 0x3f;
	CONST int s_out			= ( flags >> 12 )	& 0x3f;
	CONST int s_level_use	= ( flags >> 18 )	& 0x3f;
	CONST bool b_abs		= ( flags & (1<<24) ) != 0;
#endif

CONST vec4 color_offset = aaa_fu_vec4[0];
CONST vec4 color_factor = aaa_fu_vec4[1];

float make_level( float val )
{
#if 1
	return linearstep( level_min, level_max, val );
#else
	if( level_min < level_max )
	{
		
		if( val <= level_min )		return 0.;
		else if( val >= level_max )	return 1.;
		else 		return ( val - level_min ) / ( level_max - level_min );
	}
	else
	{
		if( val <= level_max )		return 1.;
		else if( val >= level_min )	return 0.;
		else 		return ( val - level_min ) / ( level_max - level_min );
	}
#endif
}
float make_level_tri( float val )
{
	val = (val - level_min) / (level_max - level_min );
	return fract( val );
}
float make_level_loop( float val )
{
	val = (val - level_min) / (level_max - level_min );
	if( val < 0. )
		val = - val;
	val = mod( val, 2. );
	if( val > 1.)
		val =  2. - val;
	return val;
}


void process_out_alpha( inout vec4 tex )
{
	if( s_alpha != 1 )
	{
		if( s_alpha==0 )	// force to One
			tex.a = 1.;
		else	// case 2 // inverse
			tex.a = 1. - tex.a;
	}
}

void process_out( inout vec4 tex )
{
	process_out_alpha( tex );
	tex = color_offset + color_factor * tex;
}

void main()
{
// extract
#if 0
	bool b_invert	= bitfieldExtract( s_level_use, 3, 1 ) == 1;
	bool b_hsv		= bitfieldExtract( s_level_use, 4, 1 ) == 1;
//	s_level_use		= bitfieldExtract( s_level_use, 0, 3 );
#else
	bool b_invert	= (s_level_use & 0x8) > 0;
	bool b_hsv		= (s_level_use & 0x10) > 0;
//	s_level_use		= s_level_use & 0x7;
#endif

	//	s_mode  0 "RGB"		1 "Gr"		2 "Alpha",
	//			3 "R"		4 "G"		4 "B",
	//			6 "H"		7 "S"		8 "V",
	//			9 "Hsv"		10 "2"		11 "3",
	//			12 "-1"		13 "-2"		14 "-3"
#if 0
	gl_FragColor = vec4( fs_in.tex_coor,0, 1 );
	return;
#endif

	vec4 tex;
// 	if( channel_nb == 2 )
// //		src = vec4( abs(texture( aaa_samp0, fs_in.tex_coor ).rg), 0, 1. );
// 		tex = vec4( texture( aaa_samp0, fs_in.tex_coor ).rg, 0, 1. );
// 	else
	//	src = texture2D( aaa_tex2d[0], fs_in.tex_coor );
		tex = texture( aaa_samp0, fs_in.tex_coor );

	if( b_abs )
		tex = abs(tex);
#if 0
	gl_FragColor = tex;
//	gl_FragColor = vec4(fs_in.tex_coor,0, 1);
	return;
#endif
	if( b_hsv )
		tex.xyz = rgb2hsv( tex.xyz );

	if( b_invert )
		tex.rgb = 1. - tex.rgb;
#if 0
	gl_FragColor = tex;
//	gl_FragColor = vec4(fs_in.tex_coor,0, 1);
	return;
#endif

	if( s_mode <= 0 )
	{
		process_out( tex );
	}
	else
	{	//	1 grey, 2 alpha,	3 R, 4 G, 5 B
		//	all monochannel
		float wh;
		//we pick up source
		switch( s_mode )
		{
		case 1:		wh = compute_gray( tex.rgb );		break;
		case 2:		wh = tex.a;							break;
		case 3:		wh = tex.r;							break;			//s_out = 0;
		case 4:		wh = tex.g;							break;			//s_out = 0;
		case 5:		wh = tex.b;							break;			//s_out = 0;
		}

		switch( s_mode )
		{
		case 1:		wh = color_offset.r + wh * color_factor.r;	break;
		case 2:		wh = color_offset.a + wh * color_factor.a;	break;
		case 3:		wh = color_offset.r + wh * color_factor.r;	break;
		case 4:		wh = color_offset.g + wh * color_factor.g;	break;
		case 5:		wh = color_offset.b + wh * color_factor.b;	break;
		}

		// if( b_invert )
		// 	wh = 1. - wh;

		if( s_out == 0 )
		{
			switch( s_mode )
			{
			case 1:
			case 2:		tex.rgb = vec3( wh ); 			break;
			case 3:		tex.rgb = vec3( wh, 0., 0. ); 	break;
			case 4:		tex.rgb = vec3( 0., wh, 0. ); 	break;
			case 5:		tex.rgb = vec3( 0., 0., wh ); 	break;
			}
		}
		else
		{
			if( s_out == 1 )
			{
				if( wh<0. || 1.>wh )	//todo beurk 
					tex.rgb = vec3( fract(wh) );
				else
					tex.rgb = vec3( wh );
			}
			else
				tex.rgb = hsv2rgb( vec3( wh, 1, 1 ) );
		}
		process_out_alpha( tex );
	}

//	tex.rgb = vec3( .25 + s_alpha  * .25 ) ;
//	tex.a = 1.;

	// {
	// 	float wh;
	// 	if ( s_mode == 12 )		{	wh = src.r - src.g;	}
	// 	elseif ( s_mode == 13 )	{	wh = src.g - src.b;	}
	// 	else					{	wh = src.r - src.b;	}

	// 	if( b_invert )			{	wh = 1. - wh;		}
	// 	if( s_level_use >= 1 )
	// 	{
	// 		if( s_level_use == 1 )		wh = make_level( wh );
	// 		elseif( s_level_use == 2 )	wh = make_level_tri( wh );
	// 		else						wh = make_level_loop( wh );
	// 	}
	// 	src.xyz = wh;
	// }


	gl_FragColor = tex;

#if 0
	if( channel_nb == 1 )
		gl_FragColor = vec4( 1,0,0, 1 );
	else if( channel_nb == 2 )
		gl_FragColor = vec4( 0,1,0, 1 );
	else if( channel_nb == 3 )
		gl_FragColor = vec4( 0,0,1, 1 );
	else if( channel_nb == 4 )
		gl_FragColor = vec4( 1,1,1, 1 );
#endif

//	gl_FragColor = texture( aaa_samp0, fs_in.tex_coor );
}

