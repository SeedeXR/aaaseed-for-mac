

#ifdef AAA_TOS_LUA_H
#error "TOS_LUA_H included more than once."
#endif
#define AAA_TOS_LUA_H 1

struct	lua_State;

namespace aaalua
{
	namespace n_clipboard
	{
		extern void	register_clipboard( lua_State* L );
	}
}

