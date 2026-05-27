#include "param_lua.h"
#include "param.h"
#include "language/lua/aaalua_glue.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif

namespace aaalua
{
namespace n_param
{
	static INT32 get_ref_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST	obj		= l.get_aaa_obj( -2 );

		C_PCHAR_C		name	= l.get_str();
		p_param			param	= obj->get_param_by_name( name );
		if( param )
		{
			UINT32	ref = c_obj_ui::get_param_ref( param );
			return	l.return_uint32( ref );
		}
		if( b_error )
			l.error_and_escape( "can't find param %.64s in object %.128s", name, c_lua_state::get_obj_debug_str(obj) );
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name), param_name
	AAALUACALL(	get_ref )			{	return get_ref_low( L, __FUNCTION__, true );	}
	AAALUACALL(	get_ref_no_error )	{	return get_ref_low( L, __FUNCTION__, false );	}

	static INT32 is_ref_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 1 );
		if( l.is_number() )
		{
			UINT32 CONST ref = l.get_value_direct<UINT32>();
			if( c_obj_ui::is_param_ref( ref ) )
				return l.return_bool( true );
			else
			{
				if( b_error )
					l.error_message( "%x is not a param ref", ref );
			}
		}
		else
		{
			if( b_error )
				l.error_message( "param ref should be a number" );
		}
		return l.return_bool( false );
	}

	AAALUACALL( is_ref )			{	return is_ref_low( L, __FUNCTION__, true  );	}
	AAALUACALL( is_ref_no_error )	{	return is_ref_low( L, __FUNCTION__, false );	}

	static INT32 get_index_low( lua_State* L, C_PCHAR_C fn_name, bool b_error )
	{
		LUAAAA_START( L, fn_name );
		l.check_arg_nb( 2 );
		c_obj_ui* CONST	obj		= l.get_aaa_obj( -2 );

		C_PCHAR_C		name	= l.get_str();
		INT32 CONST		index	= obj->get_param_index_by_name( name );
		if( index>=0 )
			return	l.return_int32( index );
		if( b_error )
			l.error_and_escape( "can't find param %.64s in object %.128s", name, c_lua_state::get_obj_debug_str(obj) );
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name), param_name
	AAALUACALL( get_index )				{ return get_index_low( L, __FUNCTION__, true ); }
	AAALUACALL( get_index_no_error )	{ return get_index_low( L, __FUNCTION__, false ); }

	AAALUACALL( get_nb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui* CONST	obj		= l.get_aaa_obj( 1 );
		return	l.return_int32( obj->get_param_nb_allocated() );	//param
	}

	AAALUACALL( get_type )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST			nb_arg	= l.get_arg_nb( 1, 2 );
		c_param	CONST * CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_string( param->get_type_str_lowercase() );
	}

	//	input	param_ref							value
	//	or		(obj_ref or obj_name) param_name	value
	AAALUACALL( set )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 2, 3 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg-1 );

		//todo deal with this case		TYPE_BIT32,
		//todo use type _internal
		switch( param->get_type() )
		{
		case ::aaa::param::TYPE_CLASS_BRANCH:
		case ::aaa::param::TYPE_SYMBOLIC:	//refine
		case ::aaa::param::TYPE_SYMBO_NEG:	//refine
		case ::aaa::param::TYPE_SYMBO_ZERO:	//refine
			switch( l.get_type(-1) )
			{
			case LUA_TNIL:		break;	//nil we don't change the value
			case LUA_TSTRING:	param->set_value_from_str( lua_tostring(L,-1) );			break;
			case LUA_TBOOLEAN:	param->set_value_num_from_bool( lua_toboolean(L,-1)==1 );	break;	//todo better ?
			default:			param->set_value_num_from_double( lua_tonumber(L,-1) );		break;
			}
			break;
		case ::aaa::param::TYPE_BOOL:				//refine (eg. ON OFF)
		case ::aaa::param::TYPE_INT32:
		case ::aaa::param::TYPE_UINT32:
		case ::aaa::param::TYPE_BIT32:
		case ::aaa::param::TYPE_DOUBLE:
		case ::aaa::param::TYPE_FP32:
		case ::aaa::param::TYPE_TIMECODE:			//todo is it a sub type ?
			switch( l.get_type(-1) )
			{
			case LUA_TNIL:		break;	//nil we don't change the value
			case LUA_TBOOLEAN:	param->set_value_num_from_bool( lua_toboolean(L,-1)==1 );	break;	//todo better ?
			default:			param->set_value_num_from_double( lua_tonumber(L,-1) );	break;
			}
			break;
		default:
		case ::aaa::param::TYPE_REF:
		case ::aaa::param::TYPE_STR:
		case ::aaa::param::TYPE_FILENAME:
		case ::aaa::param::TYPE_DIRNAME:
			param->set_value_str( (C_PCHAR_C) l.get_str() );
			break;
		}

		return l.return_nothing();
	}
	
	FINLINE void set_param_index( c_lua_state& l, CONST p_param p, INT32 CONST index )
	{
		DOUBLE CONST val = l.get_value_direct<DOUBLE>( index );
		if( val != 0. || l.is_number(index) )
		{
			p->set_value_num_from_double( val );	//todo deal with str types
			return;
		}
		if( l.is_nil(index) )
			return;
		l.error_and_escape( "no number at position %d but a %s", index, l.get_type_str( l.get_type(index) ) );		
	}
