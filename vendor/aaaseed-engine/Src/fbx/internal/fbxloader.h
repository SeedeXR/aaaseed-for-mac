
#ifdef AAA_FBX_LOADER_H
#error "FBX_LOADER_H included more than once."
#endif
#define AAA_FBX_LOADER_H 1


#if !defined(AAA_FBX_STRUCTS_H)
#include "fbx/internal/fbx_structs.h"
#endif


//-----------------------------------------------------------------------------

fbx_scene_load_result fbx_load_scene_from_file(
	char const *					p_filename,
	fbx_scene_load_options const &	p_options );

void fbx_release_scene_native(fbx_scene_native& p_scene);

fbx_scene_info const & fbx_get_scene_info( fbx_scene_native const & p_scene);

fbx_scene_data fbx_get_scene_data( fbx_scene_native const & p_scene, fbx_scene_data_load_options const & p_load_options);

INT32 fbx_get_keyframe_count( fbx_scene_info const & p_scene_info, fbx_scene_data_load_options const & p_load_options);

char const * fbx_get_node_name(
	fbx_scene_data const *		p_scene_data,
	UINT32 const					p_node_index );

char const* fbx_get_material_name(
	fbx_scene_data const*		p_scene_data,
	UINT32 const				p_material_index );

fbx_mesh_load_result fbx_scene_load_mesh_by_index(
	fbx_scene_data const *			p_scene, 
	fbx_scene_native const *		p_scene_data, 
	fbx_mesh_load_options const *	p_options, 
	INT32							p_mesh_index );

void fbx_release_mesh( fbx_mesh_data & p_mesh );

void fbx_release_scene( fbx_scene_data & p_mesh );

void fbx_native_get_nodes_global_transforms(
	fbx_scene_native const *	p_scene_native, 
	fbx_scene_data const *		p_scene, 
	UINT32 const				p_animation_stack_index,
	float						p_time, 
	float *						p_data_pointer );

void fbx_native_get_node_global_transform(
	fbx_scene_native const *	p_scene_native,
	fbx_scene_data const *		p_scene,
	UINT32 const				p_node_index,
	float *						p_data_pointer );

INT32 fbx_scene_find_mesh_node_index(
	fbx_scene_data const*		p_scene,
	INT32						p_mesh_index);

st_fbx_material const * fbx_get_material(
	fbx_scene_data const *		p_scene, 
	UINT32 const				p_node_idx);

INT32 const fbx_get_material_index(
	fbx_scene_data const*		p_scene,
	UINT32 const				p_node_idx );
//-----------------------------------------------------------------------------
