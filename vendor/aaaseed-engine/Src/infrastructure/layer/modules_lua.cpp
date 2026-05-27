#include "modules_lua.h"
#include "modules.h"
#include "module.h"
#include "language/lua/aaalua_glue.h"
		 

namespace aaalua
{
namespace n_modules
{
	//	no_input
		//	no input

	AAALUACALL( get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_modules::get_cur(), "no current modules" );
	}

	//	no_input
	AAALUACALL( get_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_modules::get_ui() );
	}

	FINLINE	c_modules*	get_modules( c_lua_state& l, INT32 index )
	{
		c_modules* modules;
		if( index )
			modules = l.get_aaa_obj_of_class<c_modules>( index );
		else
			modules = c_modules::get_cur();
		return modules;
	}

	
	AAALUACALL( get_module_nb_max )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONSTEXPR NB_ARG = 1;
		l.check_arg_nb( NB_ARG );
		c_modules*	modules	= get_modules( l, 1 );
		if( modules )
			l.return_int32( modules->module_get_nb_max() );
		return l.return_nothing();
	}

	AAALUACALL( set_module_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST NB_ARG = 2;
		l.check_arg_nb( NB_ARG );
		c_modules*	modules	= get_modules( l, 1 );
		if( modules )
		{
			INT32	index	=	l.get_int32( 2 )-1;	//	1-based Lua to 0-based C++
			l.pop( NB_ARG );		//	clean the stack for under
			modules->module_ui_set( index );
		}
		return l.return_nothing();
	}

	AAALUACALL( new_module )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST NB_ARG = 3;
		l.check_arg_nb( NB_ARG );
		c_modules*	modules	= get_modules( l, 1 );
		if( modules )
		{
			o_str filename( l.get_str(3) );
			INT32 index = l.is_nil(2) ? -1 : l.get_int32(2)-1;
			l.pop( NB_ARG );		//	clean the stack for under
			modules->module_forget( index );
			c_module* module = modules->module_new_from_file( filename, index );
			if( module )
			{
				modules->module_load_from_file( filename );
				return l.return_obj_ref( module ); 
			}
		}
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name, index )
	AAALUACALL( get_module )
	{		
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST nb_arg = l.get_arg_nb( 1, 2 );
		c_modules*	modules	= get_modules( l, 1 );
		if( modules )
		{	
			c_module* module = (nb_arg>1) ? modules->module_get_from_index( l.get_int32(2)-1 ) : modules->module_ui_get();
			l.pop( nb_arg );		//	clean the stack for under
			if( module )
				return l.return_obj_ref( module );
		}
		return l.return_nothing();
	}


	void	register_modules( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "modules" );
			ADD_FN( get_cur				);
			ADD_FN( get_ui				);

			ADD_FN( get_module_nb_max	);
			ADD_FN( set_module_ui		);
			ADD_FN( get_module			);
			ADD_FN( new_module			);
/*
			ADD_FN( get_camera		);
			ADD_FN( get_traxs		);
			ADD_FN( get_trax		);
			ADD_FN( get_values		);
			ADD_FN( get_value		);
			ADD_FN( get_value_ref	);
*/
		l.pop( 1 );	//pop new table
	}
}	//end namespace n_modules
}	//end namespace aaalua
