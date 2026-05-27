
// main header.
#include "err.h"
#include "fbx/aaa_fbx_scene.h"

#include "fbx/internal/fbxloader.h"
#include "fbx/internal/fbx_scene_process.h"
#include "fbx/internal/fbx_scene_validate.h"
#include "fbx/internal/fbx_utils_mem.h"

#include "fbx/aaa_fbx_graph.h"
#include "fbx/aaa_fbx_animation_curve.h"
#include "fbx/aaa_fbx_mesh.h"
#include "fbx/aaa_fbx_texture.h"

#include "fbx/internal/fbx_utils.h"
#include "spy.h"

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/matrix_decompose.hpp>

#ifndef AAA_AAA_MATH_H	// we need it depending on AAA_USE_VECTOR()
#	include "math/aaa_math.h"
#endif

//-----------------------------------------------------------------------------

aaa::c_fbx_scene::c_fbx_scene()
	: _graph							{}
	, _meshes							{}
	, _b_loaded							{ false }
	, _ubo_matrix						{ nullptr }
	, _matrix							{ aaa::matrix::identity }
	, _translate						{ 0.f, 0.f, 0.f }
	, _rotate							{ 0.f, 0.f, 0.f }
	, _scale							{ 1.f, 1.f, 1.f }
	, _scene_load_options				{}
	, _scene_data_load_options			{}
	, _scene_result						{}
	, _scene_data						{}
	, _node_nb							{ 0 }
	, _mesh_nb							{ 0 }
	, _texture_nb						{ 0 }
	, _stack_nb							{ 0 }
	, _track_nb							{ 0 }
	, _keyframe_nb						{ 0 }
	, _b_verbose						{ true }
	, _b_do_validate					{ true }
	, _b_use_ssbo_mesh					{ true }
	, _b_use_sdk_evaluator				{ false }
	, _b_use_absolute_path				{ true }
	, _root_armature_global_transform	{ nullptr }
{
	init_fbx_data();
}

aaa::c_fbx_scene::~c_fbx_scene()
{
	clear();
}

//-----------------------------------------------------------------------------
void aaa::c_fbx_scene::init_fbx_data()
{
	fbx_init_scene_load_options(	&_scene_load_options	);
	fbx_init_scene_load_result(		&_scene_result			);
	fbx_init_scene_data(			&_scene_data			);
}

void aaa::c_fbx_scene::release_fbx_data()
{
	_node_nb		= 0;
	_mesh_nb		= 0;
	_stack_nb		= 0;
	_track_nb		= 0;
	_keyframe_nb	= 0;

	fbx_release_scene(_scene_data);
	fbx_release_scene_native(_scene_result.scene);
}

//-----------------------------------------------------------------------------
aaa::c_fbx_mesh CONST * aaa::c_fbx_scene::get_mesh( INT32 CONST index ) CONST
{
	if( 0 <= index && index < _meshes.size() )
		return _meshes[index].get();
	return nullptr;
}

aaa::c_fbx_texture CONST * aaa::c_fbx_scene::get_texture( INT32 CONST index ) CONST
{
	if( 0 <= index && index < _texture_nb )
		return _textures[index].get();
	return nullptr;
}

//-----------------------------------------------------------------------------
C_PCHAR_C fbx_get_axis_name( fbx_axis axe, fbx_axis_direction direction )
{
	C_PCHAR str;
	switch(axe)
	{
	case fbx_axis::x: str = (direction == fbx_axis_direction::positive) ? "+x" : "-x";	break;
	case fbx_axis::y: str = (direction == fbx_axis_direction::positive) ? "+y" : "-y";	break;
	case fbx_axis::z: str = (direction == fbx_axis_direction::positive) ? "+z" : "-z";	break;
	}
	return str;
}

