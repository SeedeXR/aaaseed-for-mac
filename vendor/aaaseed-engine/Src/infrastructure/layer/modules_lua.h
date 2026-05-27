
#ifdef AAA_MODULES_LUA_H
#error "MODULES_LUA_H included more than once."
#endif
#define AAA_MODULES_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_modules
	{
		extern void	register_modules( lua_State* L );
	}
}

