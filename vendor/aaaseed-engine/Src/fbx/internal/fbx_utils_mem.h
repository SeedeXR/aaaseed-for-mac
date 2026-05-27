
#ifdef AAA_FBX_UTILS_MEM_H
#error "FBX_UTILS_MEM_H included more than once."
#endif
#define AAA_FBX_UTILS_MEM_H 1


#if !defined(AAA_FBX_SDK_INCLUDE_H)
#include "fbx/internal/fbx_sdk_include.h"
#endif

#if !defined(AAA_FBX_STRUCTS_H)
#include "fbx/internal/fbx_structs.h"
#endif

#if !defined(AAA_FBX_STRUCTS_INTERNAL_H)
#include "fbx/internal/fbx_structs_internal.h"
#endif


//-----------------------------------------------------------------------------

void fbx_init_scene_load_options(fbx_scene_load_options * p_dst);
void fbx_init_scene_load_result(fbx_scene_load_result * p_dst);
void fbx_init_scene_data(fbx_scene_data* p_dst);
void fbx_init_mesh_load_options(fbx_mesh_load_options * p_dst);
void fbx_init_mesh_load_result(fbx_mesh_load_result * p_dst);
void fbx_init_mesh_data(fbx_mesh_data* p_dst);

//-----------------------------------------------------------------------------


fbx_scene_load_result init_load_result();

fbx_scene_data init_scene();

fbx_mesh_attributes_memory_requirements init_mesh_memory_requirements();

fbx_mesh_data init_mesh();

//allocates mesh skinning data
fbx_mesh_skinning_data allocate_mesh_skinning_data(UINT32 vertex_count, UINT32 bone_count, UINT32 max_bone_per_vertex);

fbx_mesh_skinning_data_info allocate_mesh_skinning_data_info(UINT32 vertex_count);

void free_mesh_skinning_data(fbx_mesh_skinning_data& p_data);

void free_mesh_skinning_data_info(fbx_mesh_skinning_data_info& p_data);