bool aaa::c_fbx_scene::load( C_PCHAR_C path )
{
	// clear previous if any.
	clear();
	init_fbx_data();

	// debug print.
	DBG_PRINT_STRING("Loading FBX file \"%.512s\".", path );

	// scene load options.
	_scene_data_load_options.animation_mode				= fbx_animation_import_mode::keyframes_fixed_timestep;
	_scene_data_load_options.animation_time_step		= 1.0f / 60.0f;
	_scene_data_load_options.convert_to_target_unit		= 1;
	_scene_data_load_options.target_unit				= fbx_unit::meters;
	_scene_data_load_options.coord_system_convert_mode	= fbx_target_coord_system_convert_mode::deep;
	_scene_data_load_options.target_coord_system		= fbx_target_coord_system::gol;
	_scene_data_load_options.file_texture_load_mode		= _b_use_absolute_path ? fbx_file_mode::absolute : fbx_file_mode::relative;

	// load scene.
	SPY_PUSH_RANGE2( "fbx_load_scene_from_file", spy::FILE_HIGH, path  );
		_scene_result = fbx_load_scene_from_file( path, _scene_load_options );
	SPY_POP_RANGE2();

	if( _scene_result.result_code == fbx_scene_load_result_code::success )
	{
		DBG_PRINT_STRING("FBX loaded \"%.512s\".", path );

		// scene info.
		fbx_scene_info CONST & info = _scene_result.scene.scene_info;
		_node_nb		= info.total_node_count;
		_mesh_nb		= info.mesh_count;
		_texture_nb		= info.file_texture_count;
		_stack_nb		= info.animation_stack_nb;
		_track_nb		= info.animation_track_count;
		_keyframe_nb	= info.animation_keyframe_total_count;

		if( is_verbose() )
		{
			DBG_PRINT_STRING("node count:               %d.", _node_nb);
			DBG_PRINT_STRING("mesh count:               %d.", _mesh_nb);
			DBG_PRINT_STRING("texture count:            %d.", _texture_nb);
			DBG_PRINT_STRING("animation stack count:    %d.", _stack_nb);
			DBG_PRINT_STRING("animation track count:    %d.", _track_nb);
			DBG_PRINT_STRING("keyframe count:           %d.", _keyframe_nb);
			DBG_PRINT_STRING("up vector:                %s.", fbx_get_axis_name(info.up_axis_vector,    info.up_axis_vector_direction));
			DBG_PRINT_STRING("front vector:             %s.", fbx_get_axis_name(info.front_axis_vector, info.front_axis_vector_direction));
			DBG_PRINT_STRING("right vector:             %s.", fbx_get_axis_name(info.right_axis_vector, info.right_axis_vector_direction));
		}
	}
	else
	{
		ERR_PRINT_STRING( "Failed to load FBX \"%.512s\".", path );

		switch( _scene_result.result_code )
		{
		case fbx_scene_load_result_code::fail_manager_create:	ERR_PRINT_STRING("Failed to create FBX manager.");	break;
		case fbx_scene_load_result_code::fail_load_plugins:		ERR_PRINT_STRING("Failed to load FBX plugin(s).");	break;
		case fbx_scene_load_result_code::fail_importer_create:	ERR_PRINT_STRING("Failed to create FBX importer.");	break;
		case fbx_scene_load_result_code::fail_scene_load:		ERR_PRINT_STRING("Failed to load scene.");			break;
		}

		return false;
	}

	// read scene data.
	_scene_data = fbx_get_scene_data(_scene_result.scene, _scene_data_load_options);

	if( is_do_validate() )
	{
		fbx_scene_validate_result scene_validate_result = fbx_validate_scene(_scene_data);
		if( scene_validate_result.non_srt_info_in_nodes )
		{
			DBG_PRINT_STRING( "Non SRT data has been detected in that scene" );
			DBG_PRINT_STRING( "  using standard position/scale/rotation will not work,");
			DBG_PRINT_STRING( "  offsets and pivots have to be taken into account." );
		}
		if( scene_validate_result.multi_layer_animation )
		{
			ERR_PRINT_STRING("Some animation stacks have multiple layers.");
			return false;
		}
		if( scene_validate_result.non_consistent_rotation_order )
		{
			ERR_PRINT_STRING("Non consistent rotation order has been detected in the scene.");
			return false;
		}
	}

	// maximum matrix amount check -> TODO remove limitation.
	if( _node_nb > BDD_FBX_MAX_UBO_MATRICES )
	{
		ERR_PRINT_STRING( "Too many nodes, should not exceed 256. FBX has %d nodes.", _node_nb );
		return false;
	}

	// load graph transformation matrices.
	_graph = std::make_unique<aaa::c_fbx_graph>(this);
	_graph->load(&_scene_data);

	// load models.
	for( INT32 i = 0; i < _mesh_nb; ++i )
	{
		auto mesh = std::make_unique<aaa::c_fbx_mesh>(this);
		mesh->set_use_ssbo( _b_use_ssbo_mesh );

		if( mesh->load(i) )
			_meshes.emplace_back( std::move(mesh) );
	}
	// Update mesh count, in case some mesh do not load (ie mesh without texture coordinates)
	_mesh_nb = (INT32)_meshes.size();
	// animation curves.
	for( UINT32 i = 0; i < _scene_data.scene_info.total_node_count; ++i )
	{
		if( fbx_is_node_root_armature( _scene_data, i ) )
		{
			//mesh and skeleton roots are non dependent, so parent does not work here
			//INT32 CONST mesh_idx = fbx_node_find_parent_mesh(_scene_data, i);
			//if( mesh_idx != -1 )
			//{
				auto curve = std::make_unique<aaa::c_fbx_animation_curve>(this);
				if( curve->load( i, 0 ) )	// was curve->load( i, 0, 0 )
					_curves.emplace_back(std::move(curve));
			//}
		}
	}

	// textures.
	for( INT32 i = 0; i < _texture_nb; ++i )
	{
		auto tex = std::make_unique<aaa::c_fbx_texture>(this);
		tex->load(i);
		_textures.emplace_back(std::move(tex));
	}
	if( is_verbose() )
	{
		DBG_PRINT_STRING("Loaded %i texture files.", _texture_nb);
		for ( auto CONST & tex : _textures )
			DBG_PRINT_STRING("%s.", tex->get_path());
	}

	// create scene matrix ubo.
	_ubo_matrix = gl::ubo::make( sizeof(glm::mat4), true, (char*)&_matrix, "fbx_scene_matrix" );

	// root armature global transform.
	for( UINT32 i = 0; i < _scene_data.scene_info.total_node_count; ++i )
	{
		if( fbx_is_node_root_armature( _scene_data, i ) )
		{
			_root_armature_global_transform = get_node_global_transform(i);
			break;
		}
	}

	// return value.
	_b_loaded = true;

	return true;
}

