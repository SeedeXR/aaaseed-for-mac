
// Main header.
#include "fbx/internal/fbx_utils.h"
#include "fbx/internal/fbx_utils_mem.h"
#include "code_utils/err.h"


#if !defined(AAA_LIB_USE_H)
#	include "code_utils/lib_use.h"
#endif
#if	AAA_DEBUG()
	AAA_LIB_USE("fbx/debug/libfbxsdk-md")
	AAA_LIB_USE("fbx/debug/libxml2-md")
#else
	AAA_LIB_USE("fbx/release/libfbxsdk-md")
	AAA_LIB_USE("fbx/release/libxml2-md")
#endif


fbx_scene_info scene_get_info( FbxScene* p_scene )
{
	fbx_scene_info result;

	result.mesh_count			= p_scene->GetSrcObjectCount<FbxMesh>();
	result.animation_stack_nb	= p_scene->GetSrcObjectCount<FbxAnimStack>();
	result.material_count		= p_scene->GetSrcObjectCount<FbxSurfaceMaterial>();
	result.file_texture_count	= p_scene->GetSrcObjectCount<FbxFileTexture>();

	//allocate animations
	if( result.animation_stack_nb > 0 )
	{
		result.animations = (fbx_animation_info*)MALLOC(result.animation_stack_nb * sizeof(fbx_animation_info));
		MEMCLEAR( result.animations, result.animation_stack_nb * sizeof(fbx_animation_info) );
	}
	else
		result.animations = nullptr;

	fbx_scene_info_internal internal_info = scene_info_internal(p_scene, &result);

	result.total_node_count = internal_info.node_count;
	result.animation_track_count = internal_info.track_count;
	result.animation_keyframe_total_count = internal_info.curve_keyframe_count;
	result.mesh_blend_shape_animations_count = internal_info.mesh_blend_shape_animations_count;

	//axis and system unit
	FbxGlobalSettings & global_settings = p_scene->GetGlobalSettings();
	//FbxSystemUnit system_unit = global_settings.GetSystemUnit();

	FbxAxisSystem axis_system = global_settings.GetAxisSystem();

	INT32 up_sign;
	FbxAxisSystem::EUpVector up_vector = axis_system.GetUpVector(up_sign);

	INT32 front_sign;
	FbxAxisSystem::EFrontVector front_vector = axis_system.GetFrontVector(front_sign);

	//From fbx axis system, copy paste here to explain how we deduce the front vector
	//For example if the up axis is X, the remain two axes will be Y And Z, so the ParityEven is Y, and the ParityOdd is Z
	//	; If the up axis is Y, the remain two axes will X And Z, so the ParityEven is X, and the ParityOdd is Z;
		//If the up axis is Z, the remain two axes will X And Y, so the ParityEven is X, and the ParityOdd is Y.

	//todo use switch
	switch( up_vector )
	{
	case FbxAxisSystem::eXAxis:
		result.up_axis_vector = fbx_axis::x;
		switch( front_vector )
		{
		case FbxAxisSystem::EFrontVector::eParityEven:	result.front_axis_vector = fbx_axis::y;	result.right_axis_vector = fbx_axis::z; break;
		case FbxAxisSystem::EFrontVector::eParityOdd:	result.front_axis_vector = fbx_axis::z;	result.right_axis_vector = fbx_axis::y; break;
		}
		break;
	case FbxAxisSystem::eYAxis:
		result.up_axis_vector = fbx_axis::y;
		switch( front_vector )
		{
		case FbxAxisSystem::EFrontVector::eParityEven:	result.front_axis_vector = fbx_axis::x;	result.right_axis_vector = fbx_axis::z; break;
		case FbxAxisSystem::EFrontVector::eParityOdd:	result.front_axis_vector = fbx_axis::z;	result.right_axis_vector = fbx_axis::x; break;
		}
		break;
	case FbxAxisSystem::eZAxis:
		result.up_axis_vector = fbx_axis::z;
		switch( front_vector )
		{
		case FbxAxisSystem::EFrontVector::eParityEven:	result.front_axis_vector = fbx_axis::x;	result.right_axis_vector = fbx_axis::y; break;
		case FbxAxisSystem::EFrontVector::eParityOdd:	result.front_axis_vector = fbx_axis::y;	result.right_axis_vector = fbx_axis::x; break;
		}
		break;
	}

	result.up_axis_vector_direction = (up_sign > 0) ? fbx_axis_direction::positive : fbx_axis_direction::negative;
	result.front_axis_vector_direction = (front_sign > 0) ? fbx_axis_direction::positive : fbx_axis_direction::negative;

	//todo deal also with the direction of right_axis vector
	//	it should be deduced using also left/right handed
	return result;
}

INT32 get_file_texture_index( FbxScene* p_scene_native, FbxFileTexture* p_file_texture )
{
	for( INT32 i = 0; i < p_scene_native->GetSrcObjectCount<FbxFileTexture>(); i++ )
	{
		auto ft = p_scene_native->GetSrcObject<FbxFileTexture>(i);
		if( ft == p_file_texture )
			return i;
	}
	return -1;
}

INT32 get_material_index( FbxScene* p_scene_native, FbxSurfaceMaterial* p_material )
{
	for( INT32 i = 0; i < p_scene_native->GetSrcObjectCount<FbxSurfaceMaterial>(); i++ )
	{
		auto ft = p_scene_native->GetSrcObject<FbxSurfaceMaterial>(i);
		if( ft == p_material )
			return i;
	}
	return -1;
}


INT32 try_get_texture_index( FbxScene* p_scene_native, FbxSurfaceMaterial* material, const char* texture_type )
{
	FbxProperty property = material->FindProperty(texture_type);
	INT32 texture_count = property.GetSrcObjectCount<FbxFileTexture>();
	if( texture_count > 0 )
		return get_file_texture_index(p_scene_native, property.GetSrcObject<FbxFileTexture>(0));
	else
		return -1;
}

INT32 get_animation_step_count(
	const fbx_animation_info* animation_info,
	float const time_step)
{
	float const animation_time = animation_info->end_time - animation_info->start_time;
	return (INT32)ceil( animation_time / time_step );
}


