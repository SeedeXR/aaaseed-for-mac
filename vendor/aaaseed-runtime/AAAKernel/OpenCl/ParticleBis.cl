#define DAMP			0.95f
#define CENTER_FORCE	.1f
#define MOUSE_FORCE		3.0f
#define MIN_SPEED		0.001f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED
#define DT				0.02f

typedef struct{
	float4 pos, vel, accel;
	float mass;
	float radius;
	float dum1;
	float dum2;
	// need this to make sure the float2 vel is aligned to a 16 byte boundary
} Particle;

typedef struct{
	float	line;
	float	color;
	float	normal;
	float	attrib;
	float2	mouse_pos;
	float	dt;
	float	reset;
	float	p01;
	float	p02;
	float	p03;
	float	p04;
	float	p05;
	float	p06;
	float	p07;
	float	p08;
//	float	p09;
//	float	p10;
	float4	v01;
	float4	v02;
} kernel_param;

__kernel void KinectNoImage(			__global Particle*		particles,
								__global float4*		posBuffer,
								__global float4*		colorBuffer,
								__global float4*		normalBuffer,
								const kernel_param 		aaa,
								__read_only	image2d_t img,
								__read_only	image2d_t img2
								)
{
	int id = get_global_id(0);
	__global Particle *p = &particles[id];

	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
	const sampler_t sampler2	= CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
	float2	position;
	position.x = p->pos.x * 1. + .5f;
	position.y = p->pos.y * 1. + .5f;
	
	float2	pos;
	pos.x = position.x * get_image_width( img );
	pos.y = position.y * get_image_height( img );

	float4	color = read_imagef( img, sampler, pos );
	float4	color2 = read_imagef( img, sampler2, pos );

	float4 one = {1,1,1,1};
	float4 z_one = {0,0,1,1};
	float col = 1;
	col = col * (1.20-.4);
	posBuffer[id].z = col; //(color.x + color.y + color.z)*.24;
	posBuffer[id].x = (p->pos.x) * (1.20-col) * -1.33 ;
	posBuffer[id].y = (p->pos.y) * (1.20-col)			;
	posBuffer[id].w = 1;
	//posBuffer[id] = p->accel;

	colorBuffer[id] = one;
	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };
	
	normalBuffer[id] = z_one;
	//p->vel *= DAMP;
}

__kernel void Height(			__global Particle*		particles,
								__global float4*		posBuffer,
								__global float4*		colorBuffer,
								__global float4*		normalBuffer,
								const kernel_param 		aaa,
								__read_only	image2d_t img,
								__read_only	image2d_t img2
								)
{
	int id = get_global_id(0);
	__global Particle *p = &particles[id];
	float4	mouse = { (aaa.mouse_pos.x-.5f) * 16, (aaa.mouse_pos.y-.5f) * 16, 0, 1 };
	
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
	const sampler_t sampler2	= CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
	
	float2	position;
	position.x = p->pos.x * 1. + .5f;
	position.y = p->pos.y * 1. + .5f;
	
	float2	pos;
	pos.x = position.x * get_image_width( img );
	pos.y = position.y * get_image_height( img );
	
	float4	color = read_imagef( img, sampler2, pos );
	float	col = (color.x + color.y + color.z) * .3333333 ;
	//float	col = color.y	;
	
	//get_gradient
	pos.x += aaa.p01;
	float4	color_x = read_imagef( img, sampler2, pos );
	float	col_x = (color.x + color.y + color.z) * .3333333 ;
	//float	col_x = color_x.y;
	
	pos.x -= aaa.p01;
	pos.y += aaa.p02;
	float4	color_y = read_imagef( img, sampler2, pos );
	float	col_y = (color.x + color.y + color.z) * .3333333 ;
	//float	col_y = color_y.y;
	
	float4 nor = { 0., 0., 1., 0. };
	//float4	nor = ( color.z - color_y.z, color.z - color_x.z, 1, 1);
	//float4	nor = ( 0, 0, 1, 0);
	nor.x = (col_x - col)*get_image_width( img )/aaa.p01;
	nor.y = (col - col_y)*get_image_height( img)/aaa.p02;
	float4 n = fast_normalize( nor );

/*
	if( aaa.line > .5 )
		id *= 2;
	float4 diff = mouse - posBuffer[id];  
	float invDistSQ = 1.0f / dot(diff, diff);
	//invDistSQ = pow( invDistSQ, 2.5f );
	float4 rep = diff * MOUSE_FORCE * invDistSQ * p->mass;
	
	p->vel += (aaa.v01 - posBuffer[id]) * CENTER_FORCE - rep;
	
	float speed2 = dot(p->vel, p->vel);
	if( speed2 < MIN_SPEED2 )
	{
		posBuffer[id] = mouse + p->pos * 5.;
		//posBuffer[id] = mouse + diff * (1 + p->mass);
		if( aaa.line > .5 )
			posBuffer[id+1] = posBuffer[id];
	}
	else
	{
		if( aaa.line > .5 )
			posBuffer[id+1] = posBuffer[id];
		posBuffer[id] += p->vel * DT;
	}
*/
//	posBuffer[id].z = -(color.x + color.y + color.z);
	float4 one = {1,1,1,1};
	//col = col * (1.20-.4);
	posBuffer[id].z = col * aaa.p03;//+ n.z*aaa.p03; //(color.x + color.y + color.z)*.24;
	posBuffer[id].x = p->pos.x * -1.33 ;
	posBuffer[id].y = p->pos.y 			;
	posBuffer[id].w = 1;
	//posBuffer[id] = p->accel;

	color.w = 1;
	colorBuffer[id] = color;
	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };
	
	normalBuffer[id] = nor;
	//p->vel *= DAMP;
}

