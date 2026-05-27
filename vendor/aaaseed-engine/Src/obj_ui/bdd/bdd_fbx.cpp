
// Main header.
#include "bdd_fbx.h"

#include "draw/model.h"
#include "draw/axe.h"
#include "infrastructure/seedfile.h"
#include "gol/gol_matrix.h"
#include "gol/gol_color.h"
#include "fbx/internal/fbxloader.h"
#include "spy.h"
#include "fbx/internal/fbx_utils_mem.h"
#include "fbx/internal/fbx_scene_process.h"
#include "fbx/internal/fbx_scene_validate.h"
#include "fbx/aaa_fbx_material.h"
#include "fbx/aaa_fbx_texture.h"


FACTORY_CREATE_PROP_V1( c_bdd_fbx, bdd_fbx, FBX file with animation, bdd_fbx, sub_menu="Test"; );

namespace	n_bdd_fbx
{
	CONSTEXPR INT32 BASE_PARAM_NB		=	23 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 TRANSFO_PARAM_NB	=	15;
	CONSTEXPR INT32 MESH_PARAM_NB		=	11;
	CONSTEXPR INT32 POSITION_PARAM_NB	=	7;
	CONSTEXPR INT32 DRAW_PARAM_NB		=   10;
	CONSTEXPR INT32 BLEND_PARAM_NB		=   MAX_BLEND_SHAPE_CHANNELS * 2 + 4;
	CONSTEXPR INT32 INFO_PARAM_NB		=	7;
	CONSTEXPR INT32 MATERIAL_PARAM_NB	=  25;
	CONSTEXPR INT32 GROUP_PARAM_NB		=	7;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
									+ TRANSFO_PARAM_NB
									+ MESH_PARAM_NB
									+ POSITION_PARAM_NB
									+ DRAW_PARAM_NB
									+ BLEND_PARAM_NB
									+ INFO_PARAM_NB
									+ MATERIAL_PARAM_NB
									+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(				use_sdk_evaluator	)
		PARAM_DEF_BOOL_ON(				use_geometric_transform	)
		PARAM_DEF_BOOL_OFF(				validate_at_load	)
		PARAM_DEF_BOOL_OFF(				verbose				)
		PARAM_DEF_BOOL_LOCKED(			fbx_loaded			)
		PARAM_DEF_BOOL_OFF_SAVE_NOT(	reload_trig			)
		PARAM_DEF_FILENAME(				filename,			aaa::file::TYPE_IO_FBX, 0		)
		PARAM_DEF_BOOL_OFF(				use_absolute_path	)

		PARAM_DEF_GROUP_CLOSED(	Transformation, TRANSFO_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			transform_active	)
			PARAM_DEF_POINT_FP32_XYZ(	translate			)
			PARAM_DEF_POINT_FP32_XYZ(	rotate				)
			PARAM_DEF_SCALE_FP32_XYZF(	scale				)
			PARAM_DEF_BOOL_OFF(			recenter			)
			PARAM_DEF_POINT_FP32_XYZ(	recenter_position	)

		PARAM_DEF_GROUP_CLOSED(	Mesh, MESH_PARAM_NB	)
			PARAM_DEF_INT32_LOCKED(		mesh_nb					)
			PARAM_DEF_INT32_POS_ZERO(	mesh_index				)
			PARAM_DEF_STR_LOCKED(		mesh_name				)
			PARAM_DEF_INT32_LOCKED(		mesh_vertex_nb			)
			PARAM_DEF_INT32_LOCKED(		mesh_index_nb			)
			PARAM_DEF_FP32_LOCKED_XYZ(	mesh_bbox_min			)
			PARAM_DEF_FP32_LOCKED_XYZ(	mesh_bbox_max			)

		PARAM_DEF_GROUP_CLOSED(	Position, POSITION_PARAM_NB	)
			PARAM_DEF_FP32_LOCKED_XYZ(	position				)
			PARAM_DEF_BOOL_OFF(			get_rotation			)
			PARAM_DEF_FP32_LOCKED_XYZ(	rotation				)

