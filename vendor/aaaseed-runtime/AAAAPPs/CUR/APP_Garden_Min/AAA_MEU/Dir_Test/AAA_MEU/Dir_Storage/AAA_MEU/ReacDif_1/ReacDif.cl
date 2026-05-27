#define DAMP			0.95f
#define CENTER_FORCE	.1f
#define MOUSE_FORCE		3.0f
#define MIN_SPEED		0.001f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED
#define DT				0.02f

#define USE_F64	1

#ifdef	USE_F64
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
typedef		double		real;
typedef		double4		real_4;
#else
typedef		float		real;
typedef		float4		real_4;
#endif

//__constant sampler_t sampler		= CLK_FILTER_LINEAR	| CLK_ADDRESS_CLAMP;

/*
float extract_depht( float3 color )
{	//check the formula and check the kinect code
	return (color.y * 256. + color.z) / 256.;
}


float extract_h( float3 color, float sel )
{
	if( sel > 0. )
		return extract_depht(color);
	else
		return extract_lum(color);
}
*/

float4 gray_scott_initial_UV(	__read_only	image2d_t	img,  __read_only	float2 p )
 {
 	int2	pos;
	int2	img_size;

	img_size.x =  get_image_width( img );
	img_size.y =  get_image_height( img );
	pos.x = (p.x * 1. + .5f) * img_size.x;
	pos.y = (p.y * 1. + .5f) * img_size.y;

	const sampler_t sampler 	=	CLK_FILTER_NEAREST | CLK_ADDRESS_REPEAT | CLK_NORMALIZED_COORDS_FALSE;
	float val = rgb_to_luma( read_imagef( img, sampler, pos ).xyz );
    /* artificial contrast enhancement to produce a useful image even when the image is fairly dark */
    val = fmod( 7.0 * val, 1.0 );

    /* Map the values (0.0 .. 1.0) in a parametrized way onto (u,v)
       tuples from [(0.3, 0.3) .. (1.0, 0.0)] */
    float u = 0.3 + 0.7 * val;
    float v = 0.3 * (1.0 - val);

    float4 retval = { u, v, 0.0, 1.0 };
    return(retval);
 }

real_4 read_image(		__read_only			image2d_t	img
					,	__read_only			sampler_t 	samp
					,	__read_only			int2		coor
					)
{
#ifdef	USE_F64
	float4	c =	read_imagef( img, samp, coor );
	real_4	col;
	col.x = c.x;
	col.y = c.y;
	col.z = c.z;
	col.w = c.w;
	return col;
#else
	return read_imagef( img, samp, coor );
#endif
}

void write_image(		__write_only		image2d_t	img
					,	__read_only			int2		coor
					,	__read_only			real_4		col
				)
{
#ifdef	USE_F64
	float4	c;
	c.x = col.x;
	c.y = col.y;
	c.z = col.z;
	c.w = col.w;
	write_imagef( img, coor, c );
#else
	write_imagef( img, coor, col );
#endif
}

/*
float4 get_gradient(	__read_only				image2d_t		img
						,						float2 			pos
						,const						kernel_param*	aaa
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
	//a		=	extract_lum( read_imagef( img, sampler, pos ).xyz );
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( e0, e1, a );
	pos.x += 2*d;
	b		=	read_imagef( img, sampler, pos).x;
	//b		=	extract_lum( read_imagef( img, sampler, pos ).xyz );
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( e0, e1, b );
	nor.x = a - b;
	pos.x -= d;

	//	GRAD Y
	d		=	aaa->p02;
	pos.y -= d;
	a		=	read_imagef( img, sampler, pos ).x;
	//a		=	extract_lum( read_imagef( img, sampler, pos ).xyz );
	//a		=	clamp( (a - o) * of, 0., 1. );
	a		=	smoothstep( e0, e1, a );
	pos.y += 2*d;
	b		=	read_imagef( img, sampler, pos).x;
	//b		=	extract_lum( read_imagef( img, sampler, pos ).xyz );
	//b		=	clamp( (b - o) * of, 0., 1. );
	b		=	smoothstep( e0, e1, b );
	nor.y = a - b;

	nor.z = aaa->p10*.10;
	nor.xyz = fast_normalize( nor.xyz );

	return nor;
}
*/

