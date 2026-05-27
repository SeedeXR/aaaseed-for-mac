
// Main header.
#include "fbx/internal/fbx_utils_mem.h"
#include "code_utils/aaa_mem.h"

//-----------------------------------------------------------------------------

void fbx_init_scene_load_options( fbx_scene_load_options * p_dst )
{
	MEMCLEAR( p_dst, sizeof(fbx_scene_load_options) );
	p_dst->load_plugins = true;
}

void fbx_init_scene_load_result( fbx_scene_load_result * p_dst )
{
	MEMCLEAR( p_dst, sizeof(fbx_scene_load_result) );
	p_dst->result_code = fbx_scene_load_result_code::fail_init;
}

void fbx_init_scene_data( fbx_scene_data * p_dst )
{
	MEMCLEAR( p_dst, sizeof(fbx_scene_data) );
}

void fbx_init_mesh_load_options( fbx_mesh_load_options * p_dst )
{
	MEMCLEAR( p_dst, sizeof(fbx_mesh_load_options) );
}

void fbx_init_mesh_load_result( fbx_mesh_load_result * p_dst )
{
	MEMCLEAR( p_dst, sizeof(fbx_mesh_load_result) );
	fbx_init_mesh_data(&(p_dst->mesh));
}

void fbx_init_mesh_data( fbx_mesh_data * p_dst )
{
	MEMCLEAR( p_dst, sizeof(fbx_mesh_data) );
	p_dst->layout = fbx_mesh_layout::array_per_attribute;
}

//-----------------------------------------------------------------------------

fbx_scene_load_result init_load_result()
{
	fbx_scene_load_result result;
	MEMCLEAR( &result, sizeof(fbx_scene_load_result) );
	result.result_code = fbx_scene_load_result_code::fail_init;
	return result;
}

fbx_scene_data init_scene()
{
	fbx_scene_data result;
	MEMCLEAR( &result, sizeof(fbx_scene_data) );
	return result;
}

fbx_mesh_attributes_memory_requirements init_mesh_memory_requirements()
{
	fbx_mesh_attributes_memory_requirements result;
	MEMCLEAR( &result, sizeof(fbx_mesh_attributes_memory_requirements) );
	return result;
}

fbx_mesh_data init_mesh()
{
	fbx_mesh_data result;
	MEMCLEAR( &result, sizeof(fbx_mesh_data) );

	//set layout per attribute
	result.layout = fbx_mesh_layout::array_per_attribute;

	return result;
}

fbx_mesh_skinning_data allocate_mesh_skinning_data( UINT32 vertex_count, UINT32 bone_count, UINT32 max_bone_per_vertex )
{
	fbx_mesh_skinning_data result;
	result.indices_count_ptr		= (UINT32*) MALLOC( vertex_count * sizeof(UINT32) );
	result.indices_ptr				= (UINT32*) MALLOC( vertex_count * max_bone_per_vertex * sizeof(UINT32) );
	result.weights_ptr				= (float*)  MALLOC( vertex_count * max_bone_per_vertex * sizeof(float)	);
	result.bone_node_index_ptr		= (INT32*)				MALLOC( bone_count * sizeof(UINT32) );
	result.inverse_bind_pose_ptr	= (inverse_bind_pose*)	MALLOC( bone_count * sizeof(inverse_bind_pose) );

	//set indices count to 0, and node index to -1
	MEMCLEAR( result.indices_count_ptr,	vertex_count * sizeof(UINT32)						);
	MEMCLEAR( result.indices_ptr,		vertex_count * max_bone_per_vertex * sizeof(UINT32)	);
	MEMCLEAR( result.weights_ptr,		vertex_count * max_bone_per_vertex * sizeof(float)	);

	//beurk memset( result.bone_node_index_ptr, -1, bone_count * sizeof(UINT32) );
	for( UINT32 i=0; i<bone_count; ++i )
		*(result.bone_node_index_ptr+i) = -1;

	return result;
}
void free_mesh_skinning_data( fbx_mesh_skinning_data& p_data )
{
	IF_FREE_AND_NULL( p_data.inverse_bind_pose_ptr	);
	IF_FREE_AND_NULL( p_data.bone_node_index_ptr	);
	IF_FREE_AND_NULL( p_data.weights_ptr			);
	IF_FREE_AND_NULL( p_data.indices_ptr			);
	IF_FREE_AND_NULL( p_data.indices_count_ptr		);
}

fbx_mesh_skinning_data_info allocate_mesh_skinning_data_info(UINT32 vertex_count)
{
	fbx_mesh_skinning_data_info result;
	result.max_bones_per_vertex = 0;
	result.indices_offset_ptr	= (INT32*) MALLOC( vertex_count * sizeof(UINT32) );
	result.indices_count_ptr	= (INT32*) MALLOC( vertex_count * sizeof(UINT32) );
	result.weight_sum_ptr		= (float*) MALLOC( vertex_count * sizeof(float)  );

	//set indices count to 0, and node index to -1
	MEMCLEAR( result.indices_offset_ptr, vertex_count * sizeof(UINT32)	);
	MEMCLEAR( result.indices_count_ptr,  vertex_count * sizeof(UINT32)	);
	MEMCLEAR( result.weight_sum_ptr,     vertex_count * sizeof(float)	);

	return result;
}
void free_mesh_skinning_data_info( fbx_mesh_skinning_data_info& p_data )
{
	IF_FREE_AND_NULL( p_data.weight_sum_ptr		);
	IF_FREE_AND_NULL( p_data.indices_count_ptr	);
	IF_FREE_AND_NULL( p_data.indices_offset_ptr	);
}
