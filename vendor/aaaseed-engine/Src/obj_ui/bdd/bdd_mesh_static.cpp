
// Main header.
#include "bdd_mesh_static.h"

#include "draw/model.h"
#include "draw/axe.h"
#include "infrastructure/seedfile.h"

#include "fbx/internal/fbxloader.h"
#include "fbx/internal/fbx_utils_mem.h"
#include "fbx/internal/fbx_scene_process.h"
#include "fbx/internal/fbx_scene_validate.h"
#include "fbx/aaa_fbx_mesh.h"
#include "fbx/aaa_fbx_material.h"
#include "fbx/aaa_fbx_texture.h"


FACTORY_CREATE_PROP_V1( c_bdd_mesh_static, bdd_mesh_static, FBX file, bdd_mesh_static, sub_menu = "Test"; );

namespace	n_bdd_mesh_static
{
	CONSTEXPR INT32		BASE_PARAM_NB		= 11 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32		TRANSFO_PARAM_NB	= 10;
	CONSTEXPR INT32		MESH_PARAM_NB		= 11;
	CONSTEXPR INT32		MATERIAL_PARAM_NB	= 25;
	CONSTEXPR INT32		GROUP_PARAM_NB		= 3;
	CONSTEXPR INT32		PARAM_NB_MAX	= BASE_PARAM_NB
										+ TRANSFO_PARAM_NB
										+ MESH_PARAM_NB
										+ MATERIAL_PARAM_NB
										+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_BOOL_ON(				use_geometric_transform	)
		PARAM_DEF_BOOL_OFF(				validate_at_load		)
		PARAM_DEF_BOOL_OFF(				verbose					)
		PARAM_DEF_BOOL_LOCKED(			fbx_loaded				)
		PARAM_DEF_BOOL_OFF_SAVE_NOT(	reload_trig				)
		PARAM_DEF_FILENAME(				filename,				aaa::file::TYPE_IO_FBX, 0)

		PARAM_DEF_BOOL_OFF(				use_absolute_path	)
		PARAM_DEF_INT32_LOCKED(			texture_nb			)
		PARAM_DEF_INT32_POS_ZERO(		texture_index		)
		PARAM_DEF_STR_LOCKED(			texture_path		)

		PARAM_DEF_GROUP_CLOSED( Transfo, TRANSFO_PARAM_NB )
			PARAM_DEF_POINT_FP32_XYZ(		translate		)
			PARAM_DEF_POINT_FP32_XYZ(		rotate			)
			PARAM_DEF_SCALE_FP32_XYZF(		scale			)
		
		PARAM_DEF_GROUP_CLOSED( Mesh, MESH_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(			mesh_nb			)
			PARAM_DEF_INT32_POS_ZERO(		mesh_index		)
			PARAM_DEF_STR_LOCKED(			mesh_name		)
			PARAM_DEF_INT32_LOCKED(			mesh_vertex_nb	)
			PARAM_DEF_INT32_LOCKED(			mesh_index_nb	)
			PARAM_DEF_FP32_LOCKED_XYZ(		mesh_bbox_min	)
			PARAM_DEF_FP32_LOCKED_XYZ(		mesh_bbox_max	)

//		PARAM_DEF_POINT_XYZ(			origin				)

		PARAM_DEF_BOOL_ON(				draw_mesh			)

		PARAM_DEF_GROUP_CLOSED( Material, MATERIAL_PARAM_NB)
			PARAM_DEF_STR_LOCKED(			material_name				)
			PARAM_DEF_INT32_LOCKED(			normal_texture_index		)
			PARAM_DEF_INT32_LOCKED(			bump_texture_index			)
			PARAM_DEF_COLOR_RGB_LOCKED(		ambient						)
			PARAM_DEF_INT32_LOCKED(			ambient_texture_index		)
			PARAM_DEF_COLOR_RGB_LOCKED(		diffuse						)
			PARAM_DEF_INT32_LOCKED(			diffuse_texture_index		)
			PARAM_DEF_COLOR_RGB_LOCKED(		specular					)
			PARAM_DEF_INT32_LOCKED(			specular_texture_index		)
			PARAM_DEF_INT32_LOCKED(			specular_factor_texture_index )
			PARAM_DEF_COLOR_RGB_LOCKED(		emissive					)
			PARAM_DEF_INT32_LOCKED(			emissive_texture_index		)
			PARAM_DEF_FP32_LOCKED(			opacity						)
			PARAM_DEF_INT32_LOCKED(			opacity_texture_index		)
			PARAM_DEF_FP32_LOCKED(			shininess					)
			PARAM_DEF_INT32_LOCKED(			shininess_texture_index		)
			PARAM_DEF_FP32_LOCKED(			reflectiviy					)
	};
}