real_4 get_laplacien_deb(	__read_only		image2d_t	img
							,	__read_only		int2		size
							,	__read_only		int2 		pos
					)
{
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_REPEAT | CLK_NORMALIZED_COORDS_FALSE;

	const real fac	= 20./6. ;
	const real coef_0 = -fac;
	const real coef_1 = .2 * fac;
	const real coef_2 = .05 * fac;

	int x  = pos.x;
	int	xp = x - 1;
	if( xp < 0 )		xp = 0;
	int	xn = x + 1;
	if( xn >= size.x )	xn = size.x -1;

	int y  = pos.y;
	int	yp = y - 50;
	if( yp < 0 )		yp = 0;
	int	yn = y + 1;
	if( yn >= size.y )	yn = size.y -1;

	real_4	col;
	col.x = ((real)(xp)) / ( (real)(size.x) );
	col.y = ((real)(yp)) / ( (real)(size.y) );
	col.z = 0.;
	col.w = 1.;

	return col;
}

real_4 get_laplacien(		__read_only		image2d_t	img
						,	__read_only		int2		size
						,	__read_only		int2 		pos
					)
{
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_REPEAT | CLK_NORMALIZED_COORDS_FALSE;
	const real fac = 20./6. ;

	const real coef_0 = -fac;
	const real coef_1 = .2 * fac;
	const real coef_2 = .05 * fac;

//	const float coef_1 = .14644661;	//	.2;
//	const float coef_2 = .10355339;	//	.05;
//	const float coef_1 = 1./6.;
//	const float coef_2 = 1./12.;
//	const float coef_1 = .202;
//	const float coef_2 = .048;

//	const float coef_1 = .2734217533;
//	const float coef_2 = -.0234220502;

	int x  = pos.x;
	int	xp = x - 1;
	if( xp < 0 )		xp = size.x -1;
	int	xn = x + 1;
	if( xn >= size.x )	xn = 0;

	int y  = pos.y;
	int	yp = y - 1;
	if( yp < 0 )		yp = size.y -1;
	int	yn = y + 1;
	if( yn >= size.y )	yn = 0;

	real_4	col;
	col		=	read_image( img, sampler, pos ) * coef_0;
	pos.y = yp;
	col		+=	read_image( img, sampler, pos ) * coef_1;
	pos.y = yn;
	col		+=	read_image( img, sampler, pos ) * coef_1;

	pos.x = xp;
	col		+=	read_image( img, sampler, pos ) * coef_2;
	pos.y = y;
	col		+=	read_image( img, sampler, pos ) * coef_1;
	pos.y = yp;
	col		+=	read_image( img, sampler, pos ) * coef_2;

	pos.x = xn;
	col		+=	read_image( img, sampler, pos ) * coef_2;
	pos.y = y;
	col		+=	read_image( img, sampler, pos ) * coef_1;
	pos.y = yn;
	col		+=	read_image( img, sampler, pos ) * coef_2;

	return col;
}

real_4 get_laplacien_no_wrap(	__read_only		image2d_t	img
							,	__read_only		int2		size
							,	__read_only		int2 		pos
					)
{
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_REPEAT | CLK_NORMALIZED_COORDS_FALSE;

	const real fac	= 20./6. ;
	const real coef_0 = -fac;
	const real coef_1 = .2 * fac;
	const real coef_2 = .05 * fac;

	int x  = pos.x;
	int	xp = x - 1;
	if( xp < 0 )		xp = 0;
	int	xn = x + 1;
	if( xn >= size.x )	xn = size.x -1;

	int y  = pos.y;
	int	yp = y - 1;
	if( yp < 0 )		yp = 0;
	int	yn = y + 1;
	if( yn >= size.y )	yn = size.y -1;

	real_4	col;
	col		=	read_image( img, sampler, pos ) * coef_0;
	pos.y = yp;
	col		+=	read_image( img, sampler, pos ) * coef_1;
	pos.y = yn;
	col		+=	read_image( img, sampler, pos ) * coef_1;

	pos.x = xp;
	col		+=	read_image( img, sampler, pos ) * coef_2;
	pos.y = y;
	col		+=	read_image( img, sampler, pos ) * coef_1;
	pos.y = yp;
	col		+=	read_image( img, sampler, pos ) * coef_2;

	pos.x = xn;
	col		+=	read_image( img, sampler, pos ) * coef_2;
	pos.y = y;
	col		+=	read_image( img, sampler, pos ) * coef_1;
	pos.y = yn;
	col		+=	read_image( img, sampler, pos ) * coef_2;

	return col;
}

