
#ifdef AAA_FBX_POSE_H
#error "FBX_POSE_H included more than once."
#endif
#define AAA_FBX_POSE_H 1


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

	class c_fbx_pose final : public aaa::c_fbx_base
	{
	private:
		c_fbx_pose_gl	_pose_gl			;
		glm::mat4 *		_bind_pose_data		{ nullptr };
		UINT32 *		_bone_node_index	{ nullptr };
		bool			_b_has_bones		{ false };
		CHAR CONST *	_name				{ nullptr };

	public:
		explicit c_fbx_pose( aaa::c_fbx_scene const * p_scene );
		~c_fbx_pose();

		C_NO_CPY_MOVE( c_fbx_pose );

	public:
		bool load( fbx_mesh_data CONST * data, INT32 CONST node_idx );

		FINLINE CHAR CONST *	get_name()  CONST	{ return _name;				}
		FINLINE bool CONST		has_bones() CONST	{ return _b_has_bones;		}
		FINLINE glm::mat4*		get_root() CONST	{ return _bind_pose_data;	}
		FINLINE void			bind() CONST		{ _pose_gl.bind();			}
		FINLINE void			unbind() CONST		{ _pose_gl.unbind();		}
	}; // class c_fbx_pose

} // namespace aaa

//-----------------------------------------------------------------------------
