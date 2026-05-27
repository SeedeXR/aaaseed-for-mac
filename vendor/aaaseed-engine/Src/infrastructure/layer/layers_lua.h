
#ifdef AAA_LAYERS_LUA_H
#error "LAYERS_LUA_H included more than once."
#endif
#define AAA_LAYERS_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_layers
	{
		extern void	register_layers( lua_State* L );
	}
}

