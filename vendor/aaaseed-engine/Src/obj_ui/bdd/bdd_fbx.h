
#ifdef AAA_BDD_FBX_H
#error "BDD_FBX_H included more than once."
#endif
#define AAA_BDD_FBX_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#if !defined(AAA_FBX_SCENE_H)
#	include "fbx/aaa_fbx_scene.h"
#endif
#if !defined(AAA_DELTA_T_H)
#	include "time/delta_t.h"
#endif
#ifndef AAA_FBX_MESH_H
#	include "fbx/aaa_fbx_mesh.h"
#endif

class c_bdd_fbx final
	: public c_bdd
{
	FACTORY_DECLARE(c_bdd_fbx, c_bdd);


private:
	aaa::c_fbx_scene		_fbx_scene;
	c_delta_t				_delta_t;					// time delta.

	bool					_b_draw_mesh_ui;
	bool					_b_draw_by_index_ui;

	bool					_b_draw_debug_ui;
	bool					_b_draw_curve_ui;
	bool					_b_draw_bone_ui;
	bool					_b_draw_position_ui;
	INT32					_draw_bone_begin_idx_ui;
	INT32					_draw_bone_end_idx_ui;

	FP32					_draw_position_size_ui;
	FP32					_draw_bone_size_ui;

	bool					_b_scene_transform_cur;	//we didn't use bool be we force value at init
	bool					_b_scene_transform_ui;	

	FP32					_scene_translate_ui[3];
	FP32					_scene_rotate_turn_ui[3];
	FP32					_scene_scale_ui[4];
	FP32					_scene_center_ui[3];
	bool					_b_scene_recenter_ui;		// Translate before doing rotation and scaling

	INT32					_mesh_index_ui{ -1 };
	INT32					_mesh_index_cur{ -1 };
	INT32					_mesh_blend_shape_nb_ui{};
	INT32					_mesh_blend_shape_channel_nb_ui{};
	INT32					_mesh_vertex_nb_ui { -1 };
	INT32					_mesh_index_nb_ui  { -1 };
	FP32					_mesh_bbox_min_ui[3];
	FP32					_mesh_bbox_max_ui[3];
	o_str					_mesh_name_ui;

//todo this could be shared by the bdd_fbx...
//and regroup in struct class...
//and also we can have naterial_type too
	FP32					_material_ambient_color_ui[3];
	FP32					_material_diffuse_color_ui[3];
	FP32					_material_specular_color_ui[3];
	FP32					_material_emissive_color_ui[3];
	FP32					_material_opacity_ui;
	FP32					_material_shininess_ui;
	FP32					_material_reflectiviy_ui;
	INT32					_material_ambient_texture_idx_ui;
	INT32					_material_diffuse_texture_idx_ui;
	INT32					_material_specular_texture_idx_ui;
	INT32					_material_normal_texture_idx_ui;
	INT32					_material_emissive_texture_idx_ui;
	INT32					_material_shininess_texture_idx_ui;
	INT32					_material_specular_factor_texture_idx_ui;
	INT32					_material_bump_texture_idx_ui;
	INT32					_material_opacity_texture_idx_ui;
	o_str					_material_name_ui;

	bool					_b_play_ui;
	bool					_b_loop_ui;
	bool					_b_linear_ui;
//	bool					_b_restart_trig_ui;
	bool					_b_time_update_ui;
	bool					_b_transform_update_ui;	// update transformation
	FP32					_time_ui;
	FP32					_time;
	FP32					_time_factor_ui;
	INT32					_animation_index_ui;
	INT32					_animation_index_cur;
	bool					_blend_shape_active_ui;
	INT32					_blend_shape_idx_ui;
	INT32					_blend_shape_idx;
	bool					_blend_shape_manual_ui[MAX_BLEND_SHAPE_CHANNELS];
	FP32					_blend_shape_factor_ui[MAX_BLEND_SHAPE_CHANNELS];
	o_str					_animation_name_ui;

	bool					_b_use_sdk_evaluator_ui;
	bool					_b_use_geometric_transform_ui;
	bool					_b_validate_at_load_ui;
	bool					_b_fbx_loaded_ui;
	bool					_b_verbose_ui;
	bool					_b_reload_trig_ui;
	o_str					_filename_fbx_ui;
	o_str					_texture_path_ui;
	INT32					_texture_path_index_ui{ -1 };
	INT32					_texture_path_index_cur{ -1 };
	bool					_b_use_absolute_path_ui;
	bool					_b_need_position;
	FP32					_position_ui[3];
	FP32					_rotation_ui[3];	// root global rotation
	bool					_b_get_global_rotation_ui;	// don't want to compute it every frame

	//todo at some point deal with UINT32 param and transform these
	INT32					_mesh_nb_ui;
	INT32					_texture_nb_ui;
	INT32					_stack_nb_ui;
	INT32					_node_nb_ui;
	INT32					_track_nb_ui;
	INT32					_keyframe_nb_ui;
	FP32					_time_begin_ui;
	FP32					_time_end_ui;
	FP32					_time_node_begin_ui;
	FP32					_time_node_end_ui;
	FP32					_time_blend_shape_begin_ui;
	FP32					_time_blend_shape_end_ui;
	
	void	reset_cache();
	void	reset_data();
	void	release_data();

	void	dealloc() override final;
	void	init();

	void	draw_curve();
	void	draw_mesh();
	void	draw_bone(  FP32 CONST size );
public:
			void	update_time( FP32 time );

	virtual	void	param_init_pt();

			AAA_ERR	load_data();
	virtual AAA_ERR	load_data_from_filename(    o_str CONST & filename, INT32 type_io );
	virtual	AAA_ERR	load_do_after(				o_str CONST & filename );

	virtual	void	update();
	virtual	void	draw();
};
