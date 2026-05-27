
#ifdef AAA_COLOR_SPACE_LUA_H
#error "COLOR_SPACE_LUA_H included more than once."
#endif
#define AAA_COLOR_SPACE_LUA_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

struct	lua_State;
namespace aaalua
{
	namespace n_color
	{
		extern void	register_color( lua_State* L );
	}
}

