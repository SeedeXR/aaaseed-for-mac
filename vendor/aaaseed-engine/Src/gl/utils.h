
#ifdef AAA_GL_UTILS_H
#error "GL_UTILS_H included more than once."
#endif
#define AAA_GL_UTILS_H 1



namespace gl {

	enum e_ubo_binding_index 
	{
 //     ubo_binding_index_aaa_uni				=  8,	//2020 tested on Immersion was slower that doing it by pieces: so removed
		ubo_binding_index_light_count			= 10,	//ok
		ubo_binding_index_model					= 11,	
		ubo_binding_index_camera				= 12,	//ok
		ubo_binding_index_material				= 13,	//ok
		ubo_binding_index_scene_matrix			= 14,	//todo
	};

	enum e_ssbo_binding_index
	{
		ssbo_binding_index_directional_light	= 13,
		ssbo_binding_index_point_light			= 14,
		ssbo_binding_index_spot_light			= 15
	};

	enum e_vao_mesh_slot_index
	{
		vao_mesh_slot_index_position			= 0,
		vao_mesh_slot_index_normal				= 1,
		vao_mesh_slot_index_texcoord			= 2,
		vao_mesh_slot_index_tangent				= 3,
		vao_mesh_slot_index_binormal			= 4,
		vao_mesh_slot_index_bone_index			= 5,
		vao_mesh_slot_index_bone_weight			= 6
	};

	enum e_usage_type
	{
		e_usage_draw							= 0,
		e_usage_read							= 1,
		e_usage_copy							= 2,
	};

	enum e_usage_frequency
	{
		e_usage_stream							= 0,
		e_usage_static							= 1,
		e_usage_dynamic							= 2,
	};

} // namespace gl
