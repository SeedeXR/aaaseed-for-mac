#include "deformer_lua.h"
#include "language/lua/aaalua_glue.h"
#include "def_node.h"


namespace aaalua
{
namespace n_deformer
{

	//	no_input
	AAALUACALL( get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_def_node::get_cur(), "no current deformer" );
	}
	//	no_input
	AAALUACALL( get_prev )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_def_node::get_prev(), "no prev deformer" );
	}
	//	no_input
	AAALUACALL( get_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_def_node::get_ui(), "no ui deformer" );
	}
	//	x,y,z = aaa.deformer.apply_xyz( deformer, x,y,z )
	AAALUACALL( apply_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		c_obj_ui* CONST	obj		= l.get_aaa_obj( 1 );
		if( obj->is_inherited_from( "deformer" ) )
		{
			REAL vec[3];
			l.get_v3( vec, 2 );
			((c_deformer*)(obj))->apply( vec, vec, 1 );
			l.push_real_v3(	vec	);
			return 3;
		}
		return l.return_nothing();
	}

	//	
	void	register_deformer( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.define_table( "deformer" );

			ADD_FN( get_cur		);
			ADD_FN( get_prev	);
			ADD_FN( get_ui		);
			ADD_FN( apply_xyz	);

		l.pop( 1 );	//pop new table
	}
}	//end namespace n_deformer
}	//end namespace aaalua