
// Main header.
#include "fbx/internal/fbx_scene_process.h"

#include "code_utils/aaa_mem.h"
#include "code_utils/spy.h"
#include "fbx/internal/fbx_utils.h"
#ifndef AAA_AAA_UTIL_H	// we need it depending on AAA_USE_VECTOR()
#	include "aaa_util.h"
#endif

//glm::mat4* fbx_allocate_node_bone_transforms(const fbx_scene_info& p_scene_info, UINT32 p_instance_count)
//{
//	glm::mat4* result = (glm::mat4*)MALLOC( p_scene_info.total_node_count * sizeof(glm::mat4) );
//	return result;
//}
//
//
//void fbx_release_node_bone_transforms(glm::mat4* p_transforms)
//{
//	if( p_transforms )
//		FREE(p_transforms);
//}


float fbx_get_animation_value( c_fbx_animation_curve const & p_curve, fbx_animation_keyframe const * p_keyframes, float const p_time )
{
	INT32 count = p_curve.keyframe_count;
	if( count == 0 )
		return 0.0f;

	fbx_animation_keyframe const * first_keyframe_ptr = p_keyframes + p_curve.index_in_keyframe_table;

	//check out of bounds
	if( p_time <= first_keyframe_ptr[0].time )
		return first_keyframe_ptr[0].value;

	{
		INT32 const last = count - 1;
		if( p_time >= first_keyframe_ptr[last].time )
			return first_keyframe_ptr[last].value;
	}

	//todo binary search
	{
		fbx_animation_keyframe const * left = first_keyframe_ptr;
		fbx_animation_keyframe const * right = first_keyframe_ptr + 1;
		for( INT32 i = 0; i < count - 1; i++)
		{
			if( left->time <= p_time && p_time < right->time )
			{
				if( left->interpolation == fbx_keyframe_interpolation::no_interpolation )
					return left->value;

				if( left->interpolation == fbx_keyframe_interpolation::linear )
				{
					float interpolation = (p_time - left->time) / (right->time - left->time);
					return (1.0f - interpolation) * left->value + interpolation * right->value;
				}

				if( left->interpolation == fbx_keyframe_interpolation::cubic )
				{
					//todo linear for now, since not having an example 
					float interpolation = (p_time - left->time) / (right->time - left->time);
					return (1.0f - interpolation) * left->value + interpolation * right->value;
				}
			}
			left = right;
			++right;
		}
	}

	//that should never be called
	return 0.0f;
}

//void fbx_get_node_base_pose_local_space( const fbx_scene_data& p_scene, UINT32 p_node_index, glm::vec3& p_position, glm::vec3& p_scale, glm::vec3& p_rotation )
//{
//	fbx_node node = p_scene.nodes[p_node_index];
//
//	p_position = glm::vec3(node.position[0], node.position[1], node.position[2]);
//	p_scale = glm::vec3(node.rotation[0], node.rotation[1], node.rotation[2]);
//	p_rotation = glm::vec3(node.scaling[0], node.scaling[1], node.scaling[2]);
//}

//todo not called
void fbx_apply_scene_node_transforms( fbx_scene_data const & p_scene, glm::mat4* p_transforms )
{
	fbx_node const * pnode = p_scene.nodes;
	for( INT32 nb = p_scene.scene_info.total_node_count; nb>0; --nb )
	{
		//aaa::matrix::make_matrix_tra_rotyzx_deg_sca( p_transforms, pnode->position, pnode->rotation, pnode->scaling );
		aaa::matrix::make_matrix_tra_rotyxz_deg_sca( p_transforms, pnode->position, pnode->rotation, pnode->scaling );
		++pnode;
		++p_transforms;
	}
}

FINLINE fbx_animation_keyframe const * find_keyframe_slow( FP32 const time,  fbx_animation_keyframe const * kf, INT32 nb )
{
	if( time <= kf->time )
		return kf;
	for( ; nb > 1; --nb )
	{
		if( kf->time <= time && time < (kf+1)->time )
			return kf;
		++kf;
	}
	return kf;
}

