
#ifdef AAA_FBX_STRUCTS_INTERNAL_H
#error "FBX_STRUCTS_INTERNAL_H included more than once."
#endif
#define AAA_FBX_STRUCTS_INTERNAL_H 1


/* Those are the structures used internally by the loader, they are never exposed to the public API */

//internal scene info, used for memory requirements
struct fbx_scene_info_internal
{
	INT32 node_count;
	INT32 track_count;
	INT32 curve_keyframe_count;
	INT32 mesh_blend_shape_animations_count;
};


//memory requirements for mesh attributes
struct fbx_mesh_attributes_memory_requirements
{
	UINT32 position_size;
	UINT32 normals_size;
	UINT32 texture_coordinates_size;
	UINT32 tangents_size;
	UINT32 bitangents_size;
	UINT32 bones_count_size;
	UINT32 bones_location_size;
	UINT32 bones_indices_size;
	UINT32 bones_weight_size;
	UINT32 indices_size;
	UINT32 bones_bind_pose_size;
	UINT32 bones_node_index_table_size;
};

//mesh writer information
struct fbx_mesh_writer_info
{
	UINT32 position_size;
	UINT32 normals_size;
	UINT32 texture_coordinates_size;
	UINT32 indices_size;

	void* position_location;
	void* normals_location;
	void* texture_coordinates_location;
	void* indices_location;
};

//blend shape information
struct fbx_mesh_blend_shape_info
{
	INT32 total_shape_count;
	INT32 total_channel_count;
	INT32 total_target_count;
	bool has_normals;
};

//context data to  write scene information
struct fbx_scene_write_context_data
{
	UINT32 current_node_index;
	UINT32 node_write_index;
	INT32 node_parent_index;
	UINT32 animation_curve_write_index;
	UINT32 animation_keyframe_write_index;
};

struct inverse_bind_pose
{
	float data[16];
};

//mesh skinning, base information before to perform load
struct fbx_mesh_skinning_data_info
{
	//number of bones
	INT32 bone_count;
	//maximum number number of bones per vertex
	INT32 max_bones_per_vertex;
	//maximum number number of bones per vertex, can be clamped depending on bone limit
	INT32 max_bones_per_vertex_clamped;
	//this is the full amount of bones per vertex, used for prefix table
	INT32 sum_vertex_influence;
	//bone indices count per control point (n control points), this is not clamped
	INT32* indices_offset_ptr;
	//bone indices count per control point (n control points), this is not clamped
	INT32* indices_count_ptr;
	//sum of the bone weights per control point
	float* weight_sum_ptr;
};

//base data for mesh skinning
struct fbx_mesh_skinning_data
{
	//bone indices count per control point (n control points)
	UINT32* indices_count_ptr;
	//bone indices (n control points * max_bone_per_vertex)
	UINT32* indices_ptr;
	//bone weights (n control points * 4  max_bone_per_vertex)
	float* weights_ptr;
	//bone to node index table
	INT32* bone_node_index_ptr;
	//inverse bind pose (n bone count * 16)
	inverse_bind_pose* inverse_bind_pose_ptr;
	float max_lost_influence;
};
