
#ifdef AAA_DEFORMER_LUA_H
#error "DEFORMER_LUA_H included more than once."
#endif
#define AAA_DEFORMER_LUA_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_deformer
	{
		extern void	register_deformer( lua_State* L );
	}
}