bool is_face_expand_required( FbxMesh* p_mesh, fbx_mesh_load_options const * p_options )
{
	//if generate option is on, we already should have generated those, we can still have ignore option
	if( p_options->normals != fbx_mesh_attribute_options::ignore )
	{
		FbxGeometryElementNormal* normalElement = p_mesh->GetElementNormal();
		if( normalElement )
		{
			if( normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
				return true;
		}
	}

	if( p_options->texture_coordinates == fbx_multiple_attribute_import_mode::all )
	{
		for( INT32 i = 0; i < p_mesh->GetElementUVCount(); i++ )
		{
			FbxGeometryElementUV* uvElement = p_mesh->GetElementUV(i);
			if( uvElement )
			{
				if( uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
					return true;
			}
		}
	}
	else if( p_options->texture_coordinates == fbx_multiple_attribute_import_mode::first )
	{
		if( p_mesh->GetElementUVCount() > 0 )
		{
			FbxGeometryElementUV* uvElement = p_mesh->GetElementUV(0);
			if( uvElement )
			{
				if( uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex )
					return true;
			}
		}
	}

	return false;
}

FbxAMatrix get_geometric_transformation( FbxNode* pNode )
{
	const FbxVector4 t = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 r = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 s = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(t, r, s);
}

INT32 compute_vertex_count_expanded( FbxMesh* p_mesh )
{
	UINT32 vertex_count = 0;
	INT32 nb = p_mesh->GetPolygonCount();
	for( INT32 i = 0; i < nb; ++i )
		vertex_count += p_mesh->GetPolygonSize(i);
	return vertex_count;
}

INT32 compute_index_count( FbxMesh* p_mesh )
{
	UINT32 index_count = 0;
	INT32 nb = p_mesh->GetPolygonCount();
	for( INT32 i = 0; i < nb; ++i )
	{
		INT32 polygon_size = p_mesh->GetPolygonSize(i);
		//can be -1, so we make sure to handle that
		if( polygon_size >= 3 )
		{
			//add first triangle
			index_count += 3; 
			//add one triangle per new polygon item
			if( polygon_size > 3 )
			{
				INT32 left = polygon_size - 3;
				index_count += (left * 3);
			}
		}
	}
	return index_count;
}


void triangulate( FbxMesh* p_mesh, INT32* dst_data_index_ptr, const fbx_mesh_load_options& p_load_options )
{
	//increment for first and second edge
	INT32 tri_index_first, tri_index_second;
	//increment for fan
//	INT32 fan_index_first, fan_index_second;

	if( p_load_options.reverse_winding )
	{
		tri_index_first = 2;
		tri_index_second = 1;
//		fan_index_first = 0;
//		fan_index_second = -1;
	}
	else
	{
		tri_index_first = 1;
		tri_index_second = 2;
//		fan_index_first = -1;
//		fan_index_second = 0;
	}

	INT32 nb = p_mesh->GetPolygonCount();
	for( INT32 i = 0; i < nb; ++i )
	{
		INT32 polygon_size = p_mesh->GetPolygonSize(i);
		if( polygon_size >= 3 )
		{
			//first tri
			*dst_data_index_ptr = p_mesh->GetPolygonVertex(i, 0);
			++dst_data_index_ptr;
			*dst_data_index_ptr = p_mesh->GetPolygonVertex(i, tri_index_first );
			++dst_data_index_ptr;
			*dst_data_index_ptr = p_mesh->GetPolygonVertex(i, tri_index_second );
			++dst_data_index_ptr;

			/*for (INT32 c = 3; c < polygon_size; ++c)
			{
				*dst_data_index_ptr = p_mesh->GetPolygonVertex(i, c);
				dst_data_index_ptr++;
				*dst_data_index_ptr = p_mesh->GetPolygonVertex(i, c+1);
				dst_data_index_ptr++;
				*dst_data_index_ptr = p_mesh->GetPolygonVertex(i, c+2);
				dst_data_index_ptr++;
			}*/
		}
	}
}

void triangulate_expand( FbxMesh* p_mesh, INT32* dst_data_index_ptr, const fbx_mesh_load_options& p_load_options )
{
	//increment for first and second edge
	INT32 tri_index_first, tri_index_second;
	//increment for fan
	INT32 fan_index_first, fan_index_second;

	if( p_load_options.reverse_winding )
	{
		tri_index_first = 2;
		tri_index_second = 1;
		fan_index_first = 0;
		fan_index_second = -1;
	}
	else
	{
		tri_index_first = 1;
		tri_index_second = 2;
		fan_index_first = -1;
		fan_index_second = 0;
	}

	//note : locked to triangles for now
	INT32 current_index = 0;
	INT32 nb = p_mesh->GetPolygonCount();
	for( INT32 i = 0; i < nb; ++i )
	{
		INT32 polygon_size = p_mesh->GetPolygonSize(i);
		if( polygon_size >= 3 )
		{
			*dst_data_index_ptr = current_index;
			++dst_data_index_ptr;
			*dst_data_index_ptr = current_index + tri_index_first;
			++dst_data_index_ptr;
			*dst_data_index_ptr = current_index + tri_index_second;
			++dst_data_index_ptr;

			if( polygon_size > 3 )
			{
				for( INT32 c = 3; c < polygon_size; ++c )
				{
					*dst_data_index_ptr = current_index;
					++dst_data_index_ptr;
					*dst_data_index_ptr = current_index + c + fan_index_first;
					++dst_data_index_ptr;
					*dst_data_index_ptr = current_index + c + fan_index_second;
					++dst_data_index_ptr;
				}
			}
		}
		current_index += polygon_size;
	}
}

INT32 process_animation_curve(
	FbxAnimCurve*		p_anim_curve,
	fbx_animation_info*	p_animation_info,
	bool				p_is_blend_shape )
{
	if( p_anim_curve )
	{
		p_animation_info->track_count++;
		p_animation_info->keyframe_count += p_anim_curve->KeyGetCount();

		float time_first = p_anim_curve->KeyGetTime(0).GetMilliSeconds() * 0.001f;
		float time_last = p_anim_curve->KeyGetTime(p_anim_curve->KeyGetCount() - 1).GetMilliSeconds() * 0.001f;

		if( time_first < p_animation_info->start_time )
			p_animation_info->start_time = time_first;
		if( time_last > p_animation_info->end_time )
			p_animation_info->end_time = time_last;
		if( p_is_blend_shape )
		{
			if( time_first < p_animation_info->blend_shape_start_time )
				p_animation_info->blend_shape_start_time = time_first;
			if( time_last > p_animation_info->blend_shape_end_time )
				p_animation_info->blend_shape_end_time = time_last;
		}
		else
		{
			if( time_first < p_animation_info->node_start_time )
				p_animation_info->node_start_time = time_first;
			if( time_last > p_animation_info->node_end_time )
				p_animation_info->node_end_time = time_last;
		}
		return 1;
	}
	else
		return 0;
}

void process_property_component(
	FbxAnimLayer*				p_anim_layer, 
	FbxPropertyT<FbxDouble3>&	p_property, 
	fbx_animation_info*			p_animation_info,
	const char *				p_name )
{
	FbxAnimCurve* anim_curve = p_property.GetCurve( p_anim_layer, p_name );
	process_animation_curve( anim_curve, p_animation_info, false );
}

void process_node_property(
	FbxAnimLayer *				p_anim_layer, 
	FbxPropertyT<FbxDouble3>&	p_property, 
	fbx_animation_info *		p_animation_info )
{
	process_property_component( p_anim_layer, p_property, p_animation_info, FBXSDK_CURVENODE_COMPONENT_X );
	process_property_component( p_anim_layer, p_property, p_animation_info, FBXSDK_CURVENODE_COMPONENT_Y );
	process_property_component( p_anim_layer, p_property, p_animation_info, FBXSDK_CURVENODE_COMPONENT_Z );
}

void scene_traverse_animation_info(
	FbxNode *					p_node, 
	FbxAnimLayer *				p_anim_layer, 
	fbx_animation_info *		p_animation_info )
{
	process_node_property( p_anim_layer, p_node->LclTranslation,	p_animation_info );
	process_node_property( p_anim_layer, p_node->LclScaling,		p_animation_info );
	process_node_property( p_anim_layer, p_node->LclRotation,		p_animation_info );

	INT32 const nb = p_node->GetChildCount();
	for( INT32 i = 0; i < nb; ++i )
		scene_traverse_animation_info(p_node->GetChild(i), p_anim_layer, p_animation_info);
}

INT32 scene_load_animation_blend_shapes_info(
	FbxScene*			p_scene,
	FbxAnimLayer*		p_anim_layer,
	fbx_animation_info*	p_animation_info )
{
	INT32 result = 0;
	UINT32 mesh_count = p_scene->GetSrcObjectCount<FbxMesh>();
	for( UINT32 mesh_id = 0; mesh_id < mesh_count; ++mesh_id )
	{
		FbxMesh* mesh = p_scene->GetSrcObject<FbxMesh>(mesh_id);
		UINT32 shape_count = mesh->GetDeformerCount(FbxDeformer::eBlendShape);

		for( UINT32 shape_index = 0; shape_index < shape_count; ++shape_index )
		{
			FbxBlendShape* blend_shape_src = (FbxBlendShape*)mesh->GetDeformer(shape_index, FbxDeformer::eBlendShape);
			UINT32 channel_count = blend_shape_src->GetBlendShapeChannelCount();

			for( UINT32 channel_index = 0; channel_index < channel_count; ++channel_index )
			{
				FbxAnimCurve* anim_curve = mesh->GetShapeChannel( shape_index, channel_index, p_anim_layer );
				INT32 curve_nb = process_animation_curve( anim_curve, p_animation_info, true );
				result += curve_nb;
			}
		}
	}
	return result;
}


void scene_count_nodes(
	FbxNode*					p_node,
	fbx_scene_info_internal&	p_info )
{
	++p_info.node_count;
	INT32 const nb = p_node->GetChildCount();
	for( INT32 i = 0; i < nb; ++i )
		scene_count_nodes( p_node->GetChild(i), p_info );
}

fbx_scene_info_internal scene_info_internal(
	FbxScene *			p_scene,
	fbx_scene_info *	p_scene_info )
{
	//root node
	fbx_scene_info_internal result;
	MEMCLEAR( &result, sizeof(fbx_scene_info_internal) );

	result.track_count = 0;
	result.curve_keyframe_count = 0;

	//count nodes first
	scene_count_nodes( p_scene->GetRootNode(), result );

	//process animation layers
	FbxAnimLayer * anim_layer = nullptr;
	INT32 const animation_stack_nb = p_scene->GetSrcObjectCount<FbxAnimStack>();

	INT32 current_track_location = 0;
	INT32 current_keyframe_location = 0;

	for( INT32 i = 0; i < animation_stack_nb; ++i )
	{
		FbxAnimStack* anim_stack = p_scene->GetSrcObject<FbxAnimStack>(i);

		INT32 const anim_layer_count = anim_stack->GetMemberCount<FbxAnimLayer>();
		p_scene_info->animations[i].start_time = FLT_MAX;
		p_scene_info->animations[i].end_time = -FLT_MAX;
		p_scene_info->animations[i].blend_shape_start_time = FLT_MAX;
		p_scene_info->animations[i].blend_shape_end_time = -FLT_MAX;
		p_scene_info->animations[i].node_start_time = FLT_MAX;
		p_scene_info->animations[i].node_end_time = -FLT_MAX;

		p_scene_info->animations[i].layer_count = anim_layer_count;
		p_scene_info->animations[i].first_keyframe_location = current_keyframe_location;
		p_scene_info->animations[i].first_track_location = current_track_location;

		if( anim_layer_count > 0 )
		{
			anim_layer = anim_stack->GetMember<FbxAnimLayer>(0);

			//traverse info
			scene_traverse_animation_info(p_scene->GetRootNode(), anim_layer, &p_scene_info->animations[i]);

			//traverse blend shapes
			INT32 blend_shape_curve_nb = scene_load_animation_blend_shapes_info(p_scene, anim_layer, &p_scene_info->animations[i]);

			//accumulate counters
			current_track_location += p_scene_info->animations[i].track_count;
			current_keyframe_location += p_scene_info->animations[i].keyframe_count;

			result.track_count += p_scene_info->animations[i].track_count;
			result.curve_keyframe_count += p_scene_info->animations[i].keyframe_count;
			result.mesh_blend_shape_animations_count += blend_shape_curve_nb;
		}
	}
	return result;
}

void write_animation_curve(
	FbxAnimCurve *					p_anim_curve,
	fbx_scene_write_context_data &	p_context,
	fbx_scene_data const &			p_scene_result,
	INT32 const						p_animation_stack_index,
	c_fbx_animation_curve*			p_curve
)
{
	//if keyframe as default, write as is
	if( p_scene_result.load_options.animation_mode == fbx_animation_import_mode::keyframes_default )
	{
		p_curve->keyframe_count = p_anim_curve->KeyGetCount();
		for( UINT32 i = 0; i < p_curve->keyframe_count; ++i )
		{
			fbx_animation_keyframe keyframe;
			MEMCLEAR( &keyframe, sizeof(fbx_animation_keyframe) );
			keyframe.time = p_anim_curve->KeyGetTime(i).GetMilliSeconds() * 0.001f;
			keyframe.value = p_anim_curve->KeyGetValue(i);

			FbxAnimCurveDef::EInterpolationType interpolation = p_anim_curve->KeyGetInterpolation(i);

			if( interpolation == FbxAnimCurveDef::EInterpolationType::eInterpolationConstant )
				keyframe.interpolation = fbx_keyframe_interpolation::no_interpolation;
			else if( interpolation == FbxAnimCurveDef::EInterpolationType::eInterpolationCubic )
			{
				keyframe.interpolation = fbx_keyframe_interpolation::cubic;
				keyframe.left_tangent_velocity = p_anim_curve->KeyGetLeftTangentVelocity(i);
				keyframe.left_tangent_weight = p_anim_curve->KeyGetLeftTangentWeight(i);
				keyframe.right_tangent_velocity = p_anim_curve->KeyGetRightTangentVelocity(i);
				keyframe.right_tangent_weight = p_anim_curve->KeyGetRightTangentWeight(i);
			}
			else
				keyframe.interpolation = fbx_keyframe_interpolation::linear;

			p_scene_result.animation_keyframes[p_context.animation_keyframe_write_index] = keyframe;
			p_context.animation_keyframe_write_index++;
		}
	}
	else
	{

		/*float const animation_time = p_scene_result.scene_info.animations[p_animation_stack_index].end_time 
			- p_scene_result.scene_info.animations[p_animation_stack_index].start_time;

		float const step_count = animation_time / p_scene_result.load_options.animation_time_step;
		INT32 const keyframe_count = (INT32)step_count;*/

		INT32 const keyframe_count = get_animation_step_count(
			&p_scene_result.scene_info.animations[p_animation_stack_index],
			p_scene_result.load_options.animation_time_step);

		p_curve->keyframe_count = keyframe_count;
		//should be true, but just in case
		if( p_curve->keyframe_count > 0 )
		{
			double time_step = p_scene_result.load_options.animation_time_step;

			double current_time = p_scene_result.scene_info.animations[p_animation_stack_index].start_time;

			FbxTime current_fbx_time;
			current_fbx_time.SetSecondDouble(current_time);

			for( INT32 i = 0; i < keyframe_count; ++i )
			{
				fbx_animation_keyframe keyframe;
				keyframe.interpolation = fbx_keyframe_interpolation::linear;
				keyframe.time = (float)current_time;
				keyframe.value = (float)p_anim_curve->Evaluate(current_fbx_time);

				current_time += time_step;
				current_fbx_time.SetSecondDouble(current_time);

				p_scene_result.animation_keyframes[p_context.animation_keyframe_write_index] = keyframe;
				p_context.animation_keyframe_write_index++;
			}
		}
	}

	p_scene_result.animation_curves[p_context.animation_curve_write_index] = *p_curve;

	//increment curve index
	p_context.animation_curve_write_index++;
}


INT32 write_animation_property_component(
	FbxAnimLayer *					p_anim_layer,
	FbxPropertyT<FbxDouble3> &		p_property,
	char const *					p_name,
	INT32 const						p_attribute_index,
	fbx_scene_data const &			p_scene_result,
	fbx_scene_write_context_data &	p_context,
	INT32 const						p_animation_stack_index )
{
	FbxAnimCurve* anim_curve = p_property.GetCurve( p_anim_layer, p_name );
	if( anim_curve )
	{
		c_fbx_animation_curve curve;
		curve.curve_attribute = (fbx_curve_attribute_kind)p_attribute_index;
		curve.index_in_keyframe_table = p_context.animation_keyframe_write_index;
		write_animation_curve(anim_curve, p_context, p_scene_result, p_animation_stack_index, &curve);
		return 1;
	}
	else
		return 0;
}

void scene_write_animation_blend_shapes_curves(
	FbxScene*						p_scene,
	FbxAnimLayer*					p_anim_layer,
	fbx_scene_data const &			p_scene_result,
	fbx_scene_write_context_data &	p_context,
	INT32 const						p_animation_stack_index)
{
	UINT32 blend_shape_animation_index = 0;
	UINT32 mesh_count = p_scene->GetSrcObjectCount<FbxMesh>();

	for( UINT32 mesh_id = 0; mesh_id < mesh_count; mesh_id++ )
	{
		FbxMesh* mesh = p_scene->GetSrcObject<FbxMesh>(mesh_id);
		UINT32 shape_count = mesh->GetDeformerCount(FbxDeformer::eBlendShape);

		for( UINT32 shape_index = 0; shape_index < shape_count; ++shape_index )
		{
			FbxBlendShape* blend_shape_src = (FbxBlendShape*)mesh->GetDeformer(shape_index, FbxDeformer::eBlendShape);
			UINT32 channel_count = blend_shape_src->GetBlendShapeChannelCount();

			for( UINT32 channel_index = 0; channel_index < channel_count; ++channel_index )
			{
				FbxAnimCurve* anim_curve = mesh->GetShapeChannel(shape_index, channel_index, p_anim_layer);

				if( anim_curve )
				{
					c_fbx_animation_curve curve;
					curve.curve_attribute = fbx_curve_attribute_kind::position_x;
					curve.index_in_keyframe_table = p_context.animation_keyframe_write_index;

					fbx_mesh_blend_shape_animation blend_shape_animation;
					blend_shape_animation.animation_curve_index = p_context.animation_curve_write_index;
					blend_shape_animation.animation_index = p_animation_stack_index;
					blend_shape_animation.blend_shape_channel_index = channel_index;
					blend_shape_animation.blend_shape_index = shape_index;
					blend_shape_animation.mesh_index = mesh_id;

					write_animation_curve(anim_curve, p_context, p_scene_result, p_animation_stack_index, &curve);

					p_scene_result.mesh_blend_shape_animations[blend_shape_animation_index] = blend_shape_animation;

					++blend_shape_animation_index;
				}
			}
		}
	}
}

INT32 write_animation_property(FbxAnimLayer * p_anim_layer,
	FbxPropertyT<FbxDouble3>&		p_property,
	const INT32						attribute_start_index,
	const fbx_scene_data&			scene_result,
	fbx_scene_write_context_data&	context,
	INT32 const						p_animation_stack_index )
{
	INT32 animation_count = 0;

	animation_count += write_animation_property_component(p_anim_layer, p_property, FBXSDK_CURVENODE_COMPONENT_X, attribute_start_index, scene_result, context, p_animation_stack_index);
	animation_count += write_animation_property_component(p_anim_layer, p_property, FBXSDK_CURVENODE_COMPONENT_Y, attribute_start_index + 1, scene_result, context, p_animation_stack_index);
	animation_count += write_animation_property_component(p_anim_layer, p_property, FBXSDK_CURVENODE_COMPONENT_Z, attribute_start_index + 2, scene_result, context, p_animation_stack_index);

	return animation_count;
}

void update_material_textures( st_fbx_material& material, FbxScene* p_scene_native, FbxSurfaceMaterial* material_native )
{
	material.ambient_texture_index			= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sAmbient );
	material.diffuse_texture_index			= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sDiffuse );
	material.emissive_texture_index			= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sEmissive );
	material.specular_texture_index			= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sSpecular );
	material.normal_texture_index			= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sNormalMap );
	material.shininess_texture_index		= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sShininess );
	material.specular_factor_texture_index	= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sSpecularFactor );
	material.bump_texture_index				= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sBump );
	material.opacity_texture_index			= try_get_texture_index( p_scene_native, material_native, FbxSurfaceMaterial::sTransparentColor );
}