real_4 get_laplacien5(		__read_only			image2d_t	img
						,	__read_only			int2		size
						,	__read_only			int2 		pos

					)
{
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_REPEAT | CLK_NORMALIZED_COORDS_FALSE;

	int x  = pos.x;
	int	xp = x - 1;
	if( xp < 0 )
		xp = size.x -1;
	int	xn = x + 1;
	if( xn >= size.x )
		xn = 0;

	int y  = pos.y;
	int	yp = y - 1;
	if( yp < 0 )
		yp = size.y -1;
	int	yn = y + 1;
	if( yn >= size.y )
		yn = 0;

	real_4	col;
	col		=	-read_image( img, sampler, pos );
	pos.x = xp;
	col		+=	read_image( img, sampler, pos ) * .25;
	pos.x = xn;
	col		+=	read_image( img, sampler, pos ) * .25;

	pos.x = x;
	pos.y = yn;
	col		+=	read_image( img, sampler, pos ) * .25;
	pos.y = yp;
	col		+=	read_image( img, sampler, pos ) * .25;

	return col;
}

__kernel void ReacDif(
							__global				Particle*		particles

/*						,	__global __write_only	float4*		pos_out
						,	__global __write_only	float4*		col_out
						,	__global __write_only	float4*		nor_out
						,	__global __write_only	float4*		att_out
						,	const					kernel_param		aaa
						,	const					dataset_param		ds
						,	__global __read_only	image2d_t img		//nvidia
						,	__global __read_only	image2d_t img2		//nvidia
*/
						,	__global 				float4*			pos_out
						,	__global 				float4*			col_out
						,	__global 				float4*			nor_out
						,	__global				float4*			att_out
						,	const					kernel_param	aaa
						,	const					dataset_param	ds
						,	__read_only				image2d_t		img
						,	__write_only			image2d_t		img2
						,	__read_only				image2d_t		img3

//						,	image2d_t img2		//nvidia
//						,	__read_only image2d_t	img
//						,	__read_only image2d_t	img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle *p = &particles[id];		//	get the particle struct
//	int sub_id = id - aaa.i_offset;
//	if( aaa.line != 0. )	// if we draw line the ..._out have a double size to handle the 2 points
//		id = sub_id * 2 + aaa.i_offset;

//	const sampler_t sampler		= CLK_FILTER_NEAREST	| CLK_ADDRESS_CLAMP_TO_EDGE;
	const sampler_t sampler		= CLK_FILTER_NEAREST	| CLK_ADDRESS_REPEAT | CLK_NORMALIZED_COORDS_FALSE;
	int2	pos;
	int2	img_size;

	// pos in -.5/.5
	// x,y om[0,1.]
	//float x = fmod( (p->pos.x+.5), 1. );
	//float y = fmod( (p->pos.y+.5), 1. );
	float x = p->pos.x + .5;
	float y = p->pos.y + .5;

//	read actual state
	img_size.x =  get_image_width( img );
	img_size.y =  get_image_height( img );

	real_4 col;
	real_4 lap;

	if( aaa.dt <= 0. )
	{
		float s = 1.25;
		float tx = fmod( x * 1. , 1. ) + .25;
		float ty = fmod( y * 1. , 1. ) - .25;
		if( -s<(ty-.5) && (ty-.5)<s && -s<(tx-.5) && (tx-.5)<s )
		{
			col = (real_4) ( 0, .2+p->dum2*.6, 0., 1. );
		}
		else
		{
			col = (real_4) ( 1., 0., 0., 1. );
		}
		//color = (real_4) ( x, y, 0., 1. );
	}
	else
	{

		pos.x = x * img_size.x;
		pos.y = y * img_size.y;
		col = read_image( img, sampler, pos );
		lap =  get_laplacien_no_wrap( img, img_size, pos  );

		/* influence
		img_size.y =  get_image_height( img3 );
		pos.x = x * img_size.x;
		pos.y = y * img_size.y;
		real_4 feed = read_image( img3, sampler, pos );
		//	real_4 feed = get_laplacien( img3, img_size, pos  );
		//	lap.xy += feed.xy * aaa.dt * aaa.p09 * aaa.p08;

		if( feed.x > .8 )
		{
			col.x += feed.x * aaa.dt * aaa.p09;
		}
		if( feed.y < .1 )
		{
			col.y += (1.-feed.y) * aaa.dt * aaa.p10;
		}
		*/

		const real f = .0 + y * .07;
		const real K = .0 + x * .07;
		//f = .02 * feed.y + .2;
		real factor = aaa.dt * aaa.p08;
		if( factor > 0. )
		{
			//real off = 1./4006.;
			real react = col.x * col.y * col.y;

			//c.x += off;
			//c.y += off;
			//			dIFUSSION					rEACTION	fEED
			//c.x += ( aaa.p01 * aaa.p02 * lap.x 	- react 	+ aaa.p05 * aaa.p06 * f * ( 1. - c.x ) )* factor;
			//c.y += ( .aaa.p01 * aaa.p03 * lap.y 	+ react 	- aaa.p05 * aaa.p07 * (f + K) * c.y ) * factor;

			real df = .02f;
			real delta_x = 2. * df * lap.x - react + f * ( 1.0f - col.x );
			real delta_y = 1. * df * lap.y + react - ( f + K ) * col.y;
			col.x += delta_x * .5;
			col.y += delta_y * .5;

			//		c.x += .082f * 3.33333 * lap.x  - react 	+ f * ( 1. - c.x );
			//		c.y += .041f * 3.33333 * lap.y  + react 	- ( f + K ) * c.y;
			//											Kill

			if( lap.x <= 0.000001 )
				col.z = 0.;
			else
				col.z = lap.x * 1000.;
		}
		//	c.x = clamp( c.x, 0., 1. );
		//	c.y = clamp( c.y, 0., 1. );
	}


	int2 coor;
	img_size.x =  get_image_width(	img2 );
	img_size.y =  get_image_height( img2 );
	coor.x = x * img_size.x;
	coor.y = y * img_size.y;
//	coor.x = clamp( (int)( (p->pos.x + .5f) * img_size.x ), 0, img_size.x - 1 );
//	coor.y = clamp( (int)( (p->pos.y + .5f) * img_size.y ), 0, img_size.y - 1 );
	write_image( img2, coor, col );

//	eliminate early unwanted dots
//	pos_out[id].w = aaa.dt;
//	pos_out[id].w = ds.v[3];
	pos_out[id].w = 1.;

/*
	float	v;
	v = read_imagef( img2, sampler, pos ).x;
	//v = extract_lum( read_imagef( img2, sampler, pos ).xyz );
	//v = clamp( (v-aaa.p06) * aaa.p07, 0., 1. );
	//v = v * v * (3. - 2. * v);
	v = smoothstep( aaa.p06, aaa.p07, v );
*/
	float2 pout;
	pout.x	=	p->pos.x * 1.77;
	pout.y	=	p->pos.y;
//	if( v < aaa.p03 )
	{
		pos_out[id].xy = pout.xy;
		pos_out[id].z = 0;

		if( aaa.color != 0. )
		{
			//color.x = p->pos.x;
			//color.y = p->pos.y;
			//color.z = 0;
			//color.w = 1;
			//col_out[id] = col;
			col_out[id].x = col.x;
			col_out[id].y = col.y;
			col_out[id].z = col.z;
			col_out[id].w = col.w;
		}

		if( aaa.normal != 0. )
		{
			nor_out[id].x = 0;
			nor_out[id].y = 0;
			nor_out[id].z = 1;
			nor_out[id].w = 1;
		}

		if( aaa.attrib != 0. )
		{
			att_out[id].x = 0;
			att_out[id].y = 0;
			att_out[id].z = 0;
			att_out[id].w = 0;
		}
		return;
	}

/*
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
*/

}
