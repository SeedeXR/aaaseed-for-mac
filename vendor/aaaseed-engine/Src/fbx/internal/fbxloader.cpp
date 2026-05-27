
// Main header.
#include "fbx/internal/fbxloader.h"
#include "spy.h"

#include "fbx/internal/fbx_sdk_include.h"
#include "fbx/internal/fbx_structs_internal.h"
#include "fbx/internal/fbx_utils.h"
#include "fbx/internal/fbx_utils_mem.h"
#include "err.h"

#include <cmath>

fbx_scene_load_result fbx_load_scene_from_file( const char* p_filename, const fbx_scene_load_options& p_options )
{
	fbx_scene_load_result result = init_load_result();

	FbxManager * p_manager = FbxManager::Create();
	if( !p_manager )
	{
		result.result_code = fbx_scene_load_result_code::fail_manager_create;
		return result;
	}

	FbxIOSettings* ios = FbxIOSettings::Create( p_manager, IOSROOT );
	p_manager->SetIOSettings(ios);
	if( p_options.load_plugins )
	{
		FbxString lPath = FbxGetApplicationDirectory();
		const bool load_plugins_reult = p_manager->LoadPluginsDirectory(lPath.Buffer());

		if( !load_plugins_reult )
		{
			result.result_code = fbx_scene_load_result_code::fail_load_plugins;
			p_manager->Destroy();
			return result;
		}
	}

	//try to get an importer for the scene
	FbxImporter * p_importer = FbxImporter::Create( p_manager, "" );
	const bool importer_init_result = p_importer->Initialize( p_filename, -1, p_manager->GetIOSettings() );
	if( !importer_init_result )
	{
		result.result_code = fbx_scene_load_result_code::fail_importer_create;
		p_manager->Destroy();
		return result;
	}

	//now importer is valid, create scene and load importer onto it
	FbxScene * p_scene = FbxScene::Create(p_manager, "fbx_scene");
	const bool scene_import_result = p_importer->Import(p_scene);
	if( !scene_import_result )
	{
		result.result_code = fbx_scene_load_result_code::fail_scene_load;
		p_manager->Destroy();
		return result;
	}

	result.result_code = fbx_scene_load_result_code::success;
	result.scene.context_pointer	= p_manager;
	result.scene.scene_pointer		= p_scene;
	result.scene.scene_info			= scene_get_info(p_scene);
	return result;
}

fbx_scene_info const & fbx_get_scene_info( fbx_scene_native const & p_scene )
{
	return p_scene.scene_info;
}

INT32 fbx_get_keyframe_count( fbx_scene_info const & p_scene_info, fbx_scene_data_load_options const & p_load_options )
{
	if( p_load_options.animation_mode == fbx_animation_import_mode::keyframes_default )
		return p_scene_info.animation_keyframe_total_count;
	else
	{
		INT32 result = 0;
		//iterate on animations
		for( UINT32 i = 0; i < p_scene_info.animation_stack_nb; i++ )
		{
			/*float const animation_time = p_scene_info.animations[i].end_time - p_scene_info.animations[i].start_time;
			float const step_count = animation_time / p_load_options.animation_time_step;
			INT32 const count = (INT32)step_count * p_scene_info.animations[i].track_count;*/
			INT32 const count = get_animation_step_count( &p_scene_info.animations[i], p_load_options.animation_time_step) * p_scene_info.animations[i].track_count;
			result += count;
		}
		return result;
	}
}

namespace
{
	FbxAxisSystem * axis_system_gol = nullptr;
};

