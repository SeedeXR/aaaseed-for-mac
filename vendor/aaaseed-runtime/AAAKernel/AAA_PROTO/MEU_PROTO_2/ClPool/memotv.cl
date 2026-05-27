
__kernel void GalaxyKernel(
						__global Particle*	particles,
						__global float4*	pos_out,
						__global float4*	col_out,
						__global float4*	nor_out,
						__global float4*	att_out,
						const kernel_param	aaa,
						const dataset_param	ds,
///nvidia				__global __read_only image2d_t img,
//nvidia				__global __read_only image2d_t img2
						__read_only image2d_t img
//						,__read_only image2d_t img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle *p = &particles[id];		//	get the particle struct
	//if( aaa.line > .5f )	// if we draw line the ..._out have a double size to handle the 2 points
	//	id *= 2;

	//	float4	mouse = { (aaa.mouse_pos.x-.5f) * 16, (aaa.mouse_pos.y-.5f) * 16, 0, 1 };

	//	avoid nvidia crash when images are not used
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_REPEAT;
	//const sampler_t sampler_lin	= CLK_FILTER_LINEAR | CLK_ADDRESS_REPEAT;
	int2	position;
	position.x = (p->pos.x + .5f) * get_image_width( img );
	position.y = (p->pos.z + p->dum1) * get_image_height( img ) * aaa.p03 ;
	////position.y = (p->pos.z) * get_image_height( img2 );
	float4	color = read_imagef( img, sampler, position );
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
	float dt = fabs( aaa.dt );
	p->pos.z += dt * aaa.p01;
	//p->pos.w = 0 ;
	//float dist = dot(p->pos, p->pos);
	if( p->pos.z > 1 )
	{
		p->pos.z -= 1;
		//p->pos = p->accel * p->accel * p->accel;
		//p->vel = 0;
	}
	p->pos.w = .125 ;	//	make it 8 time bigger

	float4 pos = p->pos;
	//	transform the cube in cone
	float f = 1. + (1-p->pos.z) * aaa.p02;
	pos_out[id].x = p->pos.x * f;
	pos_out[id].y = p->pos.y * f;

	//	pass the two dim with the attrib
	if( aaa.attrib > 0. )
	{
		att_out[id].x = p->dum1+.5f;
		att_out[id].y = p->dum2+.5f;
		att_out[id].z = 0;
		float luma = color.w;
		if( pos.z < .2f )
			att_out[id].w = luma;	// * pos.z * 5.f ;
		else if( pos.z > .95f )
			att_out[id].w = luma;	// * (1-pos.z) * 20.f ;
		else
			att_out[id].w = luma;
	}
/*	if( aaa.color > 0. )
	{
		if( pos.z > .95f )
		{
			color2.w *= (1-pos.z) * 20.f ;
		}
		//color2.y = 0;
		col_out[id] = color2;
	}
*/
//	p->vel *= DAMP;
}