		PARAM_DEF_GROUP(			Draw, DRAW_PARAM_NB	)
			PARAM_DEF_BOOL_ON(			draw_mesh				)
			PARAM_DEF_BOOL_ON(			draw_mesh_by_index		)
			PARAM_DEF_BOOL_OFF(			draw_debug				)
			PARAM_DEF_BOOL_OFF(			draw_position			)
			PARAM_DEF_FP32_ONE(			draw_position_size		)
			PARAM_DEF_BOOL_OFF(			draw_curve				)
			PARAM_DEF_BOOL_OFF(			draw_bone				)
			PARAM_DEF_FP32_ONE(			draw_bone_size			)
			PARAM_DEF_INT32_POS_ZERO(	draw_bone_begin_index	)
			PARAM_DEF_INT32_POS(		draw_bone_end_index,	0, 256 )

		PARAM_DEF_INT32_LOCKED(		animation_nb			)
		PARAM_DEF_INT32_POS_ZERO(	animation_index			)
		PARAM_DEF_STR_LOCKED(		animation_name			)
		PARAM_DEF_BOOL_OFF(			transformation_update	)
		PARAM_DEF_BOOL_ON(			time_update				)
		PARAM_DEF_FP32_LOCKED(		time_begin				)
		PARAM_DEF_FP32_LOCKED(		time_end				)
		PARAM_DEF_BOOL_ON(			Play					)
		PARAM_DEF_FP32_POS_ONE(		Play_time_factor		)
		PARAM_DEF_BOOL_ON(			Play_Loop				)
		PARAM_DEF_FP32_ONE(			Time					)
		PARAM_DEF_BOOL_ON(			Interpolate				)

		PARAM_DEF_GROUP_CLOSED( Blend, BLEND_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			blend_shape_active		)
			PARAM_DEF_INT32_POS_ZERO(	blend_shape_index		)
			PARAM_DEF_INT32_LOCKED(		blend_shape_nb			)
			PARAM_DEF_INT32_LOCKED(		blend_shape_channel_nb	)
#if MAX_BLEND_SHAPE_CHANNELS != 8
#	error lines below depend on this
#endif
			PARAM_DEF_BOOL_OFF(		blend_shape_1_manual )
			PARAM_DEF_FP32_ZERO(	blend_shape_1_factor )
			PARAM_DEF_BOOL_OFF(		blend_shape_2_manual )
			PARAM_DEF_FP32_ZERO(	blend_shape_2_factor )
			PARAM_DEF_BOOL_OFF(		blend_shape_3_manual )
			PARAM_DEF_FP32_ZERO(	blend_shape_3_factor )
			PARAM_DEF_BOOL_OFF(		blend_shape_4_manual )
			PARAM_DEF_FP32_ZERO(	blend_shape_4_factor )
			PARAM_DEF_BOOL_OFF(		blend_shape_5_manual )
			PARAM_DEF_FP32_ZERO(	blend_shape_5_factor )
			PARAM_DEF_BOOL_OFF(		blend_shape_6_manual )
			PARAM_DEF_FP32_ZERO(	blend_shape_6_factor )
			PARAM_DEF_BOOL_OFF(		blend_shape_7_manual )
			PARAM_DEF_FP32_ZERO(	blend_shape_7_factor )
			PARAM_DEF_BOOL_OFF(		blend_shape_8_manual )
			PARAM_DEF_FP32_ZERO(	blend_shape_8_factor )