void update_material( st_fbx_material& material, FbxScene* p_scene_native, FbxSurfaceMaterial* material_native )
{
	if( material_native->GetClassId().Is( FbxSurfacePhong::ClassId ) )
	{
		material._type = fbx_material_type_e::phong_surface;

		FbxSurfacePhong const* surface = (FbxSurfacePhong const*)material_native;

		cpy_v3_fbx_double_to_fp32( material.ambient_color, surface->Ambient.Get() );
		cpy_v3_fbx_double_to_fp32( material.diffuse_color, surface->Diffuse.Get() );
		cpy_v3_fbx_double_to_fp32( material.specular_color, surface->Specular.Get() );
		cpy_v3_fbx_double_to_fp32( material.emissive_color, surface->Emissive.Get() );

		material.opacity = 1.0f - (float)( (FbxSurfacePhong*)material_native )->TransparencyFactor.Get();
		material.shininess = (float)( (FbxSurfacePhong*)material_native )->Shininess.Get();
		material.reflectiviy = (float)( (FbxSurfacePhong*)material_native )->ReflectionFactor.Get();

		update_material_textures( material, p_scene_native, material_native );
	}
	else if( material_native->GetClassId().Is( FbxSurfaceLambert::ClassId ) )
	{
		material._type = fbx_material_type_e::lambert_surface;

		FbxSurfaceLambert const* surface = (FbxSurfaceLambert const*)material_native;

		cpy_v3_fbx_double_to_fp32( material.ambient_color, surface->Ambient.Get() );
		cpy_v3_fbx_double_to_fp32( material.diffuse_color, surface->Diffuse.Get() );
		cpy_v3_fbx_double_to_fp32( material.emissive_color, surface->Emissive.Get() );

		material.opacity = 1.0f - (float)surface->TransparencyFactor.Get();

		update_material_textures( material, p_scene_native, material_native );
	}
	else
	{
		material._type = fbx_material_type_e::unknown;
	}
}

