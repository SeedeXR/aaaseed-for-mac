
#ifdef AAA_FBX_CAMERA_H
#error "FBX_CAMERA_H included more than once."
#endif
#define AAA_FBX_CAMERA_H 1


#if !defined(AAA_AAA_TYPE_H)
#	include "code_utils/aaa_type.h"
#endif

#if !defined(AAA_FBX_BASE_H)
#	include "fbx/aaa_fbx_base.h"
#endif


//-----------------------------------------------------------------------------

namespace aaa {

	class c_fbx_camera final : public aaa::c_fbx_base
	{
	private:
		INT32 _node_index{ -1 };

	public:
		explicit c_fbx_camera( aaa::c_fbx_scene const * p_scene );
		~c_fbx_camera() = default;

		C_NO_CPY_MOVE(c_fbx_camera);

	public:
		void update(FP32 const p_time);
		void bind();

	}; // class c_fbx_camera

} // namespace aaa

//-----------------------------------------------------------------------------