bool aaa::c_fbx_scene::get_root_armature_global_position( FP32 * CONST pt_fp32 ) CONST
{
	if( _root_armature_global_transform )
	{
		cpy_v3( pt_fp32, &_root_armature_global_transform[0][3].x );
		return true;
	}
	else
		return false;
}

bool aaa::c_fbx_scene::get_root_armature_global_rotation( FP32* CONST pt_fp32 ) CONST
{
	if( _root_armature_global_transform )
	{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(	*_root_armature_global_transform, 
						scale, rotation, translation, 
						skew, perspective );

		auto CONST euler = glm::eulerAngles(rotation);
		pt_fp32[0] = euler.x * FP32(PI_TIME_2_OVER_1);
		pt_fp32[1] = euler.y * FP32(PI_TIME_2_OVER_1);
		pt_fp32[2] = euler.z * FP32(PI_TIME_2_OVER_1);
		return true;
	}
	else
		return false;
}

void aaa::c_fbx_scene::clear()
{
	_b_loaded = false;

	release_fbx_data();

	_graph.reset();
	_meshes.clear();
	_curves.clear();
	_textures.clear();

	gl::ubo::release_and_null( _ubo_matrix );
}

//-----------------------------------------------------------------------------
void aaa::c_fbx_scene::update( 
	FP32 CONST			p_time, 
	UINT32 CONST		p_animation_index, 
	bool CONST			p_linear,
	glm::mat4 CONST *	p_post_local_transforms,
	UINT32 CONST *		p_post_local_transforms_indices,
	UINT32 CONST		p_post_local_transforms_count )
{
	//todo : add post transforms
	_graph->update( p_time, p_animation_index, p_linear, p_post_local_transforms, p_post_local_transforms_indices, p_post_local_transforms_count);

	// empty atm, uncomment when morph targets implemented.
	//for (auto & mesh : _meshes)
	//	mesh->update(p_time);
}

void aaa::c_fbx_scene::do_bind() CONST
{
	_graph->bind();
	_ubo_matrix->bind( gl::ubo_binding_index_scene_matrix );
}

void aaa::c_fbx_scene::draw_meshes() CONST
{  
	do_bind();  
	for( auto CONST & mesh : _meshes )
		mesh->draw();
}

void aaa::c_fbx_scene::draw_mesh( INT32 CONST p_mesh_index ) CONST
{  
	if( p_mesh_index < _meshes.size() )
	{
		do_bind();
		_meshes.at(p_mesh_index)->draw();
	}
}

void aaa::c_fbx_scene::draw_meshes_static() CONST
{
	do_bind();
	for( auto CONST & mesh : _meshes )
		mesh->draw_static();
}

void aaa::c_fbx_scene::draw_mesh_static( INT32 CONST p_mesh_index ) CONST
{
	if( p_mesh_index < _meshes.size() )
	{
		do_bind();
		_meshes.at( p_mesh_index )->draw_static();
	}
}

