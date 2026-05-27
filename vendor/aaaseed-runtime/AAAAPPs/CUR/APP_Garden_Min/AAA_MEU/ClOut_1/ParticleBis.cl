			#define DAMP			0.95f
#define CENTER_FORCE	.1f
#define MOUSE_FORCE		3.0f
#define MIN_SPEED		0.001f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED
#define DT				0.02f

float extract_depht( float3 color )
{	//check the formula and check the kinect code
	return (color.y * 256. + color.z) / 256.;
}

__constant sampler_t sampler		= CLK_FILTER_LINEAR	| CLK_ADDRESS_CLAMP;

float4 get_gradient(	__read_only				image2d_t		img
						,						float2 			pos
						,						kernel_param*	aaa
					)
{
	float4	nor;
	float	a;
	float	b;
	float	e0	= aaa->p06;
	float	e1	= aaa->p07;
	float	d;

	//	GRAD X
	d		=	aaa->p01;
	pos.x -= d;
	a		=	read_imagef( img, sampler, pos).x;
	//a		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( e0, e1, a );
	pos.x += 2*d;
	b		=	read_imagef( img, sampler, pos).x;
	//b		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( e0, e1, b );
	nor.x = a - b;
	pos.x -= d;

	//	GRAD Y
	d		=	aaa->p02;
	pos.y -= d;
	a		=	read_imagef( img, sampler, pos ).x;
	//a		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( e0, e1, a );
	pos.y += 2*d;
	b		=	read_imagef( img, sampler, pos).x;
	//b		=	rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( e0, e1, b );
	nor.y = a - b;

	nor.z = aaa->p10*.10;
	nor.xyz = fast_normalize( nor.xyz );

	return nor;
}

float extract_h( float3 color, float sel )
{
	if( sel > 0. )
		return extract_depht(color);
	else
		return rgb_to_luma(color);
}

__kernel void Displace_tex_2(
							__global				Particle*		particles
/*						,	__global __write_only	float4*		pos_out
						,	__global __write_only	float4*		col_out
						,	__global __write_only	float4*		nor_out
						,	__global __write_only	float4*		att_out
						,	const 			kernel_param		aaa
						,	const 			dataset_param		ds
						,__global __read_only image2d_t img			//nvidia
						,__global __read_only image2d_t img2		//nvidia
*/
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
	float2	img_size;	//	we need it later
	img_size.x =  get_image_width( img );
	img_size.y =  get_image_height( img );
	float2	pos;
	pos.x = (p->pos.x * 1. + .5f) * img_size.x;
	pos.y = (p->pos.y * 1. + .5f) * img_size.y;
//	const sampler_t sampler		= CLK_FILTER_NEAREST	| CLK_ADDRESS_CLAMP_TO_EDGE;
//	const sampler_t sampler		= CLK_FILTER_LINEAR		| CLK_ADDRESS_CLAMP;
	float4	color	= read_imagef( img, sampler, pos );

//	eliminate early unwanted dots
	pos_out[id].w = 1;

	float	v;
	v = read_imagef( img2, sampler, pos ).x;
	//v = rgb_to_luma( read_imagef( img2, sampler, pos ).xyz );
	//v = clamp( (v-aaa.p06) * aaa.p07, 0., 1. );
	//v = v * v * (3. - 2. * v);
	v = smoothstep( aaa.p06, aaa.p07, v );

	float2 pout;
	pout.x	=	p->pos.x * 1.77;
	pout.y	=	p->pos.y;
	if( v < aaa.p03 )
	{
		pos_out[id].xy = pout.xy;
		pos_out[id].z = 0;

		color.x = 0;
		color.y = 0;
		color.z = 0;
		color.w = 0;
		col_out[id] = color;

		if( aaa.normal != 0. )
		{
			nor_out[id].x = 0;
			nor_out[id].y = 0;
			nor_out[id].z = 1;
			nor_out[id].w = 1;
		}
		return;
	}

//get_gradient

 	float4 n = get_gradient( img, pos, &aaa ) * (1-aaa.p05);
	n += get_gradient( img2, pos, &aaa ) + aaa.p05;

	n.w = 1.;


//	float4 one = {1,1,1,1};
	float f = .05 * aaa.p08 + .1*v*aaa.p09;
	pout.x	+= 	n.x*f;
	pout.y	+=	n.y*f;
	pos_out[id].z = v*aaa.p04 + n.z*f;	// + n.z*f;
	if( aaa.v01.x > 0 && aaa.v01.x < 1 )
	{	//kinect case
		v = 1.20 - v * (1.20 - .4);
		pos_out[id].xy = pout.xy * v ;
	}
	else
	{
		pos_out[id].xy = pout.xy;
	}

	//v = 1-v;
	if( aaa.color != 0. )
	{
		if( aaa.color > 0. )
		{
			if( aaa.v01.x >1. )
			{
				color.x = fabs(n.x) * aaa.color * 8.;
				color.y = fabs(n.y) * aaa.color * 8.;
				color.z = 0;
			}
			else
				color.xyz *= aaa.color;
		}
		else
		{
			v *= -aaa.color;
			color.x = v;
			color.y = v;
			color.z = v;
			color.w = 1;
		}
		//color.x = 0;
		//color.y = 1;
		//color.z = 0;
		//color.w = 1;
		col_out[id] = color;
	}
	//col_out[id] = cb;
	//col_out[id].w = 1.;
	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };

	if( aaa.normal != 0. )
	{
		nor_out[id].xy	= n.xy * aaa.normal;
		nor_out[id].z	= 1.;
	}
	//p->vel *= DAMP;
}

__kernel void test_out(
							__global				Particle*		particles
						,	__global 				float4*			pos_out
						,	__global 				float4*			col_out
						,	__global 				float4*			nor_out
						,	__global				float4*			att_out
						,	const					kernel_param	aaa
						,	const					dataset_param	ds
						,	__write_only			image2d_t		img1
						,	__read_only				image2d_t		img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	//__global Particle *p = &particles[id];	//	get the particle struct
	//int sub_id = id % 1024;
	//if( aaa.line != 0. )	// if we draw line the ..._out have a double size to handle the 2 points
	//	id = sub_id * 2 + aaa.i_offset;

//	get pixel color
//	float2	img_size;	//	we need it later
//	img_size.x =  get_image_width( img );
//	img_size.y =  get_image_height( img );
	int2	pos;
	pos.x = id % 1024;
	pos.y = id / 1024;

//	pos.x = (p->pos.x * 1. + .5f) * img_size.x;
//	pos.y = (p->pos.y * 1. + .5f) * img_size.y;
//	const sampler_t sampler		= CLK_FILTER_NEAREST	| CLK_ADDRESS_CLAMP_TO_EDGE;
//	const sampler_t sampler		= CLK_FILTER_LINEAR		| CLK_ADDRESS_CLAMP;
//		float4	color	= read_imagef( img, sampler, pos );

	float2 f;
	f.x = pos.x * aaa.p01;
	f.y = pos.y * aaa.p02;

	float4	color	= read_imagef( img2, sampler, f );
	float v = ( sin( aaa.mouse_pos.x * 10. * ((float)pos.x ) / 32.)*.5+.5) * (sin( aaa.mouse_pos.y * 10. * ((float)pos.y ) / 32.)*.5+.5) ;
	//v = 1-v;
	color.xyz += v * aaa.v01.xyz	;
	color.w = 1.;

	//	barrier( CLK_LOCAL_MEM_FENCE );

	write_imagef(	img1, pos, color );
}




