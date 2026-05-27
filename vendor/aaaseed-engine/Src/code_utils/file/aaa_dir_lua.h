
#ifdef AAA_AAA_DIR_LUA_H
#error "AAA_DIR_LUA_H included more than once."
#endif
#define AAA_AAA_DIR_LUA_H 1

struct lua_State;

namespace aaalua
{
	namespace n_dir
	{
		extern void	register_dir( lua_State* L );
	}
}

