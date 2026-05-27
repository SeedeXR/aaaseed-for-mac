
__kernel void TextFlowKernel(
						__global Particle*	particles,
						__global float4*	pos_out,
						__global float4*	col_out,
						__global float4*	nor_out,
						__global float4*	att_out,
						const kernel_param	aaa,
						const dataset_param	ds
///nvidia				__global __read_only image2d_t img,
//nvidia				__global __read_only image2d_t img2
//						__read_only image2d_t img
//						,__read_only image2d_t img2
)
{
	int id = get_global_id(0);					//	get the Particle index
	__global Particle* p = &particles[id];		//	get the particle struct
	//if( aaa.line > .5f )	// if we draw line the ..._out have a double size to handle the 2 points
	//	id *= 2;

	//	float4	mouse = { (aaa.mouse_pos.x-.5f) * 16, (aaa.mouse_pos.y-.5f) * 16, 0, 1 };

	//	avoid nvidia crash when images are not used
	//const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP_TO_EDGE;
	//const sampler_t sampler_lin	= CLK_FILTER_LINEAR | CLK_ADDRESS_REPEAT;
	float3	position;
/*
	//position.x = (p->pos.x + p->dum2) * get_image_width( img );
	//position.x &= 511;
	//position.y = (p->pos.z + p->dum1) * get_image_height( img ) * aaa.p03;
	//position.y &= 511;
	////position.y = (p->pos.z) * get_image_height( img2 );
	//float4	color = read_imagef( img, sampler, position );
	//float4	color2 = read_imagef( img2, sampler	, position );
*/
	float f = 0;
	if( aaa.p01 < -3. )
	{
		p->pos.xy = p->pos_b.xy;
		p->pos.z = 0.;
		p->vel.xyz = 0.f;
		p->life = 0.;
	}
	else
	{
		float yb = p->pos_b.y;

		if( yb > 3. )
			f = -1;
		else
		{
			float x = p->pos_b.x;
			//deal with lines
			//if( yb > .0 )
			{
				f = floor( -(yb + .1) / 10. );
				f = f /	 5.5;
				//f =	 -1.;
			}
//			else if( yb > -.7 )
//				f = 1.;
			//else
			//	f = 1;
			f = 2. + aaa.p01 + f - (-x+6.) * (1./60.);	//		 + p->dum1 * .65 ;
			if( f > 0. )
			{
				if( false )
				{
					float2 v2;
					v2.x = p->dum1;	// - p->pos.x * .1;
					v2.y = p->dum2;	// - p->pos.y * .1;
					v2 = p->vel.xy + v2 * aaa.p02 * aaa.dt;
					p->vel.xy = v2;

					float2 pos = p->pos.xy;
					pos += v2 * aaa.dt * 10.;
					p->pos.xy = pos;
				}
				else
				{
					float3 v3;
					if( true )
					{
						v3 = (float3)( 0, 0, 0 );
						if( p->life < -.0001f )
						//if( false )
						{
							v3.xy = p->pos_c.xy * 1.;
							v3.z = -(p->pos_c.z+5) * 3;
							//v3 *= p->accel.w;
							//v3.y += p->accel.w;
							//v3 = (float3)( 0, 0, 0 );
						}
						else
						{
							v3.xyz = (p->accel.xyz - .35 ) * 1.;
							//v3 = (p->accel.xyz - .2) * -2.;
							//v3.z = (p->accel.z ) * 1.;
							v3.z = 	p->accel.z * 2.;
							//v3.z = (p->accel.z-.0)* 2.;
							//v3.y = (p->accel.y-.0)* 2.;
							//v3.x = 0.;
							//v3.z = 0.;
							//v3.x = (p->accel.x-.25)* 1.;
							//v3.x = p->pos.x * p->pos_b.x * ( .5 - p->life ) * .2;
							//v3.y = (p->accel.y-.25)* 1.;
							v3 *= (.3 + p->accel.w) * 15.;
							//v3.y += p->accel.w * (1. + 1. * p->mass);
						}
					}
					else
					{
						v3 = (float3)( 0, 0, 0 );
						v3.y = p->accel.w * .5;
					}
					//if( p->mass < .001 )
					//	v3 = (float3)( 0, 0, 0 );
					//else
						v3 *= 4 + 1. * p->mass;
					v3 *= aaa.p02 * aaa.p02 * aaa.dt;
					v3 += p->vel.xyz;
					p->vel.xyz = v3;
					//v3.y += 2.;
					//v3.y += 6.;

					float3 pos = p->pos.xyz;
					pos += v3 * aaa.dt * 10.;
					p->pos.xyz = pos;
				}
				p->life += aaa.dt * p->life_over_one;
			}
			p->vel.xyz *= .5f	;
		}
	}

	position.xyz = p->pos.xyz;
//	p->pos.w = 1. ;	//	make it 8 time bigger

//	float4 pos = p->pos;
	//	transform the cube in cone
//	float f = 1. + (1-p->pos.z) * aaa.p02;
//	f = 1;
	pos_out[id].x = position.x;
	pos_out[id].y = position.y;
	pos_out[id].z = position.z;
	pos_out[id].w = 1.;

	//	pass the two dum with the attrib
	if( aaa.attrib > 0. )
	{
		//	we put random to pick texture
		att_out[id].x = p->dum1 + .5f;
		att_out[id].y = p->dum2 + .5f;
		att_out[id].z = id;
/*
		float luma = 1.;	//color.x;
		if( pos.z < .05f )
			att_out[id].w = luma * pos.z * 20.f ;
		else if( pos.z > .95f )
			att_out[id].w = luma * (1-pos.z) * 20.f ;
		else
			att_out[id].w = luma;
*/
		//	positif exponent > 2 crash BlackBlood
		//att_out[id].w = pow( p->life, 5. ) / (1.+ p->dum2);
		att_out[id].w = p->life / (1.+ p->dum2);
	}

	if( aaa.color > 0. )
	{
		if( f > 0. )
			col_out[id] = (float4)( 1., 1., 1., 1. );
		else
			col_out[id] = (float4)( 1., 1., 1., 1. );
	}

//	p->vel *= DAMP;
}


