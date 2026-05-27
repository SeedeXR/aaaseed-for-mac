#define DAMP			0.95f
#define CENTER_FORCE	.1f
#define MOUSE_FORCE		3.0f
#define MIN_SPEED		0.001f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED
#define DT				0.02f

#define ONE_OVER_2PI

float extract_depht( float4 color )
{	//check the formula and check the kinect code
	return ( 256. * color.x + color.y ) * .256;
}
float extract_lum( float4 color )
{	//check the formula and check the kinect code
	return (color.x + color.y + color.z) / 3.;
}
float extract_compo( float4 color, int sel )
{
	if( sel == 0 )
		return color.x;
	else if( sel == 1 )
		return color.y;
	return color.z;
}


__kernel void Kinect_tex_to_3d(
							__global Particle*		particles
						,	__global float4*		pos_out
						,	__global float4*		col_out
						,	__global float4*		nor_out
						,	__global float4*		att_out
						,	const kernel_param		aaa
//						,	const dataset_param		ds
//nvidia				__global __read_only image2d_t img,
//nvidia				__global __read_only image2d_t img2
						,	__read_only image2d_t	img
//						,	__read_only image2d_t	img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle *p = &particles[id];		//	get the particle struct
	int sub_id = id - aaa.i_offset;

//	get pixel color
	float2	img_size;	//	we need it later
	img_size.x =  get_image_width( img );
	img_size.y =  get_image_height( img );
	float2	pos;
	pos.x = -p->pos.x;
	pos.y = p->pos.y;
	pos = (pos + .5f) * img_size;
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;
	//const sampler_t sampler	= CLK_FILTER_LINEAR	| CLK_ADDRESS_CLAMP;
	float4	color	= read_imagef( img, sampler, pos );

//	elininate early unwanted dots
	float	v = extract_depht(color);

//	float4 one = {1,1,1,1};
	float2 pout;
	float z				= - v;
//	float z				=   aaa.p02 - v * (aaa.p02-aaa.p01);	// same than aaa.p07 * (1 - v) + v * aaa.p06);
	//	we need this z to compute x and y
	float x				=   p->pos.x * z * 0.70804 * 2; // tan( 70.6 / 2)
	float y				= - p->pos.y * z * 0.70804 * 2*				424./512.;

//now we transform but most of this could optimized
	float a;
	float c, s;
//translate
	x += aaa.v01.x;
	y += aaa.v01.y;
	z += aaa.v01.z;
//rotate x
	s = sincos ( aaa.v02.x * M_PI * 2., &c );
	a = y * c + z * s;
	z = -y * s + z * c;
	y = a;
//rotate y
	s = sincos ( aaa.v02.y * M_PI * 2., &c );
	a = z * c + x * s;
	x = -z * s + x * c;
	z = a;
//rotate z
	s = sincos ( aaa.v02.z * M_PI * 2., &c );
	a = x * c + y * s;
	y = -x * s + y * c;
	x = a;
//last translation
	x += aaa.p03;
	y += aaa.p04;
	z += aaa.p05;
//last y rotation
	s = sincos ( aaa.p06 * M_PI * 2., &c );
	a = z * c + x * s;
	x = -z * s + x * c;
	z = a;

	if( z <= aaa.p01 || aaa.p02 <= z )
	{
		float s = 4.;
		pos_out[id].x = p->pos.x * s * 512. / 424.;
		pos_out[id].y = p->pos.y * s;
		pos_out[id].z = 0;
		pos_out[id].w = 1;
		if( aaa.color > 0. )
		{
			color.x = v;
			color.y = v;
			color.z = v;
			color.w = 0.;
			col_out[id] = color;
		}
		return;
	}

	pos_out[id].x = x;
	pos_out[id].y = y;
	pos_out[id].z = z;
	pos_out[id].w = 1;

	if( aaa.color != 0. )
	{
/*		if( aaa.p08 > 0 )
		{
			color.x = v;
			color.y = v;
			color.z = v;
		}
		color.w = 1;
*/
		if( aaa.p07 > 0. )
		{
			if( y <= aaa.p08 || 2.2 <= y )
			{
				color.x = 0.;
				color.y = 0.;
				color.z = 0.;
				color.w = 0.;
				col_out[id] = color;
				return;
			}
			float l =  y * aaa.p07;
			col_out[id].x = l;
			col_out[id].y = l;
			col_out[id].z = l;
			col_out[id].w = 1;
		}
		else
		{	//	color on the axis to help find transformation
			float l = aaa.p09 * .005;	//	param in cm and we need half here
			if( -l < y && y < l )
			{
				col_out[id].x = 0;
				col_out[id].y = 1;
				col_out[id].z = 0;
			}
			else if( -l < z && z < l )
			{
				col_out[id].x = 0;
				col_out[id].y = 0;
				col_out[id].z = 1;
			}
			else if( -l < x && x < l )
			{
				col_out[id].x = 1;
				col_out[id].y = 0;
				col_out[id].z = 0;
			}
			else
			{
				col_out[id].x = 1;
				col_out[id].y = 1;
				col_out[id].z = 1;
			}
		}

		col_out[id].w = 1;
	}
	else
	{
		color.x = 1.;
		color.y = 1.;
		color.z = 1.;
		color.w = 1.;
		col_out[id] = color;
	}

/*
	if( aaa.normal > 0. )
	{
		nor_out[id] = n;
	}
*/
}