FINLINE fbx_animation_keyframe const * find_keyframe( FP32 const time,  fbx_animation_keyframe const * const kf, INT32 const nb )
{
	if( time <= kf->time || nb==1 )
		return kf;
	INT32 r = nb-1;	//right
	if( (kf+r)->time <= time )
		return kf+r;
	INT32 l = 0;	//left
	while( (r-l) > 1)
	{
		INT32 m = (r+l)/2;
		if( time < (kf+m)->time )
			r = m;
		else
			l = m;
	}
	return kf+l;
}

FINLINE INT32 find_keyframe_index( FP32 const time, fbx_animation_keyframe const* const kf, INT32 const nb )
{
	if( time <= kf->time || nb == 1 )
		return 0;
	INT32 r = nb - 1;	//right
	if( (kf + r)->time <= time )
		return r;
	INT32 l = 0;		//left
	while( (r-l) > 1 )
	{
		INT32 m = (r + l) / 2;
		if( time < (kf + m)->time )
			r = m;
		else
			l = m;
	}
	return l;
}

namespace
{
	o_str	o_spy_nb;
};

fbx_node_animation* const scene_get_node_animations( fbx_scene_data const * p_scene, UINT32 const animation_stack_index )
{
	if( animation_stack_index >= p_scene->scene_info.animation_stack_nb )
		return nullptr;
	else
	{
		INT32 index = animation_stack_index * p_scene->scene_info.total_node_count;
		return &p_scene->nodes_animations[index];
	}
}

float get_curve_animation_value(
	fbx_scene_data const*			p_scene,
	c_fbx_animation_curve const *	curve,
	float const						time,
	bool const						p_linear
	)
{
	//quick nearest for testing
	UINT32 keyframe_start_index = curve->index_in_keyframe_table;

	//todomona we should
	//  1/return left and right then so
	//  2/interpolate
	UINT32 const left_index = find_keyframe_index(time, p_scene->animation_keyframes + keyframe_start_index, curve->keyframe_count);

	fbx_animation_keyframe* left = &p_scene->animation_keyframes[keyframe_start_index + left_index];

	float animation_value = left->value;

	//if liner and left is not last keyframe, interpolate
	if( p_linear && left_index+1 < curve->keyframe_count )
	{
		fbx_animation_keyframe* right = &p_scene->animation_keyframes[keyframe_start_index + left_index + 1];

		float inter_time = FWRAP(time, left->time, right->time);

		animation_value = (1.0f - inter_time) * animation_value + inter_time * right->value;
	}

	return animation_value;
}