fbx_scene_data fbx_get_scene_data( fbx_scene_native const & p_scene_native, fbx_scene_data_load_options const & options )
{
	fbx_scene_data result = init_scene();
	FbxScene* scene = (FbxScene*)p_scene_native.scene_pointer;

	fbx_scene_info const & scene_info = fbx_get_scene_info(p_scene_native);
	result.load_options = options;  // this is a ccopy
	result.scene_info = scene_info; // this is a ccopy

	if( options.convert_to_target_unit )
	{
		FbxSystemUnit const * sys;
		switch( options.target_unit )
		{
		case fbx_unit::centimeters:	sys = &FbxSystemUnit::cm;	break;
		case fbx_unit::meters:		sys = &FbxSystemUnit::m;	break;
		default:
			sys = nullptr;
			ERR_PRINT_STRING( "%s() we don't deal with target_unit %d", __FUNCTION__, options.target_unit );
			break;
		}
		if( sys )
			sys->ConvertScene(scene);
	}

	if( options.coord_system_convert_mode != fbx_target_coord_system_convert_mode::none )
	{
		//we init here to be sure the dll is loaded
		if( !axis_system_gol ) 
			axis_system_gol = new FbxAxisSystem(  FbxAxisSystem::eYAxis,
													(FbxAxisSystem::EFrontVector)(-FbxAxisSystem::eParityEven),
													FbxAxisSystem::eRightHanded ); 

		FbxAxisSystem const * axis_system;
		switch( options.target_coord_system  )
		{
		case fbx_target_coord_system::directx:	axis_system = &FbxAxisSystem::DirectX;	break;
		case fbx_target_coord_system::opengl:	axis_system = &FbxAxisSystem::OpenGL;	break;
		case fbx_target_coord_system::gol:		axis_system = axis_system_gol;			break;
		default:
			axis_system = nullptr;
			ERR_PRINT_STRING( "%s() we don't deal with target_coord_system %d", __FUNCTION__, options.coord_system_convert_mode );
			break;
		}

		if( axis_system )
		{
			FbxEuler::EOrder order_before = scene->GetRootNode()->RotationOrder.Get();
			switch( options.coord_system_convert_mode )
			{
			case fbx_target_coord_system_convert_mode::simple:	axis_system->ConvertScene(scene);		break;
			case fbx_target_coord_system_convert_mode::deep:	axis_system->DeepConvertScene(scene);	break;
			default:
				ERR_PRINT_STRING( "%s() we don't deal with fbx_target_coord_system_convert_mode %d", __FUNCTION__, options.coord_system_convert_mode );
				break;
			}

			FbxEuler::EOrder order_after = scene->GetRootNode()->RotationOrder.Get();
			DBG_PRINT_STRING( "rotation order went from %d to %d.", order_before, order_after  );
		}
	}

	//allocates memory for node data
	result.nodes = (fbx_node*) MALLOC( scene_info.total_node_count * sizeof(fbx_node) );

	//allocates name table
	result.node_names_table = (char*) MALLOC( (size_t)scene_info.total_node_count * FBX_NAME_MAX_LENGTH );

	//allocates mapping
	result.object_mapping.native_node_table = MALLOC( scene_info.total_node_count * sizeof(void*) );

	if( scene_info.material_count > 0 )
	{
		result.materials = (st_fbx_material*) MALLOC( scene_info.material_count * sizeof(st_fbx_material) );
		result.material_names_table = (char*) MALLOC( (size_t)scene_info.material_count * FBX_NAME_MAX_LENGTH );
		MEMCLEAR( result.material_names_table, (size_t)scene_info.material_count * FBX_NAME_MAX_LENGTH );
		// material will be loaded with each mesh, with multiple meshes getting material from the scene doesn't work
	//	load_scene_materials( scene, &result );
	}
	if( scene_info.file_texture_count > 0 )
		result.texture_files_name_table = (char*) MALLOC( (size_t)scene_info.file_texture_count * FBX_FILE_NAME_MAX_LENGTH );

	for( UINT32 i = 0; i < result.scene_info.file_texture_count; i++ )
	{
		FbxFileTexture * file_texture = scene->GetSrcObject<FbxFileTexture>(i);

		INT32 name_table_index = i * FBX_FILE_NAME_MAX_LENGTH;
		char * ptr_name_dest = result.texture_files_name_table;
		ptr_name_dest += name_table_index;

		if( options.file_texture_load_mode == fbx_file_mode::absolute )
		{
			char const * file_name = file_texture->GetFileName();
			strcpy_s( ptr_name_dest, FBX_NAME_MAX_LENGTH, file_name );
		}
		else
		{
			char const * file_name = file_texture->GetRelativeFileName();
			strcpy_s( ptr_name_dest, FBX_NAME_MAX_LENGTH, file_name );
		}
	}

	// allocate memory for curves and keyframes.
	if( scene_info.animation_track_count > 0 )
		result.animation_curves = (c_fbx_animation_curve*) MALLOC( scene_info.animation_track_count * sizeof(c_fbx_animation_curve) );

	if( scene_info.animation_stack_nb > 0 )
		result.animation_names_table = (char*) MALLOC( (size_t)scene_info.animation_stack_nb * FBX_FILE_NAME_MAX_LENGTH );

	INT32 total_keyframe_count = fbx_get_keyframe_count( scene_info, options );

	if( total_keyframe_count > 0 )
		result.animation_keyframes = (fbx_animation_keyframe*) MALLOC( total_keyframe_count * sizeof(fbx_animation_keyframe) );

	if( scene_info.animation_stack_nb * scene_info.total_node_count > 0 )
		//allocate node animation tables
		result.nodes_animations = (fbx_node_animation*) MALLOC( scene_info.animation_stack_nb * scene_info.total_node_count * sizeof(fbx_node_animation) );
 
	//allocate blend shape animation info
	if( scene_info.mesh_blend_shape_animations_count > 0 )
		result.mesh_blend_shape_animations = (fbx_mesh_blend_shape_animation*) MALLOC( scene_info.mesh_blend_shape_animations_count * sizeof(fbx_mesh_blend_shape_animation) );


	//traverse nodes
	scene_write_data( scene, result );

	return result;
}

