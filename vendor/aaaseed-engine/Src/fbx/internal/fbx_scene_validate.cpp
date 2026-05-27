
// Main header.
#include "fbx/internal/fbx_scene_validate.h"
#include "spy.h"

fbx_scene_validate_result fbx_validate_scene(const fbx_scene_data& p_scene)
{
	SPY_PUSH_RANGE_FUNCTION( spy::COL_2 );

	fbx_scene_validate_result result;
	result.non_srt_info_in_nodes = false;
	result.multi_layer_animation = false;
	result.non_consistent_rotation_order = false;

	//just set a random first order before to get first one, we should still not have case with no nodes but who knows
	fbx_rotation_order first_order = fbx_rotation_order::XYZ;
	if( p_scene.scene_info.total_node_count > 0 )
	{
		first_order = p_scene.nodes[0].rotation_order;
	}

	//todo master (use v.h)
	for (UINT32 i = 0; i < p_scene.scene_info.total_node_count; i++)
	{
		fbx_node node = p_scene.nodes[i];

		if( node.post_rotation[0] != 0.0f || node.post_rotation[1] != 0.0f || node.post_rotation[2] != 0.0f )
			result.non_srt_info_in_nodes = true;
		if( node.pre_rotation[0] != 0.0f || node.pre_rotation[1] != 0.0f || node.pre_rotation[2] != 0.0f )
			result.non_srt_info_in_nodes = true;
		if( node.rotation_offset[0] != 0.0f || node.rotation_offset[1] != 0.0f || node.rotation_offset[2] != 0.0f )
			result.non_srt_info_in_nodes = true;
		if( node.rotation_pivot[0] != 0.0f || node.rotation_pivot[1] != 0.0f || node.rotation_pivot[2] != 0.0f )
			result.non_srt_info_in_nodes = true;
		if( node.scaling_offset[0] != 0.0f || node.scaling_offset[1] != 0.0f || node.scaling_offset[2] != 0.0f )
			result.non_srt_info_in_nodes = true;
		if( node.scaling_pivot[0] != 0.0f || node.scaling_pivot[1] != 0.0f || node.scaling_pivot[2] != 0.0f )
			result.non_srt_info_in_nodes = true;

		//check rotation order
		if( node.rotation_order != first_order )
			result.non_consistent_rotation_order = true;
	}

	for (UINT32 i = 0; i < p_scene.scene_info.animation_stack_nb; i++)
	{
		fbx_animation_info animation = p_scene.scene_info.animations[i];

		if( animation.layer_count > 1 )
		{
			result.multi_layer_animation = true;
			break;
		}
	}

	SPY_POP_RANGE();

	return result;
}

fbx_mesh_validate_result fbx_validate_mesh(const fbx_mesh_data& p_mesh)
{
	SPY_PUSH_RANGE_FUNCTION( spy::COL_2 );

	fbx_mesh_validate_result result;
	result.overload_index_found_in_triangle = false;
	result.same_index_found_in_triangle = false;
	result.overload_bone_found_in_bone_index_buffer = false;
	result.null_vertex_to_bone_index = false;

	UINT32 const * index_buffer = (UINT32*)p_mesh.index_buffer_ptr;
	for( UINT32 nb = p_mesh.index_count / 3; nb > 0; --nb )
	{
		UINT32 i1 = *index_buffer;
		UINT32 i2 = *(index_buffer+1);
		UINT32 i3 = *(index_buffer+2);

		if( (i1 == i2) || (i2 == i3) || (i3 == i1) )
			result.same_index_found_in_triangle = true;

		if( (i1 >= p_mesh.vertex_count) || (i2 >= p_mesh.vertex_count) || (i3 >= p_mesh.vertex_count) )
			result.overload_index_found_in_triangle = true;

		index_buffer += 3;
	}

	if( p_mesh.has_bones )
	{
		UINT32 const * bone_index_buffer = (UINT32*)p_mesh.array_location_info.vertex_bones_data.bone_index_ptr;
		float* bone_weight_buffer = (float*)p_mesh.array_location_info.vertex_bones_data.bone_weight_ptr;
		UINT32 num_bone_indices = p_mesh.vertex_count * 4;

		for (UINT32 i = 0; i < num_bone_indices; i++)
		{
			UINT32 bone_index = bone_index_buffer[i];
			if( bone_index >= p_mesh.bones_data.bones_nb )
			{
				result.overload_bone_found_in_bone_index_buffer = true;
				break;
			}
		}

		//todo why we don't check anymore
		/*for (UINT32 i = 0; i < p_mesh.vertex_count; i++)
		{
			UINT32 bone_index1 = bone_index_buffer[i*4];
			UINT32 bone_index2 = bone_index_buffer[i * 4+1];
			UINT32 bone_index3 = bone_index_buffer[i * 4+2];
			UINT32 bone_index4 = bone_index_buffer[i * 4+3];

			float bone_weight1 = bone_weight_buffer[i * 4];
			float bone_weight2 = bone_weight_buffer[i * 4 + 1];
			float bone_weight3 = bone_weight_buffer[i * 4 + 2];
			float bone_weight4 = bone_weight_buffer[i * 4 + 3];

			float sum_weight = bone_weight1 + bone_weight2 + bone_weight3 + bone_weight4;

			if( bone_index1 == 0 && bone_index2 == 0 && bone_index3 == 0 && bone_index4 == 0 && sum_weight == 0.0f )
			{
				result.null_vertex_to_bone_index = true;
			}
		}*/
	}

	SPY_POP_RANGE();
	return result;
}