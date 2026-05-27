#ifdef AAA_BDD_LUA_FLEX_H
#error "BDD_LUA_FLEX_H included more than once."
#endif
#define AAA_BDD_LUA_FLEX_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

struct lua_State;

namespace aaalua
{
	namespace n_bdd_flex
	{
	extern	void	register_bdd_flex( lua_State* L );
	}
}