//todo implement table methods 
	//AAALUACALL( set_real_table )
	//{
	//	LUAAAA_START( L, __FUNCTION__ );
	//	//l.get_arg_nb( 2, 3 );
	//	p_param CONST	param = l.get_param( 1 ) );
	//	set_param_index( l, param, 2 );
	//	return l.return_nothing();
	//}

	AAALUACALL( set_real )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.get_arg_nb( 2, 3 );
		p_param CONST	param = l.get_param( 1 );
		set_param_index( l, param, 2 );
		return l.return_nothing();
	}
	AAALUACALL( set_real_2 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.get_arg_nb( 2, 3 );
		p_param			param = l.get_param( 1 );
		set_param_index( l,	param,	 2 );
		set_param_index( l, ++param, 3 );
		return l.return_nothing();
	}
	AAALUACALL( set_real_3 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.get_arg_nb( 2, 3 );
		p_param			param = l.get_param( 1 );
		set_param_index( l,	param,	 2 );
		set_param_index( l, ++param, 3 );
		set_param_index( l, ++param, 4 );
		return l.return_nothing();
	}
	AAALUACALL( set_real_4 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.get_arg_nb( 2, 3 );
		p_param			param = l.get_param( 1 );
		set_param_index( l,	param,	 2 );
		set_param_index( l, ++param, 3 );
		set_param_index( l, ++param, 4 );
		set_param_index( l, ++param, 5 );
		return l.return_nothing();
	}
	AAALUACALL( set_real_5 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.get_arg_nb( 2, 3 );
		p_param			param = l.get_param( 1 );
		set_param_index( l,	param,	 2 );
		set_param_index( l, ++param, 3 );
		set_param_index( l, ++param, 4 );
		set_param_index( l, ++param, 5 );
		set_param_index( l, ++param, 6 );
		return l.return_nothing();
	}
	AAALUACALL( set_real_6 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.get_arg_nb( 2, 3 );
		p_param			param = l.get_param( 1 );
		set_param_index( l,	param,	 2 );
		set_param_index( l, ++param, 3 );
		set_param_index( l, ++param, 4 );
		set_param_index( l, ++param, 5 );
		set_param_index( l, ++param, 6 );
		set_param_index( l, ++param, 7 );
		return l.return_nothing();
	}
	AAALUACALL( set_real_7 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.get_arg_nb( 2, 3 );
		p_param			param = l.get_param( 1 );
		set_param_index( l,	param,	 2 );
		set_param_index( l, ++param, 3 );
		set_param_index( l, ++param, 4 );
		set_param_index( l, ++param, 5 );
		set_param_index( l, ++param, 6 );
		set_param_index( l, ++param, 7 );
		set_param_index( l, ++param, 8 );
		return l.return_nothing();
	}
	AAALUACALL( set_real_8 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.get_arg_nb( 2, 3 );
		p_param			param = l.get_param( 1 );
		set_param_index( l,	param,	 2 );
		set_param_index( l, ++param, 3 );
		set_param_index( l, ++param, 4 );
		set_param_index( l, ++param, 5 );
		set_param_index( l, ++param, 6 );
		set_param_index( l, ++param, 7 );
		set_param_index( l, ++param, 8 );
		set_param_index( l, ++param, 9 );
		return l.return_nothing();
	}

	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_comment )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST			nb_arg	= l.get_arg_nb( 1, 2 );
		c_param	CONST * CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_string( *(param->get_comment()) );
	}

	//	input	param_ref							value
	//	or		(obj_ref or obj_name) param_name	value
	AAALUACALL( set_comment )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb( 1, 2, 3 );

		if( l.is_nil( 1 ) )
			l.error_and_escape( "param ref is nil" );

		p_param param = l.get_param( 1 );
		UINT32 index;
		if( !param )
		{
			param = l.get_aaa_param_from_name( 1 );
			index = 3;
		}
		else
			index = 2;
		param->set_comment( (CHAR*) l.get_str_or_nil(index) );
		return l.return_nothing();
	}

	namespace{
		thread_local o_str o_buf_get;
	} 
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param CONST	param	= l.get_aaa_param_from_stack( nb_arg );

		//todo use type _internal
		switch( param->get_type() )
		{
		case ::aaa::param::TYPE_STR:
		case ::aaa::param::TYPE_FILENAME:
		case ::aaa::param::TYPE_DIRNAME:
		case ::aaa::param::TYPE_REF:
			param->get_value_as_str( o_buf_get );
			return l.return_string( o_buf_get );
		//case ::aaa::param::TYPE_BOOL:			//refine (eg. ON OFF)
		case ::aaa::param::TYPE_CLASS_BRANCH:	//refine
		case ::aaa::param::TYPE_SYMBOLIC:		//refine
		case ::aaa::param::TYPE_SYMBO_NEG:		//refine
		case ::aaa::param::TYPE_SYMBO_ZERO:		//refine
		case ::aaa::param::TYPE_INT32:
			return l.return_int32( param->get_value_as_int32() );
		case ::aaa::param::TYPE_BIT32:
		case ::aaa::param::TYPE_UINT32:
			return l.return_uint32( param->get_value_as_uint32() );
		//case ::aaa::param::TYPE_TIMECODE:	//refine INT ?
		//case ::aaa::param::TYPE_DOUBLE:		//refine real not enough
		default:
			return l.return_double( param->get_value_as_double() );
		}

		return l.return_nothing();
	}

		//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_real )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg = l.get_arg_nb( 1, 2 );
		p_param	CONST	param = l.get_aaa_param_from_stack( nb_arg );
		return l.return_double( param->get_value_as_double() );
	}
	AAALUACALL( get_real_2 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param			param	= l.get_aaa_param_from_stack( nb_arg );
		l.push_double( param->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		return 2;
	}
	AAALUACALL( get_real_3 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param			param	= l.get_aaa_param_from_stack( nb_arg );
		l.push_double( param->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		return 3;
	}
	AAALUACALL( get_real_4 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param			param	= l.get_aaa_param_from_stack( nb_arg );
		l.push_double( param->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		return 4;
	}
	AAALUACALL( get_real_5 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param			param	= l.get_aaa_param_from_stack( nb_arg );
		l.push_double( param->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		return 5;
	}
	AAALUACALL( get_real_6 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param			param	= l.get_aaa_param_from_stack( nb_arg );
		l.push_double( param->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		return 6;
	}
	AAALUACALL( get_real_7 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param			param	= l.get_aaa_param_from_stack( nb_arg );
		l.push_double( param->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		return 7;
	}
	AAALUACALL( get_real_8 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param			param	= l.get_aaa_param_from_stack( nb_arg );
		l.push_double( param->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		l.push_double( (++param)->get_value_as_double() );
		return 8;
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( flip )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );

		bool CONST b = !param->get_value_as_bool();
		param->set_value_num_from_bool( b );	
		return l.return_bool( b );
	}

	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_bool )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_bool( param->get_value_as_bool() );
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_int32 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_int32( param->get_value_as_int32() );
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_uint32 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_uint32( param->get_value_as_uint32() );
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_str )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		param->get_value_as_str( o_buf_get );
		return l.return_string( o_buf_get );
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_str_lower )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		param->get_value_as_str( o_buf_get );
		o_buf_get.convert_to_lowercase();
		return l.return_string( o_buf_get );
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_str_upper )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		param->get_value_as_str( o_buf_get );
		o_buf_get.convert_to_uppercase();
		return l.return_string( o_buf_get );
	}

	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( is_type_real )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_bool( param->is_type_real() );
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( is_type_bool )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_bool( param->is_type_bool() );
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( is_type_number )
	{
		LUAAAA_START(L, __FUNCTION__);
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_bool( param->is_type_number() );
	}
	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( is_type_text )
	{
		LUAAAA_START(L, __FUNCTION__);
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_bool( param->is_type_text() );
	}

	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_min )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_real( param->get_min() );
	}

	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_max )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_real( param->get_max() );
	}

	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_def )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_real( param->get_def() );
	}

	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_ina )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_real( param->get_ina() );
	}

	//	input	param_ref	or	(obj_ref or obj_name) param_name
	AAALUACALL( get_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_string( param->get_name() );
	}

	AAALUACALL( set_min )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST nb_arg	= l.get_arg_nb( 2, 3 );
		p_param	CONST param		= l.get_aaa_param_from_stack( nb_arg-1 );
		REAL	CONST val		= l.get_real( -1 );		//todo deal with DOUBLE one day
		param->set_min( val );
		return l.return_nothing();
	}
	AAALUACALL( set_max )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST nb_arg	= l.get_arg_nb( 2, 3 );
		p_param	CONST param		= l.get_aaa_param_from_stack( nb_arg-1 );
		REAL	CONST val		= l.get_real( -1 );		//todo deal with DOUBLE one day
		param->set_max( val );
		return l.return_nothing();
	}
	AAALUACALL( set_def )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST nb_arg	= l.get_arg_nb( 2, 3 );
		p_param	CONST param		= l.get_aaa_param_from_stack( nb_arg-1 );
		REAL	CONST val		= l.get_real( -1 );		//todo deal with DOUBLE one day
		param->set_def( val );
		return l.return_nothing();
	}
	AAALUACALL( set_ina )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST nb_arg	= l.get_arg_nb( 2, 3 );
		p_param	CONST param		= l.get_aaa_param_from_stack( nb_arg-1 );
		REAL	CONST val		= l.get_real( -1 );		//todo deal with DOUBLE one day
		param->set_ina( val );
		return l.return_nothing();
	}

	AAALUACALL( is_save )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_bool( param->is_save() );
	}
	AAALUACALL( set_save )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 2, 3 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg-1 );
		bool CONST	b_save	= l.get_bool( nb_arg );
		param->set_save( b_save );
		return l.return_nothing();
	}


	AAALUACALL( get_obj )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param		param	= l.get_aaa_param_from_stack( nb_arg );
		c_obj_ui*	obj		= param->get_obj_owner();
		if( obj )
			return l.return_obj_ref( obj );
		l.error_and_escape( "param %.128s don't belong to an object", param->get_name() );
		return l.return_nothing();
	}

	AAALUACALL( get_obj_attached )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		c_obj_ui* CONST	obj		= param->get_obj_attached();
		if( obj )
			return l.return_obj_ref( obj );
		return l.return_nothing();
	}

	AAALUACALL( do_action_open )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		p_param	CONST	param	= l.get_aaa_param_from_stack( nb_arg );
		return l.return_bool( ui::param_do_action( param, ::aaa::param::ACTION::PARAM_OPEN ) );
