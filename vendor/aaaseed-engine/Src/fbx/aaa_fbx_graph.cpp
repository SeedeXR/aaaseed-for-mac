
// main header.
#include "fbx/aaa_fbx_graph.h"

#include "gol/gol_matrix.h"
#include "fbx/internal/fbxloader.h"
#include "fbx/internal/fbx_scene_process.h"
#include "fbx/aaa_fbx_scene.h"
#include "spy.h"


//-----------------------------------------------------------------------------

aaa::c_fbx_graph::c_fbx_graph( aaa::c_fbx_scene const * p_scene )
	: aaa::c_fbx_base(p_scene)
	, _graph_gl				{}
	, _graph_matrices_data	{ nullptr }
{}

aaa::c_fbx_graph::~c_fbx_graph()
{
	SAFE_DELETE_ARRAY( _graph_matrices_data );

	_graph_gl.release(true);
}

//-----------------------------------------------------------------------------

void aaa::c_fbx_graph::update_animation( 
	FP32 CONST		p_time, 
	UINT32 CONST		p_animation_stack_index, 
	bool CONST			p_linear,
	glm::mat4 CONST *	p_post_local_transforms,
	UINT32 CONST *		p_post_local_transforms_indices,
	UINT32 CONST		p_post_local_transforms_count )
{
	fbx_scene_data const * scene_data = get_scene()->get_fbx_scene_data();
	if( get_scene()->is_use_sdk_evaluator() )
	{
		fbx_scene_native CONST * scene_native = get_scene()->get_fbx_scene_native();
		fbx_native_get_nodes_global_transforms( scene_native, scene_data, p_animation_stack_index, p_time, (float*)_graph_matrices_data );
	}
	else
	{
		// pass 1, animation step (traverse timeline and build each node in local space)
		fbx_apply_scene_animations( scene_data, p_time, p_animation_stack_index, _graph_matrices_data, p_linear );

		// pass 2 : apply post local transforms
		for( UINT32 i = 0; i < p_post_local_transforms_count; i++ )
		{
			UINT32 index = p_post_local_transforms_indices[i];
			_graph_matrices_data[index] = _graph_matrices_data[index] * p_post_local_transforms[i];
		}

		// pass 2, traverse nodes and combine matrices to convert from local to world space
		fbx_combine_node_transforms( scene_data, _graph_matrices_data, 0 );

		//pass 3 apply geometric transformations

		if( get_scene()->is_use_geometric_transform() )
			fbx_apply_geometric_transform( scene_data, _graph_matrices_data, 0 );
	}
}

bool aaa::c_fbx_graph::load(fbx_scene_data const * p_scene_data )
{
	SPY_PUSH_RANGE_FUNCTION( spy::COL_3 );
		bool res = false;
		if( p_scene_data->scene_info.total_node_count > 0 )
		{
			_graph_matrices_data = new glm::mat4[p_scene_data->scene_info.total_node_count];

			// get first frame data.
			if( p_scene_data->scene_info.animation_stack_nb > 0 )
				update_animation( p_scene_data->scene_info.animations[0].start_time, 0, false, nullptr, nullptr, 0 );
			else
				update_animation( 0.0f, 0 , false, nullptr, nullptr, 0 );

			_graph_gl.init( _graph_matrices_data, p_scene_data->scene_info.total_node_count, true );

			res = true;
		}
		else
		{
			_graph_matrices_data = new glm::mat4[1];
			_graph_matrices_data[0] = aaa::matrix::identity;

			_graph_gl.init( _graph_matrices_data, 1, true );

			res = false;
		}
	SPY_POP_RANGE();
	return res;
}

void aaa::c_fbx_graph::update( 
	FP32 const		p_time , 
	UINT32 const		p_animation_index,
	bool CONST			p_linear, 
	glm::mat4 CONST *	p_post_local_transforms,
	UINT32 CONST *		p_post_local_transforms_indices,
	UINT32 CONST		p_post_local_transforms_count )
{
	update_animation( p_time , p_animation_index, p_linear, p_post_local_transforms, p_post_local_transforms_indices, p_post_local_transforms_count);
	_graph_gl.write( _graph_matrices_data, true );
}

void aaa::c_fbx_graph::bind() const
{
	_graph_gl.bind();
}

void aaa::c_fbx_graph::unbind() const
{
	_graph_gl.unbind();
}

//-----------------------------------------------------------------------------
