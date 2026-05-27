
#ifdef AAA_MAT_LUA_H
#error "MAT_LUA_H included more than once."
#endif
#define AAA_MAT_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_materials
	{
		extern void	register_material( lua_State* L );
	}
}
