
// main header.
#include "fbx/aaa_fbx_animation_curve.h"


#if !defined(AAA_AAA_MEM_H)
#	include "code_utils/aaa_mem.h"
#endif

#if !defined(AAA_GOL_H)
#	include "gol/gol.h"
#endif

#if !defined(AAA_GOL_MATRIX_H)
#	include "gol/gol_matrix.h"
#endif

#if !defined(AAA_V_BASE_H)
#	include "math/v_base.h"
#endif

#if !defined(AAA_FBX_LOADER_H)
#	include "fbx/internal/fbxloader.h"
#endif

#if !defined(AAA_FBX_SCENE_PROCESS_H)
#	include "fbx/internal/fbx_scene_process.h"
#endif

#if !defined(AAA_FBX_SCENE_H)
#	include "fbx/aaa_fbx_scene.h"
#endif


//-----------------------------------------------------------------------------

aaa::c_fbx_animation_curve::c_fbx_animation_curve( aaa::c_fbx_scene const * p_scene )
	: aaa::c_fbx_base( p_scene )
	,_primitive			{}
	,_curve_vertices_nb	{0}
//	,_mesh_index		{-1}
{}

aaa::c_fbx_animation_curve::~c_fbx_animation_curve()
{}

//-----------------------------------------------------------------------------

void aaa::c_fbx_animation_curve::clear()
{
	_primitive.dealloc_vertex();
	_curve_vertices_nb = 0;
}

bool aaa::c_fbx_animation_curve::load(
	INT32 const		p_node_index, 
	INT32 const		p_animation_index
//	, INT32 const &	p_mesh_index
)
{
	clear();

	bool res = false;

	INT32 result_count;
	glm::mat4 * keyframes_world = fbx_get_node_keyframe_transform(
		get_scene()->get_fbx_scene_data(), 
		p_node_index, 
		p_animation_index,
		get_scene()->get_animation_time_step(),
		&result_count);

	if( keyframes_world )
	{
		FP32 * vec = _primitive.alloc_vertex(result_count);
		for( INT32 i = 0; i < result_count; ++i )
		{
			cpy_v3( vec, &(keyframes_world[i][3][0]) );
			vec += 3;
		}
							
		_curve_vertices_nb = result_count;
		//todo see which version we keep
		FREE( keyframes_world );

		// store mesh index.
//		_mesh_index = p_mesh_index;

		res = true;
	}

	return res;
}

void aaa::c_fbx_animation_curve::draw() const
{
	if( _curve_vertices_nb > 0 )
		_primitive.draw( GL_LINE_STRIP, _curve_vertices_nb );
}

//-----------------------------------------------------------------------------
