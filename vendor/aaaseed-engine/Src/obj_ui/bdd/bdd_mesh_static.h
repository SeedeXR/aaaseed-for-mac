
#ifdef AAA_BDD_MESH_STATIC_H
#error "BDD_MESH_STATIC_H included more than once."
#endif
#define AAA_BDD_MESH_STATIC_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#if !defined(AAA_FBX_SCENE_H)
#	include "fbx/aaa_fbx_scene.h"
#endif
#if !defined(AAA_DELTA_T_H)
#	include "time/delta_t.h"
#endif



class c_bdd_mesh_static final
	: public c_bdd
{
	FACTORY_DECLARE(c_bdd_mesh_static, c_bdd );

private:
	aaa::c_fbx_scene	_fbx_scene;

	FP32				_scene_translate_ui[3];
	FP32				_scene_rotate_turn_ui[3];
	FP32				_scene_scale_ui[4];

	INT32				_mesh_nb_ui{ 0 };
	INT32				_mesh_index_ui{ -1 };
	INT32				_mesh_index_last{ -1 };
	INT32				_mesh_vertex_nb_ui { -1 };
	INT32				_mesh_index_nb_ui  { -1 };
	FP32				_mesh_bbox_min_ui[3];
	FP32				_mesh_bbox_max_ui[3];
	o_str				_mesh_name_ui;

	INT32				_texture_nb_ui;

	FP32				_material_ambient_color_ui[3];
	FP32				_material_diffuse_color_ui[3];
	FP32				_material_specular_color_ui[3];
	FP32				_material_emissive_color_ui[3];
	FP32				_material_opacity_ui;
	FP32				_material_shininess_ui;
	FP32				_material_reflectiviy_ui;
	INT32				_material_ambient_texture_idx_ui;
	INT32				_material_diffuse_texture_idx_ui;
	INT32				_material_specular_texture_idx_ui;
	INT32				_material_normal_texture_idx_ui;
	INT32				_material_emissive_texture_idx_ui;
	INT32				_material_shininess_texture_idx_ui;
	INT32				_material_specular_factor_texture_idx_ui;
	INT32				_material_bump_texture_idx_ui;
	INT32				_material_opacity_texture_idx_ui;
	o_str				_material_name_ui;

	bool				_b_use_geometric_transform_ui;
	bool				_b_validate_at_load_ui;
	bool				_b_verbose_ui;
	bool				_b_fbx_loaded_out	{false};
	bool				_b_reload_trig_ui;
	o_str				_filename_fbx_ui;
	o_str				_texture_path_ui;
	INT32				_texture_path_index_ui{ -1 };
	INT32				_texture_path_index_cur{ -1 };
	bool				_b_use_absolute_path_ui;
//	REAL				_position_ui[3];

	bool				_b_draw_mesh_ui;

private:
	void	release_data();
	void	dealloc() override final;
	void	init();

	void	draw_mesh();

public:
	virtual	void	param_init_pt();

			AAA_ERR	load_data() override;
	virtual AAA_ERR	load_data_from_filename(o_str CONST& filename, INT32 type_io);
	virtual	AAA_ERR	load_do_after(o_str CONST& filename);

	virtual	void	update();
	virtual	void	draw();

public:
	virtual INT32			get_point_dataset_nb()					override final;
	virtual	bool			set_point_dataset( INT32 dataset_id )	override final;
	virtual INT32			get_point_nb()							override final;
	virtual REAL *			get_points()							override final;
	virtual REAL *			get_normals()							override final;

	FINLINE FP32 *			get_mesh_bbox_min()								{ return _mesh_bbox_min_ui; }
	FINLINE FP32 *			get_mesh_bbox_max()								{ return _mesh_bbox_max_ui; }

			glm::mat4		get_transform_matrix() CONST;
};
