#include "ui_lua.h"
#include "language/lua/aaalua_glue.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "draw/seeddraw.h"

namespace aaalua {
namespace n_mouse {

	AAALUACALL( set_xy_pixel )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32	i_arg	= 0;
		INT32	x		= l.get_int32( ++i_arg );
		INT32	y		= l.get_int32( ++i_arg );

		c_mouse::get_cur()->set_xy_pixel( x,y );

		return l.return_nothing();
	}

	AAALUACALL( get_xy_pixel )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb_escape_no( 0 );

		INT32 x,y;
		c_mouse::get_cur()->get_xy_pixel( x,y );
		return l.return_int_2( x,y );
	}

	AAALUACALL( get_xy_render )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb_escape_no( 0 );

		FP32 xy[2];
		c_mouse::get_cur()->get_xy_render( xy[0],xy[1] );
		return l.return_fp32_v2( xy );
	}

	AAALUACALL( convert_xy_pixel_to_render )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );	
		INT32 x	= l.get_int32( 1 );
		INT32 y	= l.get_int32( 2 );
	
		FP32 xy[2];
		mouse_convert_xy_pixel_to_render( x,y, xy[0],xy[1] );
		return l.return_fp32_v2( xy );
	}

	AAALUACALL( set_grabbed )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );

		bool CONST	b		= l.get_bool( 1 );
		c_mouse::set_grabbed( b );
		return l.return_nothing();
	}

	AAALUACALL( cancel_move )
	{
		LUAAAA_START( L, __FUNCTION__ );
		ui::stop_xy();
		return l.return_nothing();
	}

	void	register_mouse( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "mouse" );
			ADD_FN( set_xy_pixel );
			ADD_FN( get_xy_pixel );
			ADD_FN( get_xy_render );
			ADD_FN( convert_xy_pixel_to_render );

			ADD_FN( set_grabbed );
			ADD_FN( cancel_move );
		lua_pop( L, 1 );	//pop table "mouse"
//		l.define_table( "event" );
//		lua_pop( L, 1 );	//pop table "event"
	}

}	//end namespace n_mouse
}	//end namespace aaalua
