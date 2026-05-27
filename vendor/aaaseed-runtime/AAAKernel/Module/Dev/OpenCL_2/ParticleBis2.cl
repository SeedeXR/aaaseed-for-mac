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

__kernel void Kinect(
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
	const sampler_t sampler		= CLK_FILTER_NEAREST	| CLK_ADDRESS_CLAMP;
	float4	color	= read_imagef( img, sampler, pos );

//	elininate early unwanted dots
	pos_out[id].w = 1;
	float	v;
	if( aaa.p07 > 0 )
		v = extract_depht(color);
	else
		v = extract_lum( color );
	if( v < aaa.p04 )
	{
		pos_out[id].x = -400;
		pos_out[id].y = -4;
		pos_out[id].y = -4;
		if( aaa.line != 0. )
		{
			pos_out[id+1] = pos_out[id];
		}
		return;
	}

//get_gradient
	float4	nor;
	float4	cb;
	float	vb;

	pos.x += aaa.p01;
	cb = read_imagef( img, sampler, pos );
	if( aaa.p07 > 0 )
		vb = extract_depht(cb);
	else
		vb = extract_lum(cb);
	nor.x = (vb - v) * aaa.normal * img_size.x / aaa.p01;

	pos.x -= aaa.p01;
	pos.y += aaa.p02;
	cb = read_imagef( img, sampler, pos );
	if( aaa.p07 > 0 )
		vb = extract_depht(cb);
	else
		vb = extract_lum(cb);
	nor.y = (v - vb) * aaa.normal * img_size.y / aaa.p02;

	nor.z = 1.;
	nor.w = 0.;
 	float4 n = fast_normalize( nor );


//	float4 one = {1,1,1,1};
	float2 pout;
	pout.x				= (p->pos.x	+ n.x*aaa.p08) * 1.33;
	pout.y				= p->pos.y	+ n.y*aaa.p08;
	pos_out[id].z	= v*aaa.p03 + n.z*aaa.p08;
	if( aaa.p07 > 0 )
	{	//kinect case
		v = 1.20 - v * (1.20-.4);
		pos_out[id].xy = pout.xy * v ;
	}
	else
	{
		pos_out[id].xy = pout.xy;
	}
	if( aaa.line != 0. )
	{
		pos_out[id+1] = pos_out[id];
		pos_out[id+1].z -= 0.05;
	}
	//v = 1-v;
	if( aaa.color > 0. )
	{
		if( aaa.p07 > 0 )
		{
			color.x = v;
			color.y = v;
			color.z = v;
		}
		color.w = 1;
		col_out[id] = color;
		if( aaa.line != 0. )
		{
			col_out[id+1] = col_out[id];
		}
	}
	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };

	if( aaa.normal > 0. )
	{
		nor_out[id] = n;
	}
	//p->vel *= DAMP;
}


