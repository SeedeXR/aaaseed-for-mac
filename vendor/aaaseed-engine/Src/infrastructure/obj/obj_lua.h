
#ifdef AAA_OBJ_LUA_H
#error "OBJ_LUA_H included more than once."
#endif
#define AAA_OBJ_LUA_H 1


struct  lua_State;

namespace	aaalua
{
	namespace	n_obj
	{
		extern void	register_obj( lua_State* L );
	}
}

