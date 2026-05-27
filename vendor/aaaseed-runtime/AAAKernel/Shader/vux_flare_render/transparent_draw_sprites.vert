

layout( std430, binding=0 ) buffer particle_pos_out_buffer	{	vec4 pos_in[];	};
layout( std430, binding=1 ) buffer particle_col_out_buffer	{	vec4 col_in[];	};
layout( std430, binding=2 ) buffer particle_norm_out_buffer	{	vec4 norm_in[];	};
layout( std430, binding=3 ) buffer particle_attr_out_buffer	{	vec4 attr_in[];	};

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_position_world;
layout(location = 2) out vec2 out_texcoord;
layout(location = 3) out vec4 out_color;
layout(location = 4) out vec4 out_rnd;

void main(void)
{
	//point mode = 1, quad mode = 6
	uint id = gl_VertexID ; // 6;

	vec3 pos_object = aaa_vu_vec4[0].xyz + pos_in[id].xyz * aaa_vu_vec4[1].xyz * aaa_vu_vec4[1].w;
	vec4 col = col_in[id];
	vec4 norm_object = norm_in[id];
	vec4 attr = attr_in[id];

//	mat4 mat = aaa_scene.matrix * aaa_model.matrix;
	vec4 position_world = vec4(	pos_object.xyz,  	1.0	);
    vec4 normal_world 	= vec4(	norm_object.xyz,	0.0	);

	out_position_world 	= position_world.xyz;
//	out_position_world 	= clamp( position_world.xyz, vec3(-1), vec3(1.) );
    out_normal 			= normalize(normal_world.xyz);
    out_texcoord 		= vec2(0.0,0.0);
	out_color           = col;
	out_rnd				= attr;

	gl_PointSize = aaa_vu_float[0] + attr.y * ( aaa_vu_float[1] - aaa_vu_float[0] );
    gl_Position = aaa_cam.view_projection * vec4(out_position_world, 1.0);
}