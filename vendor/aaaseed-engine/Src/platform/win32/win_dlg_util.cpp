#include "platform/win32/win_dlg_util.h"
#include "aaa_util.h"
#include "aaa_str.h"
#include "aaa_mem.h"
#include "system/shared/SystemUtils.h"
#include "ui/dialog_wrapper.h"


namespace aaa {
namespace dialog {


void	get_str( HWND hd_dlg, INT32 CONST ctrl_id, o_str& dst )
{
	//INT32	line; 
	INT32	len; 
	INT32	line_char_index; 
	INT32	line_count; 

	line_count = (INT32) SendDlgItemMessage( hd_dlg, 
											 ctrl_id,
											 EM_GETLINECOUNT, 
											 (WPARAM) 0, 
											 (LPARAM) 0 );

	dst.erase();
	for( INT32 line=0; line<line_count; ++line )
	{
		line_char_index = (INT32) SendDlgItemMessage( hd_dlg, ctrl_id,
										EM_LINEINDEX, 
										(WPARAM) line, 
										(LPARAM) 0 );
		if( line_char_index < 0 )
			break;
		else
		{
			len = (INT32) SendDlgItemMessage( hd_dlg, ctrl_id,
								EM_LINELENGTH,
								(WPARAM) line_char_index, 
								(LPARAM) 0 );

		//	dst.alloc_to_add( MAX(2,len) );	//see hack for bug below
//#ifdef	UNICODE
//			WCHAR	pt[1024];	//hack could be too short
//#else
//			CHAR*	pt;
//			pt = dst.get_end();	//hack
//#endif

			WCHAR	pt[1024];	//hack could be too short
			//CHAR*	pt;
			//pt = dst.get_end();	//hack

			if( len == 1)
			{	//	hack for bug : here if we ask for only one nothing is returned
				// Put the number of characters into first word of buffer. 
				*((LPWORD)pt) = 2;	//-1 for the final newline or zero
				//*((LPWORD)pt) = dst_size-(pt-dst)-1;	//-1 for the final newline or zero

				// Get the characters. 
				SendDlgItemMessage( hd_dlg, ctrl_id, EM_GETLINE, (WPARAM) line, (LPARAM) pt );
			}
			else
			{
				// Put the number of characters into first word of buffer. 
				*((LPWORD)pt) = len;	//-1 for the final newline or zero
				//*((LPWORD)pt) = dst_size-(pt-dst)-1;	//-1 for the final newline or zero

				// Get the characters. 
				SendDlgItemMessage( hd_dlg, ctrl_id, EM_GETLINE, (WPARAM) line, (LPARAM) pt );
			}
			// there's a bug if we use -1 for len, doesn't do the conversion properly
			//int		size_needed = WideCharToMultiByte( CP_UTF8, 0, pt, -1, nullptr, 0, nullptr, nullptr );
			int		size_needed = WideCharToMultiByte( CP_UTF8, 0, pt, len, nullptr, 0, nullptr, nullptr );

			dst.alloc_to_add( ::MAX( 2, size_needed ) );	//see hack for bug above

			WideCharToMultiByte( CP_UTF8, 0, pt, len, (CHAR*) dst.get_end(), size_needed, nullptr, nullptr );
			dst.inc_internal( size_needed );
			if( line!=line_count-1 )
				dst.add_char( '\n' );
		}
	}
//	// Null-terminate the string.
//	if( pt != dst )
//		--pt;
//	*pt = 0;
//	printf( "YYY %d\n", dst->get_len() );
}

void	get_str( HWND hd_dlg, INT32 CONST ctrl_id, CHAR* str, INT32 CONST len )
{
	o_str	o;
	get_str( hd_dlg, ctrl_id, o );
	strncpy( str, o.get(), len-1 );
	*(str+len-1) = 0;	// is it really necessary ?
}

void	cpy_str_dlg_newline( CHAR* dst, C_PCHAR src )
{
	while( *src )
	{
		if( *src == '\n' )
		{
			*dst++ = '\r';
			*dst++ = '\n';
			++src;
		}
		else
			*dst++ = *src++;
	}
	*dst = 0;
}

bool	convert_ansi_to_wide( WCHAR* wide_dst, const CHAR* char_src, INT32 CONST char_nb )
{
	if( IS_NULL( wide_dst ) || IS_NULL( char_src ) || char_nb < 1 )
		return false;

	int	nResult = MultiByteToWideChar( CP_ACP, 0, char_src, -1, wide_dst, char_nb );
	wide_dst[ char_nb - 1 ] = 0;

	if( nResult == 0 )
		return false;
	return true;
}

void	set_str_low( HWND hd_dlg, INT32 CONST ctrl_id, C_PCHAR_C src )
{
//	SendDlgItemMessage( hd_dlg, ctrl_id,	EM_SETSEL,		(WPARAM) 0,		(LPARAM) -1 );

	WCHAR * wide = sysutils::utf8_to_unicode( src );
    // Set the new text
	SendDlgItemMessageW( hd_dlg, ctrl_id, WM_SETTEXT,	0,	(LPARAM)wide );
	sysutils::free_str_tmp( wide );
}

void	set_str( HWND hd_dlg, INT32 ctrl_id, C_PCHAR_C src )
{
	CHAR	tmp[STR_LEN_MAX]; 
	cpy_str_dlg_newline( tmp, src );
	set_str_low( hd_dlg, ctrl_id, tmp );
}

void	insert_str( HWND hd_dlg, INT32 CONST ctrl_id, C_PCHAR_C src )
{
	CHAR	tmp[STR_LEN_MAX]; 
	cpy_str_dlg_newline( tmp, src );
	SendDlgItemMessage( hd_dlg, ctrl_id,	EM_REPLACESEL,		(WPARAM) TRUE,	(LPARAM) tmp );
}


void	get_str_select( HWND hd_dlg, INT32 CONST ctrl_id, INT32& start, INT32& end, C_PCHAR_C str_debug )
{
	HWND h_edit = GetDlgItem( hd_dlg, ctrl_id );
	DWORD dw_start;
	DWORD dw_end;
    SendMessageW( h_edit, EM_GETSEL, (WPARAM)&dw_start, (LPARAM)&dw_end );
	start = dw_start;
	end = dw_end;
	DLG_PRINT_STRING( "%s() from %s start-end %d-%d", __FUNCTION__, str_debug, start, end );
}
void	set_str_select( HWND hd_dlg, INT32 CONST ctrl_id, INT32 start, INT32 end, C_PCHAR_C str_debug )
{
	HWND h_edit = GetDlgItem( hd_dlg, ctrl_id );
    // Restore the cursor position
    SendMessageW( h_edit, EM_SETSEL, (WPARAM)start, (LPARAM)end );
	DLG_PRINT_STRING( "%s() from %s start-end will was set to %d-%d", __FUNCTION__, str_debug, start, end );
}

void	set_real( HWND hd_dlg, INT32 CONST ctrl_id, CONST REAL r )
{
	CHAR	tmp[256];
	sprintf( tmp, "%f", r );
	set_str_low( hd_dlg, ctrl_id, tmp );
}

bool	get_double( HWND hd_dlg, INT32 CONST ctrl_id, DOUBLE& d )
{
	CHAR	buf[256];
	get_str( hd_dlg, ctrl_id, buf, 256 );
	DOUBLE	val;
	INT32 ret = sscanf( buf, "%lf", &val );
	if( ret == 1 )
	{
		d = val;
		return true;
	}
	return false;
}

void	set_int32( HWND hd_dlg, INT32 CONST ctrl_id, INT32 CONST i )
{
	CHAR	tmp[256];
	sprintf( tmp, "%i", i );
	set_str_low( hd_dlg, ctrl_id, tmp );
}

bool	get_int32( HWND hd_dlg, INT32 CONST ctrl_id, INT32& i )
{
	CHAR	buf[256];
	get_str( hd_dlg, ctrl_id, buf, 256 );
	int	nb;
	INT32 ret = sscanf( buf, "%d", &nb );
	if( ret == 1 )
	{
		i = nb;
		return true;
	}
	return false;
}

}	// namespace dialog
}	// namespace aaa