void fbx_apply_scene_animations(
	fbx_scene_data const *	p_scene, 
	float const				time,
	UINT32  const			p_animation_index,
	glm::mat4 *				p_transforms,
	bool const				p_linear
	)
{
	SPY_PUSH_RANGE_FUNCTION( spy::COL_2 );

		float position[3];
		float rotation[3];
		float scale[3];
		UINT32 loop_total_nb = 0;
		fbx_node const * node = p_scene->nodes;
		fbx_node_animation const* animations = scene_get_node_animations(p_scene, p_animation_index);

		for( UINT32 nb = p_scene->scene_info.total_node_count; nb>0; --nb )
		{
			cpy_v3fp32( position,	node->position );
			cpy_v3fp32( rotation,	node->rotation );
			cpy_v3fp32( scale,		node->scaling );

			//apply animations
			UINT32 anim_nb = animations ? animations->animation_count : 0;
			loop_total_nb += anim_nb;

			for( UINT32 anim_index = 0; anim_index < anim_nb; anim_index++ )
			{
				UINT32 curve_index = animations->animation_table_location + anim_index;
				c_fbx_animation_curve const * curve = p_scene->animation_curves + curve_index;
				if( curve->keyframe_count > 0 )
				{
					float animation_value = get_curve_animation_value(p_scene, curve, time, p_linear);
					//apply value to animation
					switch( curve->curve_attribute )
					{
						case fbx_curve_attribute_kind::position_x:	position[0] = animation_value; break;
						case fbx_curve_attribute_kind::position_y:	position[1] = animation_value; break;
						case fbx_curve_attribute_kind::position_z:	position[2] = animation_value; break;

						case fbx_curve_attribute_kind::rotation_x:	rotation[0] = animation_value; break;
						case fbx_curve_attribute_kind::rotation_y:	rotation[1] = animation_value; break;
						case fbx_curve_attribute_kind::rotation_z:	rotation[2] = animation_value; break;

						case fbx_curve_attribute_kind::scale_x:		scale[0] = animation_value; break;
						case fbx_curve_attribute_kind::scale_y:		scale[1] = animation_value; break;
						case fbx_curve_attribute_kind::scale_z:		scale[2] = animation_value; break;
					}
				}
			}

	//		aaa::matrix::make_matrix_tra_rotyzx_deg_sca( p_transforms, position, rotation, scale );
			aaa::matrix::make_matrix_tra_rotyxz_deg_sca( p_transforms, position, rotation, scale );
			++node;
			++p_transforms;
			if( animations )
				++animations;
		}

		if( SPY_IS_ACTIVE() )
		{
			o_spy_nb.set( loop_total_nb );
			SPY_MARK_DIRECT( o_spy_nb );
		}
	SPY_POP_RANGE();
}

void fbx_combine_node_transforms(
	fbx_scene_data const *	p_scene, 
	glm::mat4 *				p_transforms,
	UINT32 const			p_offset	)	//todo not sure p_offset is correct because of p_transforms[node->parent_index + p_offset] 
{
	SPY_PUSH_RANGE_FUNCTION( spy::COL_2 );

		fbx_node const * nodes = p_scene->nodes;

		UINT32 const node_count = p_scene->scene_info.total_node_count;
		for( UINT32 node_index = 0; node_index < node_count; node_index++ )
		{
			UINT32 const index = node_index + p_offset;
			fbx_node const * node  = nodes + index;
			if( node->parent_index != -1 )
				p_transforms[index] = p_transforms[node->parent_index + p_offset] * p_transforms[index];
		}

	SPY_POP_RANGE();
}

void fbx_apply_geometric_transform(
	fbx_scene_data const*	p_scene,
	glm::mat4*				p_transforms,
	UINT32 const			p_offset )  //todo not sure p_offset is correct because of p_transforms[node->parent_index + p_offset] 
{
	SPY_PUSH_RANGE_FUNCTION(spy::COL_2);

	fbx_node const* nodes = p_scene->nodes;

	UINT32 const node_count = p_scene->scene_info.total_node_count;
	for( UINT32 node_index = 0; node_index < node_count; node_index++ )
	{
		UINT32 const index = node_index + p_offset;
		fbx_node const* node = nodes + index;
		glm::mat4 geometric_transform;
		aaa::matrix::make_matrix_tra_rotyxz_deg_sca( &geometric_transform, node->geometric_translation, node->geometric_rotation, node->geometric_scaling );

		p_transforms[index] = p_transforms[index] * geometric_transform;
	}

	SPY_POP_RANGE();
}

//void fbx_release_node_keyframe_transforms(glm::mat4* p_transforms)
//{
//	//todo avoid this dealloc (see alloc below)
//	if( p_transforms )
//		FREE(p_transforms);
//}

