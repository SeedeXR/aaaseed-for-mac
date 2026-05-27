
// main header.
#include "fbx/aaa_fbx_texture.h"


#if !defined(AAA_FBX_SCENE_H)
#	include "fbx/aaa_fbx_scene.h"
#endif


//-----------------------------------------------------------------------------

aaa::c_fbx_texture::c_fbx_texture(aaa::c_fbx_scene const * p_scene)
	: aaa::c_fbx_base(p_scene)
{}

//-----------------------------------------------------------------------------

void aaa::c_fbx_texture::load(INT32 const p_idx)
{
	_path = get_scene()->get_fbx_scene_data()->texture_files_name_table 
			+ (p_idx * FBX_FILE_NAME_MAX_LENGTH);
}

//-----------------------------------------------------------------------------
