#include "aaa_def.h"
#include "language/lua/aaalua_glue.h"
#include "language/lua/aaalua_wrap.h"
#include "watchdog.h"
#include "obj_ui/com/net.h"
#include "time/aaa_time.h"
#include "spy.h"
#include "file/aaa_dir.h"


#if	AAA_WATCHDOG()
	bool	c_watchdog::b_on = true;
#else
	bool	c_watchdog::b_on = false;
#endif

namespace {
	bool	b_exit		= false;
	REAL	loop_time	= REAL(.1);
};

void	c_watchdog::trig_exit()	{	b_exit = true;	}
bool	c_watchdog::is_exit()	{	return	b_exit;	}
void	c_watchdog::set_loop_time( REAL loop_time_in )	{	loop_time = loop_time_in; }

void	c_watchdog::set_on(	bool b_in )
{
	b_on = b_in;
	SWITCH_PRINT_STRING( "WatchDog mode", "%s", b_on ? "On." : "Off." );
}

void	c_watchdog::main()
{
	c_lua_wrap	lua_wrap;

	//	watchdog is in start directory
	c_dir::push_def( c_dir::get_start() );
	o_str& o = o_str::push_name( c_dir::get_user() );

		o.add( "/aaa_watchdog.lua" );
		lua_wrap.set_do();
		lua_wrap.set_script_filename( o.get() );
		lua_wrap.set_file_check();
		lua_wrap.update();
		do
		{
			lua_wrap.update();
			net->update();
			spy::sleep( INT32(loop_time * 1000), "sleep watchdog main" );
		}
		while( !b_exit );

	o_str::pop_name();
	c_dir::pop_def();
}

namespace aaalua
{
	//	input	none
	AAALUACALL( trig_exit )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_watchdog::trig_exit();
		return l.return_nothing();
	}

	//	input	link_dst name [name...]
	AAALUACALL( net_send )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb_min( 2 );
		INT32	CONST	link_dst = l.get_int32( 1 );

		for( INT32 i = 1; i < nb_arg; ++i )
		{
			C_PCHAR_C text = l.get_str( i + 1 );
			aaalua::watchdog_net_send( link_dst, text );
		}
		return l.return_nothing();
	}
	//	input	loop_time_ms
	AAALUACALL( set_loop_time )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb_min( 1 );
		c_watchdog::set_loop_time( l.get_real( 1 ) );

		return l.return_nothing();
	}
	AAALUACALL( get_time )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_double( aaa::time::get_real_time() );
	}
	

//#define	ADD_FN_WATCHDOG( name )		l.add_fn_to_table( #name, luaaaa_watchdog_##name )
	void	register_aaalua( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table_global_get( "watchdog" );
			ADD_FN( trig_exit );
			ADD_FN( net_send );
			ADD_FN( set_loop_time );
			ADD_FN( get_time );
		l.pop( 1 );	//pop table "watchdog"
	}
}


void	c_watchdog::register_aaalua( lua_State* L )
{
	aaalua::register_aaalua( L );
}
