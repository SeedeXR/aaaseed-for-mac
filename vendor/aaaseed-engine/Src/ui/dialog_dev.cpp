#include "dialog_dev.h"
#include "resource.h"
#include "platform/win32/win_dlg_util.h"


//todo add a dialog wrapper so lua can deal with it too
namespace n_dialog
{

struct st_dlg_dev_info
{
	C_PCHAR	title;
	C_PCHAR	text;
	C_PCHAR	other;
	INT32	id_icon;
};
st_dlg_dev_info dlg_dev_info = { nullptr, nullptr, nullptr, 0 };

void disable_close_button( HWND hwnd )
{
    EnableMenuItem( GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
}

INT_PTR CALLBACK	dev_proc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER(lParam); 
	switch( message )
	{
	case WM_INITDIALOG:
		if( dlg_dev_info.id_icon )
		{
			//todo free at some point
			HBITMAP hd_bitmap = LoadBitmap( GetModuleHandle(nullptr), MAKEINTRESOURCE(dlg_dev_info.id_icon) );
			SendDlgItemMessage( hDlg, IDC_DEVIMAGE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hd_bitmap );
		}
		disable_close_button( hDlg );
		{
			HWND item = GetDlgItem( hDlg, IDOTHER );   
			if( item )
			{
				if( dlg_dev_info.other )
					SetWindowTextA( item, dlg_dev_info.other );
				// Show/Hide the control
				ShowWindow( item, dlg_dev_info.other ? SW_SHOW : SW_HIDE );
			}
		}
		//SendDlgItemMessage( hDlg, ID_OTHER, dlg_dev_info.b_other ? SW_SHOW : SW_HIDE, 0,0 );

		//set_control_str( hDlg, IDE_DEV_MESSAGE, dlg_dev_info.text );
		SetWindowTextA( hDlg, dlg_dev_info.title );
		//set_control_str( hDlg, IDC_DEV_MESSAGE_STATIC, dlg_dev_info.text );
		aaa::dialog::set_str( hDlg, IDC_DEV_MESSAGE_EDIT,	dlg_dev_info.text );
		// SendMessage( hDlg, DM_SETDEFID, (WPARAM) IDOK, (LPARAM) 0 );
		// SendDlgItemMessage( hDlg, IDC_DEVIMAGE,	WM_PAINT,		(WPARAM)IMAGE_BITMAP,	(LPARAM)dlg_dev_info.hd );

		//::ShowWindow( hDlg, SW_SHOW );
		//::ShowWindow(	hDlg, SW_SHOWNOACTIVATE );
		//::SetWindowPos(	hDlg, HWND_TOP,		0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );

		// Set the default push button to "OK"
		SendMessage( hDlg, DM_SETDEFID, (WPARAM) IDOK, (LPARAM) 0 ); 
		return TRUE;

	case WM_COMMAND:
		// pass retcode returned by our dialog fn
		switch(wParam) 
		{
		case IDOTHER:	EndDialog( hDlg, RETURN_OTHER  );	return TRUE;
		case IDOK:		EndDialog( hDlg, RETURN_OK     );	return TRUE;
		case IDCANCEL:	EndDialog( hDlg, RETURN_CANCEL );	return TRUE;
		}

		break;
	}
	return FALSE;

}

INT32	do_dev( C_PCHAR_C title, C_PCHAR_C text, C_PCHAR_C button_other )
{
	dlg_dev_info.title	 = title;
	dlg_dev_info.text	 = text;
	dlg_dev_info.other	 = button_other;
	dlg_dev_info.id_icon = IDB_LOGOMAA;
	return INT32(DialogBox( nullptr, MAKEINTRESOURCE(IDD_DEV), nullptr, (DLGPROC)dev_proc ));
}

INT32	do_lua( C_PCHAR_C title, C_PCHAR_C text, C_PCHAR_C button_other )
{
	dlg_dev_info.title	 = title;
	dlg_dev_info.text	 = text;
	dlg_dev_info.other   = button_other;
	dlg_dev_info.id_icon = IDB_LUA_POWERED;
	return INT32(DialogBox( nullptr, MAKEINTRESOURCE(IDD_LUA), nullptr, (DLGPROC)dev_proc ));
}

INT32	do_finger( C_PCHAR_C title, C_PCHAR_C text, C_PCHAR_C button_other )
{
	dlg_dev_info.title	 = title;
	dlg_dev_info.text	 = text;
	dlg_dev_info.other   = button_other;
	dlg_dev_info.id_icon = IDB_FINGER;
	return INT32(DialogBox( nullptr, MAKEINTRESOURCE(IDD_DEV), nullptr, (DLGPROC)dev_proc ));
}

INT32	do_show( C_PCHAR_C title, C_PCHAR_C text, C_PCHAR_C button_other )
{
	dlg_dev_info.title	 = title;
	dlg_dev_info.text	 = text;
	dlg_dev_info.other   = button_other;
	dlg_dev_info.id_icon = IDB_LOGOMAA;
	return INT32(DialogBox( nullptr, MAKEINTRESOURCE(IDD_SHOW), nullptr, (DLGPROC)dev_proc ));
}



} //end namespace n_dialog
