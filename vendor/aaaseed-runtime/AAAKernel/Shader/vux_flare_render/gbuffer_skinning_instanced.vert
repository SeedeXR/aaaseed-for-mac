

#define MAX_BONE_MAT 256

#ifndef ENABLE_TANGENT
#define ENABLE_TANGENT 1
#endif


layout(location = 0) in vec3  in_pos_object;
layout(location = 1) in vec3  in_norm_object;
layout(location = 2) in vec2  in_texcoord_object;

layout(location = 3) in vec3  in_tangent_object;
layout(location = 4) in vec3  in_binormal_object;

layout(location = 5) in uvec4 in_bone_index_object;
layout(location = 6) in vec4  in_bone_weight_object;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_position_world;
layout(location = 2) out vec2 out_texcoord;

#if ENABLE_TANGENT == 1
layout(location = 3) out mat3 out_tangent_frame;
#endif

//we replace binding 0 (skinning matrices, by the counter block)
layout(binding = 0) uniform count_matrix_block
					{ uint node_count_per_instance;	} 	g_instance_counters;
//per mesh data, non animation dependent
layout(binding = 1) uniform bind_pose_matrix_block
					{ mat4 mat[MAX_BONE_MAT];		} 	g_bind_pose_matrices;
layout(binding = 2) uniform bone_to_node_index_block
					{ uint index[MAX_BONE_MAT];		} 	g_bone_to_node_index;

//per instance data
layout(std430, binding = 0) buffer skin_matrix_buffer
{
  mat4 skinning_matrices[];
};


mat4 get_bone_matrix( uint instance_id, uint bone_index )
{
	//bone index is constant, as its per mesh data, we need to fetch the correct node index tho
	uint node_index = g_bone_to_node_index.index[bone_index] + (g_instance_counters.node_count_per_instance * instance_id);
	return transpose( g_bind_pose_matrices.mat[bone_index] ) * transpose( skinning_matrices[node_index] );
}

vec4 transform_bone(vec4 input_vector, uint instance_id, uint bone_index)
{
	return input_vector * get_bone_matrix( instance_id, bone_index );
}

void main(void)
{
	uint instance_id = gl_InstanceID;

	vec4 initial_pos_object  = vec4(in_pos_object, 1.0);
	vec4 initial_norm_object = vec4(in_norm_object, 0.0);

	vec4 pos_object_skin;
	pos_object_skin  = transform_bone( initial_pos_object, instance_id, in_bone_index_object.x ) * in_bone_weight_object.x;
	pos_object_skin += transform_bone( initial_pos_object, instance_id, in_bone_index_object.y ) * in_bone_weight_object.y;
	pos_object_skin += transform_bone( initial_pos_object, instance_id, in_bone_index_object.z ) * in_bone_weight_object.z;
	pos_object_skin += transform_bone( initial_pos_object, instance_id, in_bone_index_object.w ) * in_bone_weight_object.w;

	vec4 norm_object_skin;
	norm_object_skin  = transform_bone( initial_norm_object, instance_id, in_bone_index_object.x ) * in_bone_weight_object.x;
	norm_object_skin += transform_bone( initial_norm_object, instance_id, in_bone_index_object.y ) * in_bone_weight_object.y;
	norm_object_skin += transform_bone( initial_norm_object, instance_id, in_bone_index_object.z ) * in_bone_weight_object.z;
	norm_object_skin += transform_bone( initial_norm_object, instance_id, in_bone_index_object.w ) * in_bone_weight_object.w;

	vec4 position_world = aaa_model.matrix * vec4(pos_object_skin.xyz, 1.0);
    vec4 normal_world 	= aaa_model.matrix * vec4(norm_object_skin.xyz, 0.0);

	out_position_world 	= position_world.xyz;
    out_normal 			= normalize(normal_world.xyz);
    out_texcoord 		= in_texcoord_object;

	#if ENABLE_TANGENT == 1
	vec4 initial_tangent_object = vec4(in_tangent_object, 0.0);
	vec4 initial_binormal_object = vec4(in_binormal_object, 0.0);

	vec4 tangent_object_skin;
	tangent_object_skin  = transform_bone( initial_tangent_object, instance_id, in_bone_index_object.x ) * in_bone_weight_object.x;
	tangent_object_skin += transform_bone( initial_tangent_object, instance_id, in_bone_index_object.y ) * in_bone_weight_object.y;
	tangent_object_skin += transform_bone( initial_tangent_object, instance_id, in_bone_index_object.z ) * in_bone_weight_object.z;
	tangent_object_skin += transform_bone( initial_tangent_object, instance_id, in_bone_index_object.w ) * in_bone_weight_object.w;

	vec4 tangent_world = aaa_model.matrix * vec4(tangent_object_skin.xyz, 0.0);

	vec4 binormal_object_skin;
	binormal_object_skin  = transform_bone( initial_binormal_object, instance_id, in_bone_index_object.x ) * in_bone_weight_object.x;
	binormal_object_skin += transform_bone( initial_binormal_object, instance_id, in_bone_index_object.x ) * in_bone_weight_object.x;
	binormal_object_skin += transform_bone( initial_binormal_object, instance_id, in_bone_index_object.x ) * in_bone_weight_object.x;
	binormal_object_skin += transform_bone( initial_binormal_object, instance_id, in_bone_index_object.x ) * in_bone_weight_object.x;

	vec4 binormal_world = aaa_model.matrix * vec4(binormal_object_skin.xyz, 0.0);

	out_tangent_frame[0] = normalize(tangent_world.xyz);
	out_tangent_frame[1] = normalize(binormal_world.xyz);
	out_tangent_frame[2] = normalize(normal_world.xyz);
	#endif
//todo matrix model
    gl_Position = aaa_cam.projection * aaa_cam.view * vec4(out_position_world, 1.0);
}