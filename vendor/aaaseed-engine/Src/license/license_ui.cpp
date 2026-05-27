#include "license/license_ui.h"
#include "license.h"
#include "license/info.h"
#include "obj_ui/com/net.h"
#include "resource.h"
#ifdef	WIN32
#	include "platform/win32/win_dlg_util.h"
#endif
#include <lm.h>
#include "spy.h"
#include "aaaseed.h"
#include "infrastructure/aaa_window.h"

C_PCHAR_C toto[] =
{
	"ter8"
	"Maas"
	"choice",	//Maa

	"EL@I"
	"pORC"
	"Ne",		//Francis

	"us13"
	"$NeX"
	"7",		//Franz

	"itea"
	"Jhab"
	"eS44"
	"Nant",		//Etienne

	"9naj"		
	"xlA0"
	"n23c"
	"61da"
	"ed",		//Alex
	
	"ioZ4"
	"Symb"
	"484",		//Yaniss

	"A98G"
	"OFon"
	"auche",	//Nadege
};


INT_PTR CALLBACK	generate_proc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
//CHAR lpszPassword[16];
//WORD cchPassword;
	//print_callback_event( hDlg, message, wParam, lParam );

	switch (message)
	{
	case WM_INITDIALOG:
		// Set password character to a asterisk sign (*)
		SendDlgItemMessage( hDlg, IDE_EDITPASSWORD,
							EM_SETPASSWORDCHAR,
							(WPARAM) '*',
							(LPARAM) 0);
	
		// Set the default push button to "Cancel."
		aaa::dialog::set_str( hDlg, IDE_EDITLANGROUP,	net->get_langroup()			);
		aaa::dialog::set_str( hDlg, IDE_EDITMACHINE,	net->get_machine()			);
		aaa::dialog::set_str( hDlg, IDE_EDITVERSION,	license_get_version_short() );
		aaa::dialog::set_str( hDlg, IDE_EDITYEAR,		license_get_year_str()		);
		aaa::dialog::set_str( hDlg, IDE_EDITMONTH,		license_get_month_str()		);
		aaa::dialog::set_str( hDlg, IDE_EDITDAY,		license_get_day_str()		);
		aaa::dialog::set_str( hDlg, IDE_EDITUSER,		"Maa"						);

		SendMessage( hDlg, DM_SETDEFID, (WPARAM) IDOK, (LPARAM) 0 );

		//::ShowWindow(	hDlg, SW_SHOWNOACTIVATE );
		//::SetWindowPos(	hDlg, HWND_TOP,		0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );

		return TRUE;

	case WM_COMMAND:
		// Set the default push button to "OK" when the user 
		// enters text. 
/*		if( HIWORD (wParam) == EN_CHANGE
				&& LOWORD(wParam) == IDE_PASSWORDEDIT) 
		{
			SendMessage( hDlg, DM_SETDEFID, 
					(WPARAM) IDOK, 
					(LPARAM) 0); 
		}
*/
		switch(wParam) 
		{
		case ID_GENERATEKEY:
			{
				CHAR	license[256];
				CHAR	what[64];
				CHAR	langroup[256];
				CHAR	machine[256];
				CHAR	user[64];
				CHAR	version_short[64];
				C_PCHAR	from_who;
				INT32	year;
				INT32	month;
				INT32	day;
				CHAR	password_in[64];
				CHAR*	pt;

				aaa::dialog::get_int32( hDlg, IDE_EDITYEAR,		year	);
				aaa::dialog::get_int32( hDlg, IDE_EDITMONTH,	month	);
				aaa::dialog::get_int32( hDlg, IDE_EDITDAY,		day		);

				aaa::dialog::get_str( hDlg, IDE_EDITLANGROUP,	langroup,		256	);
				aaa::dialog::get_str( hDlg, IDE_EDITMACHINE,	machine,		256	);
				aaa::dialog::get_str( hDlg, IDE_EDITUSER,		user,			64	);
				aaa::dialog::get_str( hDlg, IDE_EDITVERSION,	version_short,	64	);	
				aaa::dialog::get_str( hDlg, IDE_EDITPASSWORD,	password_in,	64	);

				INT32	len = (INT32)strlen( password_in );
				len = len / 8 ;
				INT32*	p = (INT32*) password_in;
				for( ; len>0; --len )
				{
					SWAP( *p, *(p+1) );
					p = p + 2;
				}

				if(			strcmp( password_in, toto[0] ) == 0 )	from_who = "Maa";
				else if (	strcmp( password_in, toto[1] ) == 0 )	from_who = "Francis";
				else if (	strcmp( password_in, toto[2] ) == 0 )	from_who = "Franz";
				else if (	strcmp( password_in, toto[3] ) == 0 )	from_who = "Etienne";
				else if (	strcmp( password_in, toto[4] ) == 0 )	from_who = "Alex";
				else if (	strcmp( password_in, toto[5] ) == 0 )	from_who = "Yaniss";
				else if (	strcmp( password_in, toto[6] ) == 0 )	from_who = "Nadege";
				else												from_who = nullptr;

				license_generate_what( what, user, year, month, day, version_short, from_who );

				strcpy( license, what );
				if( from_who )
				{
					pt = license + strlen( license );
					*pt++ = ' ';
					license_generate_key( pt, what, langroup, machine );
				}

					aaa::dialog::set_str( hDlg, IDE_EDITLICENSE,	license );
			}
			break;
		case IDOK:

/*			// Get number of characters. 
			cchPassword = (WORD) SendDlgItemMessage( hDlg, IDE_EDITKEY,
										EM_LINELENGTH,
										(WPARAM) 0,
										(LPARAM) 0);

			if (cchPassword >= 16)
			{
				MessageBox(hDlg,
						"Too many characters.",
						"Error",
						MB_OK|MB_TOPMOST);
						EndDialog(hDlg, TRUE);
				return FALSE;
			}
			else if (cchPassword == 0)
			{
				MessageBox(hDlg,
						"No characters entered.",
						"Error",
						MB_OK|MB_TOPMOST);
						EndDialog(hDlg, TRUE);
				return FALSE;
			}

			// Put the number of characters into first word
			// of buffer.
			*((LPWORD)lpszPassword) = cchPassword;

			// Get the characters.
			SendDlgItemMessage( hDlg, IDE_EDITKEY,
					EM_GETLINE,
					(WPARAM) 0,       // line 0
					(LPARAM) lpszPassword);

			// Null-terminate the string.
			lpszPassword[cchPassword] = 0;

			MessageBox( hDlg, lpszPassword,
					"Did it work?",
					MB_OK|MB_TOPMOST);

			// Call a local password-parsing function.
			//                    ParsePassword(lpszPassword);
*/
			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		return FALSE;
	}
	UNREFERENCED_PARAMETER(lParam);
	return FALSE;

}

