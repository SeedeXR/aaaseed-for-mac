
#ifdef AAA_FBX_SCENE_PROCESS_H
#error "FBX_SCENE_PROCESS_H included more than once."
#endif
#define AAA_FBX_SCENE_PROCESS_H 1


#if !defined(AAA_FBX_STRUCTS_H)
#include "fbx/internal/fbx_structs.h"
#endif

#if !defined(AAA_AAA_MATRIX_H)
#	include "math/aaa_matrix.h"
#endif


float fbx_get_animation_value( c_fbx_animation_curve const & p_curve, fbx_animation_keyframe const * p_keyframes, float const p_time);

//glm::mat4* fbx_allocate_node_bone_transforms(const fbx_scene_info& p_scene_info, UINT32 p_instance_count);

//void fbx_release_node_bone_transforms(glm::mat4* p_transforms);

//void fbx_get_node_base_pose_local_space(const fbx_scene_data& p_scene, UINT32 p_node_index, glm::vec3& p_position, glm::vec3& p_scale, glm::vec3& p_rotation);

void fbx_apply_scene_node_transforms( fbx_scene_data const & p_scene, glm::mat4* p_transforms );

fbx_node_animation* const scene_get_node_animations(
	fbx_scene_data const *	p_scene, 
	UINT32 const			animation_stack_index );

void fbx_apply_scene_animations(
	fbx_scene_data const *	p_scene, 
	float const				p_time, 
	UINT32 const			p_animation_index,
	glm::mat4 *				p_transforms,
	bool const				p_linear );

void fbx_combine_node_transforms(
	fbx_scene_data const *	p_scene, 
	glm::mat4 *				p_transforms, 
	UINT32 const			p_offset );

void fbx_apply_geometric_transform(
	fbx_scene_data const *	p_scene,
	glm::mat4*				p_transforms,
	UINT32 const			p_offset );

glm::mat4 * fbx_get_node_keyframe_transform(
	fbx_scene_data const *	p_scene, 
	UINT32 const			p_node_index, 
	UINT32 const			p_animation_index,
	float const				p_time_step, 
	INT32 *					p_result_element_count );

//void fbx_release_node_keyframe_transforms( glm::mat4* p_transforms );

bool fbx_is_node_root_armature( fbx_scene_data const & p_scene, UINT32 index );

INT32 fbx_node_find_parent_mesh( fbx_scene_data const & p_scene, UINT32 index );

float get_curve_animation_value(
	fbx_scene_data const*			p_scene,
	c_fbx_animation_curve const *	curve,
	float const						time,
	bool const						p_linear );

bool fbx_get_blend_shape_interpolation( const fbx_mesh_data& p_mesh,
	const UINT32						p_shape_index,
	const UINT32						p_channel_index,
	const float							p_amount,
	fbx_blend_shape_interpolation_info*	p_result );

bool fbx_get_scene_play_time(fbx_scene_data const* p_scene, UINT32 const p_animation_stack_idx, float const p_time, animation_sampler_mode const p_sampler, float& p_result);

INT32 fbx_find_blend_shape_curve(
	fbx_scene_data const *	p_scene,
	INT32 const				p_animation_index,
	INT32 const				p_mesh_index,
	INT32 const				p_blend_shape_index,
	INT32 const				p_blend_shape_channel_index );

