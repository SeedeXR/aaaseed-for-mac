
// Main header.
#include "fbx/internal/fbx_utils_gl.h"
#include "fbx/internal/fbx_utils_mem.h"
#include "fbx/aaa_fbx_utils.h"

//-----------------------------------------------------------------------------

void c_fbx_graph_gl::init( glm::mat4 * skinning_data, UINT32 const matrix_nb, bool const immediate )
{
	//deal again with immediate
	_hierarchy_matrices = gl::ubo::make( sizeof(glm::mat4) * matrix_nb, true, (char*)skinning_data, "hierarchy_matrices" );
}

void c_fbx_graph_gl::release( bool const immediate )
{
	//todo deal again with immediate flag
	gl::ubo::release_and_null( _hierarchy_matrices );
}

void c_fbx_graph_gl::bind() const
{
	_hierarchy_matrices->bind( aaa::fbx_binding_hierarchy );
}

void c_fbx_graph_gl::unbind() const
{
	_hierarchy_matrices->unbind( aaa::fbx_binding_hierarchy );
}

void c_fbx_graph_gl::write( glm::mat4 const * skinning_data, bool const immediate )
{
	_hierarchy_matrices->write_all( skinning_data, immediate );
}

//-----------------------------------------------------------------------------

void c_fbx_pose_gl::init( glm::mat4 * bind_pose, UINT32 * node_table, UINT32 const nb, bool const b_immediate )
{
	//deal again with immediate
	_bind_pose		= gl::ubo::make( sizeof(glm::mat4)	* nb, false, (char*)bind_pose,  "pose" );
	_node_indices	= gl::ubo::make( sizeof(UINT32)		* nb, false, (char*)node_table, "node_indices" );
}

void c_fbx_pose_gl::release( bool const immediate )
{
	//todo deal again with immediate flag
	gl::ubo::release_and_null( _bind_pose );
	gl::ubo::release_and_null( _node_indices );
}

void c_fbx_pose_gl::bind() const
{
	_bind_pose->bind( aaa::fbx_binding_pose );
	_node_indices->bind( aaa::fbx_binding_node_idx );
}

void c_fbx_pose_gl::unbind() const
{
	_node_indices->unbind( aaa::fbx_binding_node_idx );
	_bind_pose->unbind( aaa::fbx_binding_pose );
}

//-----------------------------------------------------------------------------

