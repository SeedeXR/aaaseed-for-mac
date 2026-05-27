
//	#define color_factor		aaa_fu_vec4[0]


#define inv_sx			aaa_fu_float[0]
#define inv_sy			aaa_fu_float[1]
#define in_r_sub		aaa_fu_float[2]
#define in_r_mul		aaa_fu_float[3]
#define in_gamma		aaa_fu_float[4]
#define in_repeat		aaa_fu_float[5]
#define in_l_min		aaa_fu_float[6]
#define in_l_max		aaa_fu_float[7]

#define s_in_mode		aaa_fu_int[0]
#define s_pass			aaa_fu_int[1]


//pipeline inputs
layout(location = 0) in vec2 in_texcoord;
//outputs
layout(location = 0) out vec4 out_render;
layout(location = 1) out vec4 out_result;
//texture inputs
layout(binding = 0) uniform sampler2D TEX_PREVIOUS;
layout(binding = 1) uniform sampler2D TEX_INPUT;


vec4 off = vec4( inv_sx, inv_sy, -inv_sx, 0 );

float is_on( vec2 uv )
{
	return step( .5, texture( TEX_PREVIOUS, uv ).r );
}

void main()
{
#define uv_nor	in_texcoord
#define	uv_xy	gl_FragCoord.xy


//	out_render = vec4( uv,0, 1);
#if 0
	if( s_pass == 0 )
		out_result = texture( TEX_INPUT, uv_nor );
	else
		out_result = vec4(1) - texture( TEX_INPUT, uv_nor );
	return;
#endif

//todo contour detection could be better that this ON/OFF by pixel 

	if( s_pass == 0 )
	{	//we select a state for each pixel for pass 1
		vec4 col = texture( TEX_INPUT, uv_nor );
		float l = gray(col.rgb);
		//todo this interface is not ideal
		l = clamp_01( (l-in_r_sub) * in_r_mul );
		l = pow( l, in_gamma );
		//out_result = vec4(l);
		if( l != 1. )	// todo avois this test
			l = fract( l * in_repeat );	// * in_repeat
		//out_result = vec4(l);
		//return;
		bool b = in_l_min <= l && l <= in_l_max ;
		out_result = b ? vec4(1,0,1,1) : vec4(0,0,0,0);
	}
	else
	{	//pass 1 we set uv coor for selected pixel
#if 0
		out_result = texture( TEX_PREVIOUS, uv_nor );
#else
		switch( s_in_mode )
		{
		case 1:	//select
			out_result.rg = mix( vec2(-1), uv_xy, is_on(uv_nor) );
			//out_result.rg = vec2( is_on(uv_nor) );
			//out_result.rg = vec2( 0, 1 );
			// if( is_on(uv_nor) > 0. )
			//  	out_result.rg = vec2( 1, 0 );
			// else
			//  	out_result.rg = vec2( 1, 1 );
			return;
		case 2:	//inverse
			out_result.rg = mix( uv_xy, vec2(-1), is_on(uv_nor) );
			return;
		case 3:	//edge
			if( is_on(uv_nor) > 0. )
			{	//we check the neighbours for an off
				if(      is_on( uv_nor + off.xw ) <= 0. )	out_result.rg = uv_xy;
				else if( is_on( uv_nor + off.zw ) <= 0. )	out_result.rg = uv_xy;
				else if( is_on( uv_nor + off.wy ) <= 0. )	out_result.rg = uv_xy;
				else if( is_on( uv_nor - off.wy ) <= 0. )	out_result.rg = uv_xy;
				else if( is_on( uv_nor + off.xy ) <= 0. )	out_result.rg = uv_xy;
				else if( is_on( uv_nor + off.zy ) <= 0. )	out_result.rg = uv_xy;
				else if( is_on( uv_nor - off.zy ) <= 0. )	out_result.rg = uv_xy;
				else if( is_on( uv_nor - off.xy ) <= 0. )	out_result.rg = uv_xy;
				else										out_result.rg = vec2(-1);
			}
			else
			{
				out_result.rg = vec2(-1);
				// {	//we check the neighbours
				// 		if(      is_on( uv_nor + off.xw ) )	result = uv_xy;
				// 		else if( is_on( uv_nor + off.zw ) )	result = uv_xy;
				// 		else if( is_on( uv_nor + off.wy ) )	result = uv_xy;
				// 		else if( is_on( uv_nor - off.wy ) )	result = uv_xy;
				// 		else if( is_on( uv_nor + off.xy ) )	result = uv_xy;
				// 		else if( is_on( uv_nor + off.zy ) )	result = uv_xy;
				// 		else if( is_on( uv_nor - off.zy ) )	result = uv_xy;
				// 		else if( is_on( uv_nor - off.xy ) )	result = uv_xy;
				// }
			}
			return;
		}		
#endif
	}
//		out_result = vec4( uv_nor,0, 1 );
}

