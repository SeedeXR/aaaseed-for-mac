	/*
o_str o_include_particle_full(
"	typedef struct{" LF
"	float4 pos;" LF
"	float4 vel;" LF
"	float4 pos_b;" LF
"	float dum1;		float dum2;		float dum3;		float dum4;" LF
"	float4 accel;" LF
"	float4 pos_c;" LF
"	float mass;" LF
"	float radius;" LF
"	float life;" LF
"	float life_over_one;" LF
"	} Particle;\n"
);
*/

#define 	RX		(14./9.)
__constant 	float	ry = 9./14.;
__constant	float2	f2_grad = { 1., RX };

float2 myrotate( float2 v, float angle )
{
	float c, s;
	s = sincos( angle, &c );
	float2 o;
	o.x = v.x * s + v.y * c;
	o.y = v.x * c - v.y * s;
	return o;
}

__constant sampler_t sampler_normalized	= CLK_FILTER_LINEAR	 | CLK_ADDRESS_MIRRORED_REPEAT  | CLK_NORMALIZED_COORDS_TRUE;
// avoid nvidia crash when images are not used
__constant sampler_t sampler			= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;
//const sampler_t sampler_lin			= CLK_FILTER_LINEAR | CLK_ADDRESS_REPEAT;


float2 get_gradient(	__read_only	image2d_t		img
						,			float2 			posi
					)
{
	float2	nor;
	float	a;
	float	b;

	int2 size = get_image_dim( img );
//	float fx = aaa->p01/size.x;
//	float fy = aaa->p02/size.y;

	float fx = 1./size.x;
	float fy = 1./size.y;

	//	GRAD X
	posi.x -= fx;
	a		=	read_imagef( img, sampler_normalized, posi ).x;
	posi.x += 2*fx;
	b		=	read_imagef( img, sampler_normalized, posi ).x;
	nor.x = a - b;
	posi.x -= fx;

	//	GRAD Y
	posi.y -= fy;
	a		=	read_imagef( img, sampler_normalized, posi ).x;
	posi.y += 2*fy;
	b		=	read_imagef( img, sampler_normalized, posi ).x;
	nor.y = a - b;

	return nor;
}

static float noise3D( float x, float y, float z ) {
    float ptr = 0.0f;
    return fract(sin(x*112.9898f + y*179.233f + z*237.212f) * 43758.5453f, &ptr);
}
//#define CENTER_FORCE	.005f
#define IMG_FORCE		.5f
#define MOUSE_FORCE		-.05f