void fbx_release_scene_native( fbx_scene_native& p_scene )
{
	if( p_scene.context_pointer )
	{
		FbxManager* manager = (FbxManager*)p_scene.context_pointer;
		manager->Destroy();
	}
	p_scene.context_pointer = nullptr;
	p_scene.scene_pointer = nullptr;
}


char const* fbx_get_node_name(
	fbx_scene_data const *		p_scene_data,
	UINT32 const				p_node_index )
{
	UINT32 const idx	= p_node_index * FBX_NAME_MAX_LENGTH;
	char const * name	= p_scene_data->node_names_table + idx;
	return name;
}

char const* fbx_get_material_name(
	fbx_scene_data const*		p_scene_data,
	UINT32 const				p_material_index )
{
	UINT32 const idx	= p_material_index * FBX_NAME_MAX_LENGTH;
	char const* name	= p_scene_data->material_names_table + idx;
	return name;
}


fbx_mesh_load_result fbx_scene_load_mesh_by_index(
	fbx_scene_data const *			p_scene, 
	fbx_scene_native const *		p_scene_data, 
	fbx_mesh_load_options const *	p_options, 
	INT32							p_mesh_index )
{
	fbx_mesh_load_result result;

	if( !p_scene_data->scene_pointer )
	{
		result.result_code = fbx_mesh_load_result_code::fail_invalid_scene;
		return result;
	}

	FbxScene * scene = (FbxScene*) p_scene_data->scene_pointer;

	INT32 mesh_count = scene->GetSrcObjectCount<FbxMesh>();
	if( mesh_count <= p_mesh_index)
	{
		result.result_code = fbx_mesh_load_result_code::fail_invalid_index;
		return result;
	}

	FbxMesh* mesh = scene->GetSrcObject<FbxMesh>(p_mesh_index);

	fbx_mesh_data mesh_result = init_mesh();
	fbx_mesh_attributes_memory_requirements attributes_memory_requirements = init_mesh_memory_requirements();

	/* compute memory requirements, we need to check all attributes we want but position first.
	* this is required since attributes like normals / texture coordinates can be expanded per polygon
	* once we know if one attribute requires expansion, then we can decide for position
	*/
	bool face_expand_required = is_face_expand_required( mesh, p_options );

	//normals
	{
		FbxGeometryElementNormal* normalElement = mesh->GetElementNormal();

		switch( p_options->normals )
		{
		case fbx_mesh_attribute_options::enable_if_present:
			if( normalElement )
			{
				mesh_result.has_normals = true;
				result.normal_result_code = fbx_mesh_optional_attribute_load_result::success_data_present;
			}
			else
			{
				if( p_options->normals_requirement == fbx_attribute_requirement::optional )
					result.normal_result_code = fbx_mesh_optional_attribute_load_result::success_ignored;
				else //required, we fail the mesh loading
				{
					result.result_code = fbx_mesh_load_result_code::fail_normals_not_present;
					return result;
				}
			}
			break;
		case fbx_mesh_attribute_options::generate_if_missing:
			if( normalElement ) //if present, just use existing ones
			{
				mesh_result.has_normals = true;
				result.normal_result_code = fbx_mesh_optional_attribute_load_result::success_data_present;
			}
			else
			{
				mesh_result.has_normals = mesh->GenerateNormals(true, false, false);
				if( mesh_result.has_normals ) //normals were successfully generated
					result.normal_result_code = fbx_mesh_optional_attribute_load_result::success_generated;
				else
				{
					if( p_options->normals_requirement == fbx_attribute_requirement::optional )
						//mark that normals were not generated, but we still allow to load the mesh
						result.normal_result_code = fbx_mesh_optional_attribute_load_result::fail_generate_generic;
					else //required, we fail the mesh loading
					{
						result.result_code = fbx_mesh_load_result_code::fail_normals_not_generated;
						return result;
					}
				}
			}
			break;
		case fbx_mesh_attribute_options::ignore:
			result.normal_result_code = fbx_mesh_optional_attribute_load_result::success_ignored;
			break;
		}
	}

	// texture coordinates
	{
		if( p_options->texture_coordinates == fbx_multiple_attribute_import_mode::first && mesh->GetElementUVCount() > 0 )
		{
			FbxGeometryElementUV* uvElement = mesh->GetElementUV(0);
			if( uvElement )
				mesh_result.texture_coordinates_channel_count = 1;
		}
		else if( p_options->texture_coordinates == fbx_multiple_attribute_import_mode::all )
		{
			for( INT32 i = 0; i < mesh->GetElementUVCount(); i++ )
			{
				FbxGeometryElementUV* uvElement = mesh->GetElementUV(i);
				if( uvElement )
					mesh_result.texture_coordinates_channel_count++;
			}
		}

		//fail load if we require texture coordinates
		if( mesh_result.texture_coordinates_channel_count == 0
			&& p_options->texture_coordinates_requirement == fbx_attribute_requirement::required )
		{
			result.result_code = fbx_mesh_load_result_code::fail_texture_coordinates_not_present;
			return result;
		}
	}

	//tangents
	mesh_result.has_tangent_frame = false;
	{
		FbxGeometryElementTangent* tangentElement = mesh->GetElementTangent();
		FbxGeometryElementBinormal* binormalElement = mesh->GetElementBinormal();

		switch( p_options->tangent_frame )
		{
		case fbx_mesh_attribute_options::enable_if_present:
			if( tangentElement && binormalElement )
			{
				mesh_result.has_tangent_frame = true;
				result.tangents_result_code = fbx_mesh_optional_attribute_load_result::success_data_present;
			}
			else
			{
				if( p_options->tangent_frame_requirement == fbx_attribute_requirement::optional )
					result.tangents_result_code = fbx_mesh_optional_attribute_load_result::success_ignored;
				else //required, we fail the mesh loading
				{
					result.result_code = fbx_mesh_load_result_code::fail_tangent_frame_not_present;
					return result;
				}
			}
			break;
		case fbx_mesh_attribute_options::generate_if_missing:
			if( tangentElement && binormalElement ) //if present, just use existing ones
			{
				mesh_result.has_tangent_frame = true;
				result.tangents_result_code = fbx_mesh_optional_attribute_load_result::success_data_present;
			}
			else
			{
				//try to generate tangents, we pre check that we have a texture coordinates channel
				if( mesh_result.texture_coordinates_channel_count > 0 )
				{
					mesh_result.has_tangent_frame = mesh->GenerateTangentsData(0, false, false);
					if( !mesh_result.has_tangent_frame )
						result.tangents_result_code = fbx_mesh_optional_attribute_load_result::fail_generate_generic;
				}
				else
					//mark that tangent generation failed because of no texture coordinates
					result.tangents_result_code = fbx_mesh_optional_attribute_load_result::fail_generate_tangent_no_texcoord;

				if( mesh_result.has_tangent_frame ) //generated success test
					result.tangents_result_code = fbx_mesh_optional_attribute_load_result::success_generated;
				else
				{
					if( p_options->tangent_frame_requirement == fbx_attribute_requirement::required )
					{
						result.result_code = fbx_mesh_load_result_code::fail_tangents_not_generated;
						return result;
					}
				}
			}
			break;
		case fbx_mesh_attribute_options::ignore:
			result.normal_result_code = fbx_mesh_optional_attribute_load_result::success_ignored;
			break;
		}
	}


	//bone information
	fbx_mesh_skinning_data_info bone_data_info;
	MEMCLEAR( &bone_data_info, sizeof(fbx_mesh_skinning_data_info) );

	fbx_mesh_skinning_data bone_data;
	MEMCLEAR( &bone_data, sizeof(fbx_mesh_skinning_data) );

	mesh_result.bones_data.bones_nb = 0;

	if( p_options->enable_bones )
	{
		bone_data_info  = mesh_compute_bones_info(mesh, p_options->bones_import_mode);

		mesh_load_bones(mesh, p_options->bones_import_mode, *p_scene, bone_data_info, bone_data);

		mesh_result.bones_data.bones_nb = bone_data_info.bone_count;
		result.max_bone_per_vertex = bone_data_info.max_bones_per_vertex;
		result.max_vertex_lost_influence = bone_data.max_lost_influence;
	}

	if( mesh_result.bones_data.bones_nb > 0 )
	{
		mesh_result.has_bones = true;
		//todo better than fixed sizeof
		attributes_memory_requirements.bones_bind_pose_size = mesh_result.bones_data.bones_nb * sizeof(float) * 16; //3d transform
		attributes_memory_requirements.bones_node_index_table_size = mesh_result.bones_data.bones_nb * sizeof(UINT32); //simple int
	}


	//indices first, as we need it for other attributes
	UINT32 index_count = compute_index_count(mesh);

	attributes_memory_requirements.indices_size = (index_count * 4);

	UINT32 position_count = mesh->GetControlPointsCount();

	if( face_expand_required )
		mesh_result.vertex_count = compute_vertex_count_expanded(mesh);
	else
		mesh_result.vertex_count = position_count;

	attributes_memory_requirements.position_size = mesh_result.vertex_count * 12;
	if( mesh_result.has_normals )
		attributes_memory_requirements.normals_size = (mesh_result.vertex_count * 12);

	if( mesh_result.has_tangent_frame )
	{
		attributes_memory_requirements.tangents_size = (mesh_result.vertex_count * 12);
		attributes_memory_requirements.bitangents_size = (mesh_result.vertex_count * 12);
	}

	//we need n channels * vertex count * uv size as texcoord can have several channels
	attributes_memory_requirements.texture_coordinates_size = mesh_result.texture_coordinates_channel_count * (mesh_result.vertex_count * 8);

	if( mesh_result.has_bones )
	{
		INT32 bone_per_vertex = FBX_BONE_PER_VERTEX_LIMIT_FOUR;

		if( p_options->bones_import_mode == fbx_mesh_bones_import_mode::all_bones_compact )
		{
			//lookup table is per vertex, but index/weight vertex data is compacted per control point
			attributes_memory_requirements.bones_count_size = (mesh_result.vertex_count * sizeof(UINT32));
			attributes_memory_requirements.bones_location_size = (mesh_result.vertex_count * sizeof(UINT32));

			//we did precompute the total amount of bone influences, so we can use it here
			attributes_memory_requirements.bones_indices_size = (bone_data_info.sum_vertex_influence * sizeof(UINT32));
			attributes_memory_requirements.bones_weight_size = (bone_data_info.sum_vertex_influence * sizeof(float));

			mesh_result.vertex_weights_count = bone_data_info.sum_vertex_influence;
		}
		else
		{
			//other 3 cases, per vertex, flat table allocation
			if( p_options->bones_import_mode == fbx_mesh_bones_import_mode::all_bones_flat )
				bone_per_vertex = bone_data_info.max_bones_per_vertex;

			//count and offset table
			attributes_memory_requirements.bones_count_size = (mesh_result.vertex_count * sizeof(UINT32));
			attributes_memory_requirements.bones_location_size = (mesh_result.vertex_count * sizeof(UINT32));

			attributes_memory_requirements.bones_indices_size = (mesh_result.vertex_count * bone_per_vertex * sizeof(UINT32));
			attributes_memory_requirements.bones_weight_size = (mesh_result.vertex_count * bone_per_vertex * sizeof(float));

			mesh_result.vertex_weights_count = mesh_result.vertex_count * bone_per_vertex;
		}
	}


	UINT32 mesh_memory_requirement =
		attributes_memory_requirements.position_size
		+ attributes_memory_requirements.normals_size
		+ attributes_memory_requirements.tangents_size
		+ attributes_memory_requirements.bitangents_size
		+ attributes_memory_requirements.texture_coordinates_size
		+ attributes_memory_requirements.indices_size
		+ attributes_memory_requirements.bones_indices_size
		+ attributes_memory_requirements.bones_weight_size
		+ attributes_memory_requirements.bones_count_size
		+ attributes_memory_requirements.bones_location_size
		+ attributes_memory_requirements.bones_bind_pose_size
		+ attributes_memory_requirements.bones_node_index_table_size;

	//allocate
	mesh_result.data_length = mesh_memory_requirement;
	mesh_result.data_ptr = MALLOC( mesh_memory_requirement );

	unsigned char* ptr_start = (unsigned char*)mesh_result.data_ptr;

	unsigned char* ptr_current = ptr_start;

	mesh_result.array_location_info.position_ptr = mesh_result.data_ptr;

	ptr_current += attributes_memory_requirements.position_size;

	//note that if normal is not present, size is zero, no need for boolean test
	mesh_result.array_location_info.normal_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.normals_size;

	//uv
	mesh_result.array_location_info.texture_coordinates_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.texture_coordinates_size;

	//tangents
	mesh_result.array_location_info.tangent_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.tangents_size;

	mesh_result.array_location_info.bitangent_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.bitangents_size;

	//bones
	mesh_result.array_location_info.vertex_bones_data.bone_index_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.bones_indices_size;

	mesh_result.array_location_info.vertex_bones_data.bone_weight_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.bones_weight_size;

	mesh_result.array_location_info.vertex_bones_data.bone_count_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.bones_count_size;

	mesh_result.array_location_info.vertex_bones_data.bone_start_location_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.bones_location_size;

	//index buffer
	mesh_result.index_count = index_count;
	mesh_result.index_buffer_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.indices_size;

	//bone tables
	mesh_result.bones_data.bones_bind_pose_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.bones_bind_pose_size;

	mesh_result.bones_data.bones_node_index_table_ptr = ptr_current;
	ptr_current += attributes_memory_requirements.bones_node_index_table_size;


	//index buffer first, as we will eventually need it as lookup table for the rest
	INT32* dest_data_index_ptr = (INT32*)mesh_result.index_buffer_ptr;

	if( face_expand_required )
	{
		triangulate_expand( mesh, dest_data_index_ptr, *p_options );

		FbxVector4* source_data_position_ptr = mesh->GetControlPoints();
		float* dest_data_position_ptr = (float*)mesh_result.array_location_info.position_ptr;

		INT32* polygon_vertex_indices = mesh->GetPolygonVertices();

		//here we need to go lookup in the face table
		for( UINT32 i = 0; i < mesh_result.vertex_count; i++ )
		{
			INT32 vertexId = polygon_vertex_indices[i];
			write_double4_to_vec3( dest_data_position_ptr + i*3, source_data_position_ptr[vertexId], p_options->scaling );
		}

		//copy bones if required
		if( mesh_result.has_bones )
		{
			write_bone_data(
				mesh_result, 
				p_options->bones_import_mode, 
				bone_data_info, 
				bone_data, 
				mesh->GetControlPointsCount(), 
				polygon_vertex_indices );
		}
	}
	else
	{
		triangulate( mesh, dest_data_index_ptr, *p_options );

		FbxVector4* source_data_position_ptr = mesh->GetControlPoints();
		float* dest_data_position_ptr = (float*)mesh_result.array_location_info.position_ptr;

		for( UINT32 i = 0; i < mesh_result.vertex_count; i++ )
			write_double4_to_vec3( dest_data_position_ptr + i*3, source_data_position_ptr[i], p_options->scaling );

		if( mesh_result.has_bones )
		{
			write_bone_data(
				mesh_result, 
				p_options->bones_import_mode, 
				bone_data_info, 
				bone_data, 
				mesh->GetControlPointsCount(), 
				nullptr );
		}
	}

	//compute mesh bounding box
	{
		//todo common lib maa have a fn for this
		float const* src = (float*)mesh_result.array_location_info.position_ptr;
		fbx_bounding_box& bbox = mesh_result.bounding_box;
		float * p_min = bbox.min;
		float * p_max = bbox.max;

		cpy_v3fp32( p_min, src );
		cpy_v3fp32( p_max, src );

		for( UINT32 i = 1; i < mesh_result.vertex_count; i++ )
		{
			src += 3;
			//todoopt use fn using intrinsics
			*(p_min+0) = fmin( *(p_min+0),  *(src+0) );
			*(p_max+0) = fmax( *(p_max+0),  *(src+0) );

			*(p_min+1) = fmin( *(p_min+1),  *(src+1) );
			*(p_max+1) = fmax( *(p_max+1),  *(src+1) );

			*(p_min+2) = fmin( *(p_min+2),  *(src+2) );
			*(p_max+2) = fmax( *(p_max+2),  *(src+2) );
		}
	}

	//normals
	if( mesh_result.has_normals )
	{
		FbxGeometryElementNormal* normalElement = mesh->GetElementNormal(0);
		float* dest_data_normal_ptr = (float*)mesh_result.array_location_info.normal_ptr;
		write_double4_elements_to_vec3_array(mesh, dest_data_normal_ptr, normalElement, mesh_result.vertex_count);
	}

	//texcoords (1 channel for now)
	if( mesh_result.texture_coordinates_channel_count > 0 )
	{
		FbxGeometryElementUV const * uvElement = mesh->GetElementUV(0);
		float * dest_data_uv_ptr = (float*) mesh_result.array_location_info.texture_coordinates_ptr;
		write_double2_elements_to_vec2_array(
			mesh,
			dest_data_uv_ptr, 
			uvElement, 
			mesh_result.vertex_count, 
			p_options->flip_texture_coordinates_y );
	}

	//tangents/binormals
	if( mesh_result.has_tangent_frame )
	{
		FbxGeometryElementTangent * tangentElement = mesh->GetElementTangent(0);
		float * dest_data_tangent_ptr = (float*) mesh_result.array_location_info.tangent_ptr;
		write_double4_elements_to_vec3_array( mesh, dest_data_tangent_ptr, tangentElement, mesh_result.vertex_count );

		FbxGeometryElementBinormal * binormalElement = mesh->GetElementBinormal(0);
		float * dest_data_bitangent_ptr = (float*) mesh_result.array_location_info.bitangent_ptr;
		write_double4_elements_to_vec3_array( mesh, dest_data_bitangent_ptr, binormalElement, mesh_result.vertex_count );
	}

	//blend shapes
	mesh_load_blend_shapes( mesh, &mesh_result );

	//release intermediate bone data
	if( mesh_result.bones_data.bones_nb > 0 )
	{
		//copy bone tables (same size as input, so mem copy works)
		MEMCPY( mesh_result.bones_data.bones_bind_pose_ptr,			bone_data.inverse_bind_pose_ptr,	attributes_memory_requirements.bones_bind_pose_size,		__FUNCTION__ );
		MEMCPY( mesh_result.bones_data.bones_node_index_table_ptr,	bone_data.bone_node_index_ptr,		attributes_memory_requirements.bones_node_index_table_size,	__FUNCTION__ );

		free_mesh_skinning_data(			bone_data		);
		free_mesh_skinning_data_info(		bone_data_info	);
	}

	result.result_code = fbx_mesh_load_result_code::success;
	result.mesh = mesh_result;
	return result;
}

