
#ifdef AAA_AAALUA_DEBUG_H
#error "AAALUA_DEBUG_H included more than once."
#endif
#define AAA_AAALUA_DEBUG_H 1


#ifndef	AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef __LUA_HPP__
#	include "lua/lua.hpp"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

void			aaalua_debug_grab_info( lua_State* CONST L );
CHAR*			aaalua_debug_build_trace( C_PCHAR_C str_in );

INT32			aaalua_debug_get_level();
void			aaalua_debug_set_level( INT32 CONST level );
C_PCHAR			aaalua_debug_get_script_name();
o_str CONST &	aaalua_debug_get_script_name_o_str();
INT32			aaalua_debug_get_line();


