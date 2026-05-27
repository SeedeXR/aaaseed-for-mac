
//	#define color_factor		aaa_fu_vec4[0]

#define inv_w			aaa_fu_float[0]
#define inv_h			aaa_fu_float[1]
CONST vec2 res_inv = vec2(inv_w, inv_h);

#define in_min			aaa_fu_float[2]
#define in_max			aaa_fu_float[3]

#define dist_scale		aaa_fu_float[4]
#define dist_offset		aaa_fu_float[5]
#define dist_gamma		aaa_fu_float[6]
 
#define	col_offset		aaa_fu_float[11]
#define	col_gamma		aaa_fu_float[12]

#define	gradient_size	aaa_fu_float[20]
#define vl_inter		aaa_fu_float[21]
#define	top_src			aaa_fu_float[22]
#define	top_factor		aaa_fu_float[23]

#define col_a			aaa_fu_vec4[0]
#define col_b			aaa_fu_vec4[1]

CONST int	s_col_choice	= (aaa_fu_int[0] & 0xff);
CONST bool	b_src_to_alpha	= (aaa_fu_int[0]>>8) != 0;
CONST int	s_dist_wrap		= aaa_fu_int[1];
CONST int	s_out_type		= (aaa_fu_int[2] & 0xff);
CONST int	s_out_top		= ((aaa_fu_int[2] >> 16) & 0xff);
CONST int	s_out_src		= aaa_fu_int[3];


//pipeline inputs
layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec4 in_color;

//outputs
layout(location = 0) out vec4 out_render;
layout(location = 1) out vec4 out_result;
//texture inputs
layout(binding = 0) uniform sampler2D TEX_PREVIOUS;
layout(binding = 1) uniform sampler2D TEX_INPUT;

const vec2 poisson_disk_64[64] = vec2[64]
(
	vec2(-0.613392, 0.617481),	vec2(0.170019, -0.040254),	vec2(-0.299417, 0.791925),	vec2(0.645680, 0.493210),
	vec2(-0.651784, 0.717887),	vec2(0.421003, 0.027070),	vec2(-0.817194, -0.271096),	vec2(-0.705374, -0.668203),
	vec2(0.977050, -0.108615),	vec2(0.063326, 0.142369),	vec2(0.203528, 0.214331),	vec2(-0.667531, 0.326090),
	vec2(-0.098422, -0.295755),	vec2(-0.885922, 0.215369),	vec2(0.566637, 0.605213),	vec2(0.039766, -0.396100),
	vec2(0.751946, 0.453352),	vec2(0.078707, -0.715323),	vec2(-0.075838, -0.529344),	vec2(0.724479, -0.580798),
	vec2(0.222999, -0.215125),	vec2(-0.467574, -0.405438),	vec2(-0.248268, -0.814753),	vec2(0.354411, -0.887570),
	vec2(0.175817, 0.382366),	vec2(0.487472, -0.063082),	vec2(-0.084078, 0.898312),	vec2(0.488876, -0.783441),
	vec2(0.470016, 0.217933),	vec2(-0.696890, -0.549791),	vec2(-0.149693, 0.605762),	vec2(0.034211, 0.979980),
	vec2(0.503098, -0.308878),	vec2(-0.016205, -0.872921),	vec2(0.385784, -0.393902),	vec2(-0.146886, -0.859249),
	vec2(0.643361, 0.164098),	vec2(0.634388, -0.049471),	vec2(-0.688894, 0.007843),	vec2(0.464034, -0.188818),
	vec2(-0.440840, 0.137486),	vec2(0.364483, 0.511704),	vec2(0.034028, 0.325968),	vec2(0.099094, -0.308023),
	vec2(0.693960, -0.366253),	vec2(0.678884, -0.204688),	vec2(0.001801, 0.780328),	vec2(0.145177, -0.898984),
	vec2(0.062655, -0.611866),	vec2(0.315226, -0.604297),	vec2(-0.780145, 0.486251),	vec2(-0.371868, 0.882138),
	vec2(0.200476, 0.494430),	vec2(-0.494552, -0.711051),	vec2(0.612476, 0.705252),	vec2(-0.578845, -0.768792),
	vec2(-0.772454, -0.090976),	vec2(0.504440, 0.372295),	vec2(0.155736, 0.065157),	vec2(0.391522, 0.849605),
	vec2(-0.620106, -0.328104),	vec2(0.789239, -0.419965),	vec2(-0.545396, 0.538133),	vec2(-0.178564, -0.596057)
);