glm::mat4* fbx_get_node_keyframe_transform(
	fbx_scene_data const *	p_scene, 
	UINT32 const			p_node_index, 
	UINT32 const			p_animation_index,
	float const				p_time_step, 
	INT32 *					p_result_element_count )
{
	*p_result_element_count = 0;
	if( p_node_index >= p_scene->scene_info.total_node_count )
		return nullptr;

	if( p_animation_index >= p_scene->scene_info.animation_stack_nb )
		return nullptr;

	float anim_duration = p_scene->scene_info.animations[p_animation_index].end_time - p_scene->scene_info.animations[p_animation_index].start_time;
	//todo -1, +1 barriere et poteau ...
	INT32 step_count = (INT32)(anim_duration / p_time_step);

	if( step_count > 0 )
	{
		//todo reuse and avoid so many alloc
		//allocate temp table for nodes
		// glm::mat4* frame_data = fbx_allocate_node_bone_transforms(p_scene->scene_info, 1);
		glm::mat4* frame_data = (glm::mat4*)MALLOC( p_scene->scene_info.total_node_count * sizeof(glm::mat4) );

		float time = p_scene->scene_info.animations[p_animation_index].start_time;

		//todo avoid alloc
		glm::mat4* result = (glm::mat4*)MALLOC( step_count * sizeof(glm::mat4) );

		for( INT32 i = 0; i < step_count; i++ ) 
		{
			//construct matrices for the scene in world space
			fbx_apply_scene_animations( p_scene, time, p_animation_index, frame_data, false);
			fbx_combine_node_transforms( p_scene, frame_data, 0 );
			fbx_apply_geometric_transform( p_scene, frame_data, 0);

			time += p_time_step;

			result[i] = frame_data[p_node_index];
		}

		//fbx_release_node_keyframe_transforms(frame_data);
		FREE(frame_data);

		*p_result_element_count = step_count;
		return result;
	}
	else //no animation
		return nullptr;
}

bool fbx_is_node_root_armature( const fbx_scene_data& p_scene, UINT32 index )
{
	if( p_scene.scene_info.total_node_count <= index )
		return false;

	fbx_node const * node = p_scene.nodes + index;
	if( node->is_armature )
	{
		//tbh this should normallly never happens (since theres always a "root main"), but some exporters could, so this would be true then
		if( node->parent_index == -1 )
			return true;
		else
		{
			fbx_node const * parent = p_scene.nodes + node->parent_index ;
			return parent->is_armature == false;
		}
	}
	else
		return false;
}

INT32 fbx_node_find_parent_mesh( const fbx_scene_data& p_scene, UINT32 index )
{
	if( p_scene.scene_info.total_node_count <= index )
		return -1;

	fbx_node const * node = p_scene.nodes + index;

	//check if directly attached, that would only happen if mesh attached to root node
	if( node->mesh_index != -1 )
		return node->mesh_index;

	//traverse parents and find the first with a mesh attached to it
	while( node->parent_index != -1 )
	{
		if( node->mesh_index != -1 )
			return node->mesh_index;
		node = p_scene.nodes + node->parent_index;
	}

	return -1;
}