		PARAM_DEF_GROUP_CLOSED( Info, INFO_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(		node_nb			 )
			PARAM_DEF_INT32_LOCKED(		track_nb		 )
			PARAM_DEF_INT32_LOCKED(		keyframe_nb		 )
			PARAM_DEF_FP32_LOCKED(		time_node_begin	 )
			PARAM_DEF_FP32_LOCKED(		time_node_end	 )
			PARAM_DEF_FP32_LOCKED(		time_shape_begin )
			PARAM_DEF_FP32_LOCKED(		time_shape_end	 )

		PARAM_DEF_INT32_LOCKED(		texture_nb			)
		PARAM_DEF_INT32_POS_ZERO(	texture_index		)
		PARAM_DEF_STR_LOCKED(		texture_path		)
//todo this could be shared by the bdd_fbx...
		PARAM_DEF_GROUP_CLOSED( Material, MATERIAL_PARAM_NB)
			PARAM_DEF_STR_LOCKED(		material_name			)
			PARAM_DEF_INT32_LOCKED(		normal_texture_index	)
			PARAM_DEF_INT32_LOCKED(		bump_texture_index		)
			PARAM_DEF_COLOR_RGB_LOCKED(	ambient					)
			PARAM_DEF_INT32_LOCKED(		ambient_texture_index	)
			PARAM_DEF_COLOR_RGB_LOCKED(	diffuse					)
			PARAM_DEF_INT32_LOCKED(		diffuse_texture_index	)
			PARAM_DEF_COLOR_RGB_LOCKED(	specular				)
			PARAM_DEF_INT32_LOCKED(		specular_texture_index	)
			PARAM_DEF_INT32_LOCKED(		specular_factor_texture_index	)
			PARAM_DEF_COLOR_RGB_LOCKED(	emissive				)
			PARAM_DEF_INT32_LOCKED(		emissive_texture_index	)
			PARAM_DEF_FP32_LOCKED(		opacity					)
			PARAM_DEF_INT32_LOCKED(		opacity_texture_index	)
			PARAM_DEF_FP32_LOCKED(		shininess				)
			PARAM_DEF_INT32_LOCKED(		shininess_texture_index	)
			PARAM_DEF_FP32_LOCKED(		reflectiviy				)
	};
}

