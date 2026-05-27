
#ifdef AAA_NET_LUA_H
#error "NET_LUA_H included more than once."
#endif
#define AAA_NET_LUA_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
//class	c_lua_state;
struct	lua_State;

namespace aaalua
{	
	void	osc_pkt_flush( INT32 osc_index );

	namespace n_net
	{
		extern void	register_net(	lua_State* L );
		extern void	unregister_net(	lua_State* L );
	}
}

