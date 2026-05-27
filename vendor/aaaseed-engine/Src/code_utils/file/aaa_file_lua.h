
#ifdef AAA_AAA_FILE_LUA_H
#error "AAA_FILE_LUA_H included more than once."
#endif
#define AAA_AAA_FILE_LUA_H 1


#ifndef AAA_AAALUA_GLUE_H
#	include "language/lua/aaalua_glue.h"
#endif	//AAA_AAALUA_GLUE_H

namespace aaalua
{
	namespace n_file
	{
		extern void	register_file( lua_State* L );
		extern 	AAALUACALL(	rename );
		extern 	AAALUACALL(	move );
	}
}

