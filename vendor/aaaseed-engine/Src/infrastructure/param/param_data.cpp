#include "param_data.h"
#include "param_declare.h"
#include "err.h"
#include <stdlib.h>
#include "infrastructure/obj/obj_ui.h"
#include "infrastructure/factory/factory_group.h"


namespace
{
	UINT32	nb				= 0;
	UINT32	nb_created		= 0;
	UINT32	nb_created_copy	= 0;
	void inc_count()
	{
		++nb;
		++nb_created;
	}
}

UINT32*	c_param_data::get_nb_pt()				{	return &nb;					}
UINT32*	c_param_data::get_nb_created_pt()		{	return &nb_created;			}
UINT32*	c_param_data::get_nb_created_copy_pt()	{	return &nb_created_copy;	}

void	c_param_data::set_limits( REAL CONST def, REAL CONST ina, REAL CONST min, REAL CONST max )
{
	set_def( def );
	set_ina( ina );
	set_min( min );
	set_max( max );
}

void	c_param_data::set( c_param_def CONST * CONST def )
{
	_type_and_flag = def->get_type_and_flag();	//	we don't use set_type() here because we transfert all, not only type

	set_name( def->get_name() );
	set_limits( def->get_def(), def->get_ina(), def->get_min(), def->get_max() );

	_fn_update = def->get_fn_update();
	_symbolic_str = def->get_symbolic_str();

#if	AAA_DEBUG()
	//	if( !_fn_update )
	//		debug_break( "Non null update fn" );
	mem::is_pointer_valid_or_null( _fn_update );
	mem::is_pointer_valid_or_null( _symbolic_str );
#endif
}

c_param_data::~c_param_data()
{
	if( nb > 0 )
		--nb;
	else
		debug_break( "%s() destructor called with param_data count already at zero", __FUNCTION__ );
}

c_param_data::c_param_data()
	:_type_and_flag( TYPE_NONE )
	,_fn_update( nullptr )
	,_symbolic_str( nullptr )
{
	inc_count();
	set_limits( 0.,0., 0.,0. );
}

c_param_data::c_param_data( c_param_data CONST * CONST pd )
	:_type_and_flag( pd->_type_and_flag )
	,_name( pd->_name )
	,_fn_update( pd->_fn_update )
	,_symbolic_str( pd->_symbolic_str )
{
	inc_count();
	++nb_created_copy;
	set_limits( pd->_def, pd->_ina, pd->_min, pd->_max );
//	DBG_PRINT_STRING( "c_param_data size %d", sizeof(c_param_data) );
}

//todo c_param_data and c_param_def contain the same information and should be merged 
c_param_data::c_param_data( c_param_def CONST * CONST def )
	//:_type_and_flag( def->_type_and_flag )
	//,_name( def->_name )
	//,_min( def->_min )
	//,_max( def->_max )
	//,_def( def->_def )
	//,_ina( def->_ina )
	//,_fn_update( def->_fn_update )
	//,_symbolic_str( def->_symbolic_str )
{
	inc_count();
	set( def );
}

#if 0
//	copy constructor
c_param_data::c_param_data( CONST c_param_data& obj )
{
	debug_break( "c_param copy constructor unimplemented" );
	//set( obj.data, obj.len, obj.line_nb );
}
//	copy assignment operator
c_param_data& c_param_data::operator=( CONST c_param_data& obj )
{
	if( this == &obj )
		return *this;
	debug_break( "c_param copy assignment operator unimplemented" );
	//set( obj.data, obj.len, obj.line_nb );
	return *this;
}
#endif

#if AAA_DEBUG()
void	c_param_data::build_symbolic_index_valid(	INT32& index	)	CONST
{
	if( index < 0 )
	{
		debug_break( "%s() param symbolic %s : index %d < 0. Forcing to 0", __FUNCTION__, _name.get(), index );
		index = 0;
	}
	else
	{
		auto CONST type = get_type();
		if( type == TYPE_SYMBO_NEG )
		{
			INT32 max = - INT32(get_min()) - 1;
			if( max < index )
			{
				debug_break( "%s() param symbolic %s : index %d >= %d (the -min-1 value). Forcing to %d", __FUNCTION__, _name.get(), index, max, max );
				index = max;
			}
		}
		else if( type == TYPE_SYMBO_ZERO )
		{
			INT32 max = - INT32(get_min());
			if( max < index )
			{
				debug_break( "%s() param symbolic %s : index %d >= %d (the -min value). Forcing to %d", __FUNCTION__, _name.get(), index, max, max );
				index = max;
			}
		}
		else
		{
			INT32 max = INT32(get_max());
			if( max < index )
			{			
				debug_break( "%s() param symbolic %s : index %d > %d (the max value). Forcing to %d", __FUNCTION__, _name.get(), index, max, max );
				index = max;
			}
		}
	}
}
#endif //AAA_DEBUG


static	FINLINE	INT32	find_str( C_PCHAR pat, C_PCHAR_C* array, INT32 nb )
{
	if( array )
	{
		for( INT32 i = 0; i < nb; ++i )
		{
			if( str_is_equal_bothnocase( pat, array[i] ) )
				return i;
		}
	}
	return -1;
}

//	Common error path when a symbolic / class-branch string lookup fails.
//	Reports the offending value (with the loading object context if any) and
//	returns a fallback parsed via atol so the caller still gets a numeric value.
static	INT32	report_unknown_value_and_fallback( C_PCHAR_C kind, C_PCHAR_C param_name, C_PCHAR_C str )
{
	CHAR	buf[ 256 + 128 + 128 + 128 ];
	CHAR*	pt = buf;
	c_obj_ui* obj_loading = c_param::get_obj_loading();
	if( obj_loading )
		pt += sprintf( pt, "In Object %.256s\n", obj_loading->get_my_filename() );
	sprintf( pt, "Param %s \"%.128s\" don't recognize %.128s as value", kind, param_name, str );
	ERR_PRINT_STRING( buf );
	BOX_ERR( buf );
	return atol( str );
}

INT32	c_param_data::get_value_from_symbo_str( C_PCHAR str ) CONST
{
	INT32	value;
	switch( get_type() )
	{
	case TYPE_CLASS_BRANCH:
		value = get_factory_group()->get_index_from_str(str);
		if( value < 0 )
			value = report_unknown_value_and_fallback( "Class Branch", _name.get(), str );
		break;
	case TYPE_SYMBOLIC:
		{
			bool b_syno = is_syno();
			INT32 nb = INT32(_max) + 1;
			value = find_str( str, _symbolic_str, b_syno ? nb*2 : nb );
			if( value >= 0 )
			{
				if( b_syno && value >= nb )
					value -= nb;
			}
			else
				value = report_unknown_value_and_fallback( "symbolic", _name.get(), str );
		}
		break;
	case TYPE_SYMBO_NEG:
		value = find_str( str, _symbolic_str, -INT32(_min) );
		if( value >= 0 )	// str found
			value = -value-1;
		else
			value = atol(str);
		break;
	case TYPE_SYMBO_ZERO:
		value = find_str( str, _symbolic_str, -INT32(_min)+1 );
		if( value >= 0 )	// str found
			value = -value;
		else
			value = atol(str);
		break;
	default:
		value = 0;
		debug_break( "nothing to do here" );
		break;
	}
	return value;
}
