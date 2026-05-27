#include "ui/dialog.h"
#include "dialog_str.h"
#include "dialog_number.h"
#include "infrastructure/bind_text.h"
#include "resource.h"
#include "Thread/aaa_thread.h"


namespace {
	//	create the different c_dialog_wrapper
	c_dialog_wrapper_str*	init_dialog_text( p_param param, c_obj_ui* obj, bool b_control_no )	//par
	{
		c_dialog_wrapper_str* pdw = new c_dialog_wrapper_str( obj );
	//set the right info i
		pdw->set_param( param );
		pdw->set_bind_list( bind_text::bind );
		pdw->set_control_no( b_control_no );
		return pdw;
	}


	c_dialog_wrapper*	init_dialog_number( p_param param, c_obj_ui* obj, bool CONST b_integer )
	{
		c_dialog_wrapper_number* pdw = new c_dialog_wrapper_number( obj, param );
		//set the right info i
	//	pdw->set( param );
		pdw->set_type_integer( b_integer );
		return pdw;
	}
};

void	th_dialog( c_obj_ui* CONST obj, p_param CONST param, bool CONST b_simple, C_PCHAR_C title, bool CONST b_lua_callback, INT32 CONST lua_id, bool CONST b_integer )
{
	//todo check also on param
	if( c_dialog_wrapper::is_dialog_on_same_data( param ) )
		DBG_PRINT_STRING( "dialog already open on this param or data" );
	else
	{
		UINT32	res_id;
		c_dialog_wrapper* pdw = nullptr;
		auto type_internal = param->get_type_internal();
		switch( type_internal )
		{
		case aaa::param::TYPE_INTERNAL_STRING:
			res_id = b_simple ? IDD_TYPEIN_SIMPLE : IDD_TYPEIN;
			pdw = init_dialog_text( param, obj, b_simple );
			break;
		case aaa::param::TYPE_INTERNAL_INT32:
		case aaa::param::TYPE_INTERNAL_UINT32:	//todo deal with TYPE_BIT32
		case aaa::param::TYPE_INTERNAL_FP32:
		case aaa::param::TYPE_INTERNAL_DOUBLE:
			res_id = b_simple ? IDD_REALIN_SIMPLE : IDD_REALIN;
			pdw = init_dialog_number( param, obj, b_integer );
            break;
		default:
		//	load_data( (TYPE_IO) I_FLOOR(st->def), aaa::param::flat::obj_found );
			return;
		}

		if( pdw && res_id )
		{
			//add then it will be ready to attach in the callback fn
			c_dialog_wrapper::add( pdw );
			pdw->set_lua_callback( b_lua_callback, lua_id );
			pdw->set_title( title );
#if 1
			c_thread::begin( "do_dialog", do_dialog, 0, reinterpret_cast<void*>(static_cast<intptr_t>(res_id)) );
#else
//			c_thread::begin( "do_dialog_modeless", do_dialog_modeless, 0, reinterpret_cast<void*>(static_cast<intptr_t>(res_id)) );
			do_dialog_modeless( reinterpret_cast<void*>(static_cast<intptr_t>(res_id)) );
#endif
		}
		else
			debug_break( "%s() probably not enough memory", __FUNCTION__ ); 
	}
}
