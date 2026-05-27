#include "dialog_str.h"
#include "resource.h"
#include "infrastructure/bind/bind.h"
#ifdef	WIN32
#	include "commctrl.h"	//for drag list
#	include "platform/win32/win_dlg_util.h"
#endif
#include "text_dlg.h"
#include "language/lua/aaalua_wrap.h"

#include "ui/AAA_event.h"
#include "system/shared/SystemUtils.h"

#ifdef	WIN32
#	include <lib_use.h>
	AAA_LIB_USE_MESSAGE( "comctl32" )
#endif


c_dialog_wrapper_str::c_dialog_wrapper_str( c_obj_ui* obj) : c_dialog_wrapper(obj)
	,_b_control_no		{false}
	,_b_text_changing	{false}
{
	_bind = nullptr;
}

c_dialog_wrapper_str::~c_dialog_wrapper_str()
{
}

void c_dialog_wrapper_str::set_bind_list( c_bind* in )
{
	_bind = in;
}

void c_dialog_wrapper_str::list_set( c_bind* bl, INT32 index )
{
	if( bl && bl == _bind )
	{
		HWND	hd_list = get_dlg_item( IDC_LIST ); 
		if( index != LB_ERR )
		{
			INT32	tmp = (INT32) SendMessage( hd_list, LB_GETCURSEL, 0, 0 );
			SendMessageA( hd_list, LB_INSERTSTRING, index, (LPARAM) bl->get_str(index) );
			SendMessage( hd_list, LB_SETCURSEL, tmp, 0 );
	//		SendMessage( hd_list, LB_SETITEMDATA, index, key );
			SendMessage( hd_list, LB_DELETESTRING, index+1, 0 );
		}
	}
}

void c_dialog_wrapper_str::list_set_all( INT32 index )
{
	std::lock_guard<aaa::MUTEX_RECURSIVE> guard(access);
	get_param()->get_value_as_str( _o_buf );
	_bind->set_item( index, _o_buf );

	for( auto const & p_wrapper : wrappers )
	{
		if( auto* p = dynamic_cast<c_dialog_wrapper_str*>(p_wrapper) )
			p->list_set( _bind, index );
	}
}

void c_dialog_wrapper_str::list_move_one( c_bind* bl, INT32 dst, INT32 src )
{
	HWND	hd = get_dlg_item( IDC_LIST ); 
	if( hd && bl && bl == _bind )
	{
		DLG_PRINT_STRING( "one with dst = %d, src = %d", dst, src );
		SendMessage( hd, LB_DELETESTRING, src, 0 );
		SendMessageA( hd, LB_INSERTSTRING, dst, (LPARAM) _bind->get_str( dst ) );
	//			SendMessage( hd, LB_SETITEMDATA, dst, dragged_data );

	}
}

bool c_dialog_wrapper_str::list_move( HWND hd_list, INT32 dst, INT32 src )
{
	bool	retcode = false;
	std::lock_guard<aaa::MUTEX_RECURSIVE> guard(access);
	if( dst >= 0 && src != dst )
	{
		DLG_PRINT_STRING( "drag dst = %d, src = %d", dst, src );
		_bind->swap_item( dst, src );

		for( auto const & p_wrapper : wrappers )
		{
			if( auto* p = dynamic_cast<c_dialog_wrapper_str*>(p_wrapper) )
				p->list_move_one( _bind, dst, src );
		}

		SendMessage( hd_list, LB_SETCURSEL, dst, 0 );
		retcode = true;
	}
	return retcode;
}

void c_dialog_wrapper_str::init_list()
{
	INT32	i_max_nb = get_bind_list()->get_elt_nb();
	HWND	hd = get_dlg_item( IDC_LIST );
	SendMessage( hd, LB_RESETCONTENT, 0, 0 );
	for( INT32 i = 0; i < i_max_nb; ++i )
	{
		SendMessageA( hd, LB_ADDSTRING, 0, (LPARAM) get_bind_list()->get_str(i) );
//		SendMessage( hd, LB_SETITEMDATA, i, (LPARAM) i); 
	}
	if( MakeDragList(hd ) != 0 )
		_drag_message = RegisterWindowMessage( DRAGLISTMSGSTRING );
	else
		DBG_PRINT_STRING( "Can't make list drageable" );
}

