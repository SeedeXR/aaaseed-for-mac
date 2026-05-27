#define DRAW_PARTICLE 1
#define DRAW_VELOCITY 2
#define DRAW_SPRING 3
#define DRAW_RIGID 4
#define DRAW_TRIANGLE_SOLID 5
#define DRAW_ACCELERATION 6
#define DRAW_TRIANGLE_WIRE 7


//(todo update from lua)
//#define particle_draw_mode		DRAW_PARTICLE
#define particle_draw_mode      aaa_vu_int[0]

//color for now (todo update from lua)
#define custom_color vec4(1,1,1,1)

//#define current_mode      aaa_vu_int[0]

struct st_part_custom
{
	flat	uint  _active;
			float _age;
			float _emitter_sel;
	flat	int   _hit_test;
};


layout( std430, binding=0 ) buffer particle_pos_out_buffer		{	vec4 pos_in[];				};
layout( std430, binding=1 ) buffer particles_custom_out_buffer	{	st_part_custom custom_in[];	};
layout( std430, binding=2 ) buffer particle_density_in_buffer	{	float dens_in[];			};
layout( std430, binding=3 ) buffer particles_vel_in_buffer	    {	float vel_in[];				};

layout( std430, binding=4 ) buffer particles_spring_in_buffer	{	uint spring_in[];			};
layout( std430, binding=5 ) buffer particles_triangle_in_buffer	{	uint triangle_in[];			};
layout( std430, binding=6 ) buffer particles_normal_in_buffer	{	vec4 normal_in[];			};
layout( std430, binding=7 ) buffer particles_acc_in_buffer		{	vec4 acc_in[];				};

layout(location = 0) out vec3			out_uv_dens;
layout(location = 1) out vec4			out_color;
layout(location = 2) out vec3			out_normal;
layout(location = 3) out st_part_custom out_custom;

CONST float draw_radius					= aaa_vu_float[0];
CONST float draw_density_radius_factor	= aaa_vu_float[1];

const float S = .5;
#define VERTEX_NB 6
const vec4 position[VERTEX_NB] =
{
	vec4(	-S,	-S,	0,  1	),	vec4(	S, S, 0, 1	),	vec4(	-S,	S, 0 , 1	),
	vec4(	-S,	-S, 0,	1	),	vec4( 	S, -S, 0 ,1 ),	vec4(	S,  S, 0 , 1 ),	
};
const vec2 texcoord[VERTEX_NB] =
{
	vec2( 0, 0 ),				vec2( 1, 1 ),			vec2( 0, 1 ),			
	vec2( 0, 0 ),				vec2( 1, 0 ),			vec2( 1, 1 ),			
};

const uint triangle_idx[VERTEX_NB] =
{
	0,1,1,2,2,0
};