void fbx_release_mesh( fbx_mesh_data& p_mesh )
{
	IF_FREE_AND_NULL( p_mesh.data_ptr									);
	IF_FREE_AND_NULL( p_mesh.blend_shapes_data.position_data_ptr		);
	IF_FREE_AND_NULL( p_mesh.blend_shapes_data.normal_data_ptr			);
	IF_FREE_AND_NULL( p_mesh.blend_shapes_data.shapes_info_ptr			);
	IF_FREE_AND_NULL( p_mesh.blend_shapes_data.shapes_channel_info_ptr	);
	IF_FREE_AND_NULL( p_mesh.blend_shapes_data.shapes_target_ptr		);
}

void fbx_release_scene( fbx_scene_data& p_scene )
{
	IF_FREE_AND_NULL( p_scene.scene_info.animations				);
	IF_FREE_AND_NULL( p_scene.nodes_animations					);
	IF_FREE_AND_NULL( p_scene.animation_curves					);
	IF_FREE_AND_NULL( p_scene.animation_keyframes				);
	IF_FREE_AND_NULL( p_scene.nodes								);
	IF_FREE_AND_NULL( p_scene.node_names_table					);
	IF_FREE_AND_NULL( p_scene.object_mapping.native_node_table	);
	IF_FREE_AND_NULL( p_scene.materials							);
	IF_FREE_AND_NULL( p_scene.texture_files_name_table			);
	IF_FREE_AND_NULL( p_scene.mesh_blend_shape_animations		);
	IF_FREE_AND_NULL( p_scene.animation_names_table				);
	IF_FREE_AND_NULL( p_scene.material_names_table				);
}


