//#define particle_radius		aaa_vu_float[0]

struct st_part_custom
{
	flat	uint  _active;
			float _age;
			float _emitter_sel;
	flat	int   _hit_test;
};


layout( std430, binding=0 ) buffer particle_pos_out_buffer		{	vec4 pos_in[];	};
layout( std430, binding=1 ) buffer particles_custom_out_buffer	{	st_part_custom custom_in[];	};
layout( std430, binding=2 ) buffer particle_density_in_buffer	{	float dens_in[];	};

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

void main(void)
{
	//point mode = 1, quad mode = 6
	uint vid = gl_VertexID % 6; // 6;
	uint iid = gl_VertexID / 6;
	//gl_PointSize = 0.1

	vec4 position_obj = position[vid];

	st_part_custom cust = custom_in[iid];
	out_custom = cust;

	float re = cust._emitter_sel;
	bool is_user = cust._emitter_sel > 0.5;

	float dens = dens_in[iid];
	float act = float(cust._active);

	float rad = draw_radius;

	rad *= act;

	//if (re > 0.4)
	//{
	//rad *= min( cust._age, 1. );
	//}

	if( !is_user )
		rad *= mix( 1.0, dens, draw_density_radius_factor );

	position_obj.xyz *= rad;
	position_obj.xyz = mat3(aaa_cam.view_inverse) * position_obj.xyz;


	vec4 position_world = pos_in[iid];
	position_world += position_obj;
    gl_Position = aaa_cam.view_projection * vec4(position_world.xyz, 1.0);
	out_normal = vec3(0,0,1);

	out_uv_dens = vec3( texcoord[vid], dens );

	vec4 c = gl_Color;
	//c.rgb *= dens;
	out_color = c;

//	dens *= 1.;
//	out_color.g = dens;
//	out_color.rb = vec2(1.-dens);
//	out_color = cust._hit_test > 0 ? vec4(0,1,0,1) : vec4(1,0,0,1);
}