
// main header.
#include "fbx/aaa_fbx_pose.h"

#include "gol/gol_matrix.h"
#include "fbx/internal/fbxloader.h"
#include "fbx/aaa_fbx_scene.h"
#include "err.h"


//-----------------------------------------------------------------------------

aaa::c_fbx_pose::c_fbx_pose( aaa::c_fbx_scene const * p_scene )
	: aaa::c_fbx_base(p_scene)
	, _pose_gl{}
	, _bind_pose_data{ nullptr }
	, _bone_node_index{ nullptr }
	, _b_has_bones{ false }
	, _name{ nullptr }
{}

aaa::c_fbx_pose::~c_fbx_pose()
{
	SAFE_DELETE_ARRAY( _bind_pose_data );
	SAFE_DELETE_ARRAY( _bone_node_index );

	_pose_gl.release(true);
}

//-----------------------------------------------------------------------------

bool aaa::c_fbx_pose::load( fbx_mesh_data const * data,  INT32 const node_idx )
{
	bool res = false;

	if( node_idx < 0 )
	{
		debug_break( "%s() negative node index", __FUNCTION__ );
	}
	else
	{
		_b_has_bones = data->has_bones;

		INT32 CONST bone_nb	= _b_has_bones ? data->bones_data.bones_nb : 1;
		_bind_pose_data = new glm::mat4[bone_nb];
		_bone_node_index = new UINT32[bone_nb];

		if( _b_has_bones )
		{
			MEMCPY(	_bind_pose_data,	data->bones_data.bones_bind_pose_ptr,			(size_t)bone_nb * sizeof(glm::mat4),	__FUNCTION__	);

			MEMCPY(	_bone_node_index,	data->bones_data.bones_node_index_table_ptr,	(size_t)bone_nb * sizeof(UINT32),		__FUNCTION__	);

			_name = fbx_get_node_name(	get_scene()->get_fbx_scene_data(),	node_idx	);
		}
		else
		{
			float * pt = &(_bind_pose_data[0][0][0]);
			fbx_native_get_node_global_transform(	get_scene()->get_fbx_scene_native(),	get_scene()->get_fbx_scene_data(),	node_idx,	pt	);

			_bone_node_index[0] = 0;
		}
		_pose_gl.init( _bind_pose_data, _bone_node_index, bone_nb, true );

		res = true;
	}

	return res;
}

//-----------------------------------------------------------------------------