void load_node_materials(
	FbxScene*				p_scene_native,
	FbxNode*				p_node,
	fbx_scene_data const&	scene_result )
{
	// Import the materials.
	INT32 material_count = p_node->GetMaterialCount();
	for( INT32 n = 0; n < material_count; n++ )
	{
		FbxSurfaceMaterial* material_native = p_node->GetMaterial( n );
		if( material_native )
		{
			auto CONST material_index = get_material_index( p_scene_native, material_native );
			st_fbx_material& material = scene_result.materials[ material_index ];

			//write name
			INT32 name_table_index = material_index * FBX_NAME_MAX_LENGTH;
			const char* name = material_native->GetName();
			char* ptr_name_dest = scene_result.material_names_table;
			ptr_name_dest += name_table_index;
			strcpy_s( ptr_name_dest, FBX_NAME_MAX_LENGTH, name );

			update_material( material, p_scene_native, material_native );
		}
	}
}

void scene_traverse_write_nodes(
	FbxScene*						p_scene, 
	FbxNode*						p_node,
	const fbx_scene_data&			scene_result,
	fbx_scene_write_context_data&	context)
{
	INT32 child_count = p_node->GetChildCount();

	INT32 node_index = context.current_node_index;
	fbx_node& node =  scene_result.nodes[node_index];

	node.mesh_index = -1;
	node.parent_index = context.node_parent_index;
	node.child_count = child_count;

	//write name
	INT32 name_table_index = node_index * FBX_NAME_MAX_LENGTH;
	const char* name = p_node->GetName();

	char * ptr_name_dest = scene_result.node_names_table;
	ptr_name_dest += name_table_index;

	strcpy_s(ptr_name_dest, FBX_NAME_MAX_LENGTH, name);

	//write mapping
	FbxNode** mapping_ptr = (FbxNode**)scene_result.object_mapping.native_node_table;
	mapping_ptr[node_index] = p_node;

	auto p_attached_mesh = p_node->GetMesh();
	if( p_attached_mesh )
	{
		INT32 mesh_count = p_scene->GetSrcObjectCount<FbxMesh>();
		for( INT32 mesh_index = 0; mesh_index < mesh_count; mesh_index++ )
		{
			FbxMesh* mesh = p_scene->GetSrcObject<FbxMesh>(mesh_index);

			if( mesh == p_attached_mesh )
			{
				node.mesh_index = mesh_index;
				// in case of multiple meshes, we need to get the material from the node, getting it from the scene doesn't work
				load_node_materials( p_scene, p_node, scene_result );
				break;
			}
		}
	}

	auto p_attached_skin = p_node->GetSkeleton();
	node.is_armature = p_attached_skin != nullptr;

	//write transformation
	cpy_v3_fbx_double_to_fp32( node.position,				p_node->LclTranslation		.Get() );
	cpy_v3_fbx_double_to_fp32( node.scaling,				p_node->LclScaling			.Get() );
	cpy_v3_fbx_double_to_fp32( node.rotation,				p_node->LclRotation			.Get() );
	cpy_v3_fbx_double_to_fp32( node.rotation_offset,		p_node->RotationOffset		.Get() );
	cpy_v3_fbx_double_to_fp32( node.rotation_pivot,			p_node->RotationPivot		.Get() );
	cpy_v3_fbx_double_to_fp32( node.pre_rotation,			p_node->PreRotation			.Get() );
	cpy_v3_fbx_double_to_fp32( node.post_rotation,			p_node->PostRotation		.Get() );
	cpy_v3_fbx_double_to_fp32( node.scaling_offset,			p_node->ScalingOffset		.Get() );
	cpy_v3_fbx_double_to_fp32( node.scaling_pivot,			p_node->ScalingPivot		.Get() );
	cpy_v3_fbx_double_to_fp32( node.geometric_translation,	p_node->GeometricTranslation.Get() );
	cpy_v3_fbx_double_to_fp32( node.geometric_scaling,		p_node->GeometricScaling	.Get());
	cpy_v3_fbx_double_to_fp32( node.geometric_rotation,		p_node->GeometricRotation	.Get() );

	FbxEuler::EOrder order = p_node->RotationOrder.Get();
	node.rotation_order = (fbx_rotation_order)order;
 
	INT32 material_count = p_node->GetMaterialCount();
	node.material_count = material_count;
	if( material_count > 0 )
	{
		auto material = p_node->GetMaterial(0);
		node.first_material_index = get_material_index(p_scene, material);
	}
	else
		node.first_material_index = -1;

	if( child_count > 0 )
	{
		INT32 child_first_location = context.node_write_index;
		node.child_first_index_location = child_first_location;

		context.node_write_index += child_count;

		//write all children in a contiguous manner
		for( INT32 i = 0; i < child_count; i++ )
		{
			context.current_node_index = child_first_location + i;
			context.node_parent_index = node_index;
			scene_traverse_write_nodes(p_scene, p_node->GetChild(i), scene_result, context);
		}
	}
	else
		node.child_first_index_location = -1;
}

