#include "lights_lua.h"
#include "language/lua/aaalua_glue.h"
#include "lights.h"

namespace aaalua
{
namespace n_lights
{
	//	no_input
	AAALUACALL( get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_lights::get_cur() , "no current lights" );
	}
	//	no_input
	AAALUACALL( get_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_lights::get_ui(), "no ui lights" );
	}

	AAALUACALL(	get_light )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		INT32 id = l.get_int32(1);
		return l.return_obj_ref( c_light::get_light( id-1 ), "no light" );
	}

	//	
	void	register_lights( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.define_table( "lights" );

			ADD_FN( get_cur );
			ADD_FN( get_ui );
			ADD_FN( get_light );

		l.pop( 1 );	//pop new table
	}
}	//end namespace n_lights
}	//end namespace aaalua
