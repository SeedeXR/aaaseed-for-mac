
#ifdef AAA_FBX_GRAPH_H
#error "FBX_GRAPH_H included more than once."
#endif
#define AAA_FBX_GRAPH_H 1


#if !defined(AAA_AAA_TYPE_H)
#	include "code_utils/aaa_type.h"
#endif

#if !defined(AAA_FBX_STRUCTS_H)
#	include "fbx/internal/fbx_structs.h"
#endif

#if !defined(AAA_FBX_UTILS_GL_H)
#	include "fbx/internal/fbx_utils_gl.h"
#endif

#if !defined(AAA_FBX_BASE_H)
#	include "fbx/aaa_fbx_base.h"
#endif


//-----------------------------------------------------------------------------

namespace aaa {

	class c_fbx_graph final : public aaa::c_fbx_base
	{

	private:
		c_fbx_graph_gl	_graph_gl;
		glm::mat4 *		_graph_matrices_data;

	public:
		explicit c_fbx_graph( aaa::c_fbx_scene const * p_scene );
		~c_fbx_graph();

		C_NO_CPY_MOVE(c_fbx_graph);

	private:
		void update_animation(
			FP32 CONST		p_time,
			UINT32 CONST		p_animation_stack_index,
			bool CONST			p_linear,
			glm::mat4 CONST*	p_post_local_transforms,
			UINT32 CONST*		p_post_local_transforms_indices,
			UINT32 CONST		p_post_local_transforms_count);

	public:
		bool load(fbx_scene_data const * p_data);
		void update(
			FP32 const		p_time,
			UINT32 const		p_animation_index,
			bool CONST			p_linear,
			glm::mat4 CONST*	p_post_local_transforms,
			UINT32 CONST*		p_post_local_transforms_indices,
			UINT32 CONST		p_post_local_transforms_count);
		void bind() const;
		void unbind() const;

		FINLINE  glm::mat4* CONST get_matrices_data() CONST { return _graph_matrices_data; }

	}; // class c_fbx_graph

} // namespace aaa

//-----------------------------------------------------------------------------
