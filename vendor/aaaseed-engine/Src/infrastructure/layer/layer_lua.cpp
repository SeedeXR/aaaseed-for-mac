#include "infrastructure/layer/layer_lua.h"
#include "language/lua/aaalua_glue.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layer_att.h"


namespace aaalua
{
namespace n_layer
{
	//	no_input
	AAALUACALL( get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_layer::get_cur(), "no current layer" );
	}

	//	no_input
	AAALUACALL( get_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_layer::get_ui() );
	}

	AAALUACALL( get_bdd )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_layer*	layer	= l.get_aaa_obj_of_class<c_layer>( 1 );
		c_bdd*		bdd		= layer->get_bdd();
		if( bdd )
			return l.return_obj_ref( bdd );
		return l.return_nothing();
	}

	AAALUACALL( get_bdd_prev )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_layer*	layer	= l.get_aaa_obj_of_class<c_layer>( 1 );
		c_bdd*		bdd		= layer->get_bdd_prev();
		if( bdd )
			return l.return_obj_ref( bdd );
		return l.return_nothing();
	}

	AAALUACALL( push_attrib )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 0 );
		c_layer_att::push();
		return l.return_nothing();
	}

	AAALUACALL( pop_attrib )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 0 );
		c_layer_att::pop();
		return l.return_nothing();
	}

	AAALUACALL( set_bdd_external )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_layer*	layer	= l.get_aaa_obj_of_class<c_layer>( 1 );
		c_bdd*		bdd		= (c_bdd *)		l.get_aaa_obj_derived_from( 2, "bdd" );
		layer->set_bdd_external( bdd );
		return l.return_nothing();
	}

	AAALUACALL( update_draw_shader_bdd )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_layer*	layer	= l.get_aaa_obj_of_class<c_layer>( 1 );
		layer->update_draw_shader_bdd();
		return l.return_nothing();
	}

	void	register_layer( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "layer" );

			ADD_FN( get_cur					);
			ADD_FN( get_ui					);
			ADD_FN( get_bdd					);
			ADD_FN( get_bdd_prev			);
			ADD_FN( push_attrib				);
			ADD_FN( pop_attrib				);
			ADD_FN( set_bdd_external		);
			ADD_FN( update_draw_shader_bdd	);

		l.pop( 1 );	//pop new table
	}
}	//end namespace n_layer
}	//end namespace aaalua