bool fbx_get_blend_shape_interpolation( const fbx_mesh_data& p_mesh,
	const UINT32						p_shape_index,
	const UINT32						p_channel_index,
	const float							p_amount,
	fbx_blend_shape_interpolation_info*	p_result )
{
	if( p_shape_index >= p_mesh.blend_shapes_data.shape_count )
		return false;

	fbx_blend_shape_data blend_shape = p_mesh.blend_shapes_data.shapes_info_ptr[p_shape_index];

	if( p_channel_index >= blend_shape.channel_count )
		return false;

	INT32 channel_location = blend_shape.channels_start_index + p_channel_index;
	fbx_blend_shape_channel_data blend_channel_data = p_mesh.blend_shapes_data.shapes_channel_info_ptr[channel_location];

	INT32 target_location = blend_channel_data.targets_start_index;
	INT32 last_target_location = target_location + blend_channel_data.target_count - 1;

	fbx_blend_shape_target_data first_target_data = p_mesh.blend_shapes_data.shapes_target_ptr[target_location];
	fbx_blend_shape_target_data last_target_data = p_mesh.blend_shapes_data.shapes_target_ptr[last_target_location];

	//special case if we are before first target
	if( p_amount <= first_target_data.weight )
	{
		//first pose
		p_result->source_vertex_location = 0;
		p_result->target_vertex_location = first_target_data.buffer_start_location;
		p_result->interpolation_amount = p_amount / first_target_data.weight;
		return true;
	}
	else if( p_amount >= last_target_data.weight ) //after last target (normally last is always 1)
	{
		fbx_blend_shape_target_data start_target_data = p_mesh.blend_shapes_data.shapes_target_ptr[last_target_location - 1];
		p_result->source_vertex_location = start_target_data.buffer_start_location;
		p_result->target_vertex_location = last_target_data.buffer_start_location;
		p_result->interpolation_amount = p_amount;
		return true;
	}
	else
	{
		//seek for appropriate target
		for( UINT32 target_index = 0; target_index+1 < blend_channel_data.target_count; target_index++ )
		{
			fbx_blend_shape_target_data start_target_data = p_mesh.blend_shapes_data.shapes_target_ptr[target_location + target_index];
			fbx_blend_shape_target_data end_target_data = p_mesh.blend_shapes_data.shapes_target_ptr[target_location + target_index + 1];

			if( p_amount > start_target_data.weight && p_amount < end_target_data.weight )
			{
				p_result->source_vertex_location = start_target_data.buffer_start_location;
				p_result->target_vertex_location = end_target_data.buffer_start_location;
				p_result->interpolation_amount = ((p_amount - start_target_data.weight) / (end_target_data.weight - start_target_data.weight));
				return true;
			}
		}
		//failed
	}
	return false;
}

bool fbx_get_scene_play_time( fbx_scene_data const* scene, UINT32 const animation_stack_idx, float const time, animation_sampler_mode const sampler, float& result )
{
	fbx_scene_info const * scene_info = &scene->scene_info;

	if( animation_stack_idx >= scene_info->animation_stack_nb )
		return false;

	fbx_animation_info const * animation_info = &scene_info->animations[animation_stack_idx];

#if AAASEED()
	switch( sampler )
	{
	case animation_sampler_mode::clamp:
		result = CLAMP( time, animation_info->start_time, animation_info->end_time );
		return true; 
	case animation_sampler_mode::wrap:
		result = FWRAP( time, animation_info->start_time, animation_info->end_time );
		return true;
	}
#else
	switch( sampler )
	{
	case animation_sampler_mode::clamp:
		result = time;
		if( result < animation_info->start_time )
			result = animation_info->start_time;
		else if( result > animation_info->end_time )
			result = animation_info->end_time;
		return true; 
	case animation_sampler_mode::wrap:
		float range = animation_info->end_time - animation_info->start_time;
		if( range == 0.0f )
			result = animation_info->start_time;
		else
		{
			float normalized = (time - animation_info->start_time) / range;
			INT32 count = (INT32)floor(normalized);
			normalized -= count;
			result = animation_info->start_time + normalized * range;
		}
		return true;
	}
#endif

	//invalid play mode
	return false;
}

INT32 fbx_find_blend_shape_curve(
	fbx_scene_data const *	p_scene,
	INT32 const				p_animation_index,
	INT32 const				p_mesh_index,
	INT32 const				p_blend_shape_index,
	INT32 const				p_blend_shape_channel_index
	)
{
	for( UINT32 i = 0; i < p_scene->scene_info.mesh_blend_shape_animations_count; ++i )
	{
		fbx_mesh_blend_shape_animation* p_info = &p_scene->mesh_blend_shape_animations[i];
		if( p_info->mesh_index == p_mesh_index
			&& p_info->animation_index == p_animation_index
			&& p_info->blend_shape_index == p_blend_shape_index
			&& p_info->blend_shape_channel_index == p_blend_shape_channel_index
		)
			return p_info->animation_curve_index;
	}

	return -1;
}