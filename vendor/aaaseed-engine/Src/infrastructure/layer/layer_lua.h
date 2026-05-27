
#ifdef AAA_LAYER_LUA_H
#error "LAYER_LUA_H included more than once."
#endif
#define AAA_LAYER_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_layer
	{
		extern void	register_layer( lua_State* L );
	}
}