/*
done in laplacian_convolution_norm using swizzle
 const vec2 offsets[8] = {
    vec2(	 1,	 0	),
	vec2(	-1,	 0	),
	vec2(	 0,	 1	),
	vec2(	 0,	-1	),
    vec2(	 1,	 1	),
	vec2(	-1,	 1	),
	vec2(	 1,	-1	),
	vec2(	-1,	-1	),
//	vec2(	0,	0	),
};
*/

float	laplacian_convolution_norm( in sampler2D samp, vec2 uv, in float size )
{
	vec2 ots = size / textureSize( samp, 0 );
	vec4 d = vec4( ots, -ots.y, 0 );

    vec2 vec	= texture(samp, uv).xy * -12.

				+ texture( samp, uv + d.xw ).xy * 2.
				+ texture( samp, uv - d.xw ).xy * 2.
				+ texture( samp, uv + d.wy ).xy * 2.
				+ texture( samp, uv + d.wz ).xy * 2.		
				+ texture( samp, uv + d.xy ).xy	
				+ texture( samp, uv - d.xz ).xy
				+ texture( samp, uv + d.xz ).xy
				+ texture( samp, uv - d.xy ).xy;

    return length(vec) * (1./16.);
}

//todo this is not valid anymore
//todo we have to store this information at init pass to use it at the out pass
#if 1
	bool is_inside( vec2 uv )
	{
		vec4 col = texture( TEX_INPUT, uv );
		return in_min <= col.r && col.r <= in_max ;
	}
#else
//hack version done from first pass to be finished: or we keep the info or we recompute it
	bool is_inside( vec2 uv )
	{	//we select a state for each pixel for pass 1
		vec4 col = texture( TEX_INPUT, uv );
		float l = col.r;
		l = clamp_01( (l-in_r_sub)*in_r_mul );
		l = pow( l, in_gamma );
		l = fract( l * in_repeat );
		bool b_inside = in_l_min <= l && l <= in_l_max ;
	}
#endif

vec3 make_color( float v )
{
//	v = fract( v + col_offset );
	v = pow( v, col_gamma ) + col_offset;
	switch( s_col_choice )
	{
	case 1:
	case 2: 	return mix( col_a.rgb, col_b.rgb, v );
	case 3:		return hsv2rgb( vec3( v,1,1 ) );
	}	
}

float transform_dist( in float d, in bool b_inside )
{
	d = d * dist_scale;
	d = pow( d, dist_gamma );
	d += dist_offset;

	switch( s_dist_wrap )
	{
	case 2:
		d = fract(d);
		//todo check when b_inside will be back
		if( b_inside )
			d = 1. - d;
		break;
	case 3:
		d = fract(d);
		break;
	case 4:
		d = fract(d*.5);
		d = (d < .5) ? d : 1-d;
		d *= 2.;
		break;
	}
	return d;
}