void	c_bdd_fbx::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt(	h, _b_use_sdk_evaluator_ui	        );
	param_set_pt(	h, _b_use_geometric_transform_ui	);
	param_set_pt(	h, _b_validate_at_load_ui	        );
	param_set_pt(	h, _b_verbose_ui			        );
	param_set_pt(	h, _b_fbx_loaded_ui			        );
	param_set_pt(	h, _b_reload_trig_ui		        );
	param_set_pt(	h, _filename_fbx_ui			        );
	param_set_pt(	h, _b_use_absolute_path_ui	        );

	++h;
		param_set_pt(	h, _b_scene_transform_ui	);
		param_set_pt_3(	h, _scene_translate_ui		);
		param_set_pt_3(	h, _scene_rotate_turn_ui	);
		param_set_pt_4(	h, _scene_scale_ui			);
		param_set_pt(	h, _b_scene_recenter_ui		);
		param_set_pt_3(	h, _scene_center_ui			);

	++h;
		param_set_pt(	h, _mesh_nb_ui			);
		param_set_pt(	h, _mesh_index_ui		);
		param_set_pt(	h, _mesh_name_ui		);
		param_set_pt(	h, _mesh_vertex_nb_ui	);
		param_set_pt(	h, _mesh_index_nb_ui	);
		param_set_pt_3(	h, _mesh_bbox_min_ui	);
		param_set_pt_3(	h, _mesh_bbox_max_ui	);

	++h;
		param_set_pt_3(	h, _position_ui					);
		param_set_pt(	h, _b_get_global_rotation_ui	);
		param_set_pt_3(	h, _rotation_ui					);

	++h;
		param_set_pt(	h, _b_draw_mesh_ui			);
		param_set_pt(	h, _b_draw_by_index_ui		);
		param_set_pt(	h, _b_draw_debug_ui			);
		param_set_pt(	h, _b_draw_position_ui		);
		param_set_pt(	h, _draw_position_size_ui	);
		param_set_pt(	h, _b_draw_curve_ui			);
		param_set_pt(	h, _b_draw_bone_ui			);
		param_set_pt(	h, _draw_bone_size_ui		);
		param_set_pt(	h, _draw_bone_begin_idx_ui	);
		param_set_pt(	h, _draw_bone_end_idx_ui	);

	param_set_pt(	h, _stack_nb_ui					);
	param_set_pt(	h, _animation_index_ui			);
	param_set_pt(	h, _animation_name_ui			);
	param_set_pt(	h, _b_transform_update_ui		);
	param_set_pt(	h, _b_time_update_ui			);
	param_set_pt(	h, _time_begin_ui				);
	param_set_pt(	h, _time_end_ui					);
	param_set_pt(	h, _b_play_ui					);
	param_set_pt(	h, _time_factor_ui				);
	param_set_pt(	h, _b_loop_ui					);
	param_set_pt(	h, _time_ui						);
	param_set_pt(	h, _b_linear_ui					);

	++h;
		param_set_pt(	h, _blend_shape_active_ui	);
		param_set_pt(	h, _blend_shape_idx_ui		);
		param_set_pt(	h, _mesh_blend_shape_nb_ui);
		param_set_pt(	h, _mesh_blend_shape_channel_nb_ui);
		for( INT32 i=0; i<MAX_BLEND_SHAPE_CHANNELS; ++i )
		{
			param_set_pt(	h, _blend_shape_manual_ui[i]	);
			param_set_pt(	h, _blend_shape_factor_ui[i]	);
		}

	++h;
		param_set_pt(	h, _node_nb_ui				    );
		param_set_pt(	h, _track_nb_ui				    );
		param_set_pt(	h, _keyframe_nb_ui			    );
		param_set_pt(	h, _time_node_begin_ui		    );
		param_set_pt(	h, _time_node_end_ui		    );
		param_set_pt(	h, _time_blend_shape_begin_ui	);
		param_set_pt(	h, _time_blend_shape_end_ui		);

	param_set_pt(	h, _texture_nb_ui			);
	param_set_pt(	h, _texture_path_index_ui	);
	param_set_pt(	h, _texture_path_ui			);

//todo this could be shared by the bdd_fbx...
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

void c_bdd_fbx::dealloc()
{
	release_data();

	// parent.
	//c_bdd_multiple::dealloc();
}

void c_bdd_fbx::reset_cache()
{
	_animation_index_cur	= -1;
	_mesh_index_cur			= -1;
	_texture_path_index_cur	= -1;
	_b_need_position		= true;
}

void c_bdd_fbx::reset_data()
{
	reset_cache();
	clear_v3( _mesh_bbox_min_ui );
	clear_v3( _mesh_bbox_max_ui );
	init_bool_to_invalid( _b_scene_transform_cur );
	_mesh_name_ui.erase();
}

//todo same code is in bdd_mesh_static
void c_bdd_fbx::release_data()
{
	reset_data();
	_fbx_scene.clear();
	_b_fbx_loaded_ui = false;
}

void c_bdd_fbx::init()
{
	// locked params are set by the object and so should be initialised
	clear_v3( _position_ui );
	clear_v3( _rotation_ui );
	_mesh_nb_ui			          = 0;
	_texture_nb_ui		          = 0;
	_stack_nb_ui		          = 0;	
	_node_nb_ui			          = 0;
	_track_nb_ui		          = 0;
	_keyframe_nb_ui		          = 0;
	_time_begin_ui		          = 0.0f;
	_time_end_ui		          = 0.0f;
	_time_node_begin_ui		      = 0.0f;
	_time_node_end_ui		      = 0.0f;
	_time_blend_shape_begin_ui	  = 0.0f;
	_time_blend_shape_end_ui	  = 0.0f;
	_animation_index_ui           = 0;
	_blend_shape_idx_ui           = 0;
	_blend_shape_idx	          = -1;
	_b_fbx_loaded_ui	          = false;
	clear_vf( _blend_shape_factor_ui, ARRAY_SIZE(_blend_shape_factor_ui) );
	// material locked parameters.
	clear_v3( _material_ambient_color_ui	);
	clear_v3( _material_diffuse_color_ui	);
	clear_v3( _material_specular_color_ui	);
	clear_v3( _material_emissive_color_ui	);
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
	
	reset_data();

	// ui.
	param_init_with( n_bdd_fbx::param, n_bdd_fbx::PARAM_NB_MAX );

	// load file if any.
	// No: we don't load the data at init (bad practice in AAA)
	//  and the params are not loaded so we are not set  
}

