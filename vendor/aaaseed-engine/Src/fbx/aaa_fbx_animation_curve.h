
#ifdef AAA_FBX_ANIMATION_CURVE_H
#error "FBX_ANIMATION_CURVE_H included more than once."
#endif
#define AAA_FBX_ANIMATION_CURVE_H 1


#if !defined(AAA_AAA_TYPE_H)
#	include "code_utils/aaa_type.h"
#endif
#if !defined(AAA_PRIMITIVE_H)
#	include "draw/primitive.h"
#endif
#if !defined(AAA_FBX_STRUCTS_H)
#	include "fbx/internal/fbx_structs.h"
#endif
#if !defined(AAA_FBX_BASE_H)
#	include "fbx/aaa_fbx_base.h"
#endif


//-----------------------------------------------------------------------------

namespace aaa {

	class c_fbx_animation_curve final : public aaa::c_fbx_base
	{
	private:
		c_prim3	_primitive;
		INT32	_curve_vertices_nb;	// we store it because primitive store the allocated number not the used one
//		INT32	_mesh_index;

	public:
		explicit c_fbx_animation_curve( aaa::c_fbx_scene const * p_scene );
		virtual ~c_fbx_animation_curve();

		C_NO_CPY_MOVE(c_fbx_animation_curve);

	private:
		void clear();

	public:
		bool load(INT32 const p_node_index, INT32 const p_animation_index );	//, INT32 const & p_mesh_index);
		void draw() const;

	public:
//		FINLINE INT32 CONST get_mesh_index() CONST { return _mesh_index; }

	}; // class c_fbx_animation_curve

} // namespace aaa

//-----------------------------------------------------------------------------