void scene_write_node_animations(
 //	FbxScene const * const p_scene,
	FbxAnimLayer*					p_anim_layer,
	fbx_scene_data const &			scene_result,
	fbx_scene_write_context_data&	context,
	INT32							animation_stack_index )
{
	for( UINT32 node_index = 0; node_index < scene_result.scene_info.total_node_count; node_index++ )
	{
	//todo unused so why there?		fbx_node const & node = scene_result.nodes[node_index];
		FbxNode * const node_native = ((FbxNode**)scene_result.object_mapping.native_node_table)[node_index];

		INT32 node_animation_location = (animation_stack_index * scene_result.scene_info.total_node_count) + node_index;
		fbx_node_animation* node_animation = &scene_result.nodes_animations[node_animation_location];

		node_animation->animation_table_location = context.animation_curve_write_index;

		INT32 animation_count = 0;

		//write animations for this node
		animation_count += write_animation_property(p_anim_layer, node_native->LclTranslation, (INT32)fbx_curve_attribute_kind::position_x, scene_result, context, animation_stack_index );
		animation_count += write_animation_property(p_anim_layer, node_native->LclScaling,     (INT32)fbx_curve_attribute_kind::scale_x,    scene_result, context, animation_stack_index );
		animation_count += write_animation_property(p_anim_layer, node_native->LclRotation,    (INT32)fbx_curve_attribute_kind::rotation_x, scene_result, context, animation_stack_index );

		node_animation->animation_count = animation_count;
	}
	
	if( context.animation_curve_write_index >= scene_result.scene_info.animation_track_count )
	{
		context.animation_curve_write_index = 0;
	}
}

void scene_write_data(
	FbxScene *				p_scene, 
	fbx_scene_data const &	p_scene_result )
{
	fbx_scene_write_context_data context_data;
	context_data.current_node_index				= 0;
	context_data.node_parent_index				= -1;	//todo refine
	context_data.node_write_index				= 1;//first child index
	context_data.animation_curve_write_index	= 0;
	context_data.animation_keyframe_write_index	= 0;

	scene_traverse_write_nodes( p_scene, p_scene->GetRootNode(), p_scene_result, context_data );

	//now write all animations
	fbx_node_animation* pt_node = p_scene_result.nodes_animations;
	INT32 const stack_nb = p_scene_result.scene_info.animation_stack_nb;
	for( INT32 i = 0; i < stack_nb; i++ )
	{
		//initialize node animations with valid data (no animation)  
		for( INT32 nb = p_scene_result.scene_info.total_node_count; nb > 0; --nb )
		{
			pt_node->animation_count			= 0;
			pt_node->animation_table_location	= -1;
			++pt_node;
		}

		FbxAnimStack* anim_stack = p_scene->GetSrcObject<FbxAnimStack>(i);

		//get animation name for UI
		auto name = anim_stack->GetName();
		INT32 const name_table_index = i * FBX_NAME_MAX_LENGTH;
		char* ptr_name_dest = p_scene_result.animation_names_table;
		ptr_name_dest += name_table_index;
		strcpy_s( ptr_name_dest, FBX_NAME_MAX_LENGTH, name );

		INT32 anim_layer_count = anim_stack->GetMemberCount<FbxAnimLayer>();
		if( anim_layer_count > 0 )
		{
			FbxAnimLayer * const anim_layer = anim_stack->GetMember<FbxAnimLayer>(0);

			//traverse and write animations
			scene_write_node_animations(
	//										p_scene,
											anim_layer, p_scene_result, context_data, i );

			scene_write_animation_blend_shapes_curves( p_scene, anim_layer, p_scene_result, context_data, i );
		}
	}
}

INT32 find_node_index(const fbx_scene_data& scene, FbxNode* node, const fbx_scene_info& scene_info)
{
	FbxNode** nodes = (FbxNode**)scene.object_mapping.native_node_table;

	for( UINT32 i = 0; i < scene_info.total_node_count; i++ )
	{
		if( nodes[i] == node )
			return i;
	}

	return -1;
}

void load_scene_materials(
	FbxScene *			p_scene_native, 
	fbx_scene_data *	p_scene)
{
	//result.material_count = p_scene->GetSrcObjectCount<FbxSurfaceMaterial>();
	for( UINT32 i = 0; i < p_scene->scene_info.material_count; ++i )
	{
		FbxSurfaceMaterial * material_native = p_scene_native->GetSrcObject<FbxSurfaceMaterial>();

		st_fbx_material & material = p_scene->materials[i];

		//write name
		INT32 name_table_index = i * FBX_NAME_MAX_LENGTH;
		const char* name = material_native->GetName();
		char* ptr_name_dest = p_scene->material_names_table;
		ptr_name_dest += name_table_index;
		strcpy_s( ptr_name_dest, FBX_NAME_MAX_LENGTH, name );

		update_material( material, p_scene_native, material_native );
	}
}

INT32 mesh_count_bones( FbxMesh* p_mesh )
{
	INT32 result = 0;
	INT32 skin_count = p_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for( INT32 s = 0; s < skin_count; s++ )
	{
		FbxSkin* skin = FbxCast<FbxSkin>(p_mesh->GetDeformer(s, FbxDeformer::eSkin));
		INT32 cluster_count = skin->GetClusterCount();
		result += cluster_count;
	}

	return result;
}

fbx_mesh_skinning_data_info mesh_compute_bones_info(
	FbxMesh *							p_mesh, 
	fbx_mesh_bones_import_mode const	p_import_mode)
{
	fbx_mesh_skinning_data_info result;
	result.bone_count					= 0;
	result.indices_count_ptr			= nullptr;
	result.indices_offset_ptr			= nullptr;
	result.weight_sum_ptr				= nullptr;
	result.max_bones_per_vertex			= 0;
	result.sum_vertex_influence			= 0;
	result.max_bones_per_vertex_clamped	= 0;

	INT32 skin_count = p_mesh->GetDeformerCount(FbxDeformer::eSkin);

	INT32 current_bone_index = 0;

	if( skin_count > 0 )
	{
		INT32 bone_count = mesh_count_bones(p_mesh);
		result = allocate_mesh_skinning_data_info(p_mesh->GetControlPointsCount());

		for( INT32 s = 0; s < skin_count; s++ )
		{
			FbxSkin* skin = FbxCast<FbxSkin>(p_mesh->GetDeformer(s, FbxDeformer::eSkin));
			INT32 cluster_count = skin->GetClusterCount();

			for( INT32 c = 0; c < cluster_count; c++ )
			{
				FbxCluster* cluster = skin->GetCluster(c);

				//get indices this cluster applies to
				INT32 indices_count = cluster->GetControlPointIndicesCount();

				if( indices_count > 0 )
				{
					FbxNode* node = cluster->GetLink();
					INT32* indices = cluster->GetControlPointIndices();
					double* weights = cluster->GetControlPointWeights();

					//increment bone count and weights
					for( INT32 i = 0; i < indices_count; i++ )
					{
						INT32 cpid = indices[i];

						//add 1 to indices count
						result.indices_count_ptr[cpid]++;
						result.weight_sum_ptr[cpid] += (float)weights[i];

						if( result.indices_count_ptr[cpid] > result.max_bones_per_vertex )
							result.max_bones_per_vertex = result.indices_count_ptr[cpid];

						//add 1 to sum
						result.sum_vertex_influence++;
					}
				}
			}
		}

		result.max_bones_per_vertex_clamped = FBX_BONE_PER_VERTEX_LIMIT_FOUR;
		switch( p_import_mode )
		{
		case fbx_mesh_bones_import_mode::all_bones_flat:
			result.max_bones_per_vertex_clamped = result.max_bones_per_vertex;
			break;
		case fbx_mesh_bones_import_mode::all_bones_compact:
			result.max_bones_per_vertex_clamped = result.max_bones_per_vertex;
			break;
		}

		result.bone_count = bone_count;

		INT32 control_point_count = p_mesh->GetControlPointsCount();

		//build offset table
		INT32 offset = 0;
		for( INT32 i = 0; i < control_point_count; i++ )
		{
			INT32 count = result.indices_count_ptr[i];
			result.indices_offset_ptr[i] = offset;
			offset += count;
		}
	}

	return result;
}


