
#ifdef AAA_FBX_SCENE_VALIDATE_H
#error "FBX_SCENE_VALIDATE_H included more than once."
#endif
#define AAA_FBX_SCENE_VALIDATE_H 1


#if !defined(AAA_FBX_STRUCTS_H)
#include "fbx/internal/fbx_structs.h"
#endif

#if !defined(AAA_AAA_MATRIX_H)
#	include "math/aaa_matrix.h"
#endif

fbx_scene_validate_result fbx_validate_scene(const fbx_scene_data& p_scene);

fbx_mesh_validate_result fbx_validate_mesh(const fbx_mesh_data& p_mesh);
