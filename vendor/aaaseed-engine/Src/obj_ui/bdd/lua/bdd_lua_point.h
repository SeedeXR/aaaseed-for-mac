#pragma once

#ifdef AAA_BDD_LUA_POINT_H
#error "BDD_LUA_POINT_H included more than once."
#endif
#define AAA_BDD_LUA_POINT_H 1


struct lua_State;

namespace aaalua
{
	namespace n_bdd
	{
		extern void		register_bdd_point( lua_State* L );
	}
}

