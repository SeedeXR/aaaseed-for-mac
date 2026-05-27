#include "dialog_number.h"
#ifdef	WIN32
#	include "platform/win32/win_dlg_util.h"
#endif
#include "resource.h"
#include "language/lua/aaalua_wrap.h"
#include "ui/AAA_event.h"



c_dialog_wrapper_number::c_dialog_wrapper_number( c_obj_ui* obj, p_param param )
	: c_dialog_wrapper(obj)
	,_b_integer		{false}
{
	set_param( param );
}

c_dialog_wrapper_number::~c_dialog_wrapper_number()
{
}

void c_dialog_wrapper_number::update()
{
}

void	c_dialog_wrapper_number::set_edit( DOUBLE r )
{	
	if( _b_integer )
		aaa::dialog::set_int32( get_hd(), IDC_REALIN, INT32(r) );
	else
		aaa::dialog::set_real(  get_hd(), IDC_REALIN, REAL(r)  );
}

void	c_dialog_wrapper_number::output_edit()	
{
	if( _b_integer )
	{
		INT32 out_value_int;
		aaa::dialog::get_int32( get_hd(), IDC_REALIN, out_value_int );
		_out_value = out_value_int;
		get_param()->set_value_num_from_double( _out_value );		
		if( _b_lua_callback )
			g_lua_wrap_master->do_fn_b_pass_isi( nullptr, "aaa.dialog_hook", _lua_id, "change_value", out_value_int );
	}
	else
	{
		aaa::dialog::get_double( get_hd(), IDC_REALIN, _out_value );
		get_param()->set_value_num_from_double( _out_value );
		if( _b_lua_callback )
			g_lua_wrap_master->do_fn_b_pass_isd( nullptr, "aaa.dialog_hook", _lua_id, "change_value", _out_value );
	}
}

bool c_dialog_wrapper_number::do_callback( UINT message, WPARAM wParam, LPARAM lParam )
{ 
	switch( message ) 
	{
	case WM_INITDIALOG:
		do_begin( _b_integer ? "integer" : "fp64" );

		{
			DOUBLE	d = get_param()->get_value_as_double();
			set_edit( d );
			_value_initial = d;
	/*			SendMessage( hDlg, DM_SETDEFID, 
				(WPARAM) IDOK, 
				(LPARAM) 0); 
	*/
			auto hd = get_hd();
			CheckDlgButton( hd, IDC_SYNC_TO, FALSE );
			CheckDlgButton( hd, IDC_SYNC_TO, TRUE );
	//		SendDlgItemMessage( hd, IDC_REALIN,	EM_REPLACESEL,		(WPARAM) TRUE,	(LPARAM) tmp );
		}
		return true; 

	case WM_COMMAND: 
		// Set the default push button to "OK" when the user 
		// enters text. 
/*			if( HIWORD (wParam) == EN_CHANGE
			&& LOWORD(wParam) == IDE_PASSWORDEDIT) 
			{
			SendMessage( hDlg, DM_SETDEFID, 
				(WPARAM) IDOK, 
				(LPARAM) 0); 
			}
*/
		switch(wParam) 
		{
		case IDCANCEL:
			set_edit( _value_initial );
		case IDOK: 
			output_edit();
			if( wParam == IDOK )
				do_end( 1, "ok" );
			else
				do_end( 0, "cancel" );
			return true;
		case IDC_APPLY:
			output_edit();
			return true;
		case IDC_REVERT:
			set_edit( _value_initial );
			return true;
		case IDC_DEF:
			set_edit( get_param()->get_def() );
			return true;
		case IDC_MIDDLE:
			set_edit( (get_param()->get_def()+get_param()->get_ina())*REAL(.5) );
			return true;
		case IDC_INA:
			set_edit( get_param()->get_ina() );
			return true;
		case IDC_MIN:
			set_edit( get_param()->get_min() );
			return true;
		case IDC_MAX:
			set_edit( get_param()->get_max() );
			return true;
		} 
		return false;

	case WM_CTLCOLOREDIT:
		if( is_dlg_but_checked( IDC_SYNC_FROM ) )
			{
			DOUBLE	r = get_param()->get_value_as_double();
			if( r != _out_value  )
				{
				}
			}
		if( is_dlg_but_checked( IDC_SYNC_TO ) )
			output_edit();
		return true;	//todo was false ???

	case WM_PARENTNOTIFY:
	case WM_SETFONT:
	case WM_TIMER:
	case WM_SETCURSOR:
		return false;

	case WM_SYSKEYDOWN:
	case WM_MOUSEMOVE:
	case WM_MENUCHAR:
		return false;	// to avoid message, and to implement someday 
	default:
		if( c_event::is_verbose() )
		{
			DBG_PRINT_STRING( "%s() message 0x%x unused", __FUNCTION__, message );
			print_callback_event( get_hd(), message, wParam, lParam );
		}
		return false;
	}
	return false;
//	(lParam);
}
