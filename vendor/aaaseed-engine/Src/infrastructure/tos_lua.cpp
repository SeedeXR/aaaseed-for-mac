#include "tos_lua.h"
#include "language/lua/aaalua_glue.h"
#include "infrastructure/clipboard.h"

namespace aaalua
{
namespace n_clipboard
{
	AAALUACALL( move_to )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C str	= l.get_str();
	
		::c_clipboard::move_to( str );

		return l.return_nothing();
	}

	void	register_clipboard( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "clipboard" );
			ADD_FN( move_to );
		lua_pop( L, 1 );	//pop table "clipboard"

	}
}	//end namespace n_clipboard
}	//end namespace aaalua