void aaa::c_fbx_scene::draw_curves() CONST
{
	for( auto CONST & curve : _curves )
		curve->draw();
}

//todo revive ?
//void aaa::c_fbx_scene::draw_curve( INT32 CONST mesh_index ) CONST
//{
//	// TODO: set scene matrix
//
//	for( auto CONST & curve : _curves )
//	{
//		if( curve->get_mesh_index() == mesh_index )
//			curve->draw();
//	}
//}

//-----------------------------------------------------------------------------

//todo optimize here below and up
//todo add a fn passing all the param
//todo add for diffrence to flag dirty
//todo the begining should be done in aaa_matrix
//todo degre is not the AAA standard
void aaa::c_fbx_scene::update_matrix( bool CONST b_update_ubo,  bool CONST b_ubo_immediate )
{
	//aaa::matrix::make_matrix_tra_rotyzx_rad_sca( &_matrix, _translate, _rotate, _scale );
	aaa::matrix::make_matrix_tra_rotyxz_rad_sca( &_matrix, _translate, _rotate, _scale );

	if( b_update_ubo ) 
		_ubo_matrix->write( &(_matrix[0]), sizeof(_matrix), 0, b_ubo_immediate );
}

//void aaa::c_fbx_scene::store_translate( FP32 CONST * CONST tra )
//{
//	cpy_v3fp32( _translate, tra );
//}

//void aaa::c_fbx_scene::set_position( glm::vec3 CONST & tra, bool CONST b_update_ubo, bool CONST b_update_immediate )
//{
//	_position = p_pos;
//	update_matrix(b_update_ubo, b_update_immediate);
//}

//void aaa::c_fbx_scene::set_translate( FP32 CONST * CONST tra, bool CONST b_update_ubo, bool CONST b_update_immediate )
//{
//	cpy_v3fp32( _translate, tra );
//	update_matrix( b_update_ubo, b_update_immediate );
//}

//void aaa::c_fbx_scene::set_scene_position_x( FP32 CONST tra, bool CONST b_update_ubo, bool CONST b_update_immediate )
//{
//	_scene_position.x = tra;
//	update_scene_matrix(b_update_ubo, b_update_immediate);
//}
//
//void aaa::c_fbx_scene::set_scene_position_y( FP32 CONST tra, bool CONST b_update_ubo, bool CONST b_update_immediate )
//{
//	_scene_position.y = tra;
//	update_scene_matrix(b_update_ubo, b_update_immediate);
//}
//
//void aaa::c_fbx_scene::set_scene_position_z( FP32 CONST tra, bool CONST b_update_ubo, bool CONST b_update_immediate )
//{
//	_scene_position.z = tra;
//	update_scene_matrix(b_update_ubo, b_update_immediate);
//}

//void aaa::c_fbx_scene::store_rotate( FP32 CONST * CONST rot_rad )
//{
//	cpy_v3fp32( _rotate, rot_rad );
//}

//void aaa::c_fbx_scene::set_rotation( glm::vec3 CONST & p_rot, bool CONST _update_ubo, bool CONST b_update_immediate )
//{
//	_rotation = p_rot;
//	update_matrix(b_update_ubo, b_update_immediate);
//}

//void aaa::c_fbx_scene::set_rotate(
//	FP32 CONST * CONST	rot_rad,
//	bool CONST				b_update_ubo,
//	bool CONST				b_update_immediate
//)
//{
//	cpy_v3fp32( _rotate, rot_rad );
//	update_matrix( b_update_ubo, b_update_immediate );
//}

void aaa::c_fbx_scene::set_mesh_blend_shape_factors( UINT32 CONST mesh_index, UINT32 CONST shape_index, FP32 CONST * factors )
{
	if( mesh_index < _meshes.size() )
		_meshes.at(mesh_index)->set_blend_shape_factors( shape_index, factors );
}

FP32 aaa::c_fbx_scene::get_animation_time_begin( UINT32 CONST p_animation_index )
{
	if( p_animation_index >= _scene_data.scene_info.animation_stack_nb )
		return 0.0f;
	return _scene_data.scene_info.animations[p_animation_index].start_time;
}

FP32 aaa::c_fbx_scene::get_animation_time_end( UINT32 CONST p_animation_index )
{
	if( p_animation_index >= _scene_data.scene_info.animation_stack_nb )
		return 0.0f;
	return _scene_data.scene_info.animations[p_animation_index].end_time;
}

FP32 aaa::c_fbx_scene::get_animation_node_time_begin( UINT32 CONST p_animation_index )
{
	if( p_animation_index >= _scene_data.scene_info.animation_stack_nb )
		return 0.0f;
	return _scene_data.scene_info.animations[p_animation_index].node_start_time;
}

