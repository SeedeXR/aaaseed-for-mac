
#ifdef AAA_LIGHTS_LUA_H
#error "LIGHTS_LUA_H included more than once."
#endif
#define AAA_LIGHTS_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_lights
	{
		extern void	register_lights( lua_State* L );
	}
}