void mesh_load_bones( FbxMesh* p_mesh, const fbx_mesh_bones_import_mode p_import_mode, const fbx_scene_data& p_scene, const fbx_mesh_skinning_data_info& skinning_info, fbx_mesh_skinning_data& result )
{
	INT32 skin_count = p_mesh->GetDeformerCount(FbxDeformer::eSkin);

	INT32 current_bone_index = 0;

	if( skin_count > 0 )
	{
		INT32 bone_count = skinning_info.bone_count;

		//since the bones per control points is just a temporary table, to simplify we use a flat table in all cases, copy to optimized structure is done after
		INT32 max_bone_per_vertex = 4;
		bool is_bone_limit_four = true;

		switch( p_import_mode )
		{
		case fbx_mesh_bones_import_mode::all_bones_flat:
			max_bone_per_vertex = skinning_info.max_bones_per_vertex;
			is_bone_limit_four = false;
			break;
		case fbx_mesh_bones_import_mode::all_bones_compact:
			max_bone_per_vertex = skinning_info.max_bones_per_vertex;
			is_bone_limit_four = false;
			break;
		}

		result = allocate_mesh_skinning_data(p_mesh->GetControlPointsCount(), bone_count, max_bone_per_vertex);

		for( INT32 s = 0; s < skin_count; s++ )
		{
			FbxSkin* skin = FbxCast<FbxSkin>(p_mesh->GetDeformer(s, FbxDeformer::eSkin));

			INT32 cluster_count = skin->GetClusterCount();

			for( INT32 c = 0; c < cluster_count; c++ )
			{
				FbxCluster* cluster = skin->GetCluster(c);

				//get indices this cluster applies to
				INT32 indices_count = cluster->GetControlPointIndicesCount();
				if( indices_count > 0 )
				{
					FbxNode* node = cluster->GetLink();
					INT32* indices = cluster->GetControlPointIndices();
					double* weights = cluster->GetControlPointWeights();

					INT32 node_index_in_table = find_node_index(p_scene, node, p_scene.scene_info);

					//add bone index and weights to the tables
					for( INT32 i = 0; i < indices_count; i++ )
					{
						INT32 cpid = indices[i];

						INT32 current_count = result.indices_count_ptr[cpid];

						//now check if wee apply a bone limit of four
						if( is_bone_limit_four )
						{
							//only take the FBX_BONE_PER_VERTEX_LIMIT_FOUR first indices
							if( current_count < FBX_BONE_PER_VERTEX_LIMIT_FOUR )
							{
								UINT32 write_location = cpid * max_bone_per_vertex + current_count;

								result.indices_ptr[write_location] = current_bone_index;
								result.weights_ptr[write_location] = (float)weights[i];
								result.indices_count_ptr[cpid] = current_count + 1;
							}
							else
							{
								//point location in the bone table
								float* bone_weight_data_ptr = &result.weights_ptr[cpid * max_bone_per_vertex];

								//look at the smallest influence index, if this is bigger, replace it
								INT32 lowindex = 0;
								float minweight = bone_weight_data_ptr[0];

								//get lowest influence
								for( INT32 mi = 1; mi < FBX_BONE_PER_VERTEX_LIMIT_FOUR; mi++ )
								{
									float curr_weight = bone_weight_data_ptr[mi];

									if( curr_weight < minweight )
									{
										minweight = curr_weight;
										lowindex = mi;
									}
								}

								if( (float)weights[i] > minweight )
								{
									UINT32 write_location = cpid * max_bone_per_vertex + lowindex;
									result.indices_ptr[write_location] = current_bone_index;
									result.weights_ptr[write_location] = (float)weights[i];
								}
							}
						}
						else
						{
							//just plain write
							UINT32 write_location = cpid * max_bone_per_vertex + current_count;

							result.indices_ptr[write_location] = current_bone_index;
							result.weights_ptr[write_location] = (float)weights[i];
							result.indices_count_ptr[cpid] = current_count + 1;
						}
					}

					FbxAMatrix transform_matrix;
					FbxAMatrix bind_pose_matrix;
					FbxAMatrix geometric_transform_matrix;

					//get pose matrices
					cluster->GetTransformMatrix(transform_matrix);
					cluster->GetTransformLinkMatrix(bind_pose_matrix);
					geometric_transform_matrix = get_geometric_transformation(p_mesh->GetNode());

					transform_matrix *= geometric_transform_matrix;
					FbxAMatrix inverse_bind_pose_matrix = bind_pose_matrix.Inverse() * transform_matrix;

					//set the bone data (note that for now de dont support clusters that apply to the same nodes)
					if( result.bone_node_index_ptr[current_bone_index] == -1 )
					{
						result.bone_node_index_ptr[current_bone_index] = node_index_in_table;

						inverse_bind_pose* ibp_ptr = &result.inverse_bind_pose_ptr[current_bone_index];
						const double* ibp_src_data = inverse_bind_pose_matrix;
						for( INT32 mid = 0; mid < 16; mid++ )
							ibp_ptr->data[mid] = (float)ibp_src_data[mid];

						current_bone_index++;
					}
				}
			}
		}

		//now compute lost influence, in case we are not limiting to 4, this should still be zero, so we calculate it for validation purposes
		INT32 control_point_count = p_mesh->GetControlPointsCount();

		result.max_lost_influence = 0.0f;

		for( INT32 i = 0; i < control_point_count; i++ )
		{
			float* bone_weight_data_ptr = &result.weights_ptr[i * max_bone_per_vertex];

			float sum_limit = 0.0f;
			for( INT32 boneid = 0; boneid < max_bone_per_vertex; boneid++ )
				sum_limit += bone_weight_data_ptr[boneid];
			float sum = skinning_info.weight_sum_ptr[i];

			if( sum > 0.0f )
			{
				float norm = (1.0f - (sum_limit / sum)) * 100.0f;
				if( norm > result.max_lost_influence )
					result.max_lost_influence = norm;
			}
		}

		//normalize if required 
		if( p_import_mode == fbx_mesh_bones_import_mode::limit_four_highest_influence_normalized )
		{
			for( INT32 i = 0; i < control_point_count; i++ )
			{
				float* bone_weight_data_ptr = &result.weights_ptr[i * FBX_BONE_PER_VERTEX_LIMIT_FOUR];

				float length = sqrtf(
					  bone_weight_data_ptr[0] * bone_weight_data_ptr[0] 
					+ bone_weight_data_ptr[1] * bone_weight_data_ptr[1] 
					+ bone_weight_data_ptr[2] * bone_weight_data_ptr[2] 
					+ bone_weight_data_ptr[3] * bone_weight_data_ptr[3] );

				if( length > 0.0f )
				{
					float inv_length = 1.0f / length;
					bone_weight_data_ptr[0] *= inv_length;
					bone_weight_data_ptr[1] *= inv_length;
					bone_weight_data_ptr[2] *= inv_length;
					bone_weight_data_ptr[3] *= inv_length;
				}
			}
		}
	}
}


//blend shapes
fbx_mesh_blend_shape_info mesh_get_blend_shapes_info( FbxMesh* p_mesh )
{
	fbx_mesh_blend_shape_info result;
	MEMCLEAR( &result, sizeof(fbx_mesh_blend_shape_info) );
	result.total_shape_count = p_mesh->GetDeformerCount(FbxDeformer::eBlendShape);
	//we assume we have normals by default, 
	result.has_normals = true;

	for( INT32 shape_index = 0; shape_index < result.total_shape_count; shape_index++ )
	{
		FbxBlendShape* blend_shape = (FbxBlendShape*)p_mesh->GetDeformer(shape_index, FbxDeformer::eBlendShape);

		INT32 channel_count = blend_shape->GetBlendShapeChannelCount();
		result.total_channel_count += channel_count;

		for( INT32 channel_index = 0; channel_index < channel_count; channel_index++ )
		{
			FbxBlendShapeChannel* blend_shape_channel = blend_shape->GetBlendShapeChannel(channel_index);
			INT32 target_count = blend_shape_channel->GetTargetShapeCount();
			result.total_target_count += target_count;

			for( INT32 target_index = 0; target_index < target_count; target_index++ )
			{
				FbxShape* shape = blend_shape_channel->GetTargetShape(target_index);
				if( !shape->GetNormals(0) )
					result.has_normals = false;
			}
		}
	}

	return result;
}

