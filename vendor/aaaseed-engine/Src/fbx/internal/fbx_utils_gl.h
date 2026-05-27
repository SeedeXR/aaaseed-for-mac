
#ifdef AAA_FBX_UTILS_GL_H
#error "FBX_UTILS_GL_H included more than once."
#endif
#define AAA_FBX_UTILS_GL_H 1


#if !defined(AAA_FBX_STRUCTS_H)
#include "fbx/internal/fbx_structs.h"
#endif

#if !defined(AAA_FBX_STRUCTS_INTERNAL_H)
#include "fbx/internal/fbx_structs_internal.h"
#endif

#if !defined(AAA_AAA_MATRIX_H)
#	include "math/aaa_matrix.h"
#endif

#if !defined(AAA_GL_IBO_H)
#	include "gl/ibo.h"
#endif

#if !defined(AAA_GL_VAO_H)
#	include "gl/vao.h"
#endif

#if !defined(AAA_GL_UBO_H)
#	include "gl/ubo.h"
#endif

#if !defined(AAA_GL_SSBO_H)
#	include "gl/ssbo.h"
#endif


//-----------------------------------------------------------------------------

CONSTEXPR INT32 BDD_FBX_MAX_UBO_MATRICES = 512;

//-----------------------------------------------------------------------------

/// 
/// Global graph.
/// 
class c_fbx_graph_gl final : public c_obj
{
private:
	gl::ubo * _hierarchy_matrices{ nullptr };

public:
	c_fbx_graph_gl() = default;
	~c_fbx_graph_gl() = default;

	C_NO_CPY_MOVE(c_fbx_graph_gl)

public:
	void init( glm::mat4 * skinning_data, UINT32 const matrix_nb, bool const immediate );
	void release( bool const immediate );

public:
	void bind() const;
	void unbind() const;

public:
	void write( glm::mat4 const * skinning_data, bool const immediate );
};

//-----------------------------------------------------------------------------

/// 
/// Pose data per mesh.
/// 
class c_fbx_pose_gl final : public c_obj
{
private:
	gl::ubo * _bind_pose{ nullptr };
	gl::ubo * _node_indices{ nullptr };

public:
	c_fbx_pose_gl() = default;
	~c_fbx_pose_gl() = default;

	C_NO_CPY_MOVE(c_fbx_pose_gl)

public:
	void init( glm::mat4 * bind_pose, UINT32 * node_table, UINT32 const nb, bool const immediate );
	void release( bool const immediate );

public:
	void bind() const;
	void unbind() const;
};

//-----------------------------------------------------------------------------

/*!\brief ssbo data for vertex bones */
struct st_fbx_mesh_gl_bones_ssbo
{
	gl::ssbo * vertex_bone_indices{ nullptr };
	gl::ssbo * vertex_bone_weights{ nullptr };
	gl::ssbo * vertex_bone_count{ nullptr };
	gl::ssbo * vertex_bone_location{ nullptr };
};

struct st_fbx_mesh_gl_blend_shapes_ssbo
{
	gl::ssbo* position	{ nullptr };
	gl::ssbo* normal	{ nullptr };
};

/*!\brief simple gl mesh */
struct st_fbx_mesh_gl
{
	/*!\brief vertex array object */
	gl::c_vao* c_vao { nullptr };	
	/*!\brief index buffer */
	gl::ibo* ibo { nullptr };	
	/*!\brief tells if we want to use ssbo for bones */
	bool use_ssbo_for_bones;
	/*!\brief ssbo for bones if enabled */
	st_fbx_mesh_gl_bones_ssbo bones_ssbos;
	/*!\brief ssbo for blend shapes if enables */
	st_fbx_mesh_gl_blend_shapes_ssbo blend_shapes_ssbos;

	/*!\brief attach, draw and detach */
	void draw() const;  //toto should be a class no or this fn take out
};

st_fbx_mesh_gl fbx_gl_create_mesh( const fbx_mesh_data& mesh, bool use_ssbo, bool force_root_bone, bool immediate );

void fbx_gl_release_mesh( st_fbx_mesh_gl* mesh, bool immediate );