CONSTRUCTOR_CREATE(c_bdd_mesh_static)
{
	init();
}

EMPTY_DESTRUCTOR(c_bdd_mesh_static)

void	c_bdd_mesh_static::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt(		h, _b_use_geometric_transform_ui);
	param_set_pt(		h, _b_validate_at_load_ui		);
	param_set_pt(		h, _b_verbose_ui				);
	param_set_pt(		h, _b_fbx_loaded_out			);
	param_set_pt(		h, _b_reload_trig_ui			);
	param_set_pt(		h, _filename_fbx_ui				);
	param_set_pt(		h, _b_use_absolute_path_ui		);
	param_set_pt(		h, _texture_nb_ui				);
	param_set_pt(		h, _texture_path_index_ui		);
	param_set_pt(		h, _texture_path_ui				);

	++h;
		param_set_pt_3(	h, _scene_translate_ui		);
		param_set_pt_3(	h, _scene_rotate_turn_ui	);
		param_set_pt_4(	h, _scene_scale_ui			);
	
	++h;
		param_set_pt(	h, _mesh_nb_ui				);
		param_set_pt(	h, _mesh_index_ui			);
		param_set_pt(	h, _mesh_name_ui			);
		param_set_pt(	h, _mesh_vertex_nb_ui		);
		param_set_pt(	h, _mesh_index_nb_ui		);
		param_set_pt_3(	h, _mesh_bbox_min_ui		);
		param_set_pt_3(	h, _mesh_bbox_max_ui		);

//	param_set_pt_3(	h, _origin_ui				);

	param_set_pt(		h, _b_draw_mesh_ui			);

	++h;
		param_set_pt(	h, _material_name_ui					);
		param_set_pt(	h, _material_normal_texture_idx_ui		);
		param_set_pt(	h, _material_bump_texture_idx_ui		);
		param_set_pt_3(	h, _material_ambient_color_ui			);
		param_set_pt(	h, _material_ambient_texture_idx_ui		);
		param_set_pt_3(	h, _material_diffuse_color_ui			);
		param_set_pt(	h, _material_diffuse_texture_idx_ui		);
		param_set_pt_3(	h, _material_specular_color_ui			);
		param_set_pt(	h, _material_specular_texture_idx_ui	);
		param_set_pt(	h, _material_specular_factor_texture_idx_ui );
		param_set_pt_3(	h, _material_emissive_color_ui			);
		param_set_pt(	h, _material_emissive_texture_idx_ui	);
		param_set_pt(	h, _material_opacity_ui					);
		param_set_pt(	h, _material_opacity_texture_idx_ui		);
		param_set_pt(	h, _material_shininess_ui				);
		param_set_pt(	h, _material_shininess_texture_idx_ui	);
		param_set_pt(	h, _material_reflectiviy_ui				);

	err_param_init_pt(h);
}

void c_bdd_mesh_static::dealloc()
{
	_fbx_scene.clear();

	// parent.
	//c_bdd_multiple::dealloc();
}

void c_bdd_mesh_static::release_data()
{
	_mesh_index_last = -1;
	_texture_path_index_cur	= -1;

	clear_v3( _mesh_bbox_min_ui );
	clear_v3( _mesh_bbox_max_ui );
	_mesh_name_ui.erase();
	_mesh_nb_ui		= 0;
	_texture_nb_ui	= 0;
	_fbx_scene.clear();
	_b_fbx_loaded_out = false;
}

void c_bdd_mesh_static::init()
{
	// locked params are set by the object and so should be initialized.
	_mesh_nb_ui		= 0;
	_texture_nb_ui	= 0;
	_b_fbx_loaded_out = false;

	// material locked parameters.
	clear_v3(_material_ambient_color_ui);
	clear_v3(_material_diffuse_color_ui);
	clear_v3(_material_specular_color_ui);
	clear_v3(_material_emissive_color_ui);
	_material_opacity_ui				= 0.0f;
	_material_shininess_ui				= 0.0f;
	_material_reflectiviy_ui			= 0.0f;
	_material_ambient_texture_idx_ui	= -1;
	_material_diffuse_texture_idx_ui	= -1;
	_material_specular_texture_idx_ui	= -1;
	_material_normal_texture_idx_ui		= -1;
	_material_emissive_texture_idx_ui	= -1;
	_material_shininess_texture_idx_ui	= -1;
	_material_specular_factor_texture_idx_ui = -1;
	_material_bump_texture_idx_ui		= -1;
	_material_opacity_texture_idx_ui	= -1;

	// ui.
	param_init_with( n_bdd_mesh_static::param, n_bdd_mesh_static::PARAM_NB_MAX );

	// load fbx.
	load_data();
}

