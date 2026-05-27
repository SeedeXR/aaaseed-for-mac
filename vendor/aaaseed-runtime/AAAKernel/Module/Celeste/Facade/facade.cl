
#define DAMP			0.95f
#define DIST_MIN		.001f
#define DIST_MAX		.02f
#define DIST_MAX2		DIST_MAX*DIST_MAX
#define FORCE_MAX		500.f
#define CENTER_FORCE	.1f
#define MOUSE_FORCE		1.0f
#define MIN_SPEED		0.01f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED


#define	SIZE_X			8.f
#define	SIZE_X_HALF		(SIZE_X*.5)
#define	SIZE_X_INV		.125f

#define	SIZE_Y			(SIZE_X * .69f)
#define	SIZE_Y_HALF		(SIZE_Y*.5)
#define	SIZE_Y_INV		(SIZE_X_INV * 1.45f)

#define	BOUNCE_FACTOR	.5f



#define BALCON_SX	.95f
#define BALCON_SXH	( BALCON_SX * .5 )
#define BALCON_XL	-1.175f
#define BALCON_XLL	( BALCON_XL - BALCON_SXH )
#define BALCON_XLR	( BALCON_XL + BALCON_SXH )

#define BALCON_XR	1.225f
#define BALCON_XRL	( BALCON_XR - BALCON_SXH )
#define BALCON_XRR	( BALCON_XR + BALCON_SXH )

#define BALCON_XC	( ( BALCON_XL + BALCON_XR ) * .5f )
#define BALCON_XCL	( BALCON_XC - BALCON_SXH )
#define BALCON_XCR	( BALCON_XC + BALCON_SXH )

#define BALCON_YT	-.8f
#define BALCON_YB	-1.f

float extract_lum( float4 color )
{	//check the formula and check the kinect code
	return (color.x + color.y + color.z) * .33333333;
}

float2	get_gradient(	__read_only image2d_t	img,
						float2					pos,
						float					du,
						float					dv,
						float					f
						)
{
	float2	img_size;	//	we need it later
	img_size.x =  get_image_width( img );
	img_size.y =  get_image_height( img );

	pos.x = pos.x * SIZE_X_INV + .5f;
	pos.y = pos.y * SIZE_Y_INV + .5f;
	pos *= img_size;

	//const sampler_t sampler	= CLK_FILTER_NEAREST	| CLK_ADDRESS_CLAMP_TO_EDGE;
	const	sampler_t	sampler	= CLK_FILTER_LINEAR	| CLK_ADDRESS_CLAMP;
			float4		color	= read_imagef( img, sampler, pos );

	float	v = extract_lum( color );

	float2	grad;
	float	vb;

	pos.x += du;

	float4 pixel = read_imagef( img, sampler, pos );
	vb = extract_lum(pixel);
	grad.x = vb - v;

	pos.x -= du;
	pos.y += dv;

	pixel = read_imagef( img, sampler, pos );
	vb = extract_lum(pixel);
	grad.y = vb - v;

	grad.xy *= v * f;

	return grad;
}

bool	inside( float v, float min, float max )
{
	if( min <= v && v <= max )
		return true;
	return false;
}

