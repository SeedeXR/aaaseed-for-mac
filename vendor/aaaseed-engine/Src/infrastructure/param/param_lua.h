#ifdef AAA_PARAM_LUA_H
#error "PARAM_LUA_H included more than once."
#endif
#define AAA_PARAM_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_param
	{
		extern void	register_param( lua_State* L );
	}
}

