
layout(location = 0) in vec3  in_pos_object;
//layout(location = 1) in vec3  in_norm_object;
//layout(location = 2) in vec2  in_texcoord_object;


layout(location = 0) out vec3 out_position_world;
layout(location = 1) out vec3 out_normal_world;
layout(location = 2) out vec2 out_texcoord;

void main(void)
{
//	mat4 mat = aaa_scene.matrix * aaa_model.matrix;
//	vec4 position_world = mat * vec4(	in_pos_object.xyz,  1.0	);
//	vec4 normal_world 	= mat * vec4(	gl_Normal,			0.0	);
	vec4 position_world = vec4(in_pos_object.xyz,  1.0);
	vec3 normal_world 	= gl_Normal;

	out_position_world 	= position_world.xyz;
	out_normal_world 	= normalize(normal_world.xyz);
//	out_texcoord 		= in_texcoord_object;
//	out_texcoord 		= (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;
	out_texcoord 		= gl_MultiTexCoord0.xy;

	gl_Position = aaa_cam.view_projection * vec4(out_position_world, 1.0);
}