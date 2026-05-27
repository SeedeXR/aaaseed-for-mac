#include "flatland.h"
#include "language/lua/aaalua_glue.h"
#include "flatland_lua.h"

namespace aaalua
{
namespace n_flatland
{
	//		str = aaa.flatland.get_mess( index )
	AAALUACALL( get_mess )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 index		= l.get_int32( 1 );
		return l.return_string( aaa::mess::get( index ) );
	}

	//		r,g,b,b_inverse = aaa.flatland.get_mess_color( index )
	AAALUACALL( get_mess_color )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 index			= l.get_int32( 1 );
		FP32	color[3];
		bool	b_inv;
		aaa::mess::get_color( index, color, b_inv );
		l.push_fp32_v3( color );
		l.push_bool( b_inv );
		return 4;
	}
	//		r,g,b,b_inverse = aaa.flatland.get_mess_color( index )
	AAALUACALL( get_mess_line_nb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_int32( aaa::mess::get_line_nb() );
	}
	//		b = aaa.flatland.is_in_focus( x,y )
	AAALUACALL( is_in_focus )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32 x			= l.get_int32( 1 );
		INT32 y			= l.get_int32( 2 );
		bool b = c_flatland::is_in_focus( x,y );
		return l.return_bool( b );
	}
	//		b = aaa.flatland.is_in_info( x,y )
	AAALUACALL( is_in_info )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32 x			= l.get_int32( 1 );
		INT32 y			= l.get_int32( 2 );
		bool b = c_flatland::is_in_info( x,y );
		return l.return_bool( b );
	}
	//		b = aaa.flatland.is_in( x, y )
	AAALUACALL( is_in )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32 x			= l.get_int32( 1 );
		INT32 y			= l.get_int32( 2 );
		bool b = c_flatland::is_in( x,y );
		return l.return_bool( b );
	}


	void	register_flatland( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

			l.define_table( "flatland" );
			ADD_FN( get_mess );
			ADD_FN( get_mess_color );
			ADD_FN( get_mess_line_nb );
			ADD_FN( is_in_focus );
			ADD_FN( is_in_info );
			ADD_FN( is_in );
	
		lua_pop( L, 1 );	//pop table "flatland"
	}
}	//end namespace n_flatland
}	//end namespace aaalua
