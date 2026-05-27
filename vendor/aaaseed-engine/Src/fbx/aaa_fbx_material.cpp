
// main header.
#include "fbx/aaa_fbx_material.h"
#include "code_utils/err.h"
#include "fbx/internal/fbxloader.h"
#include "fbx/aaa_fbx_scene.h"


//-----------------------------------------------------------------------------

aaa::c_fbx_material::c_fbx_material( aaa::c_fbx_scene const * p_scene )
	: aaa::c_fbx_base(p_scene)
{}

//-----------------------------------------------------------------------------

void aaa::c_fbx_material::load( INT32 CONST p_node_idx )
{
	_mat = fbx_get_material( get_scene()->get_fbx_scene_data(), p_node_idx );

	if( _mat )
	{
		auto CONST material_index = fbx_get_material_index( get_scene()->get_fbx_scene_data(), p_node_idx );
		if( material_index != -1 )
		{
			_name = fbx_get_material_name( get_scene()->get_fbx_scene_data(), material_index );
			if( get_scene()->is_verbose() )
				DBG_PRINT_STRING( "Material [%s] loaded.", _name );
		}
	}
	else
		ERR_PRINT_STRING( "Invalid material for node %i", p_node_idx );
}

//-----------------------------------------------------------------------------
