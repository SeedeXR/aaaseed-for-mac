
#ifdef AAA_AAALUA_EXP_H
#error "AAALUA_EXP_H included more than once."
#endif
#define AAA_AAALUA_EXP_H 1


struct	lua_State;

extern	void tag_error( lua_State* L, int narg, int tag );

namespace aaalua
{
	namespace n_exp
	{
		extern void	register_exp( lua_State* L );
	}
}