void main()	
{
#define uv_nor	in_texcoord
#define	uv_xy	gl_FragCoord.xy

	vec4 src = texture( TEX_INPUT, uv_nor );

//we have to make it correct first
//	bool b_inside = is_inside(uv_nor);

	vec2 best_coord	= texture( TEX_PREVIOUS, uv_nor ).rg;

	float d;
	vec4 col = vec4( 0,0,0, 0 );

	if( best_coord.x >= 0. )
	{
		col.a = 1.;
		switch( s_out_type ) 
		{
		case 1:
			break;
		case 2:	//"raw"
			col.rgb = vec3( best_coord * res_inv, 0 );
			break;
		case 3:	//distance
			// if( b_inside )
			// 	col = col_a;
			// else 
			// 	col = col_b;

			//d += .55;
			// if( b_inside )
			//   	d = fract( 1-d );
			// else
			// 	d = fract(d);

			d = distance( uv_xy, best_coord );
			col.rgb = make_color( transform_dist( d, false ) );
			// {	
			// 	d = dot( uv_xy - best_coord, uv_xy - best_coord );
			// 	out_render.b = 1;
			// 	return;
			// }
			break;
		case 4:	// src + Dist
			//col.rgb = vec3( 1,0,1 );
			d = distance( uv_xy, best_coord );
			col.a = make_color( transform_dist( d, false ) ).r;

			//col.rgb = texture( TEX_INPUT, uv_xy*res_inv ).rgb;
			if( best_coord.x >= 0. )
			{
				vec3 col_close;
				vec2 center = res_inv * best_coord;
				float f = 1.;
#if 1
				col_close.rgb = texture( TEX_INPUT, ( best_coord )*res_inv ).rgb;
#	if 1			

#		if 1
				f = 10.;
				vec2 delta = res_inv * f;
				//float f = .2 * distance( uv_xy, best_coord );
				for( uint i = 0; i < disc_sample_count; i++ )
				{
					col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + delta * disc_kernel[i]            ).rgb );
					col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center - delta * disc_kernel[i]    * 1.6   ).rgb );
					col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + delta * disc_kernel[i].yx * 2.56  ).rgb );
					col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center - delta * disc_kernel[i].yx * 4.096 ).rgb );
				}
#		else
				f = 40.;
				vec2 delta = res_inv * f;
				for( uint i = 0; i < 16; i++ )
				{
					col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + delta * poisson_disk_64[i]    ).rgb );
					col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center - delta * poisson_disk_64[i]    ).rgb );
					col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + delta * poisson_disk_64[i].yx ).rgb );
					col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center - delta * poisson_disk_64[i].yx ).rgb );
				}
#		endif
				// f *= -.5;
				// for( uint i = 0; i < disc_sample_count; i++ )
				// {
				// 	col.rgb = max( col.rgb, texture( TEX_INPUT, ( best_coord  * disc_kernel[i] * f  )*res_inv  ).rgb );
				// }
#	else
				//todoopt use swizzle do generate deltas instead of vec2
				f *= 3.5;
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(0,f)   ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(0,-f)  ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(f,0)   ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(-f,0)  ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(f,f)   ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(f,-f)  ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(-f,f)  ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(-f,-f) ).rgb );
				f *= 7;
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(0,f)   ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(0,-f)  ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(f,0)   ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(-f,0)  ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(f,f)   ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(f,-f)  ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(-f,f)  ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv* vec2(-f,-f) ).rgb );
#	endif
#else
				vec2 dir = best_coord - uv_xy;
				vec2 dir_pure = normalize( dir );
				col_close.rgb =                     texture( TEX_INPUT, center + res_inv*( dir_pure*f*2  ) ).rgb;
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv*( dir_pure*f*5. ) ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv*( dir_pure*f*4. ) ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv*( dir_pure*f*3. ) ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv*( dir_pure*f*2. ) ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv*( dir_pure*f    ) ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT, center + res_inv*( -dir_pure*f   ) ).rgb );
				col_close.rgb = max( col_close.rgb, texture( TEX_INPUT,          res_inv*	uv_xy ).rgb );	// ??
