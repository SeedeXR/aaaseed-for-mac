#include "aaa_def.h"
#include <cmath>		// std::isnan (replaces MSVC-only _isnan)
#include "param.h"
#include "param_focus.h"
#include "infrastructure/obj/command.h"
#include "file/file_io.h"
#if	AAA_CHECKSUM_PARAM_DO()
#	include "checksum.h"
	c_checksum checksum_param;
#endif
#include "infrastructure/param/trax.h"
#include "infrastructure/param/traxs.h"
#include "infrastructure/factory/factory_group.h"
#include "infrastructure/obj/node_list_ui.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "spy.h"
#include "ui/seed_ui.h"
#include "file/aaa_dir.h"
#include "file/dir_pool.h"


//todo definitively decide what we do for REAL / FP32 / DOUBLE... 
//todo split in several src

#if	AAA_DEBUG()
	INT32	c_param::s_check = 0;
#endif

c_obj_ui*	c_param::obj_loading = nullptr;

//par	add copy and equality operator to c_param or the structure who will replace it
bool c_param::b_verbose = false;
bool c_param::b_list_load_verbose = false;

namespace
{
	UINT32	nb = 0;
	UINT32	nb_created = 0;
	void inc_count()
	{
		++nb;
		++nb_created;
	}
}
UINT32*	c_param::get_nb_pt()			{ return &nb; }
UINT32*	c_param::get_nb_created_pt()	{ return &nb_created; }

c_param::c_param()
	:_pt( nullptr )
	,_flags( 0 )
	,_data( nullptr )
	,_more( nullptr )
{
	inc_count();
	//	DBG_PRINT_STRING( "c_param size %d", sizeof(c_param) );
}

#if 0
//	copy constructor
c_param::c_param( CONST c_param& obj )
{
	debug_break( "c_param copy constructor unimplemented" );
	//set( obj.data, obj.len, obj.line_nb );
	inc_count();
}
//	copy assignment operator
c_param& c_param::operator=( CONST c_param& obj )
{
	if( this == &obj )
		return *this;
	debug_break( "c_param copy assignment operator unimplemented" );
	//set( obj.data, obj.len, obj.line_nb );
	return *this;
}
#endif

c_param::~c_param()
{
	SAFE_DELETE( _more );
	if( is_data_owner() )
		SAFE_DELETE( _data );

	if( nb > 0 )
		--nb;
	else
		debug_break( "%s() destructor called with param count already at zero", __FUNCTION__ );
}

#if	AAA_DEBUG()
void c_param::check( void CONST * CONST pt ) CONST
{
	if( s_check==0 )
	{
		if( !pt )
			ERR_PRINT_STRING( "c_param::set_pt() with NULL pointer for param %s", get_name().get() );
		else
		{
			if( mem::is_pointer_fucked(pt) )
				debug_break( "c_param::bad value pointed by param" );
		}
	}
}
#endif

void	c_param::print_debug( C_PCHAR_C signature, C_PCHAR_C mess ) CONST
{
	DBG_PRINT_STRING( "%s() param %s(%s) %s ", signature, get_name().get(), aaa::param::get_type_str(get_type()), mess );
}
void	c_param::print_err( C_PCHAR_C signature, C_PCHAR_C mess ) CONST
{
	debug_break( "%s() param %s(%s) %s ", signature, get_name().get(), aaa::param::get_type_str(get_type()), mess );
}

FINLINE	void	c_param::set_int32_direct(	INT32	CONST val )	{ *((INT32  *)_pt) = val;		}
FINLINE	void	c_param::set_uint32_direct(	UINT32	CONST val )	{ *((UINT32 *)_pt) = val;		}
FINLINE	void	c_param::set_fp32_direct(	FP32	CONST val )	{ *((FP32   *)_pt) = val;		}
//FINLINE void	c_param::set_real_direct(	REAL	CONST val )	{ *((REAL   *)_pt) = val;		}
FINLINE	void	c_param::set_double_direct(	DOUBLE	CONST val )	{ *((DOUBLE *)_pt) = val;		}
FINLINE	void	c_param::set_str_direct(	C_PCHAR_C     val )	{ ((o_str	*)_pt)->set(val);	}
FINLINE	void	c_param::set_str_direct(	o_str CONST & val )	{ ((o_str   *)_pt)->set(val);	}
FINLINE	void	c_param::set_bool_direct(	bool	CONST val )	{ *((bool   *)_pt) = val;		}

void	c_param::set_id( INT32 index )
{
	//	M_ID is the 10-bit "out of range" sentinel ; valid ids are 0..M_ID-1.
	//	Negative input would sign-extend into the flag bits and corrupt them, so we clamp
	//	to the sentinel and signal the caller. Positive overflow lands on the same sentinel
	//	by design, which triggers the chain lookup in get_id().
	if( index < 0 )
	{
		debug_break( "%s() negative index %d : clamping to overflow sentinel", __FUNCTION__, index );
		index = M_ID;
	}
	else if( index > INT32(M_ID-1) )
		index = M_ID;
	_flags = (_flags & ~M_ID) | (UINT32(index) & M_ID);
}

INT32	c_param::get_id()
{
	INT32 index = _flags & M_ID;
	if( index != M_ID )
		return index;

	INT32 i_max_loop = 1024;	//avoid infinite loop when structure is corrupt 
	while( --i_max_loop )
	{
		c_param CONST * prev = this - index;
		INT32 prev_index = prev->_flags & M_ID;
		index += prev_index;
		if( prev_index != M_ID )
			return index;
	}

	print_err( __FUNCTION__, "this should not happen, param array is too big" );	
	return 0;
}

c_obj_ui*	c_param::get_obj_owner()
{	//todo qqq add some debug check here
	c_param* header = get_header();
	auto obj = header->get_obj();
#if AAA_DEBUG()
	if( mem::is_pointer_fucked(obj) )
		print_err( __FUNCTION__, "null obj owner from param should not happen. will crash soon ?" );	
#endif
	return obj;
}

void	c_param::alloc_more()
{
	_more = new c_param_more;
	if( !_more )
		print_err( __FUNCTION__, "can't allocate c_param_more. you should exit." );	
}

bool	c_param::is_pluggable() CONST
{
	auto CONST type = get_type();
	return	type != TYPE_NONE && !is_type_group(type);
}

void	c_param::set_verbose( bool b_in )
{
	b_verbose = b_in;
	SWITCH_PRINT_STATE( "Param Verbose", b_verbose );
}
void	c_param::flip_verbose()
{
	set_verbose( !b_verbose);
}

void	c_param::set_list_load_verbose( bool b_in )
{
	b_list_load_verbose = b_in;
	SWITCH_PRINT_STATE( "List load Verbose", b_list_load_verbose );
}

void	c_param::flip_list_load_verbose()
{
	set_list_load_verbose( !b_list_load_verbose );
}

//
//	FLAGS
//
void	c_param::enable_dbg_display()	{	_flags |=  M_DBG_DISPLAY; }
void	c_param::disable_dbg_display()	{	_flags &= ~M_DBG_DISPLAY; }
void	c_param::flip_dbg_display()
{
	if( is_dbg_display() )
		disable_dbg_display();
	else
		enable_dbg_display();
}

void	c_param::flip_expand_out()
{
	IF_THIS_NULL_RETURN();
	if( is_expand_out() )
		disable_expand_out();
	else
		enable_expand_out();
}

void	c_param::flip_expand_in()
{
	IF_THIS_NULL_RETURN();
	if( is_expand_in() )
		disable_expand_in();
	else
		enable_expand_in();
}
void	c_param::flip_expand_list()
{
	IF_THIS_NULL_RETURN();
	set_expand_list( !is_expand_list() );
}
void	c_param::flip_expand_all()
{
	IF_THIS_NULL_RETURN();
	if( is_expand() )
		disable_expand_all();
	else
		enable_expand_all();
}

void	c_param::disconnect()
{
	IF_THIS_NULL_RETURN();
	if( _more )
	{
		if( is_out() )	//par optimize with a is_out_nb()
			//GOOD_PRINT_STRING( "Disconnecting all traxs plugged Out to param %s", get_name() );
			_more->disconnect_out();
		else if( is_in() )
			//GOOD_PRINT_STRING( "Disconnecting all traxs plugged In to param %s", get_name() );
			_more->disconnect_in();
		else
		{
			c_obj_ui*	obj = get_obj();
			if( obj )
			{
				if( obj->is_class<c_trax>() )
					obj->unplug_in_all();
				else
				{
					if( BOX_ASK_WAR( "Confirmation", "Really disconnect all for this Obj ?" ) )
						obj->unplug_out_all();
				}
			}
		}
	}
}

//
//	FOCUS
//
namespace {
	thread_local  CHAR	str_data[1024];
	thread_local  CHAR	str_mess_to_send[1024];
}

