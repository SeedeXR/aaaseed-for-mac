
#ifdef AAA_MODULE_LUA_H
#error "MODULE_LUA_H included more than once."
#endif
#define AAA_MODULE_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_module
	{
		extern void	register_module( lua_State* L );
	}
}

