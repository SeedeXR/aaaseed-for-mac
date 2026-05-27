#include "module_lua.h"
#include "module.h"
#include "language/lua/aaalua_glue.h"

namespace aaalua
{
namespace n_module
{
	//	no_input
		//	no input

	AAALUACALL( get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_module::get_cur(), "no current module" );
	}

	//	no_input
	AAALUACALL( get_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_module::get_ui() );
	}

	/*
	FINLINE	c_layers*	get_layers( c_lua_state& l, INT32 index )
	{
		c_layers*	layers;
		if( index )	{	layers = (c_layers*) l.get_aaa_obj_of_class( index, "layers" );	}
		else		{	layers = c_layers::get_cur();	}
		return layers;
	}


	*/

	void	register_module( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "module" );
			ADD_FN( get_cur			);
			ADD_FN( get_ui			);
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
}	//end namespace n_module
}	//end namespace aaalua