__kernel void monart_back_kernel(
						__global Particle*	particles,
						__global float4*	pos_out,
						__global float4*	col_out,
						__global float4*	nor_out,
						__global float4*	att_out,
						const kernel_param	aaa,
						const dataset_param	ds
//nvidia				,__global __read_only image2d_t img
//nvidia				,__global __read_only image2d_t img2
						,__read_only image2d_t img
						,__read_only image2d_t img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle *p = &particles[id];		//	get the particle struct

	// if we draw line the ..._out have a double size to handle the 2 points
	//	if( aaa.line > .5f ) 	id *= 2;

	//	float4	mouse = { (aaa.mouse_pos.x-.5f) * 16, (aaa.mouse_pos.y-.5f) * 16, 0, 1 };
	float dt = fabs( aaa.dt );

//we get the pos
	float4 pos = p->pos;
	p->vel *= 1 - aaa.p09 * dt * 50.f;	//damp

//we make uv from it
	float2	uv = pos.xy + .5f;
	float2	img_size = convert_float2( get_image_dim(img) );
	int2	texcoor = convert_int2( uv * img_size );
//was	texcoor.x = (p->pos.x + p->dum2) * get_image_width( img );
//	texcoor = uv * (float2)( get_image_dim( img ) );
//	texcoor.x &= 511;
//was	texcoor.y = (p->pos.z + p->dum1) * get_image_height( img );	//to was * aaa.p05;

//get texture
	float4	color2	= read_imagef(	img2,	sampler_normalized, uv );
	if( .0< uv.y && uv.y < 1. )
	{
		//if( pos.x > 0 )
		//	pos.x  = - pos.x;
		float2 grad;
		grad = get_gradient(	img2,	uv );	// * f2_grad;
		//grad = read_imagef(	img2,	sampler, uv ).xy;
		//grad = (float2)(.5-sqrt(pos.x*pos.x + pos.y*pos.y)) * pos.xy;
		//p->vel.x += grad.x * (float)(aaa.p08 * (uv.y - 1.) * dt * 200.f) ;
		p->vel.x += grad.x * (float)(aaa.p08 * clamp((1.-uv.y)*3.,.0,.1) * dt * 200.f) ;
		pos.xy += p->vel.xy * dt;
		//float2 vel = grad.xy * aaa.p08 * dt * 100.f;
		//pos.xy += vel * dt;
	}

//	float mass = .1;

/*	USED IN POOL
	float o_luma = 0;
	if( color2.x >=.49 )
	{
		float rad = p->radius;
		if( rad < 1. )
		{
			p->radius = rad  + dt * .1;
		}
	}
	else if( color2.x < .45 )
	{
		o_luma = 12.;
		float rad = p->radius;
		if( rad > -1 )
		{
			p->radius = rad - dt * 2.;
		}
	}
*/
//	p->vel.xy -= (p->pos_b.xy - pos.xy ) * aaa.p05 * CENTER_FORCE ;

/*
	float mass = 1.;	//p->mass;

	float2 diff = (aaa.mouse_pos-.5f) * 2.f - pos.xy;
	float invDistSQ = 1.0f / dot(diff, diff);
	diff *= ((float)1.) * invDistSQ;
	p->vel.xy -= diff.xy * mass * MOUSE_FORCE;
*/
	//todo mass

	//pos.xy += p->vel.xy * dt;
//	pos.z = 0;

//	pos.x = p->dum1;
//	pos.y = p->dum2;

	pos.y += dt * aaa.p06;
	pos.xy -= p->accel.xy * (float)(dt * aaa.p07 * 2.);

//	p->vel *= aaa.p08;

	//color *= color2;
	//p->vel = p->accel;
	//p->vel.x = 0. ;
	//p->vel.y = 0. ;
	//p->vel.z = 1. ;
	//p->vel.w = 0. ;
	//p->vel = fast_normalize( p->vel );
	//p->vel = aaa.v01;
	//float speed2 = dot(p->vel, p->vel);

	//if( aaa.line > .5 )
	//	pos_out[id+1] = pos_out[id];
	//float dist = dot(p->pos, p->pos);
// constraint circulation in a box
//

//	if( pos.x > .5 )		pos.x -= 1.;
//	else if( pos.x < -.5 )	pos.x += 1.;
	float4	color;

	if( pos.y > .5 )		{	pos.y -= 1.; }
	else if( pos.y < -.5 )
	{
		pos.y += 1.;
		pos.x = (p->dum1 - .5)  * .4;
		//pos.x = p->dum3 + p->dum2 + p->dum1;
		//if( pos.x > 2.)			pos.x -= 2.;
		//else if( pos.x > 1.)	pos.x -= 1.;
		//pos.x -= .5;
		//pos.x = noise3D( p->pos_b.x,  p->pos_b.y,  p->pos_b.z ) - .5;
		p->vel.xy = 0;
		//p->accel.xy = 0;
		float4 color = read_imagef(	img, sampler, (float2)( img_size.x * ( pos.x + .5), .5 * img_size.y ) );
		//color.xyz = hsv_to_rgbf3( color.xyz );
		color.xyz = rgb_to_hsvf3( color.xyz );
		color.x = color.x * 2. + .5;
//		color.y *= .25;
		color.y = pow( color.y, 2.f );
	//	color.z *= 0.;
		color.xyz = hsv_to_rgbf3( color.xyz );
		p->pos_c = color;
		p->pos_c.w = 1.;
	}
	color = p->pos_c;
	//barrier( CLK_GLOBAL_MEM_FENCE );
	//barrier( CLK_LOCAL_MEM_FENCE );

	if( pos.z > .5 )		pos.z -= 1.;
	else if( pos.z < -.5 )	pos.z += 1.;


//	co = pos.z + dt * aaa.p01;
	p->pos.xyz = pos.xyz;
	pos.z =  0.;
	float co;
	float ang  = pos.x * 15.71;
	float si = sincos( ang, &co );
	pos.xz = (float2)( co, si);

//	transform the canonical cube
	pos_out[id].xyz = pos.xyz * aaa.v01.xyz;
	pos_out[id].w = pos.z;

	//	pass the two dum with the attrib
	if( aaa.attrib > 0. )
	{
		//	we put random to pick rotation
		//float the_cos;
		//att_out[id].x = sincos( p->dum2*360, &the_cos );
		//att_out[id].y = the_cos;

		att_out[id].x = 0;
		att_out[id].y = 1;


//		att_out[id].x = p->dum1 + .5f;
//		att_out[id].y = p->dum2 + .5f;
		att_out[id].z = p->dum3 + .5f;

		float luma;
//		if( id <= aaa.nb_by_set )
		{
			float co = pos.z + .5;
			if( co < aaa.p02 )		luma = co / aaa.p02 ;
			else if( co > aaa.p03 )	luma = (1.-co) / (1.-aaa.p03);
			else					luma = 1;
			luma = 1;
		}
//		else
//			luma = 0;
//	USED IN POOL
//		att_out[id].w = luma * color.w * p->radius + o_luma;
		att_out[id].w = luma * color.w;
		att_out[id].w = 1.;
	}
	p->pos.z += rgb_to_luma( color.xyz );

	if( aaa.color > 0. )
	{
		col_out[id] = 	color;
	}
}


