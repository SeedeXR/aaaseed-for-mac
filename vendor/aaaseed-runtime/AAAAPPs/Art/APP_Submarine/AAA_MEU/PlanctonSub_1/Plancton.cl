float2 myrotate( float2 v, float angle )
{
	float c, s;
	s = sincos( angle, &c );
	float2 o;
	o.x = v.x * s + v.y * c;
	o.y = v.x * c - v.y * s;
	return o;
}

__kernel void PlanctonKernel(
						__global Particle*	particles,
						__global float4*	pos_out,
						__global float4*	col_out,
						__global float4*	nor_out,
						__global float4*	att_out,
						const kernel_param	aaa,
						const dataset_param	ds
//nvidia				__global __read_only image2d_t img,
//nvidia				__global __read_only image2d_t img2
//mona					,	__read_only image2d_t img
//						,__read_only image2d_t img2
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
	int2	position;
//was	position.x = (p->pos.x + p->dum2) * get_image_width( img );
	//mona	position.x = ( p->pos.x * 1 + .5 ) * get_image_width( img );
//	position.x &= 511;
//was	position.y = (p->pos.z + p->dum1) * get_image_height( img );	//to was * aaa.p05;
	//mona	position.y = ( p->pos.y * 1 + .5 ) * get_image_height( img );	//to was * aaa.p05;
//	position.y &= 511;
	////position.y = (p->pos.z) * get_image_height( img2 );
	//use for the blinking
	//mona	float4	color = read_imagef( img, sampler, position );
	//float4	color2 = read_imagef( img2, sampler	, position );

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
	float z;
	//float dist = dot(p->pos, p->pos);
	{
		float dt = fabs( aaa.dt );
		z = p->pos.x + dt * aaa.p05 * 10.;
		if( z > .5 )		z -= 1.;
		else if( z < -.5 ) 	z += 1.;
		p->pos.x = z;

		z = p->pos.y + dt * aaa.p06;
		if( z > .5 )		z -= 1.;
		else if( z < -.5 ) 	z += 1.;
		p->pos.y = z;

		z = p->pos.z + dt * aaa.p01;
		if( z > 1. )		z -= 1.;
		else if( z < 0. ) 	z += 1.;
		p->pos.z = z;
	}

	//	transform the cube in cone
	pos_out[id].xy = p->pos.xy * (aaa.v01.xy * (1-z) + aaa.v02.xy * z);
//	pos_out[id].xy = myrotate( pos_out[id].xy, (z * 6.2432 * z) );
	pos_out[id].z = aaa.v01.z * (1-z) + aaa.v02.z * z;
	pos_out[id].xyz += aaa.v03.xyz;
	pos_out[id].w = z;

	//	pass the two dum with the attrib
	if( aaa.attrib > 0. )
	{
		//	we put random to pick texture
		//att_out[id].x = p->dum1 + .5f;
		//att_out[id].y = p->dum2 + .5f;
		//	att_out[id].z = id;

		float luma;
		if( id <= aaa.nb_by_set )
		{
			//mona	luma = color.x;
			luma = 1;	//mona
			if( z < aaa.p02 )		luma *= z / aaa.p02 ;
			else if( z > aaa.p03 )	luma *= (1.-z) / (1.-aaa.p03);
		}
		else
		{
			luma = 0;
		}
		att_out[id].w = luma;
	}

//mona		if( aaa.color > 0. )
	{
//mona			col_out[id] = //mona	color;
	}
}


