
__kernel void rain_kernel(	__global	Particle*		particles
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
//	float y = pos.y + aaa.dt * ( aaa.p01 + (p->dum1+.1) * aaa.p02 ) * 5. ;

//	if	( y < -.5 )		y += 1.;
//	else if ( y > .5 )	y -= 1.;
//	p->pos.y  = y;

	float MOUSE_FORCE = .1;
	float CENTER_FORCE = 1.;
	float DAMP = .8;
	float2 diff = (aaa.mouse_pos-.5f) * 1.f - pos.xy;
	float invDistSQ = 1.0f / dot(diff, diff);
	diff *= ((float)1.) * invDistSQ;

	p->vel.xy += (p->pos_b.xy - pos.xy ) * CENTER_FORCE * aaa.dt * aaa.p01;

//	float speed2 = dot( p->vel, p->vel );
//	if( speed2 > .0001 )

	p->vel.xy -= diff.xy * p->mass * MOUSE_FORCE * aaa.dt * aaa.p02;

	pos.xy += p->vel.xy;
	p->vel *= DAMP;

	float y = pos.y + aaa.p07 * aaa.dt;
	if( y > 1. )
		y -= 2.;
	else if( y<-1. )
		y += 2.;
	pos.y = y;
	p->pos = pos;

	pos.x *= aaa.p04;
	pos.y *= aaa.p05;
	pos.z *= aaa.p06;
	pos.w = 1.;
	//	float4 one = {1,1,1,1};
	pos_out[id] = pos;


	if( aaa.line != 0. )
	{
		pos.z += 1.;
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

/*
__kernel void updateParticle(__global Particle* particles, __global float2* posBuffer, const float2 mousePos, const float2 dimensions){
	int id = get_global_id(0);
	__global Particle *p = &particles[id];

	float2 diff = mousePos - posBuffer[id];
	float invDistSQ = 1.0f / dot(diff, diff);
	diff *= MOUSE_FORCE * invDistSQ;

	p->vel += (dimensions*0.5f - posBuffer[id]) * CENTER_FORCE - diff* p->mass;

	float speed2 = dot(p->vel, p->vel);
	if(speed2<MIN_SPEED) posBuffer[id] = mousePos + diff * (1.0f + p->mass);

	posBuffer[id] += p->vel;
	p->vel *= DAMP;
}
*/
