		
__constant sampler_t sampler		= CLK_FILTER_LINEAR	| CLK_ADDRESS_CLAMP;

float4 get_gradient_r(	__read_only	image2d_t		img
						,			float2 			pos
						,const		kernel_param*	aaa
					)
{
	float4	nor;
	float	a;
	float	b;
	const float	e0	= aaa->p06;
	const float	e1	= aaa->p07;
	float	d;

	//	GRAD X
	d		=	aaa->p01;
	pos.x -= d;
	a		=	read_imagef( img, sampler, pos).x;
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( e0, e1, a );
	pos.x += 2*d;
	b		=	read_imagef( img, sampler, pos).x;
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( e0, e1, b );
	pos.x -= d;

	nor.x = a - b;

	//	GRAD Y
	d		=	aaa->p02;
	pos.y -= d;
	a		=	read_imagef( img, sampler, pos ).x;
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( e0, e1, a );
	pos.y += 2*d;
	b		=	read_imagef( img, sampler, pos).x;
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( e0, e1, b );

	nor.y = a - b;

	nor.z = aaa->p10*.10;
	nor.xyz = fast_normalize( nor.xyz );

	return nor;
}

float4 get_gradient_smoothstep_luma_v3(	__read_only		image2d_t		img
										,				float2 			pos
										,const			kernel_param*	aaa
//										,__read_only	float4			param
//										,__read_only	float			nor_z
									)
{
	float4	nor;
	float	a;
	float	b;

	const float o = - aaa->p06;
	const float f = 1. / ( aaa->p07 - aaa->p06);
	//	GRAD X
	pos.x -= aaa->p01;
	a		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	a		=	clamp( (a - o) * f, 0.f, 1.f );
	pos.x += 2*aaa->p01;
	b		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	b		=	clamp( (b - o) * f, 0.f, 1.f );
	//b		=	smoothstep( aaa->p06, aaa->p07, b );
	nor.x = a - b;
	pos.x -= aaa->p01;

	//	GRAD Y
	pos.y -= aaa->p02;
	a		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	a		=	clamp( (a - o) * f, 0.f, 1.f );
	//a		=	smoothstep( aaa->p06, aaa->p07, a );
	pos.y += 2*aaa->p02;
	b		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	b		=	clamp( (b - o) * f, 0.f, 1.f );
	//b		=	smoothstep( aaa->p06, aaa->p07, b );
	nor.y = a - b;

	nor.z = aaa->p10*.10;
	nor.xyz = fast_normalize( nor.xyz );
	return nor;
}

float4 get_gradient_smoothstep_luma_old(	__read_only		image2d_t		img
										,					float2 			pos
										,	const			kernel_param*	aaa
//										,__read_only	float4			param
//										,__read_only	float			nor_z
									)
{
	float4	nor;
	float	a;
	float	b;

	//	GRAD X
	pos.x -= aaa->p01;
	a		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( aaa->p06, aaa->p07, a );
	pos.x += 2*aaa->p01;
	b		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( aaa->p06, aaa->p07, b );
	nor.x = a - b;
	pos.x -= aaa->p01;

	//	GRAD Y
	pos.y -= aaa->p02;
	a		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( aaa->p06, aaa->p07, a );
	pos.y += 2*aaa->p02;
	b		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( aaa->p06, aaa->p07, b );
	nor.y = a - b;

	nor.z = aaa->p10*.10;
	nor.xyz = fast_normalize( nor.xyz );
	return nor;
}

float4 get_gradient_smoothstep_luma(	__read_only		image2d_t		img
										,				float2 			pos
//										,__read_only	float4			param
//										,__read_only	float			nor_z
										,				float4			param
										,				float			nor_z
									)
{
	float4	nor;
	float	a;
	float	b;

	//	GRAD X
	pos.x -= param.x;
	a		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( param.z, param.w, a );
	pos.x += 2*param.x;
	b		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( param.z, param.w, b );
	nor.x = a - b;
	pos.x -= param.x;

	//	GRAD Y
	pos.y -= param.y;
	a		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( param.z, param.w, a );
	pos.y += 2*param.y;
	b		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( param.z, param.w, b );
	nor.y = a - b;

	nor.z = nor_z;
	nor.xyz = fast_normalize( nor.xyz );
	return nor;
}