void main(void)
{
	if( particle_draw_mode == DRAW_PARTICLE )
	{
		//draw particle itself, expand as a quad
		//point mode = 1, quad mode = 6
		uint vid = gl_VertexID % 6; // 6;
		uint iid = gl_VertexID / 6;

		vec4 position_obj = position[vid];

		st_part_custom cust = custom_in[iid];
		out_custom = cust;

		float re = cust._emitter_sel;
		bool is_user = cust._emitter_sel > 0.5;

		float dens = dens_in[iid];
		float act = float(cust._active);

		float rad = draw_radius*2.0;

		position_obj.xyz *= rad;
		position_obj.xyz = mat3(aaa_cam.view_inverse) * position_obj.xyz;

		vec4 position_world = pos_in[iid];
		float inv_mass = position_world.w;
		position_world += position_obj;
		gl_Position = aaa_cam.view_projection * vec4(position_world.xyz, 1.0);
		out_normal = vec3(0,0,1);

		out_uv_dens = vec3( texcoord[vid], dens );
		vec4 col;

		//todo pass at least a debug option and eventually the colors 
		if( inv_mass == 0.0 )
			col = vec4(1,1,0,1);	// fixed point are Yellow
		else
			col = cust._hit_test > 0 ? vec4(0,1,0,1) : gl_Color;

		out_color = col;
	}
	else if( particle_draw_mode == DRAW_VELOCITY )
	{
		//draw velocity as wire (line geometry)	
		float vel_factor = 1.0; //todo : param
		uint vid = gl_VertexID % 2;
		uint iid = gl_VertexID / 2;
		vec3 vel = vec3( vel_in[iid*3], vel_in[iid*3+1], vel_in[iid*3+2]);
		//vec3 vel = vec3(0.0,0.2,0.0);


		vec4 position_world = pos_in[iid];
		vec4 position_projected = position_world;
		position_projected.xyz += vel * vel_factor;

		if( vid == 1 )
		{
			position_world = position_projected;
		} 

		gl_Position = aaa_cam.view_projection * vec4(position_world.xyz, 1.0);
		
		out_normal = vec3(0,0,1);
		//out_uv_dens = vec3( texcoord[vid], dens );
		out_uv_dens = vec3( 0,0,0 );
		out_color = custom_color;
	}
	else if( particle_draw_mode == DRAW_SPRING )
	{
		//draw spring as wire (line geometry)
		uint spring_idx = spring_in[gl_VertexID];
		vec4 position_obj = pos_in[spring_idx];

		gl_Position = aaa_cam.view_projection * vec4(position_obj.xyz, 1.0);
		
		out_normal = vec3(0,0,1);
		out_uv_dens = vec3( 0,0,0 );
		out_color = custom_color;
	}
	else if( particle_draw_mode == DRAW_TRIANGLE_SOLID )
	{
		//draw spring as wire (line geometry)
		uint idx = triangle_in[gl_VertexID];
		vec4 position_obj = pos_in[idx];
		vec3 normal = normal_in[idx].xyz;


		//put normal in 0 1 range
		normal = normalize(normal); //guess flex does it anyway
		normal = normal * 0.5;
		normal += 0.5;

		gl_Position = aaa_cam.view_projection * vec4(position_obj.xyz, 1.0);
		
		out_normal = vec3(0,0,1);
		out_uv_dens = vec3( 0,0,0 );
		out_color = vec4(normal, 1.0);
	}
	else if( particle_draw_mode == DRAW_ACCELERATION )
	{
		//draw velocity as wire (line geometry)	
		float vel_factor = 1.0; //todo : param
		uint vid = gl_VertexID % 2;
		uint iid = gl_VertexID / 2;
		vec4 acc = acc_in[iid];
		//vec4 acc = vec4(1,1,0,0);
		//vec3 vel = vec3(0.0,0.2,0.0);


		vec4 position_world = pos_in[iid];
		vec4 position_projected = position_world;
		position_projected.xyz += acc.xyz * vel_factor;

		if( vid == 1 )
		{
			position_world = position_projected;
		} 

		gl_Position = aaa_cam.view_projection * vec4(position_world.xyz, 1.0);
		
		out_normal = vec3(0,0,1);
		//out_uv_dens = vec3( texcoord[vid], dens );
		out_uv_dens = vec3( 0,0,0 );
		out_color = custom_color;
	}	
	else if( particle_draw_mode == DRAW_TRIANGLE_WIRE )
	{
		//draw triangle as wire (line geometry)
		uint vid = gl_VertexID % 6;
		uint iid = gl_VertexID / 6;

		vec4 position_obj = pos_in[triangle_in[vid]];

		gl_Position = aaa_cam.view_projection * vec4(position_obj.xyz, 1.0);
		
		out_normal = vec3(0,0,1);
		out_uv_dens = vec3( 0,0,0 );
		out_color = custom_color;
	}
	else if( particle_draw_mode == DRAW_RIGID )
	{
		//todo : not sure about that one yet, maybe do a bounding box/sphere
	}

}