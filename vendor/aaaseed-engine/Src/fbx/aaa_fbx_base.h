
#ifdef AAA_FBX_BASE_H
#error "FBX_BASE_H included more than once."
#endif
#define AAA_FBX_BASE_H 1


#ifndef AAA_OBJ_H
#	include		"infrastructure/obj/obj.h"
#endif


//-----------------------------------------------------------------------------

namespace aaa { class c_fbx_scene; }

//-----------------------------------------------------------------------------

namespace aaa {

	class c_fbx_base : public c_obj
	{
	private:
		aaa::c_fbx_scene const * _scene{ nullptr };

	public:
		explicit c_fbx_base( aaa::c_fbx_scene const * p_scene );
		virtual ~c_fbx_base() = default;

		C_NO_CPY_MOVE(c_fbx_base);

	protected:
		inline aaa::c_fbx_scene const * get_scene() const { return _scene; }

	}; // class c_fbx_base

} // namespace aaa

//-----------------------------------------------------------------------------
