
// main header.
#include "err.h"
#include "fbx/aaa_fbx_mesh.h"
#include "fbx/internal/fbxloader.h"
#include "fbx/internal/fbx_utils_mem.h"
#include "fbx/internal/fbx_scene_validate.h"
#include "fbx/internal/fbx_scene_process.h"
#include "fbx/aaa_fbx_scene.h"
#include "fbx/aaa_fbx_material.h"
#include "spy.h"
#include "math/v.h"
#include <string>

//-----------------------------------------------------------------------------

aaa::c_fbx_mesh::c_fbx_mesh( aaa::c_fbx_scene const * p_scene )
	: aaa::c_fbx_base( p_scene )
	, _mesh_gl				{}
	, _pose					{}
	, _material				{}
	, _ubo_model_matrix		{ nullptr }
	, _ubo_blend_shape		{ nullptr }
	, _mesh_load_result		{}
	, _mesh_load_options	{}
	, _mesh_index			{ -1 }
	, _name					{ nullptr }
	, _use_ssbo				{ true }
{
//todo we need an init fns we can use and call also before we load
	MEMCLEAR( &_mesh_gl, sizeof(st_fbx_mesh_gl) );
	
	clear_v3( _bbox_min );
	clear_v3( _bbox_max );
	
	fbx_init_mesh_load_result(&_mesh_load_result);
	fbx_init_mesh_load_options(&_mesh_load_options);

	_mesh_load_options.data_layout						= fbx_mesh_layout::array_per_attribute;
	_mesh_load_options.enable_bones						= 1;
	_mesh_load_options.bones_import_mode				= fbx_mesh_bones_import_mode::all_bones_flat;

	_mesh_load_options.flip_texture_coordinates_y		= 0;
	_mesh_load_options.normals							= fbx_mesh_attribute_options::generate_if_missing;
	_mesh_load_options.tangent_frame					= fbx_mesh_attribute_options::generate_if_missing;
	_mesh_load_options.reverse_winding					= 0;
	_mesh_load_options.scaling							= 1.0f;
	_mesh_load_options.texture_coordinates				= fbx_multiple_attribute_import_mode::first;

	_mesh_load_options.normals_requirement				= fbx_attribute_requirement::required;
	_mesh_load_options.texture_coordinates_requirement	= fbx_attribute_requirement::required;
	_mesh_load_options.tangent_frame_requirement		= fbx_attribute_requirement::required;


	_ubo_model_matrix = gl::ubo::make( sizeof(aaa::matrix::identity), true, (char*)&aaa::matrix::identity, "fbx_mesh_model_matrix" );
	
	//set blend shape to sensible default, we only care about setting count to 0
	_blend_shape_ubo_data.shape_nb = 0;
	
	_ubo_blend_shape = gl::ubo::make( sizeof(st_blend_shape_ubo_data), true, (char*)&_blend_shape_ubo_data, "fbx_mesh_blend_shape" );
}

aaa::c_fbx_mesh::~c_fbx_mesh()
{
	_pose.reset();
	fbx_gl_release_mesh( &_mesh_gl, true );
	fbx_release_mesh(_mesh_load_result.mesh);
	gl::ubo::release_and_null( _ubo_model_matrix );
	gl::ubo::release_and_null( _ubo_blend_shape );
}

//-----------------------------------------------------------------------------

aaa::c_fbx_mesh & aaa::c_fbx_mesh::set_use_ssbo( bool const p_value )
{
	_use_ssbo = p_value;
	return *this;
}

//-----------------------------------------------------------------------------

void aaa::c_fbx_mesh::setup_mesh_gl()
{
	bool const force_root_bone = false;
	_mesh_gl = fbx_gl_create_mesh(
		_mesh_load_result.mesh, 
		_use_ssbo,
		force_root_bone, 
		true );
}

void aaa::c_fbx_mesh::setup_pose(INT32 CONST p_node_idx)
{
	_pose = std::make_unique<aaa::c_fbx_pose>( get_scene() );
	_pose->load( &(_mesh_load_result.mesh), p_node_idx );
}

void aaa::c_fbx_mesh::setup_material( INT32 CONST p_node_idx )
{
	_material = std::make_unique<aaa::c_fbx_material>( get_scene() );
	_material->load( p_node_idx );
	}

void aaa::c_fbx_mesh::setup_size()
{
	fbx_scene_data const * scene_data = get_scene()->get_fbx_scene_data();
	INT32 const node_idx = fbx_scene_find_mesh_node_index( scene_data, _mesh_index );

	glm::mat4 mat;
	float* ptr = &(mat[0][0]);
	fbx_native_get_node_global_transform( get_scene()->get_fbx_scene_native(), scene_data, node_idx, ptr );

	FP32* pf;
	glm::vec4 box_world_min;
	glm::vec4 box_world_max;

	pf = _mesh_load_result.mesh.bounding_box.min;
	box_world_min = mat * glm::vec4( *pf, *(pf+1), *(pf+2), 1.0f );
  
	pf = _mesh_load_result.mesh.bounding_box.max;
	box_world_max = mat * glm::vec4( *pf, *(pf+1), *(pf+2), 1.0f );

	//todo work in fish case but we need to transform the bbox in world and then use the 8 corners
	min_v3( _bbox_min, &box_world_min.x, &box_world_max.x );
	max_v3( _bbox_max, &box_world_min.x, &box_world_max.x );
}

//-----------------------------------------------------------------------------

bool aaa::c_fbx_mesh::load( INT32 const mesh_index )
{
	_mesh_load_result = fbx_scene_load_mesh_by_index(	get_scene()->get_fbx_scene_data(), 
														get_scene()->get_fbx_scene_native(), 
														&_mesh_load_options, 
														mesh_index
														);

	if( _mesh_load_result.result_code != fbx_mesh_load_result_code::success )
	{
		ERR_PRINT_STRING( "Mesh load failed." );

		//CHAR* str = "Unknown ";
		std::string str;
		switch( _mesh_load_result.result_code )
		{
		case fbx_mesh_load_result_code::fail_invalid_scene:						str = "Scene is invalid.";								break;
		case fbx_mesh_load_result_code::fail_invalid_index:						str = "Index is out of bounds.";						break;
		case fbx_mesh_load_result_code::fail_normals_not_present:				str = "No normals were found in the mesh.";				break;
		case fbx_mesh_load_result_code::fail_tangent_frame_not_present:			str = "No tangent frame was found in the mesh.";		break;
		case fbx_mesh_load_result_code::fail_texture_coordinates_not_present:	str = "No texture coordinates were found in the mesh.";	break; 
		case fbx_mesh_load_result_code::fail_bones_not_present:					str = "No bones were found in the mesh.";				break;
		case fbx_mesh_load_result_code::fail_normals_not_generated:				str = "No normals were found in the mesh, and normals generation failed."; 
																				break;
		case fbx_mesh_load_result_code::fail_tangents_not_generated:			str = "No tangents were found in the mesh, and normals generation failed."; 
																				break;
		}
		ERR_PRINT_STRING( "fbx_scene_load_mesh_by_index error : %256s", str.c_str() );

		str.clear();
		switch( _mesh_load_result.normal_result_code )
		{
		case fbx_mesh_optional_attribute_load_result::fail_generate_generic:				str = "Normal generation failed.";	break;
		case fbx_mesh_optional_attribute_load_result::fail_generate_tangent_no_texcoord:	str = "Normal generation failed, no texture coordinates available."; 
																							break;
		}
		if( !str.empty() )
			ERR_PRINT_STRING( str.c_str() );

		switch( _mesh_load_result.tangents_result_code )
		{
		case fbx_mesh_optional_attribute_load_result::fail_generate_generic:
			ERR_PRINT_STRING("Tangent generation failed.");
			break;
		case fbx_mesh_optional_attribute_load_result::fail_generate_tangent_no_texcoord: 
			ERR_PRINT_STRING("Tangent generation failed, no texture coordinates available."); 
			break;
		}
		return false;
	}

	if( get_scene()->is_do_validate() )
	{
		DBG_PRINT_STRING("Mesh maximum bone count per vertex %i.", _mesh_load_result.max_bone_per_vertex);
		DBG_PRINT_STRING("Mesh bone influence lost           %f.", _mesh_load_result.max_vertex_lost_influence);

		auto mesh_validate_result = fbx_validate_mesh(_mesh_load_result.mesh);
		if( mesh_validate_result.overload_index_found_in_triangle )
			ERR_PRINT_STRING("Index overload detected, this will read outside of the vertex buffer.");
		if( mesh_validate_result.same_index_found_in_triangle)
			ERR_PRINT_STRING("Identical indices found in triangle." );
		if( mesh_validate_result.overload_bone_found_in_bone_index_buffer)
			ERR_PRINT_STRING("Overload detected in bone index buffer." );
	}

	// store name.
	INT32 const node_idx = fbx_scene_find_mesh_node_index( get_scene()->get_fbx_scene_data(), mesh_index );

	_name = fbx_get_node_name( get_scene()->get_fbx_scene_data(), node_idx );

	// store mesh index.
	_mesh_index = mesh_index;

	// log.
	if( get_scene()->is_verbose() )
		DBG_PRINT_STRING( "Mesh [%s] loaded.", _name );

	setup_mesh_gl();
	setup_pose(node_idx);
	setup_material(node_idx);
	setup_size();

	return true;
}

INT32 CONST aaa::c_fbx_mesh::get_blend_shape_channel_nb(UINT32 const shape_index) CONST
{
	if( shape_index >= _mesh_load_result.mesh.blend_shapes_data.shape_count )
		return 0;
	return _mesh_load_result.mesh.blend_shapes_data.shapes_info_ptr[shape_index].channel_count;
}


void aaa::c_fbx_mesh::set_blend_shape_factors( UINT32 const shape_index, FP32 const * factors )
{
	fbx_blend_shape_interpolation_info interpolation_info;
	INT32 shape_nb = 0;
	for( INT32 channel_index = 0; channel_index < MAX_BLEND_SHAPE_CHANNELS; channel_index++ )
	{
		if( fbx_get_blend_shape_interpolation(_mesh_load_result.mesh, shape_index, channel_index, factors[channel_index], &interpolation_info) )
		{
			//add our channel to the list
			_blend_shape_ubo_data.shapes[shape_nb++] = interpolation_info;
		}
	}
	_blend_shape_ubo_data.shape_nb = shape_nb;

	_ubo_blend_shape->write( &_blend_shape_ubo_data, sizeof(st_blend_shape_ubo_data), 0, true );
}

//void aaa::c_fbx_mesh::update(FP32 const p_time)
//{
//	(void)p_time;
//
//
//}


namespace {
	o_str	o_mesh_info;
	bool	b_is_identity = false;
}

void aaa::c_fbx_mesh::draw() const
{
	if( SPY_IS_ACTIVE() )
	{
		o_mesh_info.set( "fbx mesh draw " );
		o_mesh_info.add( _mesh_index );
		SPY_PUSH_RANGE_DIRECT( o_mesh_info.get(), spy::DRAW_2 );
	}

		_pose->bind();
	//todomona
	//todo use _pose->get_root() fuck the thing up
	// because the studio gave us files with the bone root matrix duplicating the mode matrix (this should not happen)
	// linked with the file fbx_pose.cpp at line 47
	// b_is_identity is an opt by Maa
		if( !b_is_identity )
		{
			_ubo_model_matrix->bind_and_write( gl::ubo_binding_index_model, &(aaa::matrix::identity), sizeof(aaa::matrix::identity) );
			b_is_identity = true;
		}

		_ubo_blend_shape->bind(3);
		_mesh_gl.draw();

	SPY_POP_RANGE();
}

// no bone no blend shape
void aaa::c_fbx_mesh::draw_static() const
{
	if( SPY_IS_ACTIVE() )
	{
		o_mesh_info.set( "fbx mesh draw static " );
		o_mesh_info.add( _mesh_index );
		SPY_PUSH_RANGE_DIRECT( o_mesh_info.get(), spy::DRAW_2 );
	}

		_pose->bind();
		// todo: do this only once at init.
		auto pose_mat = _pose->get_root();
		_ubo_model_matrix->bind_and_write( gl::ubo_binding_index_model, pose_mat, sizeof(glm::mat4) );
		b_is_identity = false;

	//	_ubo_blend_shape->bind(3);
		_mesh_gl.draw();

	SPY_POP_RANGE();
}

//-----------------------------------------------------------------------------

REAL * aaa::c_fbx_mesh::get_vertex_positions() CONST
{
	REAL * res = nullptr;
	if( _mesh_load_result.mesh.layout == fbx_mesh_layout::array_per_attribute )
	{
		res = (REAL*)_mesh_load_result.mesh.array_location_info.position_ptr;
	}
	return res;
}

REAL * aaa::c_fbx_mesh::get_vertex_normals() CONST
{
	REAL* res = nullptr;
	if( _mesh_load_result.mesh.layout == fbx_mesh_layout::array_per_attribute )
	{
		res = (REAL*)_mesh_load_result.mesh.array_location_info.normal_ptr;
	}
	return res;
}

//-----------------------------------------------------------------------------