float4 get_normal_2023(	__read_only		image2d_t		img
						,				float2 			pos
						,const			kernel_param*	aaa
					)
{
	float4	nor;
	float	a,b;
	float	x,y;
	const float	e0	= aaa->p06;
	const float	e1	= aaa->p07;
	float	d;
	float	t;

	//	GRAD X
	d		=	aaa->p01;
	pos.x -= d;
	a		=	read_imagef( img, sampler, pos).x;
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( e0, e1, a );
	pos.x += 2*d;
	b		=	read_imagef( img, sampler, pos).x;
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( e0, e1, b );
	pos.x -= d;

	nor.x = a - b;
	t = nor.x * nor.x;
//	nor.z = 0.;

	//	GRAD Y
	d		=	aaa->p02;
	pos.y -= d;
	a		=	read_imagef( img, sampler, pos).x;
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( e0, e1, a );
	pos.y += 2*d;
	b		=	read_imagef( img, sampler, pos).x;
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( e0, e1, b );

	nor.y = a - b;

	t += nor.y * nor.y;
	nor.z = sqrt( 1. - t );
//	nor.z += 0.;
//	nor.z = aaa->p10*.10;//	nor.xyz = fast_normalize( nor.xyz );
//	nor.xyz = cross( (float3)(x,0,0), (float3)(0,y,0) );
//	nor.xyz = fast_normalize( nor.xyz );
//	nor.z = aaa->p10*.10;
//	nor.xyz = fast_normalize( nor.xyz );
	return nor;

//	return (float4)(0,0,1,1);
}
__kernel void Displace_tex_2(
							__global				Particle*		particles
						// ,	__global __write_only	float4*		pos_out
						// ,	__global __write_only	float4*		col_out
						// ,	__global __write_only	float4*		nor_out
						// ,	__global __write_only	float4*		att_out
						// ,	const			kernel_param		aaa
						// ,	const			dataset_param		ds
						// ,__global __read_only image2d_t img			//nvidia
						// ,__global __read_only image2d_t img2		//nvidia

						,	__global 				float4*			pos_out
						,	__global 				float4*			col_out
						,	__global 				float4*			nor_out
						,	__global				float4*			att_out
						,	const					kernel_param	aaa
						,	const					dataset_param	ds
						,	__read_only				image2d_t		img
						,	__read_only				image2d_t		img2
//						,	image2d_t img2		//nvidia
//						,	__read_only image2d_t	img
//						,	__read_only image2d_t	img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle *p = &particles[id];		//	get the particle struct
	int sub_id = id - aaa.i_offset;
	if( aaa.line != 0. )	// if we draw line the ..._out have a double size to handle the 2 points
		id = sub_id * 2 + aaa.i_offset;

//	get pixel color
	float2	img_size =  convert_float2( get_image_dim(img) );
	float2	uv = p->pos.xy + .5f;
	float2	pos = uv * img_size;
//	pos.y = (p->pos.y * 1. + .5f) * img_size.y;
//	const sampler_t sampler		= CLK_FILTER_NEAREST	| CLK_ADDRESS_CLAMP_TO_EDGE;
//	const sampler_t sampler		= CLK_FILTER_LINEAR		| CLK_ADDRESS_CLAMP;
	float4	color	= read_imagef( img, sampler, pos );
//	float4	color;

//	eliminate early unwanted dots
	pos_out[id].w = 1;

	float2 img_size2 =  convert_float2( get_image_dim(img2) );
	float2 pos2 = uv * img_size2;
	float4 col2 = read_imagef( img2, sampler, pos2 );
	float  gr;
	int ch2 = get_image_channel_order( img2 );
	switch( ch2 )
	{
	case CLK_RGB:
	case CLK_RGBA:
	case CLK_ARGB:
	case CLK_BGRA:		gr = rgb_to_luma( col2.xyz );	break;
//	case CLK_A:			case CLK_R:
//	case CLK_RG:		case CLK_RA:
//	case CLK_INTENSITY:	case CLK_LUMINANCE:
	default:		 	gr = col2.x; break;
	}

	//gr = clamp( (gr-aaa.p06) * (aaa.p07-aaa.p06), 0.f, 1.f );
	//gr = gr * gr * (3. - 2. * gr);
	gr = smoothstep( aaa.p06, aaa.p07, gr );

	float2 pout;
//	pout.x	=	p->pos.x * 1.77;
	pout.x	=	p->pos.x * img_size.x / img_size.y;
	pout.y	=	p->pos.y;
	if( gr < aaa.p03 )
	{
		pos_out[id].xy = pout.xy;
		pos_out[id].z = 0;

		if( aaa.color != 0. )
			col_out[id] = (float4)(0);
		if( aaa.normal != 0. )
			nor_out[id] = (float4)(0,0,1,1);
		return;
	}

	// get_gradient
 	float4 nor;

#if 0
	float nor_z = aaa.p10*.10;
	float4 param = { aaa.p01, aaa.p02, aaa.p06, aaa.p07 };
	if( aaa.p05==0. )
		nor = get_gradient_smoothstep_luma( img, pos, param, nor_z );
	else if( aaa.p05==1. )
		nor = get_gradient_smoothstep_luma( img2, pos2, param, nor_z );
	else
		nor = mix(
				get_gradient_smoothstep_luma( img, pos, param, nor_z ),
			 	get_gradient_smoothstep_luma( img2, pos2, param, nor_z ),
				aaa.p05 );
#else
//#define GET_GRADIENT get_gradient_smoothstep_luma_old
//#define GET_GRADIENT get_gradient_smoothstep_luma_v3
//#define GET_GRADIENT get_gradient_r
#define GET_GRADIENT get_normal_2023
	if( aaa.p05==0. )
		nor = GET_GRADIENT( img, pos, &aaa );
	else if( aaa.p05==1. )
		nor = GET_GRADIENT( img2, pos2, &aaa );
	else
		nor = mix(	GET_GRADIENT( img, pos, &aaa ),
					GET_GRADIENT( img2 ,pos2, &aaa ),
					aaa.p05 );
#endif
	nor.w = 1.;

//	float4 one = {1,1,1,1};
	float f = aaa.p08 + gr*aaa.p09;
	pout.xy	+= nor.xy*f;

	pos_out[id].z = gr*aaa.p04;	// + nor.z*f;
	//pos_out[id].z = nor.z*f;
	if( aaa.v01.x > 0 && aaa.v01.x < 1 )
	{	//kinect case
		gr = 1.20 - gr * (1.20 - .4);
		pos_out[id].xy = pout.xy * gr ;
	}
	else
	{
		pos_out[id].xy = pout.xy;
	}

	//gr = 1-gr;
	if( aaa.color != 0. )
	{
		if( aaa.color > 0. )
		{
			if( aaa.v01.x >1. )
			{
				color.x = fabs(nor.x) * aaa.color * 8.;
				color.y = fabs(nor.y) * aaa.color * 8.;
				color.z = 0;
			}
			else
				color.xyz *= aaa.color;
		}
		else
		{
			gr *= -aaa.color;
			color = (float4) (gr,gr,gr,1);
		}
		col_out[id] = color * aaa.v01;
		//color.x = 0;
		//color.y = 1;
		//color.z = 0;
		//color.w = 1;
	}
	//col_out[id] = cb;
	//col_out[id].w = 1.;
	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };

	if( aaa.normal != 0. )
	{
		nor_out[id].xy	= nor.xy * (aaa.normal * 8);
		nor_out[id].z	= 1.;
	}
	//p->vel *= DAMP;
//	col_out[id].xyz = nor_out[id].xyz;
}

