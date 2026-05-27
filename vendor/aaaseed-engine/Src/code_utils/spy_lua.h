
#ifdef AAA_SPY_LUA_H
#error "SPY_LUA_H included more than once."
#endif
#define AAA_SPY_LUA_H 1


struct lua_State;

namespace aaalua
{
	namespace n_spy
	{
		extern	void	register_spy(lua_State* L);
	}
}

