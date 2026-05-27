//#version 430


#if USE_AAA_UNIFORM == 1
	#define noise_frequency 		vec3(aaa_fu_float[0], aaa_fu_float[1], aaa_fu_float[2])
	#define noise_offset 			vec3(aaa_fu_float[3], aaa_fu_float[4], aaa_fu_float[5])
	#define noise_strength 			aaa_fu_float[6]

#else
	layout(binding = 0) uniform grid_settings_block
	{
		vec3    noise_frequency;
		float   noise_strength;
		vec3    noise_offset;
	} u_grid_settings;

	#define noise_frequency 		u_grid_settings.noise_frequency
	#define noise_offset 			u_grid_settings.noise_offset
	#define noise_strength 			u_grid_settings.noise_strength

#endif



layout(location = 0) in vec3  in_pos_object;
layout(location = 1) in vec3  in_norm_object;
layout(location = 2) in vec2  in_texcoord_object;


layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_position_world;
layout(location = 2) out vec2 out_texcoord;



void main(void)
{
	vec3 pos_object = in_pos_object;
	float n = iqFbm3d(pos_object * noise_frequency + noise_offset) * noise_strength;

	pos_object = pos_object + in_norm_object * n;

	mat4 mat = aaa_scene.matrix * aaa_model.matrix;
	vec4 position_world = mat * vec4(	pos_object.xyz,  	1.0	);
    vec4 normal_world 	= mat * vec4(	in_norm_object.xyz,	0.0	);

	out_position_world 	= position_world.xyz;
    out_normal 			= normalize(normal_world.xyz);
    out_texcoord 		= in_texcoord_object;

    gl_Position = aaa_cam.projection_view * vec4(out_position_world, 1.0);
}