void	c_dialog_wrapper_str::set_control_no( bool b )
{
	_b_control_no = b;
	_idc_typein_to_use = _b_control_no ? IDC_TYPEIN_LINE_ONE : IDC_TYPEIN;
}

void c_dialog_wrapper_str::do_text_changed( C_PCHAR_C str )
{
 	if( !_b_text_changing )	//avoid reentry and so calling back lua also
	{
		_b_text_changing = true;
		DBG_PRINT_STRING( "%s() called from %s", __FUNCTION__, str );
		aaa::dialog::get_str( get_hd(), _idc_typein_to_use, _o_buf );
		if( _b_control_no && _o_buf.remove_control() )
		{
			aaa::dialog::set_str( get_hd(), _idc_typein_to_use, _o_buf.get() );
		}
		get_param()->set_value_str( _o_buf );
		if( is_dlg_but_checked(IDC_SYNC) )
		{
			HWND	hd_list	= get_dlg_item( IDC_LIST ); 
			INT32	index	= (INT32) SendMessage( hd_list, LB_GETCURSEL, 0, 0 ); 
	//		INT32	key = SendMessage( hd_list, LB_GETITEMDATA, index, 0 );
			if( index != LB_ERR )
				list_set_all( index );
		}
		if( _b_lua_callback )
		{
			//DWORD store_begin = _sel_begin;
			//DWORD store_end	  = _sel_end;  
			// call lua passing the new value
			bool b_ok = g_lua_wrap_master->do_fn_b_pass_iss( nullptr, "aaa.dialog_hook", _lua_id, "change_value", _o_buf.get() );
			// check if lua change the value
			if( b_ok )
			{
				//_sel_prev_begin = _sel_begin;
				//_sel_prev_end   = _sel_end;
				//get_control_str_select( get_hd(), _idc_typein_to_use, _sel_begin, _sel_end, "lua_ok" );
			}
			else
			{
				get_param()->get_value_as_str( _o_tmp );
				DBG_PRINT_STRING( "lua altered change" );		
				aaa::dialog::set_str( get_hd(), _idc_typein_to_use, _o_tmp.get() );
				aaa::dialog::set_str_select( get_hd(), _idc_typein_to_use, _sel_prev_begin, _sel_prev_end, "lua_refuse" );
			}
		}
		_b_text_changing = false;
	}
//	get_obj()->callback_change( _pstr );
}


bool c_dialog_wrapper_str::do_scroll( WPARAM wParam, LPARAM lParam )
{
	HWND	hd_scroll = (HWND)lParam;
	bool	b_ret = false;

	WORD w_param_high = HIWORD(wParam);
	WORD w_param_low  = LOWORD(wParam);

	switch( w_param_low ) 
	{
	case SB_LINELEFT:	//LINEUP
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_LINELEFT with %d", __FUNCTION__, w_param_high );
		break;
	case SB_LINERIGHT:	//LINEDOWN
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_LINERIGHT with %d", __FUNCTION__, w_param_high );
		break;
	case SB_PAGELEFT:	//PAGEUP
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_PAGELEFT with %d", __FUNCTION__, w_param_high );
		break;
	case SB_PAGERIGHT:	//PAGEDOWN
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_PAGERIGHT with %d", __FUNCTION__, w_param_high );
		break;
	case SB_THUMBTRACK:	
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_THUMBTRACK with %d", __FUNCTION__, w_param_high );
		SetScrollPos( (HWND)lParam, SB_CTL, w_param_high, TRUE); 
		b_ret = true;
		break;
	case SB_THUMBPOSITION:
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_THUMBPOSITION with %d", __FUNCTION__, w_param_high );
		break;
	case SB_ENDSCROLL:
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_ENDSCROLL with %d", __FUNCTION__, w_param_high );
		break;
	case SB_LEFT:	//TOP
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_LEFT with %d", __FUNCTION__, w_param_high );
		break;
	case SB_RIGHT:	//BOTTOM
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s SB_RIGHT with %d", __FUNCTION__, w_param_high );
		break;
	default:
		if( c_event::is_verbose() )
			DLG_PRINT_STRING( "%s WM_HSCROLL from %d with %d", __FUNCTION__, w_param_low, w_param_high );
		break;
	}
	return b_ret;
}