#endif
				col.rgb = col_close.rgb;
			}
			else
			{
				col = vec4( 0,1,0, 1);
				// col.rgb  = texture( TEX_INPUT, uv_xy*res_inv ).rgb;
				// col.a = .5;
			}
			break;
		case 5:	//Exp2
			d = distance( uv_xy, best_coord );
			d = transform_dist( d, false );
			if( best_coord.x >= 0. )
				col.rgb = texture( TEX_INPUT, mix( best_coord*res_inv, uv_nor, d ) ).rgb;
			//	col.rgb = texture( TEX_INPUT, fract( mix( best_coord*res_inv, uv_nor, d )) ).rgb;
			break;
		case 6:	//Exp
			if( best_coord.x >= 0. )
				col.rgb = texture( TEX_INPUT, fract( mix( best_coord*res_inv, uv_nor, dist_offset )) ).rgb;
			break;
		case 7:	//Nimes
			//col.rgb = vec3( 1,0,1 );
			d = distance( uv_xy, best_coord );
			col.r = make_color( transform_dist( d, false ) ).r;

			//col.rgb = texture( TEX_INPUT, uv_xy*res_inv ).rgb;
			if( best_coord.x >= 0. )
			{
				vec2 center = res_inv * best_coord;
				float f = 10.;
				vec2 delta = res_inv * f;
				vec3 col_close = texture( TEX_INPUT, center ).rgb;
				for( uint i = 0; i < disc_sample_count; i++ )
				{
					col_close = max( col_close, texture( TEX_INPUT, center + delta * disc_kernel[i]            ).rgb );
					col_close = max( col_close, texture( TEX_INPUT, center - delta * disc_kernel[i]    * 1.6   ).rgb );
					col_close = max( col_close, texture( TEX_INPUT, center + delta * disc_kernel[i].yx * 2.56  ).rgb );
					col_close = max( col_close, texture( TEX_INPUT, center - delta * disc_kernel[i].yx * 4.096 ).rgb );
				}
				col.gb = vec2(col_close.r,0);
			}
			else
			{
				col.rgb = vec3( 0,0,0 );
				// col.rgb  = texture( TEX_INPUT, uv_xy*res_inv ).rgb;
				// col.a = .5;
			}
			break;
		case 8:	//Test
			{
				float g = (best_coord * res_inv).y;
				col.rgb = vec3(g);
			}
			break;
		}
	}
	//col.rgb = hsv2rgb( vec3(d,1,1) );
	// if( !b_inside )
	// 	col.rgb = src.rgb;
	// else 
	//  	col.rgb = col.rgb;

//	col.rgb *= d;

	switch( s_out_src )
	{
	case 2:	col *= src;	break;
	case 3:	col += src;	break;
	case 4:	col = col * src + src;	break;
	case 5:	col = min( col, src );	break;
	case 6:	col = max( col, src );	break;
	}

	float vt = 0.;
	switch(s_out_top)
	{
	case 1:
		break;
	case 2:	// Voronoi
		vt = get_gradient_norm_2d( TEX_PREVIOUS, uv_nor, gradient_size );
		break;
	case 3:	// Laplace
		vt = laplacian_convolution_norm( TEX_PREVIOUS, uv_nor, gradient_size );
		break;
	case 4:	// Voronoi to Laplace
		{
			float gra_vor = get_gradient_norm_2d( TEX_PREVIOUS, uv_nor, gradient_size );
			float gra_lap = laplacian_convolution_norm( TEX_PREVIOUS, uv_nor, gradient_size );
			//vt = length( mix( gra_vor, gra_lap, vl_inter ) ) * 1.;
			vt = mix( gra_vor, gra_lap, vl_inter);
		}
		break;
	}

	if( vt != 0. )
	{	//todo top_src_mix param
		//vec3 pass = vec3(1) * top_factor;
		vec3 pass = mix( vec3(1), src.rgb, top_src ) * top_factor;
		col.rgb = mix( col.rgb, pass, vt );
		//col.rgb *= pass;
	}
	if( b_src_to_alpha )
		col.a = src.r;

	out_render = col * in_color;
	//out_result = vec4( best_coord, 0, 1 );

	// if( d < .25 )
	// 	out_render = texture( TEX_INPUT, best_coord * res_inv ) ;
	// else
	// 	out_render = vec4(0,0,0, 1);
}