void	license_generate()
{
	INT_PTR result = DialogBox( nullptr, MAKEINTRESOURCE(IDD_GENERATE), get_window_main_handle(), (DLGPROC)generate_proc );
// no parent in this version so we don't see the dialog when window in full screen
//	INT_PTR result = DialogBox( nullptr, MAKEINTRESOURCE(IDD_GENERATE), nullptr, (DLGPROC)generate_proc );

	if( result == 0 || result == -1 )
	{
		ERR_PRINT_STRING( "Can't open license dialog, exiting soon." );
		spy::sleep( 5000, "sleep license_generate" );
	}
}

#if 0
// old code kept for example if needed (Maa 2023 May) 
INT_PTR CALLBACK password_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		// Set password character to a asterisk sign (*)
		SendDlgItemMessage( hDlg, IDC_EDITPASS,
								EM_SETPASSWORDCHAR,
								(WPARAM) '*',
								(LPARAM) 0);

		SendMessage( hDlg, DM_SETDEFID,
				(WPARAM) IDOK,
				(LPARAM) 0);

		return TRUE;

	case WM_COMMAND:
		switch(wParam)
		{
		CHAR	password_in[64];
		bool	b;
		case IDOK:
			get_control_str( hDlg, IDC_EDITPASS, password_in, 64 );
			b = strcmp( password_in, "69juan42") == 0;
//			b = strcmp( password_in, "170562jmr") == 0;
//			b = strcmp( password_in, "JeLo2000") == 0;
			if( !b )
				exit(0);
			EndDialog( hDlg, b ? 0 : 1 );
			return TRUE;
		case IDCANCEL:
				EndDialog( hDlg, 0 );
				return FALSE;
		}
		return 0;
	}
	UNREFERENCED_PARAMETER(lParam); 
	return FALSE;

} 

bool	password_check()
{
	return DialogBox( nullptr, MAKEINTRESOURCE(IDD_PASSWORD), nullptr, (DLGPROC)password_proc );
}
#endif