CONSTRUCTOR_CREATE(c_bdd_fbx)
{
	init();
}

EMPTY_DESTRUCTOR(c_bdd_fbx)

namespace {
	C_PCHAR_C get_axis_name( fbx_axis axe, fbx_axis_direction direction )
	{
		C_PCHAR str;
		switch( axe )
		{
		case fbx_axis::x : str = (direction == fbx_axis_direction::positive) ? "+x" : "-x";		break;
		case fbx_axis::y : str = (direction == fbx_axis_direction::positive) ? "+y" : "-y";		break;
		case fbx_axis::z : str = (direction == fbx_axis_direction::positive) ? "+z" : "-z";		break;
		}
		return str;
	}
}

AAA_ERR	c_bdd_fbx::load_data()
{
	AAA_ERR retcode = ERR_ANY;
	if( !_filename_fbx_ui.is_empty() )
	{
		if( c_file::is_exist(_filename_fbx_ui) )
		{
			release_data();
			// options.
			_fbx_scene.set_use_ssbo_mesh( true );
			_fbx_scene.set_do_validation( _b_validate_at_load_ui );
			_fbx_scene.set_verbose( _b_verbose_ui );
			_fbx_scene.set_use_absolute_path( _b_use_absolute_path_ui );
			_fbx_scene.set_use_sdk_evaluator( _b_use_sdk_evaluator_ui );
			_fbx_scene.set_apply_geometric_transform( _b_use_geometric_transform_ui );
		
			// load.
			if( _fbx_scene.load( _filename_fbx_ui.get() ) )
			{
				retcode = AAA_OK;
				_b_fbx_loaded_ui = true;

				// propagate info to ui.
				_node_nb_ui		= _fbx_scene.get_node_nb();
				_mesh_nb_ui		= _fbx_scene.get_mesh_nb();
				_texture_nb_ui	= _fbx_scene.get_texture_nb();
				_stack_nb_ui	= _fbx_scene.get_stack_nb();
				_track_nb_ui	= _fbx_scene.get_track_nb();
				_keyframe_nb_ui	= _fbx_scene.get_keyframe_nb();

				//todo : should be from animation index
				_time_begin_ui = 0.0f;
				_time_end_ui = 0.0f;
				_time_node_begin_ui = 0.0f;
				_time_node_end_ui = 0.0f;
				_time_blend_shape_begin_ui = 0.0f;
				_time_blend_shape_end_ui = 0.0f;

				_mesh_index_ui			= CLAMP( _mesh_index_ui, 0, _mesh_nb_ui-1 ) ;
				_texture_path_index_ui	= CLAMP( _texture_path_index_ui, 0, _texture_nb_ui-1 );
			}
		}
		else
			retcode = ERR_FILE_NO;
	}
	return retcode;
}

AAA_ERR c_bdd_fbx::load_data_from_filename( o_str CONST & filename, INT32 type_io )
{
	if( type_io == aaa::file::TYPE_IO_FBX )
	{
		_filename_fbx_ui.set_fname_relative( filename );
		load_data();
		return AAA_OK;
	}
	return ERR_TYPE_UNUSED;
}

AAA_ERR	c_bdd_fbx::load_do_after( o_str CONST & filename )
{
	_b_reload_trig_ui = true;
	return AAA_OK;
}

