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

float2 myrotate( float2 v, float angle )
{
	float c, s;
	s = sincos( angle, &c );
	float2 o;
	o.x = v.x * s + v.y * c;
	o.y = v.x * c - v.y * s;
	return o;
}

__constant sampler_t samplerg	= CLK_FILTER_LINEAR		| CLK_ADDRESS_CLAMP | CLK_NORMALIZED_COORDS_TRUE;

float2 get_gradient(	__read_only				image2d_t		img
						,						float2 			pos
						//,						kernel_param*	aaa
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
	pos.x -= fx;
	a		=	read_imagef( img, samplerg, pos).x;
	pos.x += 2*fx;
	b		=	read_imagef( img, samplerg, pos).x;
	nor.x = a - b;
	pos.x -= fx;

	//	GRAD Y
	pos.y -= fy;
	a		=	read_imagef( img, samplerg, pos ).x;
	pos.y += 2*fy;
	b		=	read_imagef( img, samplerg, pos).x;
	nor.y = a - b;

	return nor;
}

//#define CENTER_FORCE	.005f
#define IMG_FORCE		.5f

__kernel void Pool1Kernel(
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

	// avoid nvidia crash when images are not used
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;
	//const sampler_t sampler_lin	= CLK_FILTER_LINEAR | CLK_ADDRESS_REPEAT;
	float dt = fabs( aaa.dt );

	float4 pos = p->pos;
	float2	uv;
	uv.x = (pos.x * 1. + .5f);
	uv.y = (pos.y * 1. + .5f);
	int2	texcoor;
//was	texcoor.x = (p->pos.x + p->dum2) * get_image_width( img );
//	texcoor = uv * (float2)( get_image_dim( img ) );
//	texcoor.x &= 511;
//was	texcoor.y = (p->pos.z + p->dum1) * get_image_height( img );	//to was * aaa.p05;

	texcoor.x = uv.x * get_image_width( img );
	texcoor.y = uv.y * get_image_height( img );	//to was * aaa.p05;

	float4	color = read_imagef( img, sampler, texcoor );
	float2	grad = get_gradient( img2, uv ) * 1.f;
	float mass = .1;
	grad.y *= 13./8.;
//	p->vel.xy -= (p->pos_b.xy - pos.xy ) * aaa.p05 * CENTER_FORCE ;
/*
	//float mass = p->mass;

	float2 diff = (aaa.mouse_pos-.5f) * 2.f - pos.xy;
	float invDistSQ = 1.0f / dot(diff, diff);
	diff *= ((float)1.) * invDistSQ;
	p->vel.xy -= diff.xy * mass * MOUSE_FORCE;
*/
	p->vel.xy += grad.xy * mass * aaa.p08 * aaa.attrib * 10.f;
	pos.xy += p->vel.xy;
	pos.xy += p->accel.xy * dt * aaa.p07 * .05f;

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
	float co;
	//float dist = dot(p->pos, p->pos);
	//if( false )
	{

		co = pos.x + dt * aaa.p05;
		if( co > .5 )		co -= 1.;
		else if( co < -.5 ) co += 1.;
		pos.x = co;

		co = pos.y + dt * aaa.p06;
		if( co > .5 )		co -= 1.;
		else if( co < -.5 ) co += 1.;
		pos.y = co;

		co = pos.z + dt * aaa.p01;
		if( co > .5 )		co -= 1.;
		else if( co < -.5 ) co += 1.;
		pos.z = co;
	}

	p->pos.xyz = pos.xyz;
	p->vel *= 1-aaa.p09*dt*5.f;

	//	transform the cube in cone
	pos_out[id].xyz = pos.xyz * aaa.v01.xyz;
//	pos_out[id].xy = myrotate( pos_out[id].xy, (z * 6.2432 * z) );
	pos_out[id].w = co;

	//	pass the two dum with the attrib
	if( aaa.attrib > 0. )
	{
		//	we put random to pick rotation
		float the_cos;
		att_out[id].x = sincos( p->dum2*360, &the_cos );
		att_out[id].y = the_cos;

//		att_out[id].x = p->dum1 + .5f;
//		att_out[id].y = p->dum2 + .5f;
		att_out[id].z = p->dum3 + .5f;

		float luma;
//		if( id <= aaa.nb_by_set )
		{
			co += .5;
			if( co < aaa.p02 )		luma = co / aaa.p02 ;
			else if( co > aaa.p03 )	luma = (1.-co) / (1.-aaa.p03);
			else					luma = 1;
		}
//		else
//			luma = 0;
		att_out[id].w = luma * color.w;
	}

	if( aaa.color > 0. )
	{
		col_out[id] = color;
	}
}


