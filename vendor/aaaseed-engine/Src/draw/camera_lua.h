
#ifdef AAA_CAMERA_LUA_H
#error "CAMERA_LUA_H included more than once."
#endif
#define AAA_CAMERA_LUA_H 1


struct  lua_State;

namespace aaalua
{
	namespace n_camera
	{
		extern void	register_camera( lua_State* L );
	}
}
