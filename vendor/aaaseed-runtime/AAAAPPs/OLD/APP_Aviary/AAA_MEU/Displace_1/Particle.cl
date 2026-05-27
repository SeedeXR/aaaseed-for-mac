#define DAMP			0.95f
#define CENTER_FORCE	.1f
#define MOUSE_FORCE		3.0f
#define MIN_SPEED		0.001f
#define MIN_SPEED2		MIN_SPEED*MIN_SPEED
#define DT				0.02f


typedef struct{
	float2 pos, vel, accel;
	float mass;
	float radius;		// need this to make sure the float2 vel is aligned to a 16 byte boundary
} Particle;


typedef struct{
	float	line;
} rendering;

__kernel void updateParticle(	__global Particle* particles,
								__global float2* posBuffer,
								const rendering attrib,
								const float2 mousePos,
								const float2 dimensions )
{
	int id = get_global_id(0);
	__global Particle *p = &particles[id];

	mousePos.x -= .5f;
	mousePos.y -= .5f;
	mousePos.x *= 16;
	mousePos.y *= 16;
	if( attrib.line > .5 )
		id *= 2;
	float2 diff = mousePos - posBuffer[id];
	float invDistSQ = 1.0f / dot(diff, diff);
	//invDistSQ = pow( invDistSQ, 2.5f );
	float2 rep = diff * MOUSE_FORCE * invDistSQ * p->mass;

	p->vel += (dimensions - posBuffer[id]) * CENTER_FORCE - rep;

	float speed2 = dot(p->vel, p->vel);
	if( speed2 < MIN_SPEED2 )
	{
		posBuffer[id] = mousePos + p->pos * 5.;
		//posBuffer[id] = mousePos + diff * (1 + p->mass);
		if( attrib.line > .5 )
			posBuffer[id+1] = posBuffer[id];
	}
	else
	{
		if( attrib.line > .5 )
			posBuffer[id+1] = posBuffer[id];
		posBuffer[id] += p->vel * DT;
	}
	p->vel *= DAMP;
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