float extract_depht( float4 color )
{	//check the formula and check the kinect code
	return (color.y * 256. + color.z) / 256.;
}
float extract_lum( float4 color )
{	//check the formula and check the kinect code
	return (color.x + color.y + color.z) / 3.;
}

__kernel void Kinect(			__global Particle*		particles,
								__global float4*		posBuffer,
								__global float4*		colorBuffer,
								__global float4*		normalBuffer,
								const kernel_param 		aaa,
								__read_only	image2d_t img,
								__read_only	image2d_t img2
								)
{
	int id = get_global_id(0);
	__global Particle *p = &particles[id];
	float4	mouse = { (aaa.mouse_pos.x-.5f) * 16, (aaa.mouse_pos.y-.5f) * 16, 0, 1 };
	
	const sampler_t sampler		= CLK_FILTER_NEAREST | CLK_ADDRESS_CLAMP;
	const sampler_t sampler2	= CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;
	
	float2	position;
	position.x = p->pos.x * 1. + .5f;
	position.y = p->pos.y * 1. + .5f;
	
	float2	img_size;
	img_size.x = get_image_width( img );
	img_size.y = get_image_height( img );
	
	float2	pos;
	pos = position * img_size;

	float4	color = read_imagef( img, sampler2, pos );
	float	v; 

	if( aaa.p07 > 0 )
	{	//kinect case
		v = extract_depht(color);
		if( v <= 0 )
		{
			posBuffer[id].x = -4;
			posBuffer[id].y = -4;
			posBuffer[id].y = -4;
			return;
		}
	}
	else
		v = extract_lum( color );
	//get_gradient
	pos.x += aaa.p01;
	float4	cx = read_imagef( img, sampler2, pos );
	float vx;
	if( aaa.p07 > 0 )
		vx = extract_depht(cx);
	else
		vx = extract_lum(cx);
	
	pos.x -= aaa.p01;
	pos.y += aaa.p02;
	float4	cy = read_imagef( img, sampler2, pos );
	float	vy ;
	if( aaa.p07 > 0 )
		vy = extract_depht(cy);
	else
		vy = extract_lum(cy);
	//float	col_y = color_y.y;
	
	float4 nor = { 0., 0., 1., 0. };
	//float4	nor = ( color.z - color_y.z, color.z - color_x.z, 1, 1);
	//float4	nor = ( 0, 0, 1, 0);
	nor.x = (vx - v)*img_size.x/aaa.p01;
	nor.y = (v - vy)*img_size.y/aaa.p02;
	float4 n = fast_normalize( nor );

/*
	if( aaa.line > .5 )
		id *= 2;
	float4 diff = mouse - posBuffer[id];  
	float invDistSQ = 1.0f / dot(diff, diff);
	//invDistSQ = pow( invDistSQ, 2.5f );
	float4 rep = diff * MOUSE_FORCE * invDistSQ * p->mass;
	
	p->vel += (aaa.v01 - posBuffer[id]) * CENTER_FORCE - rep;
	
	float speed2 = dot(p->vel, p->vel);
	if( speed2 < MIN_SPEED2 )
	{
		posBuffer[id] = mouse + p->pos * 5.;
		//posBuffer[id] = mouse + diff * (1 + p->mass);
		if( aaa.line > .5 )
			posBuffer[id+1] = posBuffer[id];
	}
	else
	{
		if( aaa.line > .5 )
			posBuffer[id+1] = posBuffer[id];
		posBuffer[id] += p->vel * DT;
	}
*/
//	posBuffer[id].z = -(color.x + color.y + color.z);
	float4 one = {1,1,1,1};
	if( aaa.p07 > 0 )
	{	//kinect case
		if( v <= 0 )
		{
			posBuffer[id].x = -4;
			posBuffer[id].y = -4;
			posBuffer[id].y = -4;
		}
		else
		{
			v = v * (1.20-.4);
			posBuffer[id].x = (p->pos.x + n.x*aaa.p08) * (1.20-v) * -1.33 ;
			posBuffer[id].y = (p->pos.y + n.y*aaa.p08) * (1.20-v)			;
			posBuffer[id].z = v*aaa.p03 + n.z*aaa.p08; //(color.x + color.y + color.z)*.24;
		}
		posBuffer[id].w = 1;
	}
	else
	{	//kinect case
		posBuffer[id].x = (p->pos.x + n.x*aaa.p08) * -1.33 ;
		posBuffer[id].y = (p->pos.y + n.y*aaa.p08);
		posBuffer[id].z = v*aaa.p03 + n.z*aaa.p08; //(color.x + color.y + color.z)*.24;
		posBuffer[id].w = 1;
	}
	
	//posBuffer[id] = p->accel;

	color.w = 1;
	colorBuffer[id] = color;
	//float4 nor = {1-color.z, 1-color.z, color.z, 1 };
	
	normalBuffer[id] = n;
	//p->vel *= DAMP;
}

