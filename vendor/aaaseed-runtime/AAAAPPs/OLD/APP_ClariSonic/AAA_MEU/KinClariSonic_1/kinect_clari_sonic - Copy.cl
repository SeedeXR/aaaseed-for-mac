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

#define MAKE_INVALID()			\
	{							\
		pos_out[id].x = -4242;	\
		pos_out[id].y = -4242;	\
		pos_out[id].z = -4242;	\
		pos_out[id].w = -4242;	\
	}

float3 transform( float3 vec, const kernel_param* const aaa )
{
//now we transform but most of this could optimized
//translate
	vec += aaa->v01.xyz;
//rotate x
	float a;
	float c, s;
	s = sincos ( aaa->v02.x * M_PI * 2., &c );
	vec.yz = (float2)( vec.y * c + vec.z * s, -vec.y * s + vec.z * c );
//rotate y
	s = sincos ( aaa->v02.y * M_PI * 2., &c );
	vec.zx = (float2)( vec.z * c + vec.x * s, -vec.z * s + vec.x * c );
//rotate z
	s = sincos ( aaa->v02.z * M_PI * 2., &c );
	vec.xy = (float2)( vec.x * c + vec.y * s, -vec.x * s + vec.y * c );
//last translation
	vec += (float3)(aaa->p03, aaa->p04, aaa->p05 );
//last y rotation
	s = sincos ( aaa->p06 * M_PI * 2., &c );
	vec.zx = (float2)( vec.z * c + vec.x * s, -vec.z * s + vec.x * c );
	return vec;
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
						,	__read_only image2d_t	img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle *p = &particles[id];		//	get the particle struct
	int sub_id = id - aaa.i_offset;

	float2 xy;
	int ii;
	if( aaa.p14 == 2048 )
	{
		xy = (float2)( (id >> 3) & 0xff, (id >>14) & 0xff ) / 256.f;
		ii = ((id >> 11) & 0x7) * 8 + (id & 0x7);
	}
	else if( aaa.p14 == 1024 )
	{
//		ix = (id >> 3) & 0x7f;
//		iy = (id >>13) & 0x7f;
//		ii = ((id >> 10) & 0x7) * 8 + (id & 0x7);
		xy = (float2)( (id) & 0x7f, (id >> 7) & 0x7f ) / 128.f;
		ii = (id >> 14) & 0x3f;
	}
	else if( aaa.p14 == 512 )
	{
		xy = (float2)( (id >> 3) & 0x3f, (id >>12) & 0x3f ) / 64.f;
		ii = ((id >> 9) & 0x7) * 8 + (id & 0x7);
	}
	else if( aaa.p14 == 256 )
	{
		xy = (float2)( (id >> 3) & 0x1f, (id >>11) & 0x1f ) / 32.f;
		ii = ((id >> 8) & 0x7) * 8 + (id & 0x7);
	}
	else
	{
		xy = (float2)( (id >> 3) & 0xf, (id >>10) & 0xf ) / 16.f;
		ii = ((id >> 7) & 0x7) * 8 + (id & 0x7);
	}
//	get pixel color
	float2	uv = (float2)( get_image_width( img ), get_image_height( img ) );
	uv *= xy;
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;
	//const sampler_t sampler	= CLK_FILTER_LINEAR	| CLK_ADDRESS_CLAMP;
	float4	color	= read_imagef( img, sampler, uv );
	float4	color2	= read_imagef( img2, sampler, uv );


	float	v = extract_depht(color);
	float	v2 = extract_depht(color2);
	float 	grey;

//	float4 one = {1,1,1,1};
	int fra_index = (int)(aaa.p10) & 0x3f;
	fra_index -= ii;

	float3 pos;
	float3 vel;
	if( fra_index != 0 )
	{
		pos = p->pos.xyz;

//		if( (pos.xyz != max( pos.xyz, aaa.v03.xyz )) || (pos.xyz != min( pos.xyz, aaa.v04.xyz)) )
		if( any( pos.xyz != max( pos.xyz, aaa.v03.xyz ) ) || any(pos.xyz != min( pos.xyz, aaa.v04.xyz)) )
		{
			MAKE_INVALID();
			return;
		}

		if( fra_index < 0 )
			fra_index += 64;

		#define TRAIL aaa.p11
		int val = TRAIL - fra_index;
		if( val <= 0. )
			grey = 0;
		else
			grey = val / TRAIL;

		vel = p->vel.xyz;
		vel.y += aaa.dt * (-aaa.p12);
		pos += vel * aaa.dt;

		p->pos.xyz = pos;
		p->vel.xyz = vel;
	}
	else
	{
		grey = 1.;
		float z = v * 0.70804 * 2; // tan( 70.6 / 2)
		pos.z = -v;
		pos.xy = (xy - .5f) * (float2)( -z, z * 424./512. );
		pos = transform( pos, &aaa );

		float vz = 0;
		if( v > 0 && v2 > 0 )	//todo refine validity of points
		{
			if( fabs(v2-v) > .01 )	//avoid low level noise
				vz = (v2-v) * aaa.p13	/ aaa.dt;
		}
		vel = (float3)( 0., 0, vz );

		p->pos.xyz = pos;
		p->vel.xyz = vel;

		if( any( pos.xyz != max( pos.xyz, aaa.v03.xyz ) ) || any(pos.xyz != min( pos.xyz, aaa.v04.xyz)) )
		{
			MAKE_INVALID();
			return;
		}
	}

	pos_out[id].xyz = pos.xyz;
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
			if( pos.y <= aaa.p08 || 2.2 <= pos.y )
			{
				col_out[id] = (float4)(1,1,1,1);
				return;
			}
			float l =  pos.y * aaa.p07;
			col_out[id].xyz = (float3)(l,l,l);
		}
		else
		{	//	color on the axis to help find transformation
			float l = aaa.p09 * .005;	//	param in cm and we need half here
			if( -l < pos.y && pos.y < l )
				col_out[id].xyz = (float3)(0,1,0);
			else if( -l < pos.z && pos.z < l )
				col_out[id].xyz = (float3)(0,0,1);
			else if( -l < pos.x && pos.x < l )
				col_out[id].xyz = (float3)(1,0,0);
			else
				col_out[id].xyz = (float3)(1,1,1);
		}
	}
	else
		col_out[id].xyz = (float3)(1,1,1);
	col_out[id].w = grey;
/*
	if( aaa.normal > 0. )
	{
		nor_out[id] = n;
	}
*/
}