void c_bdd_fbx::update_time( FP32 time )
{
	if( _time != time )
	{
		_time = time;
		_b_need_position = true;
		SPY_PUSH_RANGE_OBJ( "c_bdd_fbx::update_time", spy::COL_1 );
			// evaluator.
			_fbx_scene.set_use_sdk_evaluator( _b_use_sdk_evaluator_ui );
			_fbx_scene.set_apply_geometric_transform( _b_use_geometric_transform_ui );

			//can set additional matrices here
			glm::mat4* post_local_transforms = nullptr;
			uint32_t* post_local_transform_indices = nullptr;
			uint32_t post_local_transform_nb = 0;
			_fbx_scene.update( time , _animation_index_ui, _b_linear_ui, post_local_transforms,post_local_transform_indices,post_local_transform_nb);	//todo probably need to clamp to valid
		SPY_POP_RANGE2();
	}
}

void c_bdd_fbx::update()
{
	if( _b_reload_trig_ui )
	{
		_b_reload_trig_ui = false;
		load_data();
	}

	if( _fbx_scene.is_loaded() )
	{
		// mesh idx.
		if( _mesh_index_ui >= _mesh_nb_ui )
			_mesh_index_ui = _mesh_nb_ui - 1;
		// current mesh.
		if( _mesh_index_cur != _mesh_index_ui ) 
		{
			reset_cache();
			_mesh_index_cur = _mesh_index_ui;

			// mesh locked parameters.
			aaa::c_fbx_mesh const * mesh = _fbx_scene.get_mesh( _mesh_index_cur );

			cpy_v3( _mesh_bbox_min_ui, mesh->get_bbox_min() );
			cpy_v3( _mesh_bbox_max_ui, mesh->get_bbox_max() );

			_mesh_name_ui = mesh->get_name();
			_mesh_vertex_nb_ui = mesh->get_vertex_nb();
			_mesh_index_nb_ui =  mesh->get_index_nb();

			_mesh_blend_shape_nb_ui = mesh->get_blend_shape_nb();
			_mesh_blend_shape_channel_nb_ui = mesh->get_blend_shape_channel_nb(static_cast<UINT32>(_blend_shape_idx_ui));

//todo this could be shared by the bdd_fbx...
//and eventually the param could point to material members avoid copies and duplication of information
			// material locked parameters.
			aaa::c_fbx_material const * material = mesh->get_material();
			
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

		// fbx scene transform
#if 0
		if( _b_scene_transform_ui )
		{
			FP32 sca[3];
			scale_v3( sca, _scene_scale_ui, _scene_scale_ui[3] );
			FP32 rot_rad[3];
			scale_v3( rot_rad, _scene_rotate_turn_ui, PI_TIME_2 );
			_fbx_scene.set_trs( _scene_translate_ui, rot_rad, sca, true, true );
		}
		else
		{
			_fbx_scene.set_identity( true, true );
		}
		_b_need_position = true;
#else
//todo this is ugly we should cache a state of identity in _fbx_scene so when we call set_identity we do the minimum 
//	faster but buggy...
		if( ( _b_scene_transform_cur != _b_scene_transform_ui ) || _b_transform_update_ui )
		{	
			_b_scene_transform_cur = _b_scene_transform_ui;
			if( !_b_scene_transform_cur )	// we need to set it to identity once at least )
			{
				_fbx_scene.set_identity( true, true );
				_b_need_position = true;
			}
		}

		if( _b_scene_transform_cur )
		{
			FP32 sca[3];
			scale_v3( sca, _scene_scale_ui, _scene_scale_ui[3] );
			FP32 rot_rad[3];
			scale_v3( rot_rad, _scene_rotate_turn_ui, PI_TIME_2 );
			if( _b_scene_recenter_ui )
			{
				auto mat = glm::translate( aaa::matrix::identity, glm::vec3( _scene_translate_ui[ 0 ], _scene_translate_ui[ 1 ], _scene_translate_ui[ 2 ] ) );
				if( rot_rad[ 2 ] != .0f )
					mat = glm::rotate( mat, rot_rad[ 2 ], glm::vec3( .0f, .0f, 1.0f ) );
				if( rot_rad[ 0 ] != .0f )
					mat = glm::rotate( mat, rot_rad[ 0 ], glm::vec3( 1.f, .0f, .0f ) );
				if( rot_rad[ 1 ] != .0f )
					mat = glm::rotate( mat, rot_rad[ 1 ], glm::vec3( .0f, 1.0f, .0f ) );
				mat = glm::scale( mat, glm::vec3( sca[ 0 ], sca[ 1 ], sca[ 2 ] ) );
				mat = glm::translate( mat, glm::vec3( -_scene_center_ui[ 0 ], -_scene_center_ui[ 1 ], -_scene_center_ui[ 2 ] ) );
				_fbx_scene.set_trs( mat, true, true );
			}
			else
			{
				_fbx_scene.set_trs( _scene_translate_ui, rot_rad, sca, true, true );
			}
			_b_need_position = true;
		}
#endif

		//time
		if( _animation_index_cur != _animation_index_ui )
		{
			_animation_index_cur = _animation_index_ui;
			_time_begin_ui		= _fbx_scene.get_animation_time_begin(	_animation_index_ui );	//todo probably need to clamp to valid
			_time_end_ui		= _fbx_scene.get_animation_time_end(	_animation_index_ui );
			
			_time_node_begin_ui = _fbx_scene.get_animation_node_time_begin(	_animation_index_ui );
			_time_node_end_ui	= _fbx_scene.get_animation_node_time_end(	_animation_index_ui );
			
			_time_blend_shape_begin_ui = _fbx_scene.get_animation_blend_shape_time_begin(	_animation_index_ui );
			_time_blend_shape_end_ui   = _fbx_scene.get_animation_blend_shape_time_end(  	_animation_index_ui );

			_animation_name_ui	= _fbx_scene.get_animation_name(		_animation_index_ui );
		}
		
		if( _b_time_update_ui )
		{
			FP32 time = _time_ui;
			
			if( _b_play_ui )
			{
				_delta_t.update();
				time = REAL(time + _delta_t.get_dt() * _time_factor_ui);
			}

			_time_ui = _fbx_scene.get_animation_play_time( _animation_index_ui, time, _b_loop_ui ? animation_sampler_mode::wrap : animation_sampler_mode::clamp );
			update_time( _time_ui );
		}

		//set blend factor
		if( _blend_shape_active_ui )
		{
			if( _blend_shape_idx != _blend_shape_idx_ui )
			{
				// update blend shape channel nb
				_blend_shape_idx = _blend_shape_idx_ui;
				aaa::c_fbx_mesh const* mesh = _fbx_scene.get_mesh(_mesh_index_cur);
				_mesh_blend_shape_channel_nb_ui = mesh->get_blend_shape_channel_nb(static_cast<UINT32>(_blend_shape_idx_ui));
			}
			for (uint32_t i = 0; i < MAX_BLEND_SHAPE_CHANNELS; i++)
			{
				if( !_blend_shape_manual_ui[i] )
				{
					//try to get the curve
					int curve_idx = fbx_find_blend_shape_curve
					(
						_fbx_scene.get_fbx_scene_data(),
						_animation_index_ui,
						_mesh_index_ui,
						_blend_shape_idx_ui,
						i
					);

					if( curve_idx > -1 )
					{
						_blend_shape_factor_ui[i] = get_curve_animation_value
						(
							_fbx_scene.get_fbx_scene_data(),
							&_fbx_scene.get_fbx_scene_data()->animation_curves[curve_idx],
							_time_ui,
							true
						) * 0.01f; //animation value from sdk is from 0 -> 100
					}
				}
			}
			_fbx_scene.set_mesh_blend_shape_factors( _mesh_index_cur, _blend_shape_idx_ui, _blend_shape_factor_ui );
		}


		//set position field
		if( _b_need_position )
		{
			_b_need_position = false;
			_fbx_scene.get_root_armature_global_position( _position_ui );
			if( _b_get_global_rotation_ui )
				_fbx_scene.get_root_armature_global_rotation( _rotation_ui );
		}

		// texture idx.
		_texture_path_index_ui = CLAMP( _texture_path_index_ui, 0, _texture_nb_ui-1 );
		// current texture path.
		if( _texture_path_index_cur != _texture_path_index_ui )
		{
			_texture_path_index_cur = _texture_path_index_ui;
			aaa::c_fbx_texture const * tex = _fbx_scene.get_texture(_texture_path_index_cur);
			if (tex)
				_texture_path_ui = tex->get_path();
			else
				_texture_path_ui = "";
		}
	}
}

void c_bdd_fbx::draw_mesh()
{
	if( _b_draw_by_index_ui )
	{
		if( _mesh_index_ui < _mesh_nb_ui )
			_fbx_scene.draw_mesh( _mesh_index_ui );
	}
	else
	{
		_fbx_scene.draw_meshes();
	}
}

namespace {
	void draw_bone_recurse( aaa::c_fbx_scene CONST * scene, UINT32 CONST idx, FP32 CONST size)
	{
		if( glm::mat4 CONST * CONST mat = scene->get_node_global_transform(idx) )
		{
			GOL::matrix::push();
			GOL::matrix::mul_matrix(mat);
			n_axe::draw_orientation(size);
			GOL::matrix::pop();
		}

		fbx_node* node_tbl = scene->get_fbx_scene_data()->nodes;
		fbx_node* node_cur = &node_tbl[idx];
		UINT32 CONST child_nb = node_cur->child_count;
		for( UINT32 i = 0; i < child_nb; i++ )
		{
			UINT32 child_idx = i + node_cur->child_first_index_location;
			draw_bone_recurse( scene, child_idx, size );
		}
	}
};

void c_bdd_fbx::draw_bone( FP32 CONST size )
{
	if( _draw_bone_begin_idx_ui >= 0 )
	{
		int CONST draw_nb = _draw_bone_end_idx_ui - _draw_bone_begin_idx_ui + 1;
		if( draw_nb > 0 ) // draw the range
		{
			for( UINT32 i = 0; i < static_cast<UINT32>(draw_nb); ++i )
			{
				if( glm::mat4 CONST * CONST mat = _fbx_scene.get_node_global_transform( _draw_bone_begin_idx_ui + i ) )
				{
					GOL::matrix::push();
						GOL::matrix::mul_matrix(mat);
						n_axe::draw_orientation(size);
					GOL::matrix::pop();
				}
			}
		}
		else //draw children
			draw_bone_recurse( &_fbx_scene, _draw_bone_begin_idx_ui, size );
	}
}

void c_bdd_fbx::draw_curve()
{
	_fbx_scene.draw_curves();
}

void c_bdd_fbx::draw()
{
	if( !_fbx_scene.is_loaded() )
		return;

	if( _b_draw_mesh_ui )
		draw_mesh();

	//was cached in update but this a bad practice we should extend with draw_gizmoz/visu/ui for all bdds 
	if( _b_draw_debug_ui && ( _b_draw_curve_ui || _b_draw_bone_ui || _b_draw_position_ui ) )
	{
		c_draw_ui_guard guard;
	
		if( _b_draw_curve_ui )
		{
			GOL::color_white();
			draw_curve();
		}
		if( _b_draw_bone_ui )
		{
			draw_bone( _draw_bone_size_ui );
		}
		if( _b_draw_position_ui )
		{
			GOL::color_white();
			n_axe::draw_null_3d( _position_ui, _draw_position_size_ui );
		}
	}
}