AAA_ERR c_bdd_mesh_static::load_data()
{
	release_data();

	AAA_ERR res = ERR_ANY;
	if( !_filename_fbx_ui.is_empty() && c_file::is_exist(_filename_fbx_ui) ) 
	{
		// options.
		_fbx_scene.set_use_ssbo_mesh( false );
		_fbx_scene.set_do_validation(_b_validate_at_load_ui);
		_fbx_scene.set_verbose(_b_verbose_ui);
		_fbx_scene.set_use_absolute_path( _b_use_absolute_path_ui );
		_fbx_scene.set_apply_geometric_transform( _b_use_geometric_transform_ui );

		// load.
		if( _fbx_scene.load( _filename_fbx_ui.get() ) )
		{
			res = AAA_OK;
			_b_fbx_loaded_out = true;
			// propagate info to ui.
			_mesh_nb_ui		= _fbx_scene.get_mesh_nb();
			_texture_nb_ui	= _fbx_scene.get_texture_nb();

			if( _mesh_index_ui >= _mesh_nb_ui )
				_mesh_index_ui = _mesh_nb_ui - 1;

			if( _texture_path_index_ui >= _texture_nb_ui )
				_texture_path_index_ui = _texture_nb_ui - 1;
		}
	}
	return res;
}

AAA_ERR c_bdd_mesh_static::load_data_from_filename( o_str CONST & filename, INT32 type_io )
{
	if( type_io == aaa::file::TYPE_IO_FBX )
	{
		_filename_fbx_ui.set_fname_relative( filename );
		return load_data();
	}
	return ERR_TYPE_UNUSED;
}

AAA_ERR	c_bdd_mesh_static::load_do_after( o_str CONST & filename )
{
	_b_reload_trig_ui = true;
	return AAA_OK;
}

void c_bdd_mesh_static::update()
{
	if (_b_reload_trig_ui)
	{
		_b_reload_trig_ui = false;
		load_data();
	}
	
	if( _fbx_scene.is_loaded() )
	{
		// mesh idx.
		_mesh_index_ui = CLAMP( _mesh_index_ui, 0, _mesh_nb_ui-1 );
		// current mesh.
		if( _mesh_index_last != _mesh_index_ui )
		{
			_mesh_index_last = _mesh_index_ui;

			// mesh locked parameters.
			aaa::c_fbx_mesh const * mesh = _fbx_scene.get_mesh( _mesh_index_last );
			
			cpy_v3( _mesh_bbox_min_ui, mesh->get_bbox_min() );
			cpy_v3( _mesh_bbox_max_ui, mesh->get_bbox_max() );

			_mesh_name_ui = mesh->get_name();
			_mesh_vertex_nb_ui = mesh->get_vertex_nb();
			_mesh_index_nb_ui =  mesh->get_index_nb();

			// material locked parameters.
			aaa::c_fbx_material const * material = mesh->get_material();
			_material_name_ui = material->get_name();

			if( material->is_valid() )
			{
				_material_name_ui = material->get_name();
				material->get_ambient_color( _material_ambient_color_ui );
				material->get_diffuse_color( _material_diffuse_color_ui );
				material->get_specular_color( _material_specular_color_ui );
				material->get_emissive_color( _material_emissive_color_ui );
				_material_opacity_ui				= material->get_opacity_factor();
				_material_shininess_ui				= material->get_shininess_factor();
				_material_reflectiviy_ui			= material->get_reflectiviy_factor();
				_material_ambient_texture_idx_ui	= material->get_ambient_texture_index();
				_material_diffuse_texture_idx_ui	= material->get_diffuse_texture_index();
				_material_specular_texture_idx_ui	= material->get_specular_texture_index();
				_material_normal_texture_idx_ui		= material->get_normal_texture_index();
				_material_emissive_texture_idx_ui	= material->get_emissive_texture_index();
				_material_shininess_texture_idx_ui	= material->get_shininess_texture_index();
				_material_specular_factor_texture_idx_ui = material->get_specular_factor_texture_index();
				_material_bump_texture_idx_ui		= material->get_bump_texture_index();
				_material_opacity_texture_idx_ui	= material->get_opacity_texture_index();
			}
			else
			{
				_material_name_ui = "";
				clear_v3( _material_ambient_color_ui );
				clear_v3( _material_diffuse_color_ui );
				clear_v3( _material_specular_color_ui );
				clear_v3( _material_emissive_color_ui );
				_material_opacity_ui				= .0f;
				_material_shininess_ui				= .0f;
				_material_reflectiviy_ui			= .0f;
				_material_ambient_texture_idx_ui	= -1;
				_material_diffuse_texture_idx_ui	= -1;
				_material_specular_texture_idx_ui	= -1;
				_material_normal_texture_idx_ui		= -1;
				_material_emissive_texture_idx_ui	= -1;
				_material_shininess_texture_idx_ui	= -1;
				_material_specular_factor_texture_idx_ui = -1;
				_material_bump_texture_idx_ui		= -1;
				_material_opacity_texture_idx_ui	= -1;
			}
		}

		// texture idx.
		_texture_path_index_ui = CLAMP( _texture_path_index_ui, 0, _texture_nb_ui-1 );

		// current texture path.
		if( _texture_path_index_cur != _texture_path_index_ui )
		{
			_texture_path_index_cur = _texture_path_index_ui;
			aaa::c_fbx_texture const * tex = _fbx_scene.get_texture(_texture_path_index_cur);
			if( tex )
				_texture_path_ui.set( tex->get_path() );
			else
				_texture_path_ui.erase();
		}

		// fbx scene.
		FP32 sca[3];
		scale_v3( sca, _scene_scale_ui, _scene_scale_ui[3] );
		FP32 rot_rad[3];
		scale_v3( rot_rad, _scene_rotate_turn_ui, PI_TIME_2 );
		_fbx_scene.set_trs(	_scene_translate_ui, rot_rad, sca, true, true );
	}
}

