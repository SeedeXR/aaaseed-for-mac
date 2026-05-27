
#ifdef AAA_AAALUA_DRAW_H
#error "AAALUA_DRAW_H included more than once."
#endif
#define AAA_AAALUA_DRAW_H 1


struct	lua_State;

namespace aaalua
{
	namespace n_draw
	{
		extern void	register_draw( lua_State* L );
	}
}

