
#ifdef AAA_MATH_LUA_H
#error "MATH_LUA_H included more than once."
#endif
#define AAA_MATH_LUA_H 1


struct  lua_State;

namespace aaalua
{
	namespace n_math
	{
		extern void	register_math( lua_State* L );
	}
}