void c_bdd_mesh_static::draw_mesh()
{
	//we don't use _mesh_index_last but _mesh_index_ui, because we only update once and draw multiple times from lua
	if( _fbx_scene.is_loaded() )
		_fbx_scene.draw_mesh_static( _mesh_index_ui );
}

void c_bdd_mesh_static::draw()
{
	if( _b_draw_mesh_ui )
		draw_mesh();
}

//-----------------------------------------------------------------------------


INT32  c_bdd_mesh_static::get_point_dataset_nb()
{
	return _mesh_nb_ui;
}
bool c_bdd_mesh_static::set_point_dataset( INT32 dataset_id )
{
	bool res = false;
	if( _b_fbx_loaded_out )
	{
		if (dataset_id < _mesh_nb_ui)
		{
			_mesh_index_ui = dataset_id;
			res = true;
		}
		else
			ERR_PRINT_STRING( "Requested dataset ID is out of bounds, dataset number is %i.", _mesh_nb_ui );
	}
	return res;
}

INT32 c_bdd_mesh_static::get_point_nb()
{
	INT32 res = -1;
	if( _b_fbx_loaded_out )
	{
		auto const * mesh = _fbx_scene.get_mesh(_mesh_index_ui);
		if( mesh ) 
			res = mesh->get_vertex_nb();
	}
	return res;
}

REAL * c_bdd_mesh_static::get_points()
{
	REAL * res = nullptr;
	if( _b_fbx_loaded_out )
	{
		auto const* mesh = _fbx_scene.get_mesh(_mesh_index_ui);
		if( mesh ) 
			res = mesh->get_vertex_positions();
	}
	return res;
}

REAL * c_bdd_mesh_static::get_normals()
{
	REAL * res = nullptr;
	if( _b_fbx_loaded_out )
	{
		auto const * mesh = _fbx_scene.get_mesh(_mesh_index_ui);
		if( mesh ) 
			res = mesh->get_vertex_normals();
	}
	return res;
}

glm::mat4 c_bdd_mesh_static::get_transform_matrix() CONST
{
	glm::mat4 res;
	if( _b_fbx_loaded_out )
	{
		auto const * mesh = _fbx_scene.get_mesh(_mesh_index_ui);
		if( mesh )
			res = *(mesh->get_pose()->get_root());
	}
	return res;
}

//-----------------------------------------------------------------------------

