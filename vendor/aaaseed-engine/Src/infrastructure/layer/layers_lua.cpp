#include "layers_lua.h"
#include "layers.h"
#include "language/lua/aaalua_glue.h"
#include "infrastructure/layer/layers_att.h"
#include "infrastructure/param/traxs.h"
#include "infrastructure/param/trax.h"
#include "ui/obj_value.h"

namespace aaalua
{
namespace n_layers
{
/*
	c_layers*	get_layers_or_cur( c_lua_state& l )
	{
		l.check_arg_nb( 0 );
		c_layers*	layers;
		if( nb_arg == 1 )
			layers	= (c_layers*) l.get_aaa_obj( 1, "layers" );
		else
		{
			layers = c_layers::cur;
			if( !layers )
				l.error_message( "can't with no current Layers" );
		}
		l.pop_all();
		return layers;
	}
*/

	//todo generalize for c_obj_ui (template)
	//	no_input
	AAALUACALL( get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_layers* obj = c_layers::get_cur();
		if( obj )
			return l.return_obj_ref( obj );
	//	l.error_message( "no current layers" );
		return l.return_nothing();
	}

	//	no_input
	AAALUACALL( get_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_layers::get_ui() );
	}

	FINLINE	c_layers*	get_layers( c_lua_state& l, INT32 index )
	{
		c_layers*	layers;
		if( index )
			layers = (c_layers*) l.get_aaa_obj_of_class( index, "layers" );
		else
			layers = c_layers::get_cur();
		return layers;
	}

	//	draw_layer_all()	-- use layers cur
	//	draw_layer_all( obj_ref or obj_name )
	AAALUACALL( draw_layer_all )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_layers*	CONST	layers	=	get_layers( l, nb_arg ? 1 : 0 );
		if( layers )
		{
			l.pop( nb_arg );		//	clean the stack for under
			layers->draw_layer_all();
		}
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name, index )
	AAALUACALL( get_layer )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST NB_ARG = 2;
		DBG_CHECK_ARG_NB( NB_ARG );
		c_layers*	layers	=	get_layers( l, 1 );
		if( layers )
		{
			INT32	index	=	l.get_int32( 2 );
			l.pop( NB_ARG );		//	clean the stack for under
			c_layer* layer = layers->layer_get_from_index( index-1 );
			if( layer )
				return l.return_obj_ref( layer );
		}
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name )
	AAALUACALL( draw_fbo_and_cam )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_layers*	CONST	layers	=	get_layers( l, nb_arg ? 1 : 0 );
		if( layers )
		{
			l.pop( nb_arg );		//	clean the stack for under
			layers->draw_fbo_and_cam();
		}
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name )
	AAALUACALL( draw_begin )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_layers*	CONST	layers	=	get_layers( l, nb_arg ? 1 : 0 );
		if( layers )
		{
			l.pop( nb_arg );		//	clean the stack for under
			layers->draw_begin();
		}
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name )
	AAALUACALL( draw_end )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_layers*	CONST	layers	=	get_layers( l, nb_arg ? 1 : 0 );
		if( layers )
		{
			l.pop( nb_arg );		//	clean the stack for under
			layers->draw_end();
		}
		return l.return_nothing();
	}
	//	input	( obj_ref or obj_name )
	AAALUACALL( skip_rest )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_layers*	CONST	layers	=	get_layers( l, nb_arg ? 1 : 0 );
		if( layers )
		{
			l.pop( nb_arg );		//	clean the stack for under
			layers->skip_rest();
		}
		return l.return_nothing();
	}

	AAALUACALL( push_attrib )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb_escape_no( 0 );
		c_layers_att::push();
		return l.return_nothing();
	}

	AAALUACALL( pop_attrib )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb_escape_no( 0 );
		c_layers_att::pop();
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name, index )
	AAALUACALL( get_camera )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
		c_layers*	CONST	layers	=	get_layers( l, 1 );
		if( layers )
		{
			if( nb_arg==2 )
			{
				INT32	index	=	l.get_int32( 2 );
				l.pop( nb_arg );		//	clean the stack for under
				if( INSIDE_MIN_MAX( index, 0, c_layers::CAMERA_NB-1 ) )
				{
					c_seedcam*	cam = layers->get_camera( index );
					return l.return_obj_ref( cam );
				}
				else
					l.error_message( "layers camera should be in range [0,%d]", c_layers::CAMERA_NB-1 );
			}
			else
			{
				c_seedcam*	cam = layers->get_camera_used();
				return l.return_obj_ref( cam );
			}
		}
		else
			l.error_message( "no current layers" );
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name or nil )
	AAALUACALL( get_traxs )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_layers*	CONST	layers	=	get_layers( l, nb_arg ? 1 : 0 );
		if( layers )
		{
			return l.return_obj_ref( layers->get_traxs() );
		}
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name, index )
	AAALUACALL( get_trax )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_layers*	layers	=	get_layers( l, 1 );
		if( layers )
		{
			auto traxs =  layers->get_traxs();
			if( traxs )
				return l.return_obj_ref( traxs->get_trax( l.get_int32(2) - 1 ) );
		}
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name or nil )
	AAALUACALL( get_values )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_layers*	CONST	layers	=	get_layers( l, nb_arg ? 1 : 0 );
		if( layers )
			return l.return_obj_ref( layers->get_values() );
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name, index )
	AAALUACALL( get_value )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_layers*	layers	=	get_layers( l, 1 );
		if( layers )
		{
			auto values =  layers->get_values();
			if( values )
				return l.return_real( values->get_from_index( l.get_int32(2) - 1 ) );	//todo better
		}
		return l.return_nothing();
	}

	//	input	( obj_ref or obj_name, index )
	AAALUACALL( get_value_ref )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_layers*	layers	=	get_layers( l, 1 );
		if( layers )
		{
			auto values =  layers->get_values();
			if( values )
			{
				INT32	index	=	l.get_int32( 2 );
				p_param param	=	values->get_value_param( index-1 );
				UINT32	ref		=	c_obj_ui::get_param_ref( param );
				return l.return_uint32( ref );
			}
		}
		return l.return_nothing();
	}

	void	register_layers( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "layers" );
			ADD_FN( get_cur				);
			ADD_FN( get_ui				);
			ADD_FN( draw_fbo_and_cam	);

			ADD_FN( draw_begin			);
			ADD_FN( draw_layer_all		);
			ADD_FN( draw_end			);
			ADD_FN( skip_rest			);

			ADD_FN( push_attrib			);
			ADD_FN( pop_attrib			);
			ADD_FN( get_layer			);
			ADD_FN( get_camera			);
			ADD_FN( get_traxs			);
			ADD_FN( get_trax			);
			ADD_FN( get_values			);
			ADD_FN( get_value			);
			ADD_FN( get_value_ref		);
		l.pop( 1 );	//pop new table
	}
}	//end namespace n_layers
}	//end namespace aaalua
