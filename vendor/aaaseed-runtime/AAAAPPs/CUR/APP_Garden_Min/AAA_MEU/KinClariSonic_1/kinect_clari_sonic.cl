#define DAMP			0.95f
#define CENTER_FORCE	.1f
#define MOUSE_FORCE		3.0f
#define MIN_SPEED		0.001f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED
#define DT				0.02f

#define ONE_OVER_2PI

float extract_depht( float4 color )
{	//check the formula and check the kinect code
//	return ( 256. * color.x + color.y ) * .256;
	return color.w;
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

#define OLD 1
#ifdef OLD
#define DO_INVALID()	\
{							\
		p->pos = (float4)( -4242, -4242, -4242, -4242 );	\
		pos_out[id] = p->pos;		\
		col_out[id].w = 0.;				\
		return;						\
}
#else
#define DO_INVALID()	\
{							\
		p->pos = (float4)( -4242, -4242, -4242, -4242 );	\
		pos_out[id] = p->pos;		\
		col_out[id].w = 0.;				\
		++id;							\
		continue;						\
}
#endif

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


#define POWER_OF_2 		aaa.p10
#define FRAME_INDEX 	aaa.p11
#define TRAIL_NEW 		aaa.p12
#define TRAIL_SPEED 	aaa.p13
#define SPEED_THRESHOLD	(aaa.p14*1.f)
#define SPEED_FACTOR	(aaa.p15*1.f)
#define SPEED_FACTOR_XY	(aaa.p16*1.f)
#define GRAVITY			(-aaa.p17)
#define FLOOR			(aaa.p18)

#ifdef OLD
 __kernel
#else
 __kernel __attribute__((reqd_work_group_size(8,8,1)))
#endif
 	void Kinect_tex_to_3d(
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

#ifdef OLD
	int id = get_global_id(0);					//	get the Particle index
#else
	int id = get_group_id( 0 );
#endif
	//int sub_id = id - aaa.i_offset;

	int ii = POWER_OF_2;
	int mask = (1 << ii) - 1;
	float f_step =  1./ (float)(1<<ii);
	int index_y = (id >>ii) & mask;
	float2 xy = (float2)( id & mask, index_y ) * f_step;
	if( index_y & 1 )	xy.x += f_step * .5;
	//xy.y *= .0;
	//xy = -xy;

#ifdef OLD
	ii = id >> (ii<<1);
#else
	id = id * 64;
	for( ii = 0; ii<64; ii++ )
	{
#endif
	//	get the particle struct
	__global Particle* p = &particles[id];

	float 	grey;

//	float4 one = {1,1,1,1};
	int fra_index = (int)(FRAME_INDEX) & 0x3f;
	fra_index -= ii;

	float3 pos;
	float3 vel;
	if( fra_index != 0 )
	{	// we process
		//DO_INVALID();
		if( p->dum.w == 0  )
			DO_INVALID();

		pos = p->pos.xyz;

		if( fra_index < 0 )
			fra_index += 64;

		int val = p->dum.w - fra_index;
		if( val <= 0. )
			DO_INVALID();
		
		grey = 1 - clamp( val / p->dum.w, 0.f, 1.f );
		grey = 1 - grey * grey;

		vel = p->vel.xyz;
		vel.y += aaa.dt * GRAVITY;
		pos += vel * aaa.dt;
		if( pos.y < FLOOR )
		{
			pos.y -= vel.y * aaa.dt * 2.;
			vel.y *= -.5;
			grey = 1;
		}
	}
	else
	{	// we emit
// tan( 70.6 / 2)
//#define CONE_FX		( 0.70804 * 2 )
//#define CONE_FY		( CONE_FX * 424./512. )
#define CONE_FX		( 1.18 )
#define CONE_FY		( CONE_FX * 534./1024. * 1.)
#define CONE_F		((float2)( CONE_FX, CONE_FY ))

//	get pixel color
		float2	size = convert_float2( get_image_dim( img ) );
		//uv *= xy;
		const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;
		float4	color;
		float z;
		//const sampler_t sampler	= CLK_FILTER_LINEAR	| CLK_ADDRESS_CLAMP;

//	position courante
		color	= read_imagef( img, sampler, xy*size );
		z = extract_depht( color);
		if( z == 0. )	DO_INVALID();
		pos.z = -z;
		pos.xy = (xy - .5f) * CONE_F * z ;
		pos = transform( pos, &aaa );

		p->dum.w = 0;
		if( any( pos.xyz != max( pos.xyz, aaa.v03.xyz ) ) || any(pos.xyz != min( pos.xyz, aaa.v04.xyz)) )
			DO_INVALID();
		float3 nor = color.xyz;

//	position precedente
		float3 posp;
		float v;
		color	= read_imagef( img2, sampler, xy*size );
		z = extract_depht( color );
		if( z == 0. )
		{	//	new point
			v = 0;
			p->dum.w = TRAIL_NEW;
		}
		else
		{	//	existing
			posp.z = -z;
			posp.xy = (xy - .5f) * CONE_F * z ;
			posp = transform( posp, &aaa );
			if( z <= 0. )
				v = 0;
			else
			{
#define		TH 		1.f
//				v = clamp( distance( pos, posp ), -TH, TH );
				v = clamp( pos.z - posp.z, -TH, TH );
			}
		}

#if 0
		float3 posx;
		xy.x += f_step;
		color	= read_imagef( img, sampler_tmpler_tmpler, uv * xy );
		z = extract_depht(color);
		//if( z <= 0. )	DO_INVALID();
		posx.z = -z;
		posx.xy = (xy - .5f) * CONE_F * z ;
		posx = transform( posx, &aaa );
		float2 norx = normalize( posx.xz - pos.xz );

		xy += (float2)( -f_step, f_step );
		color	= read_imagef( img, sampler, uv * xy );
		z = extract_depht(color);
		//if( z <= 0. )	DO_INVALID();
		posx.z = -z;
		posx.xy = (xy - .5f) * CONE_F * z ;
		posx = transform( posx, &aaa );
		float2 nory = normalize( posx.yz - pos.yz );
#endif


		if( v > (SPEED_THRESHOLD * aaa.dt) )
		{
			vel = (float3)(		nor.x * SPEED_FACTOR_XY * SPEED_FACTOR,
								nor.y * SPEED_FACTOR_XY * SPEED_FACTOR,
								nor.z * (pos.z - posp.z) ) * SPEED_FACTOR;
//			vel.z += v * 10.;
//			vel *= SPEED_FACTOR;
//			vel.xy /= (pos.z-5.f);
//			vel = (float3)( 0, 0, 0 );
			p->dum.w = TRAIL_SPEED;
		}
		else
		{
			vel = (float3)( 0, 0, 0 );
		}




//		if( nor.x < .2 )	DO_INVALID();
		//we store normal here
		//(2-fabs( nor.x )) * (2-fabs( nor.y )) *.25 ;
		p->dum.xyz = nor.xyz;

/*
		if( pos.z > 0 && v2 > 0 )	//todo refine validity of points
		{
			if( fabs(v2-pos.z) > .01 )	//avoid low level noise
				vz = (v2-pos.z) * aaa.p13	/ aaa.dt;
		}
		vel = (float3)( 0., 0, vz );
*/

		//if( any( pos.xyz != max( pos.xyz, aaa.v03.xyz ) ) || any(pos.xyz != min( pos.xyz, aaa.v04.xyz)) )
		//	DO_INVALID();

		p->pos_b.xyz = hsv_to_rgb( pos.x + 10.7, 1, 1);
		grey = 1;
	}
	p->pos.xyz = pos;
	p->vel.xyz = vel;

	pos_out[id].xyz = pos.xyz;
	pos_out[id].w = 1;

/*
	if( aaa.color != 0. )
	{
//		if( aaa.p08 > 0 )
//			color.xyz = (float3)( v, v, v );
//		color.w = 1;

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
*/
	col_out[id] = (float4)( p->pos_b.xyz, grey );

	nor_out[id] = (float4)( p->dum.xyz, grey );
#ifdef OLD
#else
	++id;
	}
#endif
}

