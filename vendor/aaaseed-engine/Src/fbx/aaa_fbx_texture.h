
#ifdef AAA_FBX_TEXTURE_H
#error "FBX_TEXTURE_H included more than once."
#endif
#define AAA_FBX_TEXTURE_H 1


#if !defined(AAA_AAA_TYPE_H)
#	include "code_utils/aaa_type.h"
#endif

#if !defined(AAA_AAA_STR_H)
#	include "code_utils/aaa_str.h"
#endif

#if !defined(AAA_FBX_BASE_H)
#	include "fbx/aaa_fbx_base.h"
#endif


//-----------------------------------------------------------------------------

namespace aaa {

	class c_fbx_texture final : public aaa::c_fbx_base
	{
	private:
		CHAR CONST * _path{ nullptr };

	public:
		explicit c_fbx_texture(aaa::c_fbx_scene const * p_scene);
		~c_fbx_texture() = default;

		C_NO_CPY_MOVE(c_fbx_texture);

	public:
		void load(INT32 const p_idx);

	public:
		FINLINE CHAR CONST * get_path() CONST { return _path; }

	}; // class c_fbx_material

} // namespace aaa

//-----------------------------------------------------------------------------
