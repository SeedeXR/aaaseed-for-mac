
#ifdef AAA_BDD_LUA_H
#error "BDD_LUA_H included more than once."
#endif
#define AAA_BDD_LUA_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

struct lua_State;
class c_bdd;
class c_lua_state;

namespace aaalua
{
	namespace n_bdd
	{
		extern			void	register_bdd( lua_State* L );
		extern			 c_bdd*	get_bdd_cur( c_lua_state& l );
		extern FINLINE	c_bdd*	get_bdd(		c_lua_state& l, bool b_cur );
		extern FINLINE	c_bdd*	get_bdd(		c_lua_state& l, bool b_cur, INT32& index );
		extern FINLINE	c_bdd*	get_bdd_first(	c_lua_state& l );
	}
}