//		return l.return_bool( obj->param_do_action( param, aaa::param::ACTION::PARAM_OPEN ) );
	}


	void	register_param( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table_global_get( "param" );
			ADD_FN( is_ref				);
			ADD_FN( is_ref_no_error		);

			ADD_FN( get_ref				);
			ADD_FN( get_ref_no_error	);

			ADD_FN( get_index			);
			ADD_FN( get_index_no_error	);

			ADD_FN( get_nb				);

			ADD_FN( get_type		);
			ADD_FN( is_type_real	);
			ADD_FN( is_type_bool	);
			ADD_FN( is_type_number	);
			ADD_FN( is_type_text	);

			ADD_FN( get_comment );
			ADD_FN( set_comment );

			ADD_FN( set );
			ADD_FN( set_real );
			ADD_FN( set_real_2 );
			ADD_FN( set_real_3 );
			ADD_FN( set_real_4 );
			ADD_FN( set_real_5 );
			ADD_FN( set_real_6 );
			ADD_FN( set_real_7 );
			ADD_FN( set_real_8 );

			ADD_FN( get );
			ADD_FN( get_real );
			ADD_FN( get_real_2 );
			ADD_FN( get_real_3 );
			ADD_FN( get_real_4 );
			ADD_FN( get_real_5 );
			ADD_FN( get_real_6 );
			ADD_FN( get_real_7 );
			ADD_FN( get_real_8 );

			ADD_FN( flip		);
			ADD_FN( get_bool	);
			ADD_FN( get_int32	);
			ADD_FN( get_uint32	);

			ADD_FN( get_str			);
			ADD_FN( get_str_lower	);
			ADD_FN( get_str_upper	);

			ADD_FN( get_min		);
			ADD_FN( get_max		);
			ADD_FN( get_def		);
			ADD_FN( get_ina		);
			ADD_FN( get_name	);

			ADD_FN( set_min		);
			ADD_FN( set_max		);
			ADD_FN( set_def		);
			ADD_FN( set_ina		);

			ADD_FN( is_save		);
			ADD_FN( set_save	);

			ADD_FN( get_obj );
			ADD_FN( get_obj_attached );

			ADD_FN( do_action_open );
//removed 2023 May			l.set_field(	"infini",	 ::aaa::param::PARAM_MAX_REAL );

		l.pop( 1 );	//pop new table
	}
}	//end namespace param
}	//end namespace aaalua
