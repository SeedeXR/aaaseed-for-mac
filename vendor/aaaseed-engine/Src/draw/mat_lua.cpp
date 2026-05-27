#include "mat_lua.h"
#include "language/lua/aaalua_glue.h"
#include "mat.h"


namespace aaalua
{
namespace n_materials
{
	//	no_input
	AAALUACALL( get_front_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_materials::get_cur()->get_material_front(), "no current material" );
	}
	//	no_input
	AAALUACALL( get_back_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_materials::get_cur()->get_material_back(), "no current material" );
	}
	//	no_input
	AAALUACALL( get_front_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_materials::get_ui()->get_material_front(), "no ui material" );
	}
	AAALUACALL( get_back_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_materials::get_ui()->get_material_back(), "no ui material" );
	}

	/*AAALUACALL(	get_light )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		if( !c_materials::cur )
			l.error_message( "no current lights" );

		else
		{
			INT32 id = l.get_int32(1);
			if( INSIDE_MIN_MAX( id, 1, 8 ) )
				return l.return_obj_ref( c_materials::cur->get_light(id-1), "no light" );
		}
		return l.return_nothing();
	}*/

	//	
	void	register_material( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.define_table( "material" );

			ADD_FN( get_front_cur );
			ADD_FN( get_back_cur );
			ADD_FN( get_front_ui );
			ADD_FN( get_back_ui );
			//ADD_FN( get_light );

		l.pop( 1 );	//pop new table
	}
}	//end namespace n_materials
}	//end namespace aaalua