
#ifdef AAA_GOL_LUA_H
#error "GOL_LUA_H included more than once."
#endif
#define AAA_GOL_LUA_H 1


struct lua_State;

namespace aaalua
{
	namespace n_gol
	{
		extern	void	register_gol( lua_State* L );
	}
}