void	c_param::send_change()
{
	c_obj_ui*	oui = focus_param::get_obj();

	if( !oui )
	{
		ERR_PRINT_STRING(  "%s() no obj_ui for param : %s ", __FUNCTION__, get_name().get() );
		return;
	}

	CHAR CONST * CONST	cname = oui->get_class_name();
	// we don't sync the net's obj (net and net_link)
	if(		*cname		==	'n'
		&&	*(cname+1)	==	'e'
		&&	*(cname+2)	==	't'
		&&	( *(cname+3) ==	0 || str_is_equal( cname+3, "_link") )
		)
		return;	

	auto CONST type = get_type();
	switch( aaa::param::get_type_internal( type ) )
	{
		//hack	TYPE_CLASS_BRANCH dangerous if index are different because of different version should send str or hash:
	case TYPE_INTERNAL_BOOL:	strcpy( str_data, get_bool() ? "1" : "0" );	break;
	case TYPE_INTERNAL_INT32:	sprintf( str_data, "%d", get_int32()	 );	break;
	case TYPE_INTERNAL_UINT32:	sprintf( str_data, "%u", get_uint32()	 );	break;
	case TYPE_INTERNAL_FP32:	sprintf( str_data, "%g", get_fp32()		 );	break;
	case TYPE_INTERNAL_DOUBLE:	sprintf( str_data, "%g", get_double()	 );	break;
	case TYPE_INTERNAL_NONE:
		*str_data = 0;
		print_debug( __FUNCTION__, "should not change" );	
		break;
	default:
		*str_data = 0;
		print_err( __FUNCTION__, "unimplemented param type" );
		break;
	}

	if( *str_data )
	{
		sprintf( str_mess_to_send, "set_param( \"%s\", %s)", get_name().get(), str_data );
		command_send( oui, str_mess_to_send);
	}
}

DOUBLE	c_param::start_value;
DOUBLE	c_param::step_value;

extern	void	param_set_action_str_last( C_PCHAR_C action_str );