void fbx_native_get_nodes_global_transforms(
	fbx_scene_native const *	p_scene_native, 
	fbx_scene_data const *		p_scene,
	UINT32 const				p_animation_stack_index,
	float						p_time, 
	float *						p_data_pointer )
{
	SPY_PUSH_RANGE_FUNCTION( spy::COL_2 );

		FbxScene* scene					= (FbxScene*)p_scene_native->scene_pointer;
		FbxNode * const * node_data		= (FbxNode**)p_scene->object_mapping.native_node_table;

		FbxAnimStack* const anim_stack	= scene->GetSrcObject< FbxAnimStack>(p_animation_stack_index);

		//keep previous stack if invalid
		if( anim_stack )
			scene->SetCurrentAnimationStack(anim_stack);

		FbxAnimEvaluator* const eval	= scene->GetAnimationEvaluator();

		static FbxTime current_fbx_time;
		current_fbx_time.SetSecondDouble(p_time);

		UINT32 nb = p_scene->scene_info.total_node_count;

		for( ; nb > 0; --nb )
		{
			double const * src_data = eval->GetNodeGlobalTransform( *node_data++, current_fbx_time );
			//0-3
			*p_data_pointer++ = (float)*  src_data;
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			//4-7
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			//8-11
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			//12-15
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
			*p_data_pointer++ = (float)*++src_data;
		}

	SPY_POP_RANGE();
}

