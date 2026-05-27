#include "aaa_menu.h"
#include "language/lua/aaalua_glue.h"
#include "language/lua/aaalua_wrap.h"
#include "ui/seedmenu.h"

namespace aaalua
{
namespace n_menu
{
	/*
	void	c_lua_wrap::do_fn(  )
	{
		lua_lock.lock();
		STACK_ENFORCE_STORE( _l );

		l.push_real
		{
			// the function takes 0 parameters and no return
			lua_caller = 0;
			if( _l->pcall( 0, 0 ) != LUA_OK )
				_l->pop_str_and_print_error( "PCALL", __FUNCTION__, fn_name );
		}
		STACK_ENFORCE_SAME( _l );
		lua_lock.unlock();
	}
	*/

	AAALUACALL( get_main )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_int32( c_menu::main_menu );
	}
	AAALUACALL( set_main )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 CONST menu_id = l.get_int32( 1 );
		c_menu::main_menu = menu_id;
		return	0;
	}

	AAALUACALL( get_tree_param )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_int32( c_menu::tree_param );
	}
	AAALUACALL( set_tree_param )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 CONST	menu_id = l.get_int32( 1 );
		c_menu::tree_param = menu_id;
		return	0;
	}

	AAALUACALL( get_param_list )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_int32( c_menu::param_list_id );
	}

	//int		reflua;
	//C_PCHAR menu_key = "aaamenu_key";

	void MAACALLBACK	menu_callback( int value )
	{
		GOOD_PRINT_STRING( "lua glue menu_callback with %d", value );
		g_lua_wrap_master->do_fn_pass_i( nullptr, "aaa.menu.hook", value );
	}

	//	extern	INT32	create( PT_MENU_FN menu_fn );
	AAALUACALL( create )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.check_arg_nb( 1 );
		//printf( "type is %s", l.get_type_str( l.get_type(1) ) );
		//reflua = luaL_ref( L, LUA_REGISTRYINDEX );	//won't link ?
		/*
		//put fn in the lua registry
		lua_pushlightuserdata( L, (void*)menu_key );
		lua_pushvalue( L, -2 );
		lua_settable( L, LUA_REGISTRYINDEX );
		l.pop( 1 );
		*/
		//lua_pushstring
		return l.return_int32( menu::create( menu_callback ) );
	}
	AAALUACALL( set_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 CONST	menu_id		= l.get_int32( 1 );
		menu::set_cur( menu_id, __FUNCTION__ );
		return l.return_nothing();
	}
	AAALUACALL( get_item_nb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_int32( menu::get_item_nb() );
	}


	AAALUACALL( insert_item )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 arg_nb = l.get_arg_nb( 3,4 );
		INT32 CONST item_pos	= l.get_int32( 1 );
		C_PCHAR_C	label		= l.get_str( 2 );
		INT32 CONST	value		= l.get_int32( 3 );
		INT32 CONST	check		= (arg_nb == 4) ? l.get_int32( -1 ) : 0;
		menu::insert_item( item_pos, label, value, check );
		return l.return_nothing();
	}
	AAALUACALL( add_item )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 arg_nb = l.get_arg_nb( 2,3 );
		C_PCHAR_C	label		= l.get_str( 1 );
		INT32 CONST	value		= l.get_int32( 2 );
		INT32 CONST	check		= (arg_nb == 3) ? l.get_int32( -1 ) : 0;
 		menu::add_item( label, value, check );
		return l.return_nothing();
	}
	AAALUACALL( change_item )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 arg_nb = l.get_arg_nb( 3,4 );
		INT32 CONST item_pos	= l.get_int32( 1 );
		C_PCHAR_C	label		= l.get_str( 2 );
		INT32 CONST	value		= l.get_int32( 3 );
		INT32 CONST	check		= (arg_nb == 4) ? l.get_int32( -1 ) : 0;
		menu::change_item( item_pos, label, value, check );
		return l.return_nothing();
	}
	AAALUACALL( remove_item )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 CONST item_pos	= l.get_int32( 1 );
		menu::remove_item( item_pos );
		return l.return_nothing();
	}

	AAALUACALL( insert_separator )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 CONST item_pos	= l.get_int32( 1 );
		menu::insert_separator( item_pos );
		return l.return_nothing();
	}
	AAALUACALL( add_separator )
	{
		LUAAAA_START( L, __FUNCTION__ );
		menu::add_separator();
		return l.return_nothing();
	}

	AAALUACALL( insert_menu_sub )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		INT32 CONST	item_pos	= l.get_int32( 1 );
		C_PCHAR_C	label		= l.get_str( 2 );
		INT32 CONST	submenu		= l.get_int32( 3 );
		menu::insert_menu_sub( item_pos, label, submenu );
		return l.return_nothing();
	}
	AAALUACALL( add_menu_sub )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		C_PCHAR_C	label		= l.get_str( -2 );
		INT32 CONST	submenu		= l.get_int32( -1 );
		menu::add_menu_sub( label, submenu );
		return l.return_nothing();
	}

//extern	AAA_ERR	change_menu_sub( UINT32 pos, CONST CHAR *label, INT32 menu_sub_id );

	void	register_menu( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "menu" );

		ADD_FN( get_main			);
			ADD_FN( set_main			);

			ADD_FN( get_tree_param		);
			ADD_FN( set_tree_param		);

			ADD_FN( get_param_list		);

			ADD_FN( create				);
			ADD_FN( set_cur				);
			ADD_FN( get_item_nb			);

			ADD_FN( insert_item			);
			ADD_FN( add_item			);

			ADD_FN( insert_separator	);
			ADD_FN( add_separator		);

			ADD_FN( insert_menu_sub		);
			ADD_FN( add_menu_sub		);

			ADD_FN( change_item			);
			ADD_FN( remove_item			);

		lua_pop( L, 1 );	//pop table "menu"
	}
}	//end namespace n_menu
}	//end namespace aaalua
