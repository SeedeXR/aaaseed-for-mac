
__kernel void seine_kernel(	__global	Particle*		particles
						,	__global	float4*			pos_out
						,	__global	float4*			col_out
						,	__global	float4*			nor_out
						,	__global	float4*			att_out
						,	const		kernel_param	aaa
//						,	const dataset_param		ds
//nvidia				__global __read_only image2d_t img,
//nvidia				__global __read_only image2d_t img2
//						,	__read_only image2d_t	img
//						,__read_only image2d_t img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle* p = &particles[id];		//	get the particle struct

	int sub_id = id - aaa.i_offset;
	if( aaa.line != 0. )	// if we draw line the ..._out have a double size to handle the 2 points
		id = sub_id * 2 + aaa.i_offset;


	/*
	if( aaa.line != 0. )
	{
		pos_out[id+1].x = p->pos.x * 8.;
		pos_out[id+1].y = p->pos.y * 4.5;
		pos_out[id+1].z = p->pos.z * 4;
		pos_out[id+1].w = 1.;
	}
	*/
	float4 pos = p->pos;
	float z = pos.z + aaa.dt * ( aaa.p01  * .1 + (p->dum1+.1) * aaa.p02 ) ;

	if	( z < -.5 )		z += 1.;
	else if ( z > .5 )	z -= 1.;
	p->pos.z  = z;

	pos.x *= 8.;
	pos.y *= 1;
	pos.z *= 8.;
	pos.w = 1.;
	//	float4 one = {1,1,1,1};
	pos_out[id] = pos;


	if( aaa.line != 0. )
	{
		pos.y += 1.;
		pos_out[id+1] = pos ;
	}
	if( aaa.normal > 0. )
	{
		nor_out[id] = pos;
	}
	//v = 1-v;

	if( aaa.color > 0. )
	{
		float4 color;
		float v =  1 ;
		pos = p->pos_b;
		color.x = pos.x + .5;
		color.y = pos.y + .5;
		color.z = pos.z + .5;
		color.w = 1;
		col_out[id] = color;
		if( aaa.line != 0. )
		{
			col_out[id+1] = color;
		}
	}
	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };

	if( aaa.normal > 0. )
	{
		nor_out[id] = pos_out[id];
	}

}