FP32 aaa::c_fbx_scene::get_animation_node_time_end( UINT32 CONST p_animation_index )
{
	if( p_animation_index >= _scene_data.scene_info.animation_stack_nb )
		return 0.0f;
	return _scene_data.scene_info.animations[p_animation_index].node_end_time;
}

FP32 aaa::c_fbx_scene::get_animation_blend_shape_time_begin( UINT32 CONST p_animation_index )
{
	if( p_animation_index >= _scene_data.scene_info.animation_stack_nb )
		return 0.0f;
	return _scene_data.scene_info.animations[p_animation_index].blend_shape_start_time;
}

FP32 aaa::c_fbx_scene::get_animation_blend_shape_time_end( UINT32 CONST p_animation_index )
{
	if( p_animation_index >= _scene_data.scene_info.animation_stack_nb )
		return 0.0f;
	return _scene_data.scene_info.animations[p_animation_index].blend_shape_end_time;
}

FP32 aaa::c_fbx_scene::get_animation_play_time( UINT32 CONST p_animation_index, float CONST p_time, animation_sampler_mode const p_sampler )
{
	FP32 time = 0.0f;
	bool result = fbx_get_scene_play_time( &_scene_data, p_animation_index, p_time, p_sampler, time );

	//todo log on invalid result
	return time;
	}

char const* aaa::c_fbx_scene::get_animation_name( UINT32 CONST p_animation_index )
{
	if( p_animation_index >= _scene_data.scene_info.animation_stack_nb )
		return "";
	INT32 const idx = p_animation_index * FBX_NAME_MAX_LENGTH;
	return _scene_data.animation_names_table + idx;
}


glm::mat4 CONST * aaa::c_fbx_scene::get_node_global_transform( UINT32 CONST node_index ) CONST
{
	if( node_index >= _scene_data.scene_info.total_node_count )
		return nullptr;
	return &(_graph->get_matrices_data()[node_index]);
}


//void aaa::c_fbx_scene::set_scene_rotation_x( FP32 CONST rot, bool CONST b_update_ubo, bool CONST b_update_immediate )
//{
//	_scene_rotation.x = rot;
//	update_scene_matrix(b_update_ubo, b_update_immediate);
//}
//
//void aaa::c_fbx_scene::set_scene_rotation_y( FP32 CONST rot, bool CONST b_update_ubo, bool CONST b_update_immediate )
//{
//	_scene_rotation.y = rot;
//	update_scene_matrix(b_update_ubo, b_update_immediate);
//}
//
//void aaa::c_fbx_scene::set_scene_rotation_z( FP32 CONST rot, bool CONST b_update_ubo, bool CONST b_update_immediate )
//{
//	_scene_rotation.z = rot;
//	update_scene_matrix(b_update_ubo, b_update_immediate);
//}

//void aaa::c_fbx_scene::store_scale(  FP32 CONST * CONST  scale )
//{
//	cpy_v3fp32( _scale, scale );
//}
//
//void aaa::c_fbx_scene::set_scale(	FP32 CONST * CONST	scale,
//									bool CONST				b_update_ubo,
//									bool CONST				b_update_immediate
//								)
//{
//	cpy_v3fp32( _scale, scale );
//	update_matrix( b_update_ubo, b_update_immediate );
//}

void aaa::c_fbx_scene::set_trs(	FP32 CONST * CONST tra, FP32 CONST * CONST rot_rad, FP32 CONST * CONST sca,
								bool CONST b_update_ubo, bool CONST b_update_immediate
								)
{
	cpy_v3fp32( _translate, tra		);
	cpy_v3fp32( _rotate,	rot_rad	);
	cpy_v3fp32( _scale,		sca		);
	update_matrix( b_update_ubo, b_update_immediate );
}

void aaa::c_fbx_scene::set_trs( glm::mat4 CONST& mat, bool CONST b_update_ubo, bool CONST b_update_immediate )
{
	_matrix = mat;
	if( b_update_ubo )
		_ubo_matrix->write( &( _matrix[ 0 ] ), sizeof( _matrix ), 0, b_update_immediate );
}

void aaa::c_fbx_scene::set_identity( bool CONST b_update_ubo, bool CONST b_update_immediate )
{
	clear_v3fp32( _translate	);
	clear_v3fp32( _rotate		);
	set1_v3fp32(  _scale		);
	update_matrix( b_update_ubo, b_update_immediate );
}

