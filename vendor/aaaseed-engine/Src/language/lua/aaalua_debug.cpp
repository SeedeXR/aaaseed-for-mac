
#include "aaalua_debug.h"
#include "err.h"


#if AAA_OS_WINDOWS()
#	ifndef AAA_SYSTEMUTILS_H
#		include "system/shared/SystemUtils.h"
#	endif

#	if (NTDDI_VERSION >= NTDDI_WIN2K)
#		ifndef _SHLOBJ_H_
#			include "shlobj.h"
#		endif
#	else
#		include "ShFolder.h"
#	endif
#endif

namespace {
	CONSTEXPR	INT32		AR_LEVEL_MAX = 64;
				lua_Debug	ars[AR_LEVEL_MAX];
				INT32		ar_level = 0;
				o_str		script_name;
				INT32		script_line;
};

void	aaalua_debug_grab_info( lua_State* L ) 
{
	ar_level = 0;

	while( lua_getstack( L, ar_level+1, &ars[ar_level] ) )
	{	//	check function at level
		lua_Debug* par =  &ars[ar_level];
		lua_getinfo( L, "Snl", par );	//	get info about it
		++ar_level;
		if( ar_level >= AR_LEVEL_MAX )
		{
			ERR_PRINT_STRING( "Lua traceback support only %d level in AAASeed skipping the rest", AR_LEVEL_MAX );
			break;
		}
	}
	//lua_pushliteral( L, "" );  /* else, no information available... */
}

//hack this have to be cleaner aaalua_debug_build_trace() should not be able to trash memory
namespace {
	CHAR	lua_trace[1024 * 2 * 64];
}

C_PCHAR			aaalua_debug_get_script_name()				{	return	script_name.get();	}
o_str CONST &	aaalua_debug_get_script_name_o_str()		{	return	script_name;		}
INT32			aaalua_debug_get_line()						{	return	script_line;		}
INT32			aaalua_debug_get_level( )					{	return ar_level;			}
void			aaalua_debug_set_level( INT32 CONST level )	{	ar_level = level;			}

CHAR*	aaalua_debug_build_trace( C_PCHAR_C str_in )
{
	script_name.erase();
	script_line = 0;

	CHAR*	pt = lua_trace;
	if( str_in )
		pt += sprintf( pt, "%s\n", str_in );

	for( INT32 level = 0; level < ar_level; ++level )
	{	//	check function at level
		lua_Debug* par =  &ars[level];
		if( par->currentline > 0 )
		{	// is there info ?
			if( *(par->namewhat) == 0 )									pt += sprintf( pt, "%.512s\n\t%d:%.512s",			par->what,						par->currentline, par->source	);
			else if( str_is_equal( "metamethod",	par->namewhat ) )	pt += sprintf( pt, "%.512s %.512s()\n\t%d:%.512s",	par->namewhat,	par->name,		par->currentline, par->source	);
			else if( str_is_equal( "method",		par->namewhat ) )	pt += sprintf( pt, "%.512s %.512s()\n\t%d:%.512s",	par->namewhat,	par->name,		par->currentline, par->source	);
			else if( str_is_equal( "field",			par->namewhat ) )	pt += sprintf( pt, "%.512s %.512s()\n\t%d:%.512s",	par->namewhat,	par->name,		par->currentline, par->source	);
			else if( str_is_equal( "global",		par->namewhat ) )	pt += sprintf( pt, "%.512s %.512s()\n\t%d:%.512s",	par->namewhat,	par->name,		par->currentline, par->source	);
			else if( str_is_equal( "local",			par->namewhat ) )	pt += sprintf( pt, "%.512s %.512s()\n\t%d:%.512s",	par->namewhat,	par->name,		par->currentline, par->source	);
			else 														pt += sprintf( pt, "%.512s\n\tline %d",				par->short_src,	par->currentline								);
			if( script_line == 0 && (*(par->source)!='@') && !str_have_common_char( par->source, "\n" ) )		//hack	@ is there on the error provoke by strict.lua we use it to go up one level
			{														//hack	last test come from pratical experience in the field case the source is the lua text and the @ is not there
				script_line = par->currentline;
				script_name.set( par->source );
			}
			//lua_pushfstring(L, "%s:%d: ", par->short_src, par->currentline);
		}
		else if( str_is_equal( "C", par->what ) )
		{
			pt += sprintf( pt, "%s %s()",	par->namewhat, par->name );
		}
		pt += sprintf( pt, "\n" );
	}
	return lua_trace;
}