__kernel void facade1(
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
	if( aaa.line != 0. )	// if we draw line the ..._out have a double size to handle the 2 points
		id = sub_id * 2 + aaa.i_offset;

	//	get pixel color

	//	elininate early unwanted dots
	pos_out[id].w = 1;
/*
	if( v < aaa.p04 )
	{
		pos_out[id].x = -400;
		pos_out[id].y = -4;
		pos_out[id].y = -4;
		return;
	}
*/
	float2 grad = get_gradient( img, p->pos.xy, aaa.p01, aaa.p02, aaa.p05 );
	float2 grad_b = get_gradient( img2, p->pos.xy, aaa.p01, aaa.p02, aaa.p05 );

//	MOUSE REPULSION
	if( aaa.p08 != 0. )
	{
		float2 mvec = aaa.mouse_pos - p->pos.xy;

		float factor = dot( mvec, mvec );
		float speed2;
		if( factor < DIST_MAX2 )
		{
			factor = MOUSE_FORCE / factor - MOUSE_FORCE / DIST_MAX2;
			//	factor = pow( factor, 2.5f );
			factor = fmin( factor, FORCE_MAX );
			float2 rep = mvec * factor;	// * p->mass;

			p->vel.xy += rep * aaa.dt;

			speed2 = dot(p->vel, p->vel);
		}
		else
			speed2 = 0;
	}
//	END MOUSE REPULSION


//	nor.z = 1.;
//	nor.w = 0.;
 //	float4 n = fast_normalize( nor );
	grad.xy *= aaa.dt;
	p->vel.xy +=  grad.xy * aaa.p06;

	grad_b.xy *= aaa.dt;
	p->vel.xy +=  grad_b.xy * aaa.p07;

	p->vel *= aaa.p09;

	float2	delta =  p->vel.xy * aaa.dt * aaa.p10;
	float2	pos = p->pos.xy + delta;

	pos.y += aaa.p04 * aaa.dt;

//
//	KEEPING IT IN THE BOX
//
#if 1
	if( pos.x < -SIZE_X_HALF )
	{
		pos.x = -SIZE_X_HALF - SIZE_X_HALF - pos.x;
		p->vel.x = -p->vel.x * BOUNCE_FACTOR;
		p->vel.y *= BOUNCE_FACTOR;
	}
	else if( pos.x > SIZE_X_HALF )
	{
		pos.x = SIZE_X_HALF + (SIZE_X_HALF - pos.x);
		p->vel.x = -p->vel.x * BOUNCE_FACTOR;
		p->vel.y *= BOUNCE_FACTOR;
	}
#else
	if( pos.x < -SIZE_X_HALF )
		pos.x += SIZE_X_HALF * 2. ;
	else if( pos.x > SIZE_X_HALF )
		pos.x -= SIZE_X_HALF * 2. ;
#endif

#if 0
	if( pos.y < -SIZE_Y_HALF )
	{
		pos.y = -SIZE_Y_HALF - SIZE_Y_HALF - pos.y;
		p->vel.y = -p->vel.y * BOUNCE_FACTOR;
		p->vel.x *= BOUNCE_FACTOR;
	}
	else if( pos.y > SIZE_Y_HALF )
	{
		pos.y = SIZE_Y_HALF + (SIZE_Y_HALF - pos.y);
		p->vel.y = -p->vel.y * BOUNCE_FACTOR;
		p->vel.x *= BOUNCE_FACTOR;
	}
#else
	if( pos.y < -SIZE_Y_HALF )
		pos.y += SIZE_Y_HALF * 2. ;
	else if( pos.y > SIZE_Y_HALF )
		pos.y -= SIZE_Y_HALF * 2. ;
#endif


	p->pos.xy = pos.xy;

	float d = p->dum1;
	if( inside( pos.y, BALCON_YB, BALCON_YT ) )
	{
		if( d == 0. )
		{
			if( inside( pos.x, BALCON_XLL, BALCON_XLR ) )
			{
				p->dum1 = 1.;
				d = 1.;
			}
			else if( inside( pos.x, BALCON_XRL, BALCON_XRR ) )
			{
				p->dum1 = -1;
				d = -1;
			}
		}
		else if( inside( pos.x, BALCON_XCL, BALCON_XCR ) )
		{
			p->dum1 = 0.;
			d = 0.;
		}
	}

//
//	SETTING OUT
//
	//	float4 one = {1,1,1,1};
	float2 pout;
	pout.xy			= pos.xy;
//	pout.xy			+= grad.xy * aaa.p08;
	pos_out[id].z	= 0;	//	v*aaa.p03;
	pos_out[id].xy	= pout.xy;

	//v = 1-v;
	if( aaa.color > 0. )
	{
		float4 color;
		if( d == 0. )
		{
			color.x = 1.;
			color.y = 1.;
			color.z = 1.;
		}
		else if( d > 0. )
		{
			color.x = 1.;
			color.y = 0.;
			color.z = 1.;
		}
		else
		{
			color.x = 0.;
			color.y = 1.;
			color.z = 1.;
		}
		color.w = 1;
		col_out[id] = color;
	}

	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };

/*
	if( aaa.normal > 0. )
	{
		nor_out[id] = n;
	}
	//p->vel *= DAMP;
*/
}