//todo make it better for UINT32 for example
void	c_param::change_value_for_ui( DOUBLE in )
{
	IF_THIS_NULL_RETURN();

	bool b_need_update = true;
	auto CONST type = get_type();
	switch( type )
	{
	case TYPE_TIMECODE:
		{
			DOUBLE	val;
			FP32	min = get_min();
			FP32	max = get_max();

			if(	   min != PARAM_MIN_FP32
				&& min != PARAM_MAX_FP32
				&& max != PARAM_MIN_FP32
				&& max != PARAM_MAX_FP32
				)
			{
				val = start_value + (max - min) * in / 256.;
			}
			else
			{
				val = start_value + ABS(get_def() - get_ina()) * in / 256.;
				//was val = start_value + step_value * REAL(in);
				val = CLAMP( val, min, max );
			}
			set_fp32_direct( FP32(val) );
		}
		break;
	default:
		switch( aaa::param::get_type_internal( type ) )
		{
		case TYPE_INTERNAL_BOOL:
			set_bool_direct( in >= .5 );
			break;
		case TYPE_INTERNAL_INT32:
			{
				INT32	i_val = I_FLOOR( start_value + in / 16.) ;
				set_int32_direct( clamp(i_val) );	//todo add set_value_from_int32_type_number
			}
			break;
		case TYPE_INTERNAL_UINT32:
			{
				UINT32	i_val = I_FLOOR(start_value + in / 16.);
				set_uint32_direct( clamp(i_val) );	//todo add set_value_from_int32_type_number
			}
			break;
		case TYPE_INTERNAL_FP32:
			in = start_value + step_value * in;
			set_fp32_direct( clamp( FP32(in) ) );
			break;
		case TYPE_INTERNAL_DOUBLE:
			in = start_value + step_value * in;
			set_double_direct( clamp( in ) );
			break;
		case TYPE_INTERNAL_NONE:
			b_need_update = false;
			print_debug( __FUNCTION__, "should not change" );	
			break;
		default:
			b_need_update = false;
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}

	if( b_need_update )
	{
		send_change();
		if( !c_param::get_obj_loading() )
		{
			if( auto fn = _data->get_fn_update() )
				fn( this );
		}
		param_set_action_str_last( "change" );
	}
}

//todo make it better for UINT32 for example
//todo	should be call only when needed
DOUBLE	c_param::update_start_value()
{
	DOUBLE	ret;
	switch( get_type_internal() )
	{
	case TYPE_INTERNAL_BOOL:	ret = get_bool() ? 1. : 0.; break;
	case TYPE_INTERNAL_INT32:	ret = (DOUBLE)get_int32();	break;
	case TYPE_INTERNAL_UINT32:	ret = (DOUBLE)get_uint32();	break;
	case TYPE_INTERNAL_FP32:
		ret = get_fp32();
		if( ret == 0. )
			ret = (get_def() - get_ina()) * .01;
		break;
	case TYPE_INTERNAL_DOUBLE:
		ret = get_double();
		if( ret == 0. )
			ret = (get_def() - get_ina()) * .01;
		break;
	case TYPE_INTERNAL_STRING:
	case TYPE_INTERNAL_NONE:
		print_debug( __FUNCTION__, "have no sense for this param type" );	
		return 0.;
	default:
		print_err( __FUNCTION__, "unimplemented param type" );
		return 0.;
	}
	return CLAMP( ret, get_min(), get_max() );
}

void	c_param::get_value_as_str( o_str& o ) CONST
{
	if( is_pt() )
	{
		auto CONST type = get_type();
		switch( get_type() )
		{
		case TYPE_REF:
		case TYPE_STR:
		case TYPE_FILENAME:
		case TYPE_DIRNAME:
			o.set( get_const_o_str() );
			break;

		case TYPE_GROUP:
		case TYPE_GROUP_CLOSED:
		case TYPE_NONE:
			print_err( __FUNCTION__, "this should not happen for this type" );
			o.erase();
			break;
		default:
			{
				CHAR str_tmp[256];
				c_param::sprint_ui( str_tmp, 256, nullptr, get_type(), get_pt_const_void(), get_symbolic_str() ) ;
				o.set( str_tmp );
			}
			break;
		}
	}
	else
		o.erase();
}

//	user interface and lua
//todo check for symbolic string we don't go outside of the limit
// not so easy we need to check before or pass limit for example
INT32 c_param::sprint_ui( CHAR* dst, INT32 len, C_PCHAR_C name, INT32 type, CP_CVOID pt, C_PCHAR_C * CONST symbolic )
{
	//	min buffer to start writing at all : enough for at least a "x:0" + null terminator.
	CONSTEXPR INT32 SPRINT_UI_LEN_MIN			= 4;
	//	min room left after writing the name : enough for the longest formatted number.
	CONSTEXPR INT32 SPRINT_UI_LEN_MIN_AFTER_NAME	= 32;
	//	safety margin shaved off `len` before passing to inner sprintf calls so we never overshoot.
	CONSTEXPR INT32 SPRINT_UI_SAFETY_MARGIN		= 16;

	if( pt<(void *)0x1000 )
		type = TYPE_NONE;
	if( !dst )
	{
		debug_break( "no buffer to write", __FUNCTION__ );
		return 0;
	}

	if( len <= SPRINT_UI_LEN_MIN )
	{
		debug_break( "%s() no buffer left to write", __FUNCTION__ );
		*dst = 0;
		return 0;
	}
	CHAR* dst_start = dst;
	if( name )
	{
		INT32 count = sprintf( dst, "%.*s  ", len-2, name );
		if( count == -1 )
		{
			debug_break( "%s() sprintf() could not write", __FUNCTION__ );
			*dst = 0;
			return 0;
		}
		dst += count;
		len -= count;
	}

	if( len <= SPRINT_UI_LEN_MIN_AFTER_NAME )	// leave enough room to print number
	{
		debug_break( "%s() no enough buffer left to write", __FUNCTION__ );
		*dst = 0;
		return INT32(dst_start - dst);
	}

	len -= SPRINT_UI_SAFETY_MARGIN;
	bool b_clean_trailing_zero = false;
	switch( type )
	{
	case TYPE_BIT32:
		dst += sprintf( dst, "0x%08X", *(UINT32 *)pt );
		break;
	case TYPE_SYMBO_NEG:
		{
		INT32	i = *(INT32 *)pt;
		if( i >= 0 )
			dst += sprintf( dst, "%d", i );
		else
			dst += sprintf( dst, "%.*s", len, symbolic ? symbolic[-i-1] : "Huh?" );
		}
		break;
	case TYPE_SYMBO_ZERO:
		{
		INT32	i = *(INT32 *)pt;
		if( i > 0 )
			dst += sprintf( dst, "%d", i );
		else
			dst += sprintf( dst, "%.*s", len, symbolic ? symbolic[-i] : "Huh?" );
		}
		break;
	case TYPE_CLASS_BRANCH:
		dst += sprintf( dst, "%.*s", len, symbolic ? ((c_factory_group*)symbolic)->get_str_ui( *(INT32 *)pt ) : "Huh?" );
		break;
	case TYPE_SYMBOLIC:
		dst += sprintf( dst, "%.*s", len, symbolic ? symbolic[ *(INT32 *)pt ] : "Huh?" );
		break;
	case TYPE_TIMECODE:
		{
			//	Timecode format : MM:SS:FF where FF is sub-second frame index at TIMECODE_FPS.
			//	Hardcoded to 25 fps (PAL/EBU) for now ; parameterise per project if NTSC ever needs supporting.
			CONSTEXPR INT32 TIMECODE_FPS = 25;
			FP32 t = *(FP32 *)pt;
			if( t < 0)
			{
				*dst = '-';
				t = -t;
			}
			else
				*dst = ' ';
			strcpy( dst+1, "xx:xx:xx");
			strnum::make( dst+1, 2, I_FLOOR( t/60. ) );
			strnum::make( dst+4, 2, IMOD( I_FLOOR(t), 60) );
			strnum::make( dst+7, 2, IMOD( I_FLOOR(t*TIMECODE_FPS), TIMECODE_FPS) );
			dst += 9;
		}
		break;
	default:
		switch( aaa::param::get_type_internal( type ) )
		{
		case TYPE_INTERNAL_BOOL:
			if( symbolic )
				dst += sprintf( dst, "%.*s", len, symbolic[(*(bool*)pt) ? 1 : 0] );
			else
				dst += sprintf( dst, "%.*s", len, (*(bool*)pt) ? "ON" : "OFF" );
			break;
		case TYPE_INTERNAL_INT32:	dst += sprintf( dst, "%d", *(INT32*)pt );	break;
		case TYPE_INTERNAL_UINT32:	dst += sprintf( dst, "%u", *(UINT32*)pt );	break;
		case TYPE_INTERNAL_FP32:
			{
				FP32 val = *(FP32*)pt;
				if( std::isnan(val) )	//todo not sure this is correct (2023 April)
					dst += sprintf( dst, "%.*s", len, "NaN" );
				else
					dst += sprintf( dst, "%#g", (double)val );	//hack ?
				b_clean_trailing_zero = true;
			}
			break;
		case TYPE_INTERNAL_DOUBLE:
			{
				DOUBLE val = *(DOUBLE*)pt;
				if( std::isnan(val) )
					dst += sprintf( dst, "%.*s", len, "NaN" );
				else
					dst += sprintf( dst, "%#g", val );			//hack ?
				b_clean_trailing_zero = true;
			}
			break;
		case TYPE_INTERNAL_STRING:
			{
				o_str* o = (o_str*)pt;
				if( pt && !o->is_empty() )
				{
					INT32	line_nb = 0;
					C_PCHAR	src = o->get();

					line_nb = 1;
					*dst = '"';
					--len;
					--src;
					while( len > 0 )
					{
						++src;
						if( *src == 0)
							break;
						if( *src == '\n' && ++line_nb > UI_STR_LINE_MAX )
							break;
						*++dst = *src;
						--len;
					}
					*++dst = '"';
					--len;
					++dst;
				}
				else
					dst += sprintf( dst, "\"\"" );
			}
			break;
		case TYPE_INTERNAL_NONE:
			break;
		default:
			debug_break( "%s() param %s(%s) unimplemented param type", __FUNCTION__, name, aaa::param::get_type_str(type) );
			return 0;
		}
		break;
	}

	//	%g supposed to drop trailing 0 already
	if( b_clean_trailing_zero )
	{
		if( *(dst-5) != 'e' ) 
		{
			while( *--dst == '0' );
			++dst;
		}
	}

	*dst = 0;
	return INT32(dst - dst_start);
}

//	used in save only
void	c_param::save_fprint( char* &dst )
{
	dst += sprintf( dst, "%s = ", get_name().get() );
	auto CONST type = get_type();
	switch( type )
	{
	case TYPE_BIT32:	//todo check the ithe way reading
		dst += sprintf( dst, "0x%08X", get_uint32() );
		break;
	case TYPE_SYMBO_NEG:
		{
			INT32	i = get_int32();
			if( i >= 0 )
				dst += sprintf( dst, "%d", i );
			else if( get_symbolic_str() )
				dst += sprintf( dst, "\"%s\"", get_symbolic_str(-i-1) );
			else
				dst += sprintf( dst, "%d", i );
		}
		break;
	case TYPE_SYMBO_ZERO:
		{
			INT32	i = get_int32();
			if( i > 0 )
				dst += sprintf( dst, "%d", i);
			else if( get_symbolic_str() )
				dst += sprintf( dst, "\"%s\"", get_symbolic_str(-i) );
			else
				dst += sprintf( dst, "%d", i);
		}
		break;
	case TYPE_CLASS_BRANCH:
		{
			c_factory_group* pcb = get_factory_group();
			C_PCHAR str = pcb ? pcb->get_str_storage( get_int32() ) : nullptr;
			if( str )
				dst += sprintf( dst, "\"%s\"", str );
			else
				dst += sprintf( dst, "%d", get_int32() );
		}
		break;
	case TYPE_SYMBOLIC:
		{
			C_PCHAR str = get_symbolic_str( get_int32() );
			if( str )
				dst += sprintf( dst, "\"%s\"", str );
			else
				dst += sprintf( dst, "%d", get_int32() );
		}
		break;
	case TYPE_GROUP:
	case TYPE_GROUP_CLOSED:
		print_err( __FUNCTION__, "should not trigger save" );
		dst += sprintf( dst, "Group_of_%d", (INT32)get_def() );
		break;
	default:
		switch( aaa::param::get_type_internal( type ) )
		{
			//hack	TYPE_CLASS_BRANCH dangerous if index are different because of different version should send str or hash:
		case TYPE_INTERNAL_BOOL:	dst += sprintf( dst, get_bool()   ? "ON" : "OFF" );	break;
		case TYPE_INTERNAL_INT32:	dst += sprintf( dst, "%d", get_int32() );		break;
		case TYPE_INTERNAL_UINT32:	dst += sprintf( dst, "%u", get_uint32() );		break;
		case TYPE_INTERNAL_FP32:	dst += sprintf( dst, "%g", get_fp32() );		break;
		case TYPE_INTERNAL_DOUBLE:	dst += sprintf( dst, "%g", get_double() );		break;
		case TYPE_INTERNAL_STRING:
			*dst++ = '\"';
			if( is_pt() && !get_const_o_str().is_empty() )
			{
				switch( type )
				{
				case TYPE_FILENAME:	
				case TYPE_DIRNAME:	c_dir_pool::cur->compact_fname( dst, get_const_o_str() );	break;
				default:			str_add_escape_for_quote(  dst, get_const_o_str().get() );		break;
				}
			}
			*dst++ = '\"';
			break;
		case TYPE_INTERNAL_NONE:
			*str_data = 0;
			print_err( __FUNCTION__, "should not trigger save" );
			break;
		default:
			*str_data = 0;
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
		break;
	}

	if( INT32 nb = get_out_nb() )
	{
		CHAR str[] = " TraxTyp00Out";
		for( INT32 i = 0; i < nb; ++i )
		{
			p_param p = get_out(i)->get_out_param();
			c_obj_ui* obj = p->get_obj();
			if( obj )
			{
				if( obj->is_class<c_trax>() )
				{
					strncpy( &str[5], c_trax::get_type_from_param_pt(p), 3 );
					strnum::make( str+8, 2, c_trax::get_index_from_param_pt(p) + 1 );
					dst += sprintf( dst, str );
				}
			}
		}
	}
	if( INT32 nb = get_in_nb() )
	{
		CHAR str[] = " TraxTyp00In";
		for( INT32 i = 0; i < nb; ++i )
		{
			p_param p = get_in(i)->get_in_param();
			c_obj_ui* obj = p->get_obj();
			if( obj )
			{
				if( obj->is_class<c_trax>() )
				{
					strncpy( &str[5], c_trax::get_type_from_param_pt(p), 3  );
					strnum::make( str+8, 2, c_trax::get_index_from_param_pt(p) + 1 );
					dst += sprintf( dst, str );
				}
			}
		}
	}
	dst += sprintf( dst, " ;" );
}

void	c_param::set_value_str_direct( o_str CONST & src )
{
#if	AAA_DEBUG()
	if( !is_type_text() )
	{
		debug_break( "%s() this not a text type", __FUNCTION__ );
		return;
	}
#endif
	auto dst = get_pt_o_str();
	if( !c_param::get_obj_loading() )
	{
		if( auto fn = _data->get_fn_update() )
		{	
			if( !dst->is_equal( src ) )
			{
				dst->set( src );
				fn( this );
				return;
			}
		}
	}
	dst->set( src ); 
}
void	c_param::set_value_str_direct( C_PCHAR_C src )
{
#if	AAA_DEBUG()
	if( !is_type_text() )
	{
		debug_break( "%s() this not a text type", __FUNCTION__ );
		return;
	}
#endif
	auto dst = get_pt_o_str();
	if( !c_param::get_obj_loading() )
	{
		if( auto fn = _data->get_fn_update() )
		{
			if( !dst->is_str_equal( src ) )
			{
				dst->set( src );
				fn( this );
				return;
			}
		}
	}
	dst->set( src );
}

void	c_param::set_value_str( o_str CONST & src )
{
	// to check : this done at a lower level but kept for opt in this bad case
	if( !is_changeable() )
	{
		debug_break( "%s() param is locked", __FUNCTION__ );
		return;
	}
	set_value_str_direct( src );
}
void	c_param::set_value_str( C_PCHAR_C src )
{
	// to check : this done at a lower level but kept for opt in this bad case
	if( !is_changeable() )
	{
		debug_break( "%s() param is locked", __FUNCTION__ );
		return;
	}
	set_value_str_direct( src );
}

void	c_param::set_value_to_def()
{
	if( is_changeable() )
	{
		if( is_type_text() )
			set_value_str_direct( get_symbolic_str_direct(1) );	//todo use def value ?
		else
			set_value_num_from_double( get_def() );
	}
}

void	c_param::set_value_to_ina()
{
	if( is_changeable() )
	{
		if( is_type_text() )
			set_value_str_direct( get_symbolic_str_direct(0) );	//todo use ina value
		else
			set_value_num_from_double( get_ina() );	//todo should we optimize this
	}
}

CONST	REAL	PARAM_FLOAT_STEP = REAL(0.05);

bool	c_param::do_action( aaa::param::ACTION action )
{
	if( !is_changeable() )
		return false;

	bool	retcode = true;
	DOUBLE	val;
	DOUBLE	inc;
	DOUBLE	factor;
	DOUBLE	old;
	bool	b_change = false;
	bool	b_done = false;

	auto CONST type = get_type();
	auto CONST type_internal = aaa::param::get_type_internal( type );
	switch( type )
	{
	//	case TYPE_BIT32:	//todo ?
	case TYPE_TIMECODE:
		old = get_fp32();
		inc = REAL(.04);
		factor = 1.;
		b_change = true;
		break;
	case TYPE_STR:	// this is a special case
	case TYPE_REF:
		switch( action )
		{
		case PARAM_DEF:
			set_value_to_def();
			b_change = true;
			b_done = true;
			break;
		case PARAM_INA:
			set_value_to_ina();
			b_change = true;
			b_done = true;
			break;
		}
		goto exit;
	default:
		switch( type_internal )
		{
			//hack	TYPE_CLASS_BRANCH dangerous if index are different because of different version should send str or hash:
		case TYPE_INTERNAL_BOOL:
		case TYPE_INTERNAL_INT32:
		case TYPE_INTERNAL_UINT32:
			inc = 1.;
			factor = 1.;
			b_change = true;
			switch( type_internal )
			{
			case TYPE_INTERNAL_BOOL:	old = get_bool() ? 1. : 0.;	break;
			case TYPE_INTERNAL_INT32:	old = (DOUBLE)get_int32();	break;
			case TYPE_INTERNAL_UINT32:	old = (DOUBLE)get_uint32();	break;
			}
			break;
		case TYPE_INTERNAL_FP32:
			old = get_fp32();
			inc = 0.;
			factor = DOUBLE(1. + PARAM_FLOAT_STEP);
			b_change = true;
			break;
		case TYPE_INTERNAL_DOUBLE:
			old = get_double();
			inc = 0.;
			factor = DOUBLE(1. + PARAM_FLOAT_STEP);
			b_change = true;
			break;
		default:
			*str_data = 0;
			print_debug( __FUNCTION__, "unimplemented param type" );
			retcode = false;
			break;
		}
		break;
	}

	if( action < PARAM_PREV && b_change && !b_done )
	{
		switch( action )
		{
		case PARAM_INC:
		case PARAM_INC_LOOP:
			val = ( old == 0. && inc == 0. ) ? .001 : (old + inc) * factor;
			if( action == PARAM_INC_LOOP && val > get_max() )
				val =  get_min();
			break;
		case PARAM_DEC:
		case PARAM_DEC_LOOP:
			val = (old - inc) / factor;
			if( action == PARAM_DEC_LOOP && val < get_min() )
				val = get_max();
			break;
		case PARAM_MUL:
			val = ( old == 0. ) ? .001 : old * 2.;
			break;
		case PARAM_DIV:		val = old / 2.;			break;
		case PARAM_MAX:		val = get_max();		break;
		case PARAM_MIN:		val = get_min();		break;
		case PARAM_DEF:		val = get_def();		break;
		case PARAM_INA:		val = get_ina();		break;
		case PARAM_ROUND:	val = (INT32) old;		break;
		case PARAM_SIGN:
			val = (type_internal == TYPE_INTERNAL_BOOL) ? 1. - old : -old;
			break;
		}
		set_value_num_from_double( val );
	}

exit:
	if( b_change )
	{	
		if( !c_param::get_obj_loading() )
			if( auto fn = _data->get_fn_update() )
				fn( this );
		param_set_action_str_last( param_action_str[action] );
		send_change();
	}

	return retcode;
}

//	this should be call only when reading from string
//		because of the checksum
//		but now called from lua glue and clipboard
AAA_ERR	c_param::set_value_from_str( C_PCHAR_C str )
{
	IF_THIS_NULL()
		return ERR_OBJ_NULL;

	if( !is_changeable() )
	{
		ERR_PRINT_STRING( "%s() param %s is locked, will not change it", __FUNCTION__, get_name().get() );
		return ERR_LOCKED;
	}

	auto CONST type = get_type();
	AAA_ERR	retcode = AAA_OK;
	switch( type )
	{
	case TYPE_CLASS_BRANCH:
	case TYPE_SYMBOLIC:
	case TYPE_SYMBO_ZERO:
		{
			INT32 tmp = _data->get_value_from_symbo_str( str );	// value symbo are never big so we can get away with int32
			set_uint32_direct( clamp(tmp) );
#if	AAA_CHECKSUM_PARAM_DO()
			checksum_param.add_int32( tmp );
#endif
		}
		break;
	case TYPE_SYMBO_NEG:
		{
			INT32 tmp = _data->get_value_from_symbo_str( str );	// value symbo are never big so we can get away with int32
			set_int32_direct( clamp(tmp) );
#if	AAA_CHECKSUM_PARAM_DO()
			checksum_param.add_uint32( tmp );
#endif
		}
		break;
	case TYPE_FILENAME:
	case TYPE_DIRNAME:
//todo deal with big str (add size in param field)
		{
			o_str& o = o_str::push_name();
				c_dir_pool::cur->expand_fname( o, str );
				set_value_str_direct( o );		
#if	AAA_CHECKSUM_PARAM_DO()
				checksum_param.add_str( str );
#endif
			o_str::pop_name();
		}
		break;
	default:
		switch( aaa::param::get_type_internal( type ) )
		{
			//hack	TYPE_CLASS_BRANCH dangerous if index are different because of different version should send str or hash:
		case TYPE_INTERNAL_BOOL:
			set_bool_direct( str_is_equal_nocase( str, "on" ) );
#if	AAA_CHECKSUM_PARAM_DO()
			checksum_param.add_str( str );
#endif
			break;
		case TYPE_INTERNAL_INT32:
			{
				INT32 tmp = atol(str);
				set_int32_direct( clamp(tmp) );
#if	AAA_CHECKSUM_PARAM_DO()
				checksum_param.add_int32( tmp );
#endif
			}
			break;
		case TYPE_INTERNAL_UINT32:
			{
				UINT32 tmp = (UINT32)atoll(str);
				set_uint32_direct( clamp(tmp) );
#if	AAA_CHECKSUM_PARAM_DO()
				checksum_param.add_uint32( tmp );
#endif
			}
			break;
		case TYPE_INTERNAL_FP32:
			{
				FP32 tmp = FP32(atof(str));
				set_fp32_direct( clamp(tmp) );
#if	AAA_CHECKSUM_PARAM_DO()
				checksum_param.add_fp32( tmp );
#endif
			}
			break;
		case TYPE_INTERNAL_DOUBLE:
			{
				DOUBLE tmp = atof(str);
				set_double_direct( clamp(tmp) );
#if	AAA_CHECKSUM_PARAM_DO()
				checksum_param.add_double( tmp );
#endif
			}
			break;
		case TYPE_INTERNAL_STRING:
//todo deal with NULL case and big str(add size in param field
			set_value_str_direct(str);
#if	AAA_CHECKSUM_PARAM_DO()
			checksum_param.add_str( str );
#endif					
			break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
#if	AAA_CHECKSUM_PARAM_DO()
			checksum_param.add_str( str );
#endif	
			break;
		}
	}
	return	retcode;
}


//	create new data when we change it
void	c_param::set_data_owner()
{
	if( is_data_owner() )
	{
// it is ok I think
//		debug_break( "%s() already data owner, probably a memory leak now.", __FUNCTION__ );
	}
	else
	{
		c_param_data * pd;
		if( _data )	// we already have data but need to change some so we copy 
			pd = new c_param_data( _data );
		else
			pd = new c_param_data();
		if( pd )
		{
			_data = pd;
			_flags |= M_DATA_OWNER;
		}
		else
			debug_break( "%s() can't allocate c_param_data, you should exit.", __FUNCTION__ );
	}
}

void	c_param::set_name( o_str CONST & name )
{
	if( !_data || !_data->is_name_equal(name) )
	{
		set_data_owner();
		_data->set_name( name );
	}
}

void	c_param::set_name( C_PCHAR_C name )
{
	if( !_data || !_data->is_name_equal(name) )
	{
		set_data_owner();
		_data->set_name( name );
	}
}

void	c_param::set_min( REAL CONST min )
{
	if( !_data || min != get_min() )
	{
		set_data_owner();
		_data->set_min( min );
	}
}
void	c_param::set_max( REAL CONST max )
{
	if( !_data || max != get_max() )
	{
		set_data_owner();
		_data->set_max( max );
	}
}
void	c_param::set_def( REAL CONST def )
{
	if( !_data || def != get_def() )
	{
		set_data_owner();
		_data->set_def( def );
	}
}
void	c_param::set_ina( REAL CONST ina )
{
	if( !_data || ina != get_ina() )
	{
		set_data_owner();
		_data->set_ina( ina );
	}
}

void	c_param::set_type( UINT32 CONST type )
{
	if( !_data || aaa::param::TYPE(type) != get_type() )
	{
		set_data_owner();
		_data->set_type( aaa::param::TYPE(type) );
	}
}

void	c_param::set_save( bool CONST b_save )
{
	if( !_data || is_save()!=b_save )
	{
		set_data_owner();
		_data->set_save( b_save );
	}
}


void	c_param::set_limits( REAL CONST def, REAL CONST ina, REAL CONST min, REAL CONST max )
{
	if( !_data )
		set_data_owner();
	_data->set_limits( def, ina, min, max );
}
/*
void	c_param::set( void* pt_in, INT32 type, CHAR* name, REAL def, REAL ina, REAL min, REAL max )
{
	pt = pt_in;
	_data->set_type( type );
	_data->set_name( name );
	_data->set_limits( def, ina, min, max );
}
void	c_param::set( CONST c_param_def* CONST p )
{
	set( p->_pt, p->type_, (const_cast<c_param_def*>(p))->get_name(),
		p->def_, p->ina_, p->min_, p->max_ );	//todo cast because of CONST to vanish
	p->update, p->symbolic_str
}
*/

void	c_param::attach_param( c_param * CONST pt, INT32 CONST nb )
{
	if( pt==nullptr && nb==0 )
	{
		if( _more )
			_more->attach_param( pt, nb );
	}
	else
		get_more_always()->attach_param( pt, nb );
}

//infact we attach the param_header in this case
void	c_param::attach_obj( c_obj_ui* CONST obj )
{
	if( obj )
	{
#if	AAA_DEBUG()
		if( mem::is_pointer_fucked(obj) )
			debug_break( "c_param::attach_obj() bad c_obj_ui pointer" );
		else
#endif
		{
			p_param	header = obj->get_param_header();
			if( header )
				attach_param( header, 1 );
			else
			{
				debug_break( "%s() obj have no param_header to attach, this should not happen", __FUNCTION__ );
				attach_param( nullptr, 0 );
			}
		}
	}
	else
		attach_param( nullptr, 0 );
}

//////
////	COMMENT
//
void	c_param::set_comment( C_PCHAR_C str )
{
	if( str && *str )
		get_more_always()->set_comment( str );
	else
		clear_comment();
}
//void	c_param::set_comment( o_str* o )
//{
//	if( o && !o->is_empty() )
//		get_more()->set_comment( o->get() );
//	else
//		clear_comment();
//}
void	c_param::set_comment( o_str CONST & o )
{
	if( !o.is_empty() )
		get_more_always()->set_comment( o.get() );
	else
		clear_comment();
}
void	c_param::set_comment( o_str CONST & o, bool CONST b_error )
{
	if( !o.is_empty() )
	{
		get_more_always()->set_comment( o.get() );
		set_flag_error( b_error );
	}
	else
	{
		clear_comment();
		clear_flag_error();
	}
}
void	c_param::set_comment( C_PCHAR_C str, bool CONST b_error )
{
	if( str && *str )
	{
		get_more_always()->set_comment( str );
		set_flag_error( b_error );
	}
	else
	{
		clear_comment();
		clear_flag_error();
	}
}

//////
////	GET / SET
//
INT32	c_param::get_value_as_int32() CONST
{
	INT32 tmp = 0;
	if( is_pt() )
	{
		auto CONST type = get_type();
		switch( type )
		{
		case TYPE_TIMECODE:
			debug_break( "%s() unsupported type %d", __FUNCTION__, type );
			break;
		default:
			switch( aaa::param::get_type_internal( type ) )
			{
			case TYPE_INTERNAL_BOOL:	tmp = get_bool()   ? 1 : 0;	break;
			case TYPE_INTERNAL_INT32:	tmp = get_int32();			break;
			case TYPE_INTERNAL_UINT32:	tmp = get_uint32();			break;
			case TYPE_INTERNAL_FP32:	tmp = (INT32) get_fp32();	break;
			case TYPE_INTERNAL_DOUBLE:	tmp = (INT32) get_double();	break;
			default:
				print_err( __FUNCTION__, "unimplemented param type" );
				break;
			}
		}
	}
	return tmp;
}

UINT32	c_param::get_value_as_uint32() CONST
{
	UINT32 tmp = 0;
	if( is_pt() )
	{
		auto CONST type = get_type();
		switch( type )
		{
		case TYPE_TIMECODE:
			debug_break( "%s() unsupported type %d", __FUNCTION__, type );
			break;
		default:
			switch( aaa::param::get_type_internal( type ) )
			{
			case TYPE_INTERNAL_BOOL:	tmp = get_bool()  ? 1 : 0;		break;
			case TYPE_INTERNAL_INT32:	tmp = get_int32();				break;
			case TYPE_INTERNAL_UINT32:	tmp = get_uint32();				break;
			case TYPE_INTERNAL_FP32:	tmp = (UINT32) get_fp32();		break;
			case TYPE_INTERNAL_DOUBLE:	tmp = (UINT32) get_double();	break;
			default:
				print_err( __FUNCTION__, "unimplemented param type" );
				break;
			}
		}
	}
	return tmp;
}

bool	c_param::get_value_as_bool() CONST
{
	bool b;
	if( is_pt() )
	{
		switch( get_type_internal() )
		{
		case TYPE_INTERNAL_BOOL:	b = get_bool();				break;
		case TYPE_INTERNAL_INT32:	b = get_int32() != 0;		break;
		case TYPE_INTERNAL_UINT32:	b = get_uint32() != 0;		break;
		case TYPE_INTERNAL_FP32:	b = get_fp32() >= 0.5;		break;
		case TYPE_INTERNAL_DOUBLE:	b = get_double() >= 0.5;	break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
	else
		b = false;
	return b;
}

REAL	c_param::get_value_as_real() CONST
{
	//	Initialise to 0 so the switch's default branch (unimplemented type) does
	//	not return uninitialised memory. Was UB on the default path ; behavior-
	//	preserving on every reachable case because each case overwrites tmp
	//	before the break. Clang -Wsometimes-uninitialized caught this.
	REAL tmp = REAL(0.);
	if( is_pt() )
	{
		switch( get_type_internal() )
		{
		case TYPE_INTERNAL_BOOL:	tmp = get_bool() ? REAL(1.) : REAL(0.);	break;
		case TYPE_INTERNAL_INT32:	tmp = (REAL) get_int32();				break;
		case TYPE_INTERNAL_UINT32:	tmp = (REAL) get_uint32();				break;
		case TYPE_INTERNAL_FP32:	tmp = (REAL) get_fp32();				break;
		case TYPE_INTERNAL_DOUBLE:	tmp = (REAL) get_double();				break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
	else
		tmp = REAL(0.);	
	return tmp;
}

DOUBLE	c_param::get_value_as_double() CONST
{
	//	Same fix as get_value_as_real() above : avoid UB on the default branch.
	DOUBLE tmp = DOUBLE(0.);
	if( is_pt() )
	{
		switch( get_type_internal() )
		{
		case TYPE_INTERNAL_BOOL:	tmp = get_bool() ? DOUBLE(1.) : DOUBLE(0.);	break;
		case TYPE_INTERNAL_INT32:	tmp = (DOUBLE) get_int32();					break;
		case TYPE_INTERNAL_UINT32:	tmp = (DOUBLE) get_uint32();				break;
		case TYPE_INTERNAL_FP32:	tmp = (DOUBLE) get_fp32();					break;
		case TYPE_INTERNAL_DOUBLE:	tmp = get_double();							break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
	else
		tmp = DOUBLE(0.);	
	return tmp;
}

//	these two fns are the bottleneck of param change
//		with the set fn
//		add the read_from_mem
//		so 4 for now
//		plus some bad access in trax
//		at least for a single value 3d stuff are now change directly in trax and should be corrected
//todo inplement TYPE_BIT32 and check everywhere
/* 2023 May removed
#if	!AAA_REAL_IS_DOUBLE()
void	c_param::set_value_num_from_real( REAL value )
{
	if( is_changeable() )
	{
		clamp( value );
		switch( get_type_internal() )
		{
		case TYPE_INTERNAL_BOOL:	set_bool(	value > 0.5 );		break;
		case TYPE_INTERNAL_INT32:	set_int32(	(INT32) value );	break;
		case TYPE_INTERNAL_UINT32:	set_uint32( (UINT32) value );	break;
		case TYPE_INTERNAL_FP32:	set_fp32(	(FP32) value );		break;
		case TYPE_INTERNAL_DOUBLE:	set_double( (DOUBLE) value );	break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
}
#endif
*/

void	c_param::set_value_num_from_double( DOUBLE value )
{
	if( is_changeable() )
	{
		switch( get_type_internal() )
		{
		case TYPE_INTERNAL_BOOL:	set_bool_direct(	value > 0.5 );	break;
		case TYPE_INTERNAL_INT32:	set_int32_direct(	(INT32)  clamp(value) );	break;
		case TYPE_INTERNAL_UINT32:	set_uint32_direct(	(UINT32) clamp(value) );	break;
		case TYPE_INTERNAL_FP32:	set_fp32_direct(	(FP32)   clamp(value) );	break;
		case TYPE_INTERNAL_DOUBLE:	set_double_direct(           clamp(value) );	break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
	else
		ERR_PRINT_STRING( "%s() param %s is locked, will not change it", __FUNCTION__, get_name().get() );
}

/* 2023 May removed
void	c_param::set_value_num_from_int32( INT32 value )
{
	if( is_changeable() )
	{
		switch( get_type_internal() )
		{
		case TYPE_INTERNAL_BOOL:	set_bool_direct(	value > 0 );	break;
		case TYPE_INTERNAL_INT32:	set_int32(	value );				break;
		case TYPE_INTERNAL_UINT32:	set_uint32( (UINT32) value );		break;
		case TYPE_INTERNAL_FP32:	set_fp32(	(FP32) value );			break;
		case TYPE_INTERNAL_DOUBLE:	set_double_direct( clamp((DOUBLE) value) );		break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
}

void	c_param::set_value_num_from_uint32( UINT32 value )
{
	if( is_changeable() )
	{
		switch( get_type_internal() )
		{
		case TYPE_INTERNAL_BOOL:	set_bool(	value > 0 );		break;
		case TYPE_INTERNAL_INT32:	set_int32(	(INT32) value );	break;
		case TYPE_INTERNAL_UINT32:	set_uint32( value );			break;
		case TYPE_INTERNAL_FP32:	set_fp32(	(FP32) value );		break;
		case TYPE_INTERNAL_DOUBLE:	set_double( (DOUBLE) value );	break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
}
*/

void	c_param::set_value_num_from_bool( bool value )
{
	if( is_changeable() )
	{
		switch( get_type_internal() )
		{
		case TYPE_INTERNAL_BOOL:	set_bool_direct(	value );				break;
		case TYPE_INTERNAL_INT32:	set_int32_direct(	value ? 1 : 0 );		break;	//we assume we don't need to clamp
		case TYPE_INTERNAL_UINT32:	set_uint32_direct(	value ? 1 : 0 ); 		break;	//we assume we don't need to clamp
		case TYPE_INTERNAL_FP32:	set_fp32_direct(	clamp(value ? FP32(1) : FP32(0)) );	break;
		case TYPE_INTERNAL_DOUBLE:	set_double_direct(  clamp(value ? 1 : 0) );	break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
	else
		ERR_PRINT_STRING( "%s() param %s is locked, will not change it", __FUNCTION__, get_name().get() );
}

//hack the setlocale here is a hack for lcp should be corrected
//	global pref ? or pref by trax ?....
//todo multitask
namespace {
	CHAR str[8192];
}
void	c_param::set_value_str_from_double( C_PCHAR_C format, DOUBLE value )
{
	if( is_changeable() )	// this done at a lower level but kept for opt in this bad case
	{
		try	//because bad format screw up the thing
		{
			//	setlocale( LC_NUMERIC, "French" );
			sprintf( str, format, value );	//hack dangerous
			set_value_str_direct( str );	//check file type here
			//				setlocale( LC_NUMERIC, "English" );
		}
		catch(...)
		{
			debug_break();				
		}
	}
}


void	c_param::cpy_value_from_param_type_same( c_param CONST * CONST src )
{
	if( is_changeable() )
	{
		auto CONST type = get_type();
#if AAA_DEBUG()
		if( type != src->get_type() )
			debug_break( "%s() these param don't have the same type", __FUNCTION__ );
#endif
		switch( aaa::param::get_type_internal( type ) )
		{
		case TYPE_INTERNAL_BOOL:	set_bool_direct(	src->get_bool() );			break;
		case TYPE_INTERNAL_INT32:	set_int32_direct(	clamp(src->get_int32()) );	break;
		case TYPE_INTERNAL_UINT32:	set_uint32_direct(	clamp(src->get_uint32()) );	break;
		case TYPE_INTERNAL_FP32:	set_fp32_direct(	clamp(src->get_fp32())	);	break;
		case TYPE_INTERNAL_DOUBLE:	set_double_direct(	clamp(src->get_double()) );	break;
		case TYPE_INTERNAL_STRING:
			{
				o_str& o_tmp = o_str::push_name();
					src->get_value_as_str( o_tmp );
					set_value_str_direct( o_tmp );
				o_str::pop_name();
			}
			break;	
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
}

//	Snapshot to/from memory. Write trusts the value is already in [min,max] ;
//	read clamps as a security against corrupted or out-of-range buffer data.
//todo do better padding or compact ?
CHAR*	c_param::write_to_mem( CHAR* dst ) CONST
{
	if( is_pt() )
	{
		switch( get_type_internal() )
		{
		// make bool more compact will destroy alignment
		case TYPE_INTERNAL_BOOL:	*(INT32*)	dst	= get_bool() ? 1 : 0;	dst += sizeof(INT32);	break;
		case TYPE_INTERNAL_INT32:	*(INT32*)	dst	= get_int32();			dst += sizeof(INT32);	break;
		case TYPE_INTERNAL_UINT32:	*(UINT32*)	dst	= get_uint32();			dst += sizeof(UINT32);	break;
		case TYPE_INTERNAL_FP32:	*(FP32*)	dst	= get_fp32();			dst += sizeof(FP32);	break;
		case TYPE_INTERNAL_DOUBLE:	*(DOUBLE*)	dst	= get_double();			dst += sizeof(DOUBLE);	break;
//		case TYPE_INTERNAL_STRING:		//hack we should do something here
//			break;	
//		case TYPE_INTERNAL_NONE:	break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
	return dst;
}

//todo do better padding or compact ?
//hack we should deal with all cases
CHAR CONST *	c_param::read_from_mem( CHAR CONST * src )
{
	if( is_pt() )
	{
		switch( get_type_internal() )
		{
		// make bool more compact will destroy alignment
		case TYPE_INTERNAL_BOOL:	set_bool_direct(	(*(INT32*) src) != 0 );					src += sizeof(INT32);	break;
		case TYPE_INTERNAL_INT32:	set_int32_direct(	clamp(*(INT32*)  src ));				src += sizeof(INT32);	break;
		case TYPE_INTERNAL_UINT32:	set_uint32_direct(	clamp(*(UINT32*) src ));				src += sizeof(UINT32);	break;
		case TYPE_INTERNAL_FP32:	set_fp32_direct(	clamp(*(FP32*)   src ));				src += sizeof(FP32);	break;
		case TYPE_INTERNAL_DOUBLE:	set_double_direct(	clamp(*(DOUBLE*) src ));				src += sizeof(DOUBLE);	break;
//		case TYPE_INTERNAL_STRING:		//hack we should do something here
//			break;	
//		case TYPE_INTERNAL_NONE:	break;
		default:
			print_err( __FUNCTION__, "unimplemented param type" );
			break;
		}
	}
	return src;
}

bool	c_param::is_ina() CONST
{
	bool	b;
	switch( get_type_internal() )
	{
	// make bool more compact will destroy alignment
	case TYPE_INTERNAL_BOOL:	b = get_bool()		== (bool)	get_ina();	break;
	case TYPE_INTERNAL_INT32:	b = get_int32()		== (INT32)	get_ina();	break;
	case TYPE_INTERNAL_UINT32:	b = get_uint32()	== (UINT32)	get_ina();	break;	//problems in high values
	case TYPE_INTERNAL_FP32:	b = get_fp32()		== (FP32)	get_ina();	break;
	case TYPE_INTERNAL_DOUBLE:	b = get_double()	== (DOUBLE)	get_ina();	break;	
	case TYPE_INTERNAL_STRING:	b = (!is_pt()) || get_const_o_str().is_empty();	break;	//todo not sure of this
//	case TYPE_INTERNAL_NONE:	break;
	default:
		b = true;
		print_err( __FUNCTION__, "unimplemented param type" );
		break;
	}
	return b;
}



//todo split in a param_file src ?

//////
////	LOAD/SAVE
//

typedef	c_node_list_ui<c_obj_active_ui>	BUILDER;
namespace 
{
	//todo	add a stack structure ?
	std::vector<BUILDER*>	builders;
	BUILDER*				builder_cur = nullptr;

	std::vector<c_file_io*>	readers;
	std::vector<c_file_io*>	readers_free;

	c_file_io*				reader		= nullptr;
}

#if AAA_STATE_COMPILE()
bool	c_param::need_state_store()
{
	if( is_lock() || !is_save() )
		return false;
	return is_pt() && is_state_sensitive();
}
#endif //AAA_STATE_COMPILE

FINLINE bool	c_param::need_save() CONST
{
	//todo Maa don't think last test is necessary
	if( is_out() || is_in() || (builder_cur && get_param_attached_nb()>0 && !is_group()) )
		return true;
	if( is_lock() || !is_save() )
		return false;
	return is_pt() && !is_ina();
}


/*
bool	param_list_need_save( p_param param, INT32 nb)
{
bool	need = false;
while( nb-- )
{
if( need = param->need_save() )
break;
++param;
}
return	need;
}
*/


static	void	reader_alloc()
{
	reader = new c_file_io;
}
static	void	reader_dealloc()
{
	for( auto const & pt : readers )
		delete pt;
	readers.clear();

	for( auto const & pt : readers_free )
		delete pt;
	readers_free.clear();

	delete reader;
}

static	void	reader_push()
{	
	readers.push_back( reader);
	if( readers_free.empty() )
	{
		reader = new c_file_io;
	}
	else
	{
		reader = readers_free.back();
		readers_free.pop_back();
	}
}

static	void	reader_pop()
{
	if( readers.empty() )
	{
		BOX_ERR( "c_param::reader_pop(): stack empty\n will crash soon ???" );
	}
	else
	{
		readers_free.push_back( reader );
		reader = readers.back();
		readers.pop_back();
	}
}	

void	c_param::builder_push( c_obj_ui* one )
{	
	builders.push_back( builder_cur );
	builder_cur = (BUILDER*)one;
}	

//todo	c_node_list_ui<class T>*	param_builder_pop()
void	c_param::builder_pop()
{
	if( builders.empty() )
	{
		BOX_ERR( "c_param::builder_pop(): stack empty\n will crash soon ???" );
	}
	else
	{
		builder_cur = builders.back();
		builders.pop_back();
	}
}	
FINLINE	FILE*	c_param::save_open( CONST char* CONST filename )
{
	//	open file
	FILE* file = c_file::FOPEN( filename, "wt" );
	if( !file )
		ERR_PRINT_STRING( "Can't open file : %s", filename );
	return file;
}

FINLINE	AAA_ERR	c_param::save_close( FILE* file )
{
	c_file::FCLOSE( file );
	return AAA_OK;
}

FINLINE	AAA_ERR	c_param::save( char* &dst )
{
	if( get_type() != TYPE_NONE )
	{
		if( builder_cur )
		{
			c_obj_ui* obj = get_obj_attached();
			if( obj )
			{
				o_str& fname = o_str::push_name();
					builder_cur->make_fname_branch( fname, obj );

					builder_push( nullptr );
						obj->save_to_file(fname);
					builder_pop();
				o_str::pop_name();

				dst += sprintf( dst, "AddFileRel \"%s\" ", fname.get_fname() );
			}
		}
		if( need_save() )
		{
			save_fprint( dst );
			dst += sprintf( dst, "\n\t");
		}
	}
	return  AAA_OK;
}

//todo get rid and do better
namespace{
	 //todo hard coded max is ugly
	CONSTEXPR	INT32	BUF_SIZE = 1024*1024 ;
	//	Empirical worst case for a single param->save() : name + value + a few trax
	//	tags + " ;\n\t". 1 KB is meant to comfortably contain that, and is the safety
	//	margin used by param_list_save : we abort when the next save would risk
	//	stepping past BUF_SIZE - SAVE_MARGIN_BYTES. The post-write debug check below
	//	traps any save that ever exceeds this margin so the constant can be tightened
	//	or the buffer grown.
	CONSTEXPR	INT32	SAVE_MARGIN_BYTES = 1024 * 32;
	struct ST_BUF
	{
		CHAR	data[BUF_SIZE];
		CHAR*	pt;
	};

	std::vector<ST_BUF*>	bufs;
	UINT32					bufs_deep = 0;	//todo check if try catch don't fuck up this

	void	alloc_buf()
	{
	}
	void	dealloc_buf()
	{
		for( auto const & pt : bufs )
			delete pt;
		bufs.clear();
	}
	FINLINE	ST_BUF*	push_buf()
	{
		ST_BUF*	buf;

		if( ++bufs_deep > bufs.size() )
		{	//	alloc if needed
			buf = new ST_BUF;
			bufs.push_back( buf );
			if( !buf )
			{
				ERR_PRINT_STRING( "Can't allocate buffer to save, you should exit here." );
			}
		}
		else
			buf = bufs[bufs_deep-1];
		return buf;
	}
	FINLINE	void	pop_buf()
	{
		--bufs_deep;
	}
}

FINLINE	AAA_ERR	param_list_save( ST_BUF* save_buf, p_param param, INT32 nb )
{
	*save_buf->pt = 0;
	while( nb-- )
	{
		//	Check BEFORE writing : if the current position is already within the
		//	safety margin of the end, abort cleanly so the caller knows the file
		//	was truncated rather than risking an out-of-bounds write.
		if( save_buf->pt - save_buf->data > BUF_SIZE - SAVE_MARGIN_BYTES )
		{
			BOX_ERR( "param_list_save buffer too small.\nCan't save all this file." );
			return	ERR_ANY;
		}
		CHAR* CONST pt_before = save_buf->pt;
		param->save( save_buf->pt );	//todo this fn should have a len parameter to avoid overflow nd all thses checks
		if( save_buf->pt - pt_before > SAVE_MARGIN_BYTES )
			debug_break( "%s() a single param->save wrote %lld bytes, exceeding the %d safety margin", __FUNCTION__, INT64(save_buf->pt - pt_before), SAVE_MARGIN_BYTES );
		++param;
	}
	return	AAA_OK;
}

FINLINE	AAA_ERR c_param::save_begin( FILE* file, C_PCHAR_C title, INT32 version )
{
	fprintf( file, "%s ( Version %d )\n\t{\n\t", title, version );
	return AAA_OK ;
}

FINLINE	AAA_ERR c_param::save_end( FILE* file )
{
	fprintf( file, "}\n");
	return AAA_OK ;
}
/*
AAA_ERR param_list_save_to_stream( FILE * file, CONST char * CONST param_name, INT32 version, p_param param, INT32 nb)
{
	save_begin( file, param_name, version);
	param_list_save( file, param, nb);
	param_save_end( file);
	return AAA_OK ;
}
*/


AAA_ERR	c_param::list_save_to_file( C_PCHAR_C filename, C_PCHAR_C param_name, INT32 version, p_param param, INT32 nb, bool b_default_save_always )
{
	if( !filename )
		return ERR_ANY;

	//	get a buffer to write in memory
	ST_BUF*	buf = push_buf();

	if( buf )
	{
		//	write to buffer
		buf->pt = buf->data;
		if( ERR( param_list_save( buf, param, nb ) ) )
			goto err_exit;

		if( buf->pt != buf->data || b_default_save_always )
		{	//	if something to save then save it 
			FILE*	file = save_open( filename );
			if( !file )
				goto err_exit;

			save_begin( file, param_name, version );
			c_file::FPUTS( buf->data, file );
			save_end( file );

			save_close( file );
		}
		else	//	nothing to save so we need to remove the file
		{
			c_file::FREMOVE( filename );
		}
		pop_buf();
		return AAA_OK;
	}
err_exit:
	BOX_ERR( "Can't save %s!", filename );
	pop_buf();
	return ERR_ANY;
}

FINLINE	AAA_ERR	c_param::load_open( o_str CONST & filename )
{
	AAA_ERR	ret = reader->read_file( filename );
	if( NOERR(ret) )
	{
		if( b_verbose )
			VERBOSE_PRINT_STRING( "Parsing %s :", filename.get() );
	}
	else
	{
		if( b_list_load_verbose )
			ERR_PRINT_STRING( "%s don't exist", filename.get() );
	}
	return ret;
}


FINLINE	void	c_param::load_close()
{
}

//todo should make symmetric load/save reader/buffer to reuse buffer
//todo make it dynamic
namespace {
	CONSTEXPR	INT32	BUF_LOAD_SIZE = 1024*32;
	static		CHAR	buf_load[BUF_LOAD_SIZE];
};


AAA_ERR	c_param::list_load( c_obj_ui* obj )
{
	SPY_PUSH_RANGE( "param_list_load", spy::INFRA );

	//todo	deal with a big text buffer
	//	see bddtex2d.h also
	p_param	param_base	= obj->get_param_begin();
	INT32	nb_param	= obj->get_param_nb_allocated();
	AAA_ERR	ret			= ERR_ANY;
	INT32	param_index = -1;
	INT32	len;

	if( b_verbose )
		VERBOSE_PRINT_STRING( "Scanning now for parameters" );

	obj_loading = obj;
	do
	{
		//read
		len = reader->scan_str_till( buf_load, BUF_LOAD_SIZE-1, '=' );
		if( len <= 0 )
		{
			BOX_ERR( "File %s\nunexpected end of file.", reader->get_filename() );
			ret = ERR_FILE_EOF_UNEXPECTED;
			break;
		}
		if( b_verbose )
			VERBOSE_PRINT_STRING( "\t%s ", buf_load );
		//check for end
		if( *buf_load == '}' )
		{
			ret = AAA_OK;
			break;
		}
		//len = strlen( buf_load );
		if( len >= 2 && ( (*buf_load=='/' && buf_load[1]=='/') || (*buf_load=='-' && buf_load[1]=='-') ) )
		{	//	we consider // or -- as start if a comment
			reader->skip_until_control();	//store comment with param
			continue;
		}
		else if( len > 0 && buf_load[len-1]==';' )
		{
			BOX_ERR( "File %s\nblank statement because semicolom should not be there :\n%s", reader->get_filename(), buf_load );
			continue;
		}
			
		//a file to load used for deformer
		if( len>=7 && str_is_equal_nocase( buf_load, "addfile", 7 ) )
		{
			//	we detect AddFileRel
			bool b_rel = len != 7;
			if( ERR( reader->my_scan_str( buf_load, BUF_LOAD_SIZE-1 ) ) )
			{
				BOX_ERR( "unexpected end of file in file %s .", reader->get_filename() );
				break;
			}

			if( b_verbose )
			{
				if(	*buf_load )
					PRINT_STRING( "\"%s\" ", buf_load );
				else
					PRINT_STRING( "\"\" " );
			}
			if( builder_cur )
			{
				if( b_rel )
				{
					o_str& buf = o_str::push_name();
						buf.set_char( '/' );
						buf.add( buf_load );
						fname::cpy_dir( buf_load, builder_cur->get_my_filename() );
						strcat( buf_load, buf.get() );
					o_str::pop_name();
				}

				reader_push();
#if AAA_STATE_COMPILE()
				if( !c_state_master::is_state_ref() )
				{	//	in this case we don't want to create but just read
					//hack these fns should not exist (by_my_filename)
					c_obj_ui*	obj_to_read = builder_cur->find_branch_by_my_filename( buf_load );
					if( obj_to_read )
					{
						c_namer* tmp = obj_to_read->get_namer();
//						if( tmp )
//							printf( "%s %s\n", tmp->get_dir(), tmp->get_file() );
						obj_to_read->load_from_file( buf_load );
					}
					else
					{
						ERR_PRINT_STRING( "previous state don't have this object %s", builder_cur->get_my_filename() );
					}
				}
				else
#endif //AAA_STATE_COMPILE
				{	//	regular case we add stuff
					builder_cur->insert_at_tail( buf_load );
				}
				reader_pop();

				//	could have change this obj (Maa not sure)
				obj->param_init_pt();	//call	//now is needed
				param_base = obj->get_param_begin();
				nb_param = obj->get_param_nb_allocated();	//param
			}
		}
		else
		//so str should be param name
		{
			// find param with same name
			p_param				param = &param_base[param_index];
			aaa::param::TYPE	type;
			bool	b_found = false;
			for( INT32 i=nb_param; i>0; --i )
			{
				if( ++param_index >= nb_param )	//this way we search from current position
				{
					param_index = 0;
					param = param_base;
				}
				else
					++param;
				type = param->get_type();
				if( is_type_group(type) || type == TYPE_NONE )
					continue;

				//todo add str_is_equal_bothnocase gto o_str and call it
				if( param->get_name().is_equal_nocase( buf_load, len ) )
				{	//	we found it
#if	AAA_DEBUG()
					if( !param->is_pt() )
						DBG_PRINT_STRING( "File %s : Identifier %s have no pointer.", reader->get_filename(), param->get_name().get() );
#endif
					b_found = true;
					break;
				}
			}
			// message if not found
			if( !b_found )
			{
				BOX_ERR( "Can't find parameter\t%s\nwhen loading\t\t%s\nfrom dir\t\t\t%s", buf_load, reader->get_filename(), c_dir::get_def().get() );
				do_alert_with_sound( "ParamDontIdentified" );
				if( !reader->skip_until_char( ';' ) )
					ERR_PRINT_STRING( "File %s : a ';' should be there after %s", reader->get_filename(), buf_load );
				continue;
			}
			// skip =
			if( !reader->skip_space_or_ctrl_until_char( '=' ) )
			{
				BOX_ERR( "File %s\nblank statement because = should be there after :\n%s", reader->get_filename(), buf_load );		
				b_found = false;
			}
			else if( b_verbose )
			{
				VERBOSE_PRINT_STRING( "= " );
			}
			// scan if ok or skip
			bool b_lock_message = false;
			if( b_found )
			{
				switch( type )
				{
				case TYPE_REF:
				case TYPE_STR:
				case TYPE_FILENAME:
				case TYPE_DIRNAME:

//				case TYPE_GROUP:
//				case TYPE_GROUP_CLOSED:

				case TYPE_CLASS_BRANCH:
				case TYPE_SYMBOLIC:
				case TYPE_SYMBO_NEG:
				case TYPE_SYMBO_ZERO:
					{
						AAA_ERR err = reader->my_scan_str( buf_load, BUF_LOAD_SIZE-1 );
						if( ERR(err) )
						{
							if( err == ERR_OUT_OF_BOUND )
								BOX_ERR( "File %s\nparsing param %s read buffer is too small for this param.", reader->get_filename(), param->get_name().get() );
							else
								BOX_ERR( "File %s\nparsing param %s, Error %d", reader->get_filename(), param->get_name().get(), err );
							break;
						}
	//					if( param->get_type() == TYPE_FILENAME )
	//					{
	//					}
						if( b_verbose )
						{
							if( *buf_load )
								PRINT_STRING("\"%s\" ", buf_load );
							else
								PRINT_STRING("\"\" ");
						}
#if	AAA_DEBUG()
//						if( str_is_equal( buf_load, "CURRENT" ) && !str_is_equal(  param->get_name().get(), "FBO" ))
//							ebug_break( "File %s : param %s.", reader->get_filename(), param->get_name().get() );
#endif
						//todoq should we use the update function somewhere ?
					}
					break;
				default:
					if( reader->scan_str_till( buf_load, BUF_LOAD_SIZE-1, ';' )<=0 )
					{
						BOX_ERR( "File %s\nunexpected end of file parsing param %s.", reader->get_filename(), param->get_name().get() );
						break;
					}
					if( b_verbose )
						PRINT_STRING( "%s ", buf_load );
					//todoq should we use the update function somewhere ?
					//todo deal with TYPE_BIT32			
					break;
				}
				if( param->is_changeable() )
					param->set_value_from_str( buf_load );
				else
					b_lock_message = true;
			}
			else
			{
				if( reader->scan_str_till( buf_load, BUF_LOAD_SIZE-1, ';' )<=0 )
				{
					BOX_ERR( "File %s\nunexpected end of file.", reader->get_filename() );
					break;
				}
				if( b_verbose )
					PRINT_STRING("%s ", buf_load );
			}
			//	find ; or trax
			do
			{
				//todo should check error here and there
				//todo should resync on next line in case of error
				//			search for next ;
				//if( fscanf( file, "%s", buf_load) != 1 )
				len = reader->scan_str( buf_load, BUF_LOAD_SIZE-1 );
				if( len <= 0 )
				{
					BOX_ERR( "File %s\nunexpected end of file.", reader->get_filename() );
					break;
				}
				if( b_verbose )
					VERBOSE_PRINT_STRING( "%s", buf_load );
				if( *buf_load == ';' )
					break;
				else if( str_is_equal_nocase( buf_load, "trax", 4) && (len >= 9) )
				{
#if AAA_STATE_COMPILE()
					if( !c_state_master::is_state_ref() )
					{	//we don't connect in this case
					}
					else
#endif //AAA_STATE_COMPILE
					if( b_found )
					{
						bool	b_plug_in;
						c_traxs*	traxs;
						INT32	trax_id;

						trax_id = 10 * (*(buf_load+7) - '0');
						trax_id += *(buf_load+8) - '0';

						traxs = nullptr;
						if( *(buf_load+9) != 0)
							b_plug_in = str_is_equal_nocase( buf_load+9, "in", 2);
						else
							b_plug_in = false;

						switch ( *(buf_load+4) )
						{
						case 'G' :
							if( c_modules::get_cur() )
								traxs = c_modules::get_cur()->get_traxs();
							else
								BOX_ERR( "File %s\nCan't plug Global trax %d when they don't exists yet", reader->get_filename(), trax_id );
							break;
						case 'M' :
							if( c_module::get_cur() )
								traxs = c_module::get_cur()->get_traxs();
							else
								BOX_ERR( "File %s\nCan't plug Module trax %d when they don't exists yet", reader->get_filename(), trax_id );
							break;
						case 'L' :
							//hack why ?
							if( c_layers::get_cur() )
								traxs = c_layers::get_cur()->get_traxs_always();
							else
								BOX_ERR( "File %s\nNo local traxs possible here", reader->get_filename() );
							break;	//local
						default:
							BOX_ERR( "File %s\nDon't know this kind of traxs: %s", reader->get_filename(), buf_load );
							break;
						}
						if( traxs )
						{
							if( b_plug_in )
								traxs->trax_plug_in( trax_id-1, obj, param );		
							else
								traxs->trax_plug_out( trax_id-1, obj, param );
							b_lock_message = false;
						}
					}
					else
					{
						BOX_ERR( "File %s\nskipping %s\nbecause the param was not found.", reader->get_filename(), buf_load );
					}
				}
				else
				{	
					if( *buf_load=='}' && *(buf_load+1)==0 )
					{
						BOX_ERR( "File %s\n!!\tparsing for\n\t; or TraxXX or AddFile :\n\tgot a } instead\n\ttrying to interpret it as ;", reader->get_filename() );
						break;
					}
					else
					{
						BOX_ERR( "File %s\n!!\tparsing for\n\t; or TraxXX or AddFile :\n\tgot\n\t%s", reader->get_filename(), buf_load );
					}
				}

			}
			while( true );

			if( b_lock_message )
			{
				obj->dbg_print( "%s() param %s is locked, did not change it", __FUNCTION__, param->get_name().get() );
				debug_break( "could be a file saved by a previous version when the param was not locked" );
			}
		}
	}
	while( true );

	obj_loading = nullptr;

	SPY_POP_RANGE();
	return AAA_OK;
}

//todo really get version number and check name
//todo		and then check caller
FINLINE	AAA_ERR	c_param::load_begin( CHAR* title, INT32* version )
{
	CHAR	str[256];
	//get title
	if( title )	//todo protect from overrun on str and title
	{
		*title = 0;
		if( reader->scan_str( str, 255 ) <= 0 )
			goto exit;
		if( b_verbose )
			VERBOSE_PRINT_STRING( "Title : %s ", str );
		strcpy( title, str );
	}
	//get version
	if( version )
	{
		*version = 0;
		if( !reader->scan_until_str_nocase( "version" ) )
			goto exit;
		if( reader->scan_int32( *version ) <= 0 )
			goto exit;
		if( b_verbose )
			VERBOSE_PRINT_STRING( "Version : %d ", *version );
	}
//scan till { CHAR
	if( !reader->scan_until_str_nocase( "{" ) )
		goto exit;

	return	AAA_OK;
exit:
	return	ERR_ANY;
}

FINLINE	void	c_param::load_end()
{
	if( b_verbose )
		VERBOSE_PRINT_STRING("}\n");
}

AAA_ERR	c_param::load_file_type( CONST char* CONST filename, char* title, INT32* version )
{
	SPY_PUSH_RANGE( filename, spy::FILE_LOW );
		AAA_ERR	ret = load_open( filename );
		if( NOERR(ret) )
		{
			ret = load_begin( title, version );
			if( ERR(ret) )
				BOX_ERR( "Could not read the header of %s.\nthis is probably an empty file.", filename );
			load_end();
			load_close();
		}
		else
			BOX_ERR( "Could not read the file %s.", filename );
	SPY_POP_RANGE();

	return ret;
}

FINLINE	AAA_ERR	c_param::list_load_from_stream( c_obj_ui* obj )
{
	AAA_ERR	ret = ERR_ANY;
	INT32	version;
	if( NOERR( load_begin( nullptr, &version ) ) )
	{
		ret = list_load( obj );
		load_end();
		obj->set_file_version( version );
	}
	return ret;
}

//todo deal everywhere with the version stuff
AAA_ERR	c_param::list_load_from_file( o_str CONST & filename, c_obj_ui* obj )
{
	SPY_PUSH_RANGE( filename, spy::INFRA );
		AAA_ERR	ret = load_open( filename );
		if( NOERR(ret) )
		{
			ret = list_load_from_stream( obj );
			load_close();
		}
	SPY_POP_RANGE();
	return ret;
}

void	c_param::c_init()
{
	reader_alloc();
	alloc_buf();
}
void	c_param::c_deinit()
{
	reader_dealloc();
	dealloc_buf();
}

bool	c_param::action( aaa::param::ACTION	CONST action )
{
	bool retcode = true;
	switch( action )
	{
	case aaa::param::PARAM_PREV:	retcode = param_focus_set_on_line( focus_param::get_line() - 1 );	break;
	case aaa::param::PARAM_NEXT:	retcode = param_focus_set_on_line( focus_param::get_line() + 1 );	break;
	//todoqqq revive fifo_switch
	case aaa::param::PARAM_POP:		c_fifo::pop();	break;
	//todoqqq revive fifo_switch
	case aaa::param::PARAM_PUSH:	c_fifo::push();	break;
	default:
		retcode = false;
		{
			p_param	param = focus_param::get_param();
			if( param )
			{
				retcode = ui::param_do_action( param, action );
				if( !retcode )	//todo check these
					retcode = param->do_action( action );
			}
		}	
		break;
	}
	return retcode;
}