bool c_dialog_wrapper_str::do_callback( UINT message, WPARAM wParam, LPARAM lParam )
{ 
	//HEAP_CHECK();
	DLG_PRINT_STRING( "%s() message 0x%x in", __FUNCTION__, message );
	
	WORD w_param_high = HIWORD(wParam);
	WORD w_param_low  = LOWORD(wParam);

	//	drag message are send this way
	if( _drag_message && message == _drag_message )
	{
		DRAGLISTINFO*	pdrag = (DRAGLISTINFO*)lParam;
		DLG_PRINT_STRING( "%s() DRAG for control 0x%x with notification 0x%x", __FUNCTION__, w_param_low, w_param_high );
		switch( pdrag->uNotification ) 
		{
		case DL_BEGINDRAG:
			// Keep track of the item number that is being dragged.
			// In this case, store the number in the dialog box's
			// DWL_USER position.
			//	SetWindowLong( hwnd, DWL_USER, LBItemFromPt( dli->hWnd, dli->ptCursor, FALSE));
			_drag_index_start = _drag_index = LBItemFromPt( pdrag->hWnd, pdrag->ptCursor, TRUE );
//			dragged_data = SendMessage( pdrag->hWnd, LB_GETITEMDATA, drag_index_, 0 );
			// Store the return value in DWL_MSGRESULT.
#if AAA_WIN64()
			SetWindowLong( get_hd(), DWLP_MSGRESULT, TRUE);
#else
			SetWindowLong( get_hd(), DWL_MSGRESULT, TRUE);
#endif
			if( c_event::is_verbose() )
				DLG_PRINT_STRING( "%s() DL_BEGINDRAG of %d", __FUNCTION__, _drag_index );
			return true;
		case DL_CANCELDRAG:
			if( c_event::is_verbose() )
				DLG_PRINT_STRING( "%s() DL_CANCELDRAG", __FUNCTION__ );
			list_move( pdrag->hWnd, _drag_index_start, _drag_index );
			return true;
		case DL_DRAGGING:
			{
				INT32	index = LBItemFromPt( pdrag->hWnd, pdrag->ptCursor, TRUE );
//				if( index >= 0 )
//					DrawInsert( get_hd(), pdrag->hWnd, index);
				if( list_move( pdrag->hWnd, index, _drag_index ) )
					_drag_index = index;
			}
			if( c_event::is_verbose() )
				DLG_PRINT_STRING( "%s() DL_DRAGGING", __FUNCTION__ );
			return true;
		case DL_DROPPED:
			{
			}
			return true;
		default:
			if( c_event::is_verbose() )
				DLG_PRINT_STRING( "%s() DRAGLISTINFO with uNotification 0x%x", __FUNCTION__, pdrag->uNotification );
			return true;
		}
	}
	else switch( message ) 
	{
	case WM_PARENTNOTIFY:
	case WM_SETFONT:
	case WM_TIMER:
		return false;

	case WM_INITDIALOG:
		do_begin( "string" );
		//good but bad idea on top of opengl
		//	make dialog transparent Set WS_EX_LAYERED on this window 
		//SetWindowLong( get_hd(), GWL_EXSTYLE, GetWindowLong( get_hd(), GWL_EXSTYLE) | WS_EX_LAYERED);
			//SetLayeredWindowAttributes( get_hd(), 0, (255 * 70) / 100, LWA_ALPHA);
		HWND hd;
		if( hd = get_dlg_item( IDC_TYPEIN ) )
		{
			ShowWindow( hd, _b_control_no ? SW_HIDE : SW_SHOW );
			EnableWindow( hd, _b_control_no ? FALSE : TRUE );
			//get_dlg_item( IDC_TYPEIN )->EnableWindow( !_b_control_no );
			//SendMessage( hd, EM_LIMITTEXT, (WPARAM) 256*1024, (LPARAM) 0);
		}
		if( hd = get_dlg_item( IDC_TYPEIN_LINE_ONE ) )
		{
			ShowWindow( hd, _b_control_no ? SW_SHOW : SW_HIDE );
			EnableWindow( hd,  _b_control_no ? TRUE : FALSE );
			//SendMessage( hd, EM_LIMITTEXT, (WPARAM) 1024, (LPARAM) 0);
		}
		CheckDlgButton( get_hd(), IDC_DIRECT, TRUE);
	
		init_list();

//		SendDlgItemMessage( get_hd(), IDC_TYPEIN, WM_SETFOCUS, 0, 0);
//		SendMessage( get_hd(), WM_NEXTDLGCTL, (WPARAM) get_dlg_item(IDC_TYPEIN), TRUE );


		get_param()->get_value_as_str( _value_initial );
		aaa::dialog::set_str( get_hd(), _idc_typein_to_use, _value_initial.get() );
		aaa::dialog::get_str_select( get_hd(), _idc_typein_to_use, _sel_begin, _sel_end, "WM_INITDIALOG" );
		_sel_prev_begin = _sel_begin;
		_sel_prev_end   = _sel_end;

		do_text_changed( "WM_INITDIALOG" );

//		SendDlgItemMessage(  get_hd(), _idc_typein_to_use, EM_TAKEFOCUS,	(WPARAM) 0,		(LPARAM) 0 );
//		SendDlgItemMessage(  get_hd(), _idc_typein_to_use, EM_SETSEL,		(WPARAM) 0,		(LPARAM) -1 );
//		set_control_str( get_hd(), _idc_typein_to_use, _pstr->get() );
		return true;  
	case WM_HSCROLL:
		return do_scroll( wParam, lParam );
	case WM_COMMAND: 
		// Set the default push button to "OK" when the user 
		// enters text. 
/*		if( w_param_high == EN_CHANGE
			&& LOWORD(wParam) == IDE_PASSWORDEDIT) 
		{
			SendMessage( hDlg,	DM_SETDEFID, 
								(WPARAM) IDOK, 
								(LPARAM) 0);
		}
*/
		DLG_PRINT_STRING( "%s() VM_COMMAND for control 0x%x with notification 0x%x", __FUNCTION__, w_param_low, w_param_high );
		switch( w_param_low ) 
		{
		case IDCANCEL:
			aaa::dialog::set_str( get_hd(), _idc_typein_to_use, _value_initial.get() );
			do_text_changed( "IDCANCEL" );
			do_end( 0, "cancel" );
			return true;
		case IDOK:
			do_text_changed( "IDOK" );
			do_end( 1, "ok" );
			return true;
		case IDC_APPLY:
			do_text_changed( "IDC_APPLY" );
			return true;
		case IDC_REVERT:
			aaa::dialog::set_str( get_hd(), _idc_typein_to_use, _value_initial.get() );
			do_text_changed( "IDC_REVERT" );
			return true;
		case IDC_TYPEIN:
		case IDC_TYPEIN_LINE_ONE:
			DLG_PRINT_STRING( "%s() IDC_TYPEIN 0x%x", __FUNCTION__, w_param_high );
			switch( w_param_high ) 
			{ 
			case EN_CHANGE:
				DLG_PRINT_STRING( "%s() IDC_TYPEIN EN_CHANGE", __FUNCTION__ );
				//get_control_str_select( get_hd(), _idc_typein_to_use, _sel_begin, _sel_end, "EN_CHANGE" );
				if( !get_dlg_item( IDC_DIRECT) || is_dlg_but_checked( IDC_DIRECT) )	// if no button or button exist and check
					do_text_changed( "EN_CHANGE" );
				return true;
			case EN_SETFOCUS:
				DLG_PRINT_STRING( "%s() IDC_TYPEIN EN_SETFOCUS not used", __FUNCTION__ );
				break;
			case EN_KILLFOCUS:
				DLG_PRINT_STRING( "%s() IDC_TYPEIN EN_KILLFOCUS not used", __FUNCTION__ );
				break;
			case EN_UPDATE:
 				DLG_PRINT_STRING( "%s() IDC_TYPEIN EN_UPDATE", __FUNCTION__ );
				//get_control_str_select( get_hd(), _idc_typein_to_use, _sel_begin, _sel_end, "EN_UPDATE" );
				break;
			default:
				DLG_PRINT_STRING( "%s() IDC_TYPEIN 0x%x not used", __FUNCTION__, w_param_high );
				break;
			}
			break;
		case IDC_SYNC:
			DLG_PRINT_STRING( "%s() IDC_SYNC 0x%x", __FUNCTION__, w_param_high );
			do_text_changed( "IDC_SYNC" );
			return true;
		case IDC_DIRECT:
			if( is_dlg_but_checked( IDC_DIRECT ) )
				do_text_changed( "IDC_DIRECT" );
			return true;
		case IDC_GENE:
			{
				CHAR	typein[aaa::dialog::STR_LEN_MAX];
				do_text_changed( "IDC_GENE" );
				//todo do a fn for this ?
				get_param()->get_value_as_str( _o_buf );
				strcpy( typein, _o_buf.get() );
				//todoqq generate_str and execute_str with o_str
				generate_str( "generate", typein, aaa::dialog::STR_LEN_MAX );
				get_param()->set_value_str( typein );
				aaa::dialog::set_str( get_hd(), _idc_typein_to_use, typein );
				//printf( "type In : %s\n", typein );
			}
			return true;
		case IDC_EXEC:
			{
				CHAR	typein[aaa::dialog::STR_LEN_MAX];
				do_text_changed( "IDC_APPLY" );

				get_param()->get_value_as_str( _o_buf );
				strcpy( typein, _o_buf.get() );
				execute_str( typein, aaa::dialog::STR_LEN_MAX );
				get_param()->set_value_str( typein );
				aaa::dialog::set_str( get_hd(), _idc_typein_to_use, typein );
			}
			return true;
		case IDC_LIST:
			switch( w_param_high )
			{
			case LBN_SELCHANGE:
				{
					HWND hd_list = get_dlg_item( IDC_LIST );
					INT32 index = (INT32) SendMessage( hd_list, LB_GETCURSEL, 0, 0 );
					//INT32 index = SendMessage( hd_list, LB_GETITEMDATA, index, 0 );
					if( index != LB_ERR )
					{
						get_param()->set_value_str( get_bind_list()->get_o_str(index) );
						get_param()->get_value_as_str( _o_buf );
						aaa::dialog::set_str( get_hd(), _idc_typein_to_use, _o_buf.get() );
					}
				}
				return true;
			default:
				DLG_PRINT_STRING( "%s() IDC_LIST 0x%x not used", __FUNCTION__, w_param_high );
				break;
			}
			break;
		default:
			DLG_PRINT_STRING( "%s() VM_COMMAND no known control", __FUNCTION__ );
			break;
		} 
		break;
	case WM_CTLCOLOREDIT:
		//DLG_PRINT_STRING( "%s() WM_CTLCOLOREDIT wParam 0x%x 0x%x", __FUNCTION__, w_param_low,    w_param_high );
		//DLG_PRINT_STRING( "%s() WM_CTLCOLOREDIT lParam 0x%x 0x%x", __FUNCTION__, LOWORD(lParam), HIWORD(lParam) );
 		DLG_PRINT_STRING( "%s() WM_CTLCOLOREDIT", __FUNCTION__ );
		_sel_prev_begin = _sel_begin;
		_sel_prev_end   = _sel_end;
		aaa::dialog::get_str_select( get_hd(), _idc_typein_to_use, _sel_begin, _sel_end, "WM_CTLCOLOREDIT" );	
		break;
	case SPI_GETMOUSEDRAGOUTTHRESHOLD:
		DLG_PRINT_STRING( "%s() SPI_GETMOUSEDRAGOUTTHRESHOLD unused", __FUNCTION__ );
		break;
	case SPI_SETMOUSEDOCKTHRESHOLD:
		DLG_PRINT_STRING( "%s() SPI_SETMOUSEDOCKTHRESHOLD unused", __FUNCTION__ );
		break;
	case WM_SETCURSOR:
		DLG_PRINT_STRING( "%s() WM_SETCURSOR", __FUNCTION__ );
		aaa::dialog::get_str_select( get_hd(), _idc_typein_to_use, _sel_begin, _sel_end, "WM_SETCURSOR" );
		_sel_prev_begin = _sel_begin;
		_sel_prev_end   = _sel_end;
		break;
	default:
		if( c_event::is_verbose() )
		{
			DBG_PRINT_STRING( "%s() message 0x%x unused", __FUNCTION__, message );
			print_callback_event( get_hd(), message, wParam, lParam );
		}
		break;
	} 
	return false; 
} 
