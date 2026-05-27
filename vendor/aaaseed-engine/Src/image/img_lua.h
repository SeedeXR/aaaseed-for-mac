
#ifdef AAA_IMG_LUA_H
#error "IMG_LUA_H included more than once."
#endif
#define AAA_IMG_LUA_H 1


//class	c_lua_state;
struct	lua_State;

namespace aaalua
{
	namespace n_img
	{
		extern void	register_img( lua_State* L );
	}
}