void mesh_load_blend_shapes(FbxMesh* p_mesh_src, fbx_mesh_data* p_mesh_dst)
{
	fbx_mesh_blend_shape_data* result = &p_mesh_dst->blend_shapes_data;
	MEMCLEAR( result, sizeof(fbx_mesh_blend_shape_data) );

	result->shape_count = p_mesh_src->GetDeformerCount(FbxDeformer::eBlendShape);

	if( result->shape_count == 0 )
		return;

	UINT32 position_vertex_count_float = p_mesh_dst->vertex_count * 3;
	UINT32 position_vertex_buffer_size = position_vertex_count_float * sizeof(float);

	fbx_mesh_blend_shape_info shape_info = mesh_get_blend_shapes_info(p_mesh_src);
	//fill one big buffer with positions / put base pose in first as well
	result->buffer_element_count = (shape_info.total_target_count + 1) * p_mesh_dst->vertex_count;

	//allocate some memory
	result->position_data_ptr = MALLOC(result->buffer_element_count * 3 * sizeof(float));
	result->shapes_info_ptr = (fbx_blend_shape_data*)MALLOC(shape_info.total_shape_count * sizeof(fbx_blend_shape_data));
	result->shapes_channel_info_ptr = (fbx_blend_shape_channel_data*)MALLOC(shape_info.total_channel_count * sizeof(fbx_blend_shape_channel_data));
	result->shapes_target_ptr = (fbx_blend_shape_target_data*)MALLOC(shape_info.total_target_count * sizeof(fbx_blend_shape_target_data));

	//allocate normals only if enabled
	if( p_mesh_dst->has_normals )
	{
		result->normal_data_ptr = MALLOC(result->buffer_element_count * 3 * sizeof(float));
		result->has_normals = true;
	}

	INT32 current_channel_location = 0;
	INT32 current_target_location = 0;

	INT32 current_vertex_write_location = 0;
	float* position_dst = (float*)result->position_data_ptr;
	float* normal_dst	= (float*)result->normal_data_ptr;

	//copy the base pose
	MEMCPY( position_dst, p_mesh_dst->array_location_info.position_ptr, position_vertex_buffer_size, __FUNCTION__ );
	position_dst += position_vertex_count_float;

	if( result->has_normals )
	{
		//position_vertex_buffer_size is same as normal (both are vec3, so we can safely use those)
		MEMCPY( normal_dst, p_mesh_dst->array_location_info.normal_ptr, position_vertex_buffer_size, __FUNCTION__ );
		normal_dst += position_vertex_count_float;
	}
 
	current_vertex_write_location += p_mesh_dst->vertex_count;

	for( UINT32 shape_index = 0; shape_index < result->shape_count; ++shape_index )
	{
		FbxBlendShape* blend_shape_src = (FbxBlendShape*)p_mesh_src->GetDeformer(shape_index, FbxDeformer::eBlendShape);

		fbx_blend_shape_data* blend_shape_data_dst = &result->shapes_info_ptr[shape_index];

		blend_shape_data_dst->channel_count = blend_shape_src->GetBlendShapeChannelCount();

		//set shape channel at current location
		blend_shape_data_dst->channels_start_index = current_channel_location;

		for( UINT32 channel_index = 0; channel_index < blend_shape_data_dst->channel_count; ++channel_index )
		{
			FbxBlendShapeChannel* blend_shape_channel_src = blend_shape_src->GetBlendShapeChannel(channel_index);

			double* weights = blend_shape_channel_src->GetTargetShapeFullWeights();

			fbx_blend_shape_channel_data* blend_shape_channel_data_dst = &result->shapes_channel_info_ptr[current_channel_location];
			blend_shape_channel_data_dst->target_count = blend_shape_channel_src->GetTargetShapeCount();
			blend_shape_channel_data_dst->targets_start_index = current_target_location;

			for( UINT32 target_index = 0; target_index < blend_shape_channel_data_dst->target_count; target_index++ )
			{
				FbxShape* blend_shape_target_src = blend_shape_channel_src->GetTargetShape(target_index);
				fbx_blend_shape_target_data* blend_shape_target_dst = &result->shapes_target_ptr[current_target_location];

				blend_shape_target_dst->weight = (float)weights[target_index] * 0.01f;  //weights are in percents, we convert them in  normalized 0->1 values
				blend_shape_target_dst->buffer_start_location = current_vertex_write_location;

				//write vertices
				FbxVector4* source_data_position_ptr = blend_shape_target_src->GetControlPoints();
				INT32* polygon_vertex_indices = p_mesh_src->GetPolygonVertices();

				for( UINT32 i = 0; i < p_mesh_dst->vertex_count; i++ )
				{
					INT32 vertexId = polygon_vertex_indices[i];
					write_double4_to_vec3(position_dst + i * 3, source_data_position_ptr[vertexId]);
				}

				position_dst += position_vertex_count_float;

				if( result->has_normals )
				{
					// test if target shape has normals, if not i'm not sure what is the best option
					// either we copy the initial buffer or we give an error
					// in worst case should at least push a warning 
					FbxGeometryElementNormal* normalElement = blend_shape_target_src->GetElementNormal();
					if( normalElement )
					{
						write_double4_elements_to_vec3_array(p_mesh_src, normal_dst, normalElement, p_mesh_dst->vertex_count);
						normal_dst += position_vertex_count_float;
					}
					else
					{
						//either we memcpy the original buffer with warning or we break for now
						MEMCPY( normal_dst, p_mesh_dst->array_location_info.normal_ptr, position_vertex_buffer_size, __FUNCTION__ );
						normal_dst += position_vertex_count_float;

						//for debugging, later put a warning there
						debug_break( "FBX Error: Mesh has normals but blend shape target has none");
					}
				}

				current_vertex_write_location += p_mesh_dst->vertex_count;
				current_target_location++;
			}

			++current_channel_location;
		}
	}
}



bool fbx_check_element_mapping_mode(
	fbxsdk::FbxLayerElement::EMappingMode const p_mode )
{
	if( p_mode != FbxGeometryElement::eByPolygonVertex
		&& p_mode != FbxGeometryElement::eByControlPoint )
	{
		debug_break( "FBX Error: Unhandled geometry element mapping mode.");
		return false;
	}
	return true;
}

bool fbx_check_element_reference_mode(
	fbxsdk::FbxLayerElement::EReferenceMode const p_mode )
{
	if( p_mode != FbxGeometryElement::eDirect
		&& p_mode != FbxGeometryElement::eIndexToDirect )
	{
		debug_break( "FBX Error: Unhandled geometry element reference mode.");
		return false;
	}
	return true;
}

void write_double4_elements_to_vec3_array(
	FbxMesh *									p_mesh,
	float *										p_dest_ptr, 
	FbxLayerElementTemplate<FbxVector4> const *	p_src_data, 
	UINT32 const								p_vertex_count)
{
	fbxsdk::FbxLayerElement::EMappingMode const mapping_mode = p_src_data->GetMappingMode();
	if( !fbx_check_element_mapping_mode(mapping_mode) )
		return;

	fbxsdk::FbxLayerElement::EReferenceMode const ref_mode = p_src_data->GetReferenceMode();
	if( !fbx_check_element_reference_mode(ref_mode) )
		return;

	if( mapping_mode == FbxGeometryElement::eByPolygonVertex )
	{
		if( ref_mode == FbxGeometryElement::eDirect )
		{
			for( UINT32 i = 0; i < p_vertex_count; ++i)
			{
				auto vector = p_src_data->GetDirectArray().GetAt(i);
				write_double4_to_vec3(p_dest_ptr + i*3, vector);
			}
		}
		else if( ref_mode == FbxGeometryElement::eIndexToDirect )
		{
			for( UINT32 i = 0; i < p_vertex_count; ++i )
			{
				INT32 id = p_src_data->GetIndexArray().GetAt(i);
				auto vector = p_src_data->GetDirectArray().GetAt(id);
				write_double4_to_vec3(p_dest_ptr + i*3, vector);
			}
		}
	}
	else if( mapping_mode == FbxGeometryElement::eByControlPoint )
	{
		INT32 const polygon_num = p_mesh->GetPolygonCount();
		if( ref_mode == FbxGeometryElement::eDirect )
		{
			UINT32 vtx = 0;
			for( INT32 poly_idx = 0; poly_idx < polygon_num; ++poly_idx )
			{
				INT32 const poly_sz = p_mesh->GetPolygonSize(poly_idx);
				for( INT32 vtx_idx = 0; vtx_idx < poly_sz; ++vtx_idx )
				{
					INT32 const poly_vtx_idx = p_mesh->GetPolygonVertex(poly_idx, vtx_idx);
					fbxsdk::FbxVector4 const & vec = p_src_data->GetDirectArray().GetAt(poly_vtx_idx);
					write_double4_to_vec3(p_dest_ptr + vtx*3, vec);

				#if AAA_DEBUG()
					if( vtx >= p_vertex_count )
					{
						debug_break( "FBX Error: Vertex count is wrong.");
						return;
					}
				#endif

					++vtx;
				}
			}
		}
		else if( ref_mode == FbxGeometryElement::eIndexToDirect )
		{
			UINT32 vtx = 0;
			for( INT32 poly_idx = 0; poly_idx < polygon_num; ++poly_idx )
			{
				INT32 const poly_sz = p_mesh->GetPolygonSize(poly_idx);
				for( INT32 vtx_idx = 0; vtx_idx < poly_sz; ++vtx_idx )
				{
					INT32 const poly_vtx_idx = p_mesh->GetPolygonVertex(poly_idx, vtx_idx);
					INT32 const idx = p_src_data->GetIndexArray().GetAt(poly_vtx_idx);
					fbxsdk::FbxVector4 const & vec = p_src_data->GetDirectArray().GetAt(idx);
					write_double4_to_vec3(p_dest_ptr + vtx*3, vec);
#if AAA_DEBUG()
					if( vtx >= p_vertex_count )
					{
						debug_break( "FBX Error: Vertex count is wrong.");
						return;
					}
#endif
					++vtx;
				}
			}
		}
	}
}

