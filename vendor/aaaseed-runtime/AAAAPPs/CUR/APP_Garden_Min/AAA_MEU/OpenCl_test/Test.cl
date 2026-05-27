#define DAMP			0.95f
#define CENTER_FORCE	.1f
#define MOUSE_FORCE		3.0f
#define MIN_SPEED		0.001f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED
#define DT				0.02f

float extract_depht( float4 color )
{	//check the formula and check the kinect code
	return (color.y * 256. + color.z) / 256.;
}
float extract_lum( float4 color )
{	//check the formula and check the kinect code
	return (color.x + color.y + color.z) / 3.;
}

__kernel void test(
							__global Particle*		particles
						,	__global float4*		pos_out
						,	__global float4*		col_out
						,	__global float4*		nor_out
						,	__global float4*		att_out
						,	const kernel_param		aaa
						,	const dataset_param		ds
//nvidia				__global __read_only image2d_t img,
//nvidia				__global __read_only image2d_t img2
						,	__read_only image2d_t	img
//						,__read_only image2d_t img2
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
	const sampler_t sampler		= CLK_FILTER_LINEAR	| CLK_ADDRESS_REPEAT;
	float4	color	= read_imagef( img, sampler, pos );

	float	val = extract_lum( color );
	float2	speed = p->vel.xy;
	float4	nor;
	nor.z = 1.;
	nor.w = 1.;
	if( val < aaa.p01 )
	{
		speed *= aaa.p03;
	}
	else if( aaa.p02 < val )
	{
		speed *= aaa.p05;
		//speed.x = 1.;
		//speed.y = 1.;
	}
	else
	{
	//get_gradient
		float4	cb;
		float	vb;

		pos.x += aaa.p09;
		cb = read_imagef( img, sampler, pos );
		vb = extract_lum(cb);
		nor.x = (vb - val);// * img_size.x / aaa.p09;
		pos.x -= aaa.p09;

		pos.y += aaa.p10;
		cb = read_imagef( img, sampler, pos );
		vb = extract_lum(cb);
		nor.y = (vb - val);// * img_size.y / aaa.p10;

		//p->accel.xy = nor.xy * aaa.p06;
		speed += nor.xy * aaa.dt * aaa.p06;
		speed *= aaa.p04;
		//nor.z = 1.;
		//nor.w = 0.;
		//float4 n = fast_normalize( nor );
	}
	float f = aaa.dt * aaa.p07 * 20;
	pos.xy = p->pos.xy + speed.xy * f;
#if	1
	if( pos.x<-.5 )			pos.x += 1.;
	else if ( pos.x>.5 )	pos.x -= 1.;
	if( pos.y<-.5 )			pos.y += 1.;
	else if ( pos.y>.5 )	pos.y -= 1.;
#else
	if( pos.x<-.5 || .5<pos.x )
	{
		speed.x = - speed.x;
		pos.x +=  speed.x * f * 2;
	}
	if( pos.y<-.5 || .5<pos.y )
	{
		speed.y = - speed.y;
		pos.y +=  speed.y * f * 2;
	}
#endif
	p->vel.xy = speed;

	p->pos.xy = pos.xy;


/*
	float f = aaa.p08 * .01;
	float2 pout;
	pout.x				= (p->pos.x	+ n.x*f) * 1.33;
	pout.y				= p->pos.y	+ n.y*f;
	pos_out[id].z	= val*aaa.p03;// + n.z*f;
	pos_out[id].xy = pout.xy;
*/
	float2 pout;
	pout.x			=	p->pos.x * 1.33;
	pout.y			=	p->pos.y;
	pos_out[id].z	=	val * aaa.p08;
	pos_out[id].xy	=	pout.xy;
	pos_out[id].w	=	1.;
	//val = 1-val;
	if( aaa.color > 0. )
	{
		col_out[id] = aaa.color;
		col_out[id].w = 1.;
	}
	else
	{
	//	col_out[id] = color;
	//	col_out[id].w = 1.;
	}
	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };

	if( aaa.normal > 0. )
	{
		nor_out[id] = nor;
	}
	//p->vel *= DAMP;
}


