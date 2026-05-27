
#ifdef AAA_FBO_LUA_H
#error "FBO_LUA_H included more than once."
#endif
#define AAA_FBO_LUA_H 1


struct  lua_State;

namespace aaalua
{
	namespace n_fbo
	{
		extern void	register_fbo( lua_State* L );
	}
}

