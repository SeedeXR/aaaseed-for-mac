
#ifdef AAA_FBX_UTILS_H
#error "FBX_UTILS_H included more than once."
#endif
#define AAA_FBX_UTILS_H 1


//-----------------------------------------------------------------------------

namespace aaa {

	enum fbx_binding_e {
		fbx_binding_hierarchy	= 0,
		fbx_binding_pose		= 1,
		fbx_binding_node_idx	= 2, 
	};

} // namespace aaa

//-----------------------------------------------------------------------------