void write_double2_elements_to_vec2_array(
	FbxMesh *									p_mesh,
	float *										p_dest_ptr, 
	FbxLayerElementTemplate<FbxVector2> const *	p_src_data, 
	UINT32 const								p_vertex_count, 
	bool const									p_flip_y)
{
	fbxsdk::FbxLayerElement::EMappingMode const mapping_mode = p_src_data->GetMappingMode();
	if( !fbx_check_element_mapping_mode(mapping_mode) )
		return;

	fbxsdk::FbxLayerElement::EReferenceMode const ref_mode = p_src_data->GetReferenceMode();
	if( !fbx_check_element_reference_mode(ref_mode) )
		return;

	if( mapping_mode == FbxGeometryElement::eByPolygonVertex )
	{
		if( ref_mode == FbxGeometryElement::eDirect )
		{
			for( UINT32 i = 0; i < p_vertex_count; ++i )
			{
				auto uv = p_src_data->GetDirectArray().GetAt(i);
				write_double2_to_vec2(p_dest_ptr + i*2, uv, p_flip_y );
			}
		}
		else if( ref_mode == FbxGeometryElement::eIndexToDirect )
		{
			for( UINT32 i = 0; i < p_vertex_count; ++i )
			{
				INT32 id = p_src_data->GetIndexArray().GetAt(i);
				auto uv = p_src_data->GetDirectArray().GetAt(id);
				write_double2_to_vec2(p_dest_ptr + i*2, uv, p_flip_y );
			}
		}
	}
	else if( mapping_mode == FbxGeometryElement::eByControlPoint )
	{
		INT32 const polygon_num = p_mesh->GetPolygonCount();
		if( ref_mode == FbxGeometryElement::eDirect )
		{
			UINT32 vtx = 0;
			for( INT32 poly_idx = 0; poly_idx < polygon_num; ++poly_idx )
			{
				INT32 const poly_sz = p_mesh->GetPolygonSize(poly_idx);
				for( INT32 vtx_idx = 0; vtx_idx < poly_sz; ++vtx_idx )
				{
					INT32 const poly_vtx_idx = p_mesh->GetPolygonVertex(poly_idx, vtx_idx);
					fbxsdk::FbxVector2 const & vec = p_src_data->GetDirectArray().GetAt(poly_vtx_idx);
					write_double2_to_vec2(p_dest_ptr + vtx*2, vec, p_flip_y);
#if AAA_DEBUG()
					if( vtx >= p_vertex_count )
					{
						debug_break( "FBX Error: Vertex count is wrong.");
						return;
					}
#endif
					++vtx;
				}
			}
		}
		else if( ref_mode == FbxGeometryElement::eIndexToDirect )
		{
			UINT32 vtx = 0;
			for( INT32 poly_idx = 0; poly_idx < polygon_num; ++poly_idx )
			{
				INT32 const poly_sz = p_mesh->GetPolygonSize(poly_idx);
				for( INT32 vtx_idx = 0; vtx_idx < poly_sz; ++vtx_idx )
				{
					INT32 const poly_vtx_idx = p_mesh->GetPolygonVertex(poly_idx, vtx_idx);
					INT32 const idx = p_src_data->GetIndexArray().GetAt(poly_vtx_idx);
					fbxsdk::FbxVector2 const & vec = p_src_data->GetDirectArray().GetAt(idx);
					write_double2_to_vec2(p_dest_ptr + vtx*2, vec, p_flip_y);
#if AAA_DEBUG()
					if( vtx >= p_vertex_count )
					{
						debug_break( "FBX Error: Vertex count is wrong.");
						return;
					}
#endif
					++vtx;
				}
			}
		}
	}
}


void write_bone_data(
	fbx_mesh_data&						mesh_result,
	const fbx_mesh_bones_import_mode	p_import_mode,
	const fbx_mesh_skinning_data_info&	bone_data_info,
	const fbx_mesh_skinning_data&		skinning_data,
	const INT32							control_point_count,
	INT32*								p_polygon_vertex_indices )
{
	//per vertex , one item
	UINT32* bone_start_location_data_ptr = (UINT32*)mesh_result.array_location_info.vertex_bones_data.bone_start_location_ptr;
	UINT32* bone_count_data_ptr = (UINT32*)mesh_result.array_location_info.vertex_bones_data.bone_count_ptr;

	//bone influences
	UINT32* bone_index_data_position_ptr = (UINT32*)mesh_result.array_location_info.vertex_bones_data.bone_index_ptr;
	float* bone_weight_data_position_ptr = (float*)mesh_result.array_location_info.vertex_bones_data.bone_weight_ptr;

	//in case of compat clamped = max
	INT32 max_bone_per_vertex = bone_data_info.max_bones_per_vertex_clamped;

	//compact mode works on a per control point basis, other works on a per vertex basis
	if( p_import_mode == fbx_mesh_bones_import_mode::all_bones_compact )
	{
		//copy data in table
		for( INT32 i = 0; i < control_point_count; i++ )
		{
			INT32 write_offset = bone_data_info.indices_offset_ptr[i];
			INT32 bone_count_for_vertex = bone_data_info.indices_count_ptr[i];

			//use a write offset here, using prefix table
			UINT32* bone_index_data_write_ptr = &bone_index_data_position_ptr[write_offset];
			float* bone_weight_data_write_ptr = &bone_weight_data_position_ptr[write_offset];

			//read is from a flat table
			UINT32* bone_index_data_read_ptr = &skinning_data.indices_ptr[i * max_bone_per_vertex];
			float* bone_weight_data_read_ptr = &skinning_data.weights_ptr[i * max_bone_per_vertex];

			for( INT32 j = 0; j < bone_count_for_vertex; j++ )
			{
				bone_index_data_write_ptr[j] = bone_index_data_read_ptr[j];
				bone_weight_data_write_ptr[j] = bone_weight_data_read_ptr[j];
			}
		}

		//copy read locations per vertex
		for( UINT32 i = 0; i < mesh_result.vertex_count; i++ )
		{
			INT32 vertexId = i;
			if( p_polygon_vertex_indices )
				vertexId = p_polygon_vertex_indices[i];

			bone_count_data_ptr[i] = bone_data_info.indices_count_ptr[vertexId];
			bone_start_location_data_ptr[i] = bone_data_info.indices_offset_ptr[vertexId];
		}
	}
	else //flat table
	{
		for( UINT32 i = 0; i < mesh_result.vertex_count; i++ )
		{
			INT32 vertexId = i;
			if( p_polygon_vertex_indices )
				vertexId = p_polygon_vertex_indices[i];

			//per vertex
			bone_count_data_ptr[i] = skinning_data.indices_count_ptr[vertexId];
			bone_start_location_data_ptr[i] = i * max_bone_per_vertex; //flat table, simple offset

			//influences
			UINT32* bone_index_data_write_ptr = &bone_index_data_position_ptr[i * max_bone_per_vertex];
			float* bone_weight_data_write_ptr = &bone_weight_data_position_ptr[i * max_bone_per_vertex];

			UINT32* bone_index_data_read_ptr = &skinning_data.indices_ptr[vertexId * max_bone_per_vertex];
			float* bone_weight_data_read_ptr = &skinning_data.weights_ptr[vertexId * max_bone_per_vertex];

			for( INT32 j = 0; j < max_bone_per_vertex; j++ )
			{
				bone_index_data_write_ptr[j] = bone_index_data_read_ptr[j];
				bone_weight_data_write_ptr[j] = bone_weight_data_read_ptr[j];
			}
		}
	}
}
