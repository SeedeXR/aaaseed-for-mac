
#ifdef AAA_UI_LUA_H
#error "UI_LUA_H included more than once."
#endif
#define AAA_UI_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_mouse
	{
		extern void	register_mouse( lua_State* L );
	}
}

