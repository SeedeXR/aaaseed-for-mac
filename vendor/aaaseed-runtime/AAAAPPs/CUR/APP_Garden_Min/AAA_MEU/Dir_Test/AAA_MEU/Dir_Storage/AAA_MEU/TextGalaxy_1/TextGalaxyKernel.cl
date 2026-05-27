
__kernel void TextGalaxyKernel(
						__global Particle*	particles,
						__global float4*	pos_out,
						__global float4*	col_out,
						__global float4*	nor_out,
						__global float4*	att_out,
						const kernel_param	aaa,
						const dataset_param	ds
//nvidia				__global __read_only image2d_t img,
//nvidia				__global __read_only image2d_t img2
//						__read_only image2d_t img
//						,__read_only image2d_t img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle* p = &particles[id];		//	get the particle struct

	// if we draw line the ..._out have a double size to handle the 2 points
	//	if( aaa.line > .5f ) 	id *= 2;

	//	float4	mouse = { (aaa.mouse_pos.x-.5f) * 16, (aaa.mouse_pos.y-.5f) * 16, 0, 1 };

/*
	// avoid nvidia crash when images are not used
	//const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;
	//const sampler_t sampler_lin	= CLK_FILTER_LINEAR | CLK_ADDRESS_REPEAT;
	float2	position;
	position.x = (p->pos.x + p->dum2) * get_image_width( img );
	position.x &= 511;
	position.y = (p->pos.z + p->dum1) * get_image_height( img ) * aaa.p03;
	position.y &= 511;
	//position.y = (p->pos.z) * get_image_height( img2 );
	float4	color = read_imagef( img, sampler, position );
	//float4	color2 = read_imagef( img2, sampler	, position );
*/
	float z;
	if( aaa.p01 < -2. )
	{
		//p->pos.x = ( p->dum1 - .5 ) * 8.;
		//p->pos.y = ( p->dum2 - .5 ) * 8.;
		//p->pos.z = 0;
		p->pos.xyz = p->accel.xyz;
		p->pos_b.z = 0;
		p->vel = 0.;

		z = p->pos.z;
	}
	else
	{
		float dt = aaa.p05;
		float3 target;
		if( aaa.p01 < 2.)
		{
			target = p->pos_b.xyz;
			target.z = 1.;
		}
		else
			target = p->accel.xyz;

		float3 accel = target - p->pos.xyz;
		float len = length( accel );
		float3 nor = accel / pow( len, .5 );
		//p->accel.xyz = accel;
		float3 vel = p->vel.xyz + nor * dt;
		vel *= .5;

		if( aaa.p01 > 0. && aaa.p01 < 2.)
		{
			p->vel.xyz = vel;
			p->pos.xyz += vel * dt;
		}
		else
			len = 1;
		float z = p->pos.z + dt * aaa.p01;
		//float dist = dot(p->pos, p->pos);
		if( z > 1. )		z -= 1.;
		else if( z < 0. ) 	z += 1.;
		p->pos.z = z;
	}


	//	p->pos.w = 1. ;	//	make it 8 time bigger

	float4 pos = p->pos;
	//	transform the cube in cone
	float f = aaa.p02 * aaa.p02;
	f += (1-f) * z;
	pos_out[id].x = pos.x * f;
	pos_out[id].y = pos.y * f;
	pos_out[id].z = z;
	pos_out[id].w = 1.;

	//	pass the two dum with the attrib
	if( aaa.attrib > 0. )
	{
		//	we put random to pick texture
		att_out[id].x = p->dum1 + .5f;
		att_out[id].y = p->dum2 + .5f;
		att_out[id].z = 0;

		float luma = 1;	//color.x;
		if( z < aaa.p02 )		luma *= z / aaa.p02 ;
		else if( z > aaa.p03 )	luma *= (1.-z) / (1.-aaa.p03) ;
		att_out[id].w = luma;
	}

	if( aaa.color > 0. )
	{
		col_out[id] = (float4)( 1, 1, 1, 1 );
	}
}


