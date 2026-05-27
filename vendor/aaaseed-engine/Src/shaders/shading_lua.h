
#ifdef AAA_SHADING_LUA_H
#error "SHADING_LUA_H included more than once."
#endif
#define AAA_SHADING_LUA_H 1


struct lua_State;
class c_lua_state;

namespace aaalua
{
	namespace n_shading
	{
		extern void		register_shading( lua_State* L );
	}
}