/*
#define CENTER_X 640.0f
#define CENTER_Y 360.0f

#define DAMP 0.8f
#define MIN_DIST 50
#define MIN_DIST_SQ MIN_DIST*MIN_DIST

#define REPULSION_FORCE 4.0f

typedef struct {
	float x, y;
} Vec2;

typedef struct{
	Vec2 pos, accel, vel;
	float mass;
	float radius;
} Particle;

__kernel void updateParticle(	__global Particle* pIn,
								__global Particle* pOut,
								const int particleCount,
								const float mouseX,
								const float mouseY){
	float dx, dy, distSQ, invDistSQ, dist, forceAmount, distNorm;
	int id = get_global_id(0);
	dx = mouseX - pIn[id].pos.x;
	dy = mouseY - pIn[id].pos.y;
	distSQ = dx*dx+dy*dy;
	if(distSQ < 100*100)
	{
		invDistSQ = 1.0f / distSQ;
		dx *= 300.0f * invDistSQ;
		dy *= 300.0f * invDistSQ;
		//distNorm = 100 * 100 * invDistSQ;
		//dx *= 500 * -distNorm;
		//dy *= 500 * -distNorm;
		pIn[id].accel.x -= dx;
		pIn[id].accel.y -= dy; 
	}
	pIn[id].accel.x += (CENTER_X - pIn[id].pos.x) * 0.01f;
	pIn[id].accel.y += (CENTER_Y - pIn[id].pos.y) * 0.01f;
	for(int i=id+1; i<particleCount; i++)
	{
		dx = pIn[id].pos.x - pIn[i].pos.x;
		dy = pIn[id].pos.y - pIn[i].pos.y;
		distSQ = (dx*dx+dy*dy);
		if(distSQ < MIN_DIST_SQ
		{
			if(distSQ < 1.0f) distSQ = 1.0f;
			invDistSQ = 1.0f / distSQ;
			dx *= REPULSION_FORCE * invDistSQ;
			dy *= REPULSION_FORCE * invDistSQ;
			pIn[id].accel.x += dx;
			pIn[id].accel.y += dy;
			pIn[i].accel.x -= dx;
			pIn[i].accel.y -= dy;
		}
	}
	
	pIn[id].vel.x += pIn[id].accel.x;
	pIn[id].vel.y += pIn[id].accel.y;
	pIn[id].vel.x *= DAMP;
	pIn[id].vel.y *= DAMP;
	pIn[id].pos.x += pIn[id].vel.x;
	pIn[id].pos.y += pIn[id].vel.y;
	pIn[id].accel.x = pIn[id].accel.y = 0.0f;
	
	pOut[id].pos.x = pIn[id].pos.x;
	pOut[id].pos.y = pIn[id].pos.y;
	pOut[id].vel.x = pIn[id].vel.x;
	pOut[id].vel.y = pIn[id].vel.y;
	pOut[id].accel.x = pIn[id].accel.x;
	pOut[id].accel.y = pIn[id].accel.y;
}
*/


