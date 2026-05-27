#define DAMP			0.9f
#define CENTER_FORCE	.005f
#define MOUSE_FORCE		.001f
#define IMG_FORCE		.01f
#define MIN_SPEED		0.001f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED
#define DT				0.02f

float extract_depht( float3 color )
{	//check the formula and check the kinect code
	return (color.y * 256. + color.z) / 256.;
}

__constant sampler_t sampler	= CLK_FILTER_LINEAR		| CLK_ADDRESS_CLAMP_TO_EDGE | CLK_NORMALIZED_COORDS_TRUE;
//	const sampler_t sampler		= CLK_FILTER_NEAREST	| CLK_ADDRESS_CLAMP_TO_EDGE;
//	const sampler_t sampler		= CLK_FILTER_LINEAR		| CLK_ADDRESS_CLAMP;

float4 get_gradient_min_max(	__read_only				image2d_t		img
						,						float2 			pos
						,						kernel_param*	aaa
						,__read_only			float 			e0
						,__read_only			float 			e1
					)
{
	float4	nor;
	float	a;
	float	b;
	float	d;

	int2 size = get_image_dim( img );
	float fx = 1./size.x;
	float fy = 1./size.y;

	//	GRAD X
	d		=	aaa->p01 * fx;
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
	d		=	aaa->p02 * fy;
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
	//nor.xy *= aaa->p10*.10;
	return nor;
}

float2 get_gradient(	__read_only				image2d_t		img
						,						float2 			pos
						,						kernel_param*	aaa
					)
{
	float2	nor;
	float	a;
	float	b;

	int2 size = get_image_dim( img );
	float fx = aaa->p01/size.x;
	float fy = aaa->p02/size.y;

	//	GRAD X
	pos.x -= fx;
	a		=	read_imagef( img, sampler, pos).x;
	pos.x += 2*fx;
	b		=	read_imagef( img, sampler, pos).x;
	nor.x = a - b;
	pos.x -= fx;

	//	GRAD Y
	pos.y -= fy;
	a		=	read_imagef( img, sampler, pos ).x;
	pos.y += 2*fy;
	b		=	read_imagef( img, sampler, pos).x;
	nor.y = a - b;

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
						,	const					kernel_param		aaa
						,	const 					dataset_param		ds
						,	__global __read_only	image2d_t img		//nvidia
						,	__global __read_only	image2d_t img2		//nvidia
*/
//						,	image2d_t img2		//nvidia

						,	__global 				float4*			pos_out
						,	__global 				float4*			col_out
						,	__global 				float4*			nor_out
						,	__global				float4*			att_out

						,	const					kernel_param	aaa
						,	const					dataset_param	ds
						,	__read_only				image2d_t		img
						,	__read_only				image2d_t		img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle* p = &particles[id];		//	get the particle info as struct

//	int sub_id = id - aaa.i_offset;
//	if( aaa.line != 0. )	// if we draw line the ..._out have a double size to handle the 2 points
//		id = sub_id * 2 + aaa.i_offset;

	float4 pos = p->pos;
	float2	uv;
	uv.x = (pos.x * 1. + .5f);
	uv.y = (pos.y * 1. + .5f) ;

//	get pixel color
//	float2	img_size;	//	we need it later
//	img_size.x =  get_image_width(	img );
//	img_size.y =  get_image_height( img );
//	float2	st		= uv;	// * img_size;
	float4	color	= read_imagef( img, sampler, uv );

//	float2	img_size2;	//	we need it later
//	img_size2.x =  get_image_width(	 img2 );
//	img_size2.y =  get_image_height( img2 );
//	float2	st2		= uv;	// * img_size2;
	float4	color2	= read_imagef( img2, sampler, uv );

//	eliminate early unwanted dots
	pos_out[id].w = 1;

//get_gradient
 	float2 grad;
// 	grad = get_gradient( img, uv, &aaa ) * (1-aaa.p04);
	grad = get_gradient( img2, uv, &aaa );

	float mass = 1.;
	// go to original position
	p->vel.xy += normalize(p->pos_b.xy - pos.xy ) * aaa.p06 * 0.1f;
/*
	//float mass = p->mass;

	float2 diff = (aaa.mouse_pos-.5f) * 2.f - pos.xy;
	float invDistSQ = 1.0f / dot(diff, diff);
	diff *= ((float)1.) * invDistSQ;
	p->vel.xy -= diff.xy * mass * MOUSE_FORCE;
*/

	p->vel.xy += aaa.p04 * mass * 10.f * grad.xy;
	p->vel.xy += aaa.p05 * mass * 20.f * (float2)(-grad.x,grad.y);
	pos.xy += p->vel.xy * aaa.dt;

	p->vel *= aaa.p09;	//	DAMP

	//GRAVITY
	pos.xy += aaa.dt * (float2)( aaa.p07, aaa.p08 );

#define SX 0.5f
#define SY SX

	if( pos.x < -SX )		pos.x += 2 * SX ;
	else if( SX < pos.x )	pos.x -= 2 * SX ;
	if( pos.y < -SY )		pos.y += 2 * SY ;
	else if( SY < pos.y )	pos.y -= 2 * SY ;

	p->pos.xy = pos.xy;

	float2 pout;
	int2 size = get_image_dim( img );
	pout.xy	=	pos.xy;

	pos_out[id].xy = pout.xy;
	pos_out[id].z = 0.;

	if( aaa.color != 0. )
	{
		if( aaa.color > 0. )
		{
			if( aaa.v01.x >1. )
			{
				color.x = fabs(grad.x) * aaa.color * 8.;
				color.y = fabs(grad.y) * aaa.color * 8.;
				color.z = 0;
			}
			else
				color.xyz *= aaa.color;
			color *= aaa.v01;
		}
		else
		{
			float v = -aaa.color;
			color = aaa.v01 * v;
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
		nor_out[id].xy	= grad.xy * aaa.normal;
		nor_out[id].z	= 1.;
	}
	//p->vel *= DAMP;
}