st_fbx_mesh_gl fbx_gl_create_mesh( const fbx_mesh_data& mesh, bool use_ssbo, bool force_root_bone, bool immediate )
{

	gl::c_vao_props vao_props;

	st_fbx_mesh_gl result;
	result.c_vao = nullptr;
	result.ibo = nullptr;
	result.bones_ssbos.vertex_bone_count = nullptr;
	result.bones_ssbos.vertex_bone_indices = nullptr;
	result.bones_ssbos.vertex_bone_location = nullptr;
	result.bones_ssbos.vertex_bone_weights = nullptr;
	result.use_ssbo_for_bones = false;
	result.blend_shapes_ssbos.normal = nullptr;
	result.blend_shapes_ssbos.position = nullptr;

	UINT32 vertex_nb = mesh.vertex_count;
	//position
	vao_props.add_slot( GOL::VAO_TYPE::VEC3, mesh.array_location_info.position_ptr, vertex_nb, false );
	//normals
	vao_props.add_slot( GOL::VAO_TYPE::VEC3, mesh.array_location_info.normal_ptr, vertex_nb, false );
	//tex_coor
	if( mesh.texture_coordinates_channel_count > 0 )
		vao_props.add_slot( GOL::VAO_TYPE::VEC2, mesh.array_location_info.texture_coordinates_ptr, vertex_nb, false );
	//tangent bitangent
	if( mesh.has_tangent_frame )
	{
		vao_props.add_slot( GOL::VAO_TYPE::VEC3, mesh.array_location_info.tangent_ptr,	 vertex_nb, false );
		vao_props.add_slot( GOL::VAO_TYPE::VEC3, mesh.array_location_info.bitangent_ptr, vertex_nb, false );
	}

	//create ssbo for blend shapes
	fbx_mesh_blend_shape_data const & bsd = mesh.blend_shapes_data;
	if( bsd.buffer_element_count > 0)
	{
		if( bsd.position_data_ptr )
		{
			result.blend_shapes_ssbos.position = gl::ssbo::make(	bsd.buffer_element_count * sizeof(glm::vec3),
																	false,
																	(char*)bsd.position_data_ptr,
																	"bs_position" );
		}
		else
		{
			//todo error message here
		}



		if( bsd.normal_data_ptr )
		{
			result.blend_shapes_ssbos.normal = gl::ssbo::make(		bsd.buffer_element_count * sizeof(glm::vec3),
																	false,
																	(char*)bsd.normal_data_ptr,
																	"bs_normal" );
		}
	}

	st_fbx_mesh_gl_bones_ssbo & bones_ssbos = result.bones_ssbos;
	if( mesh.has_bones )
	{
		fbx_mesh_bones_vertex_data const & vbd = mesh.array_location_info.vertex_bones_data;  
		if( use_ssbo )
		{
			result.use_ssbo_for_bones = true;

			bones_ssbos.vertex_bone_indices		= gl::ssbo::make(	mesh.vertex_weights_count * sizeof(UINT32),
																		false,
																		(char*)vbd.bone_index_ptr,
																		"bone_indices" );
			bones_ssbos.vertex_bone_weights		= gl::ssbo::make(	mesh.vertex_weights_count * sizeof(float),
																		false,
																		(char*)vbd.bone_weight_ptr,
																		"bone_weight" );
			bones_ssbos.vertex_bone_count		= gl::ssbo::make(	vertex_nb * sizeof(UINT32),
																		false,
																		(char*)vbd.bone_count_ptr,
																		"bone_count" );
			bones_ssbos.vertex_bone_location	= gl::ssbo::make(	vertex_nb * sizeof(UINT32),
																		false,
																		(char*)vbd.bone_start_location_ptr,
																		"bone_location" );
		}
		else
		{
			vao_props.add_slot(GOL::VAO_TYPE::UINT4, vbd.bone_index_ptr,  vertex_nb, false );
			vao_props.add_slot(GOL::VAO_TYPE::VEC4,  vbd.bone_weight_ptr, vertex_nb, false );
		}
	}
	else
	{
		if( force_root_bone )
		{
			//we need a single bone index, which points to 0
			UINT32 root_bone_index = 0;
			bones_ssbos.vertex_bone_indices = gl::ssbo::make(	sizeof(UINT32),	false, (char*)&root_bone_index,  "one bone_index 0"   );

			//single bone weight, 1.0f
			float root_bone_weight = 1.0f;
			bones_ssbos.vertex_bone_weights = gl::ssbo::make(	sizeof(float),	false, (char*)&root_bone_weight, "one bone_weight 1.0"   );

			UINT32 size;
			//bone count is per vertex, should be 1 for all
			size = vertex_nb * sizeof(UINT32);
			UINT32* pt_bone_nb = (UINT32*)MALLOC( size );
			for( UINT32 i=0; i<vertex_nb; ++i )
				*(pt_bone_nb+i) = 1;
			//beurk MEMSET( pt_bone_nb, 1, size );
			bones_ssbos.vertex_bone_count	= gl::ssbo::make(	size,			false, (char*)pt_bone_nb,		"bone_count at 1"   );
 
			//bone location is per vertex, should be 0 for all (first index)
			size = vertex_nb * sizeof(UINT32);
			UINT32* pt_bone_location = (UINT32*)MALLOC( size );
			MEMCLEAR( pt_bone_location, size );
			bones_ssbos.vertex_bone_location = gl::ssbo::make(  size,			false, (char*)pt_bone_location,	"bone_location at 0"   );
 

			FREE( pt_bone_nb );
			FREE( pt_bone_location );
		}
	}

	result.c_vao = gl::c_vao::make( vao_props, "fbx_mesh" );

	// can be 0 is not index buffer present, not in loaders case tho _mesh_data.index_count
	gl::ibo_props ibo_props;
	ibo_props._data = (INT32 CONST *)mesh.index_buffer_ptr;
	ibo_props._nb = mesh.index_count;
	ibo_props._b_dynamic = false;

	result.ibo = gl::ibo::make( ibo_props, "fbx_mesh" );

	return result;
}

void fbx_gl_release_mesh( st_fbx_mesh_gl* mesh, bool immediate )
{

	gl::c_vao::release_and_null(	mesh->c_vao	);
	gl::ibo::release_and_null(		mesh->ibo	);
	gl::ssbo::release_and_null(		mesh->bones_ssbos.vertex_bone_count		);
	gl::ssbo::release_and_null(		mesh->bones_ssbos.vertex_bone_indices	);
	gl::ssbo::release_and_null(		mesh->bones_ssbos.vertex_bone_location	);
	gl::ssbo::release_and_null(		mesh->bones_ssbos.vertex_bone_weights	);
	gl::ssbo::release_and_null(		mesh->blend_shapes_ssbos.position		);
	gl::ssbo::release_and_null(		mesh->blend_shapes_ssbos.normal			);

}

void st_fbx_mesh_gl::draw() const
{
	if( use_ssbo_for_bones )
	{
		bones_ssbos.vertex_bone_location->bind(0);
		bones_ssbos.vertex_bone_count->bind(1);
		bones_ssbos.vertex_bone_indices->bind(2);
		bones_ssbos.vertex_bone_weights->bind(3);
	}
	//todomona do it it only if we need it
	if( blend_shapes_ssbos.position )
	{
		blend_shapes_ssbos.position->bind(4);
		if( blend_shapes_ssbos.normal )
			blend_shapes_ssbos.normal->bind(5);
	}

	c_vao->bind();

	ibo->draw_triangles();

	c_vao->unbind();

	 //todomona do it it only if we need it
	if( blend_shapes_ssbos.position )
	{
		if( blend_shapes_ssbos.normal )
			blend_shapes_ssbos.normal->unbind(5);
		blend_shapes_ssbos.position->unbind(4);
	}
	 //todomona do it it only if we need it
	if( use_ssbo_for_bones )
	{
		bones_ssbos.vertex_bone_weights->unbind(3);
		bones_ssbos.vertex_bone_indices->unbind(2);
		bones_ssbos.vertex_bone_count->unbind(1);
		bones_ssbos.vertex_bone_location->unbind(0);
	}
}
