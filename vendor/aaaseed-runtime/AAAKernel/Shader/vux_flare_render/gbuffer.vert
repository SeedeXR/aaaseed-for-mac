
#ifndef ENABLE_TANGENT
#	define ENABLE_TANGENT 1
#endif


layout(location = 0) in vec3  in_pos_object;
layout(location = 1) in vec3  in_norm_object;
layout(location = 2) in vec2  in_texcoord_object;

layout(location = 3) in vec3  in_tangent_object;
layout(location = 4) in vec3  in_binormal_object;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_position_world;
layout(location = 2) out vec2 out_texcoord;

#if ENABLE_TANGENT == 1
	layout(location = 3) out mat3 out_tangent_frame;
#endif


void main(void)
{
	mat4 mat = aaa_scene.matrix * aaa_model.matrix;
	vec3 position	= ( mat * vec4(in_pos_object,		1.0) ).xyz;
    vec3 normal		= ( mat * vec4(in_norm_object,		0.0) ).xyz;
#if ENABLE_TANGENT == 1
	vec3 tangent	= ( mat * vec4(in_tangent_object,	0.0) ).xyz;
	vec3 binormal	= ( mat * vec4(in_binormal_object,	0.0) ).xyz;
#endif

	//normalize and output
	normal = normalize( normal );
#if ENABLE_TANGENT == 1
	out_tangent_frame[0] = normalize( tangent  );
	out_tangent_frame[1] = normalize( binormal );
	out_tangent_frame[2] = normal;
#endif
	out_position_world 	= position;
    out_normal 			= normal;
    out_texcoord 		= in_texcoord_object;

//	gl_Position = aaa_cam.projection * aaa_cam.view * vec4(out_position_world, 1.0);
    gl_Position = aaa_cam.view_projection * vec4( position, 1.0 );
}