void fbx_native_get_node_global_transform(
	fbx_scene_native const*	p_scene_native,
	fbx_scene_data const*	p_scene,
	UINT32					node_index,
	float*					p_data_pointer )
{
	if( node_index < 0 )
		debug_break( "%s() negative node index", __FUNCTION__ );
	else if( p_scene->scene_info.total_node_count <= node_index )
		debug_break( "%s() node index too big", __FUNCTION__ );
	else
	{
		FbxNode** node_data = (FbxNode**)p_scene->object_mapping.native_node_table;

		FbxScene* scene = (FbxScene*)p_scene_native->scene_pointer;

		auto eval = scene->GetAnimationEvaluator();

		FbxNode* fbx_node = node_data[node_index];
		auto mat = eval->GetNodeGlobalTransform(fbx_node);

		const double* src_data = mat;
		//0-3
		*  p_data_pointer = (float)*  src_data;
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		//4-7
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		//8-11
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		//12-15
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
		*++p_data_pointer = (float)*++src_data;
	}
}

INT32 fbx_scene_find_mesh_node_index(
	fbx_scene_data const *	p_scene,
	INT32					p_mesh_index )
{
	for( UINT32 i = 0; i < p_scene->scene_info.total_node_count; ++i )
	{
		if( p_scene->nodes[i].mesh_index == p_mesh_index )
			return i;
	}
	return -1;
}

st_fbx_material const * fbx_get_material(
	fbx_scene_data const *	p_scene, 
	UINT32 const			p_node_idx )
{
	st_fbx_material * res = nullptr;
	//todo check neg
	if( p_node_idx < p_scene->scene_info.total_node_count ) 
	{
		fbx_node const * node = &(p_scene->nodes[p_node_idx]);
		if( node->material_count > 0 ) 
			res = &(p_scene->materials[node->first_material_index]);
	}
	return res;
}

INT32 const fbx_get_material_index(
	fbx_scene_data const *	p_scene,
	UINT32 const			p_node_idx )
{
	INT32 res = -1;
	//todo check neg
	if( p_node_idx < p_scene->scene_info.total_node_count )
	{
		fbx_node const* node = &( p_scene->nodes[ p_node_idx ] );
		if( node->material_count > 0 )
			res = node->first_material_index;
	}
	return res;
}

