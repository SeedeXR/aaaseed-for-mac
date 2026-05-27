#include "err.h"
#include "clipboard.h"
#include "infrastructure/aaa_window.h"
#include "aaa_mem.h"

bool	win_clipboard_move_to( C_PCHAR str_src, INT32 len )
{
	HWND	hd = get_window_main_handle();
	if( !OpenClipboard(hd) )
	{
		ERR_PRINT_STRING( "Cannot open the Clipboard" );
		return false;
	}
	if( !EmptyClipboard() )
	{
		ERR_PRINT_STRING( "Cannot empty the Clipboard" );
		return false;
	}

	HGLOBAL	hglbCopy;
	hglbCopy = GlobalAlloc( GMEM_MOVEABLE, (len + 1) * sizeof(TCHAR) );
	if( !hglbCopy )
	{
		CloseClipboard();
		return false;
	} 

	// Lock the handle and copy the text to the buffer.
	LPVOID	pt = GlobalLock(hglbCopy);
	MEMCPY( pt, str_src, (len+1) * sizeof(TCHAR), __FUNCTION__ ); 
	GlobalUnlock(hglbCopy);

	// Place the handle on the clipboard.
	SetClipboardData( CF_TEXT, hglbCopy );
	CloseClipboard();
	GOOD_PRINT_STRING( "copy \"%s\" to clipboard", str_src );
	return true;
}

bool	win_clipboard_get( o_str& o_dst )
{
	bool b_ret = false;
	HWND	hd = get_window_main_handle();
	if( !IsClipboardFormatAvailable(CF_TEXT) ) 
		ERR_PRINT_STRING( "Clipboard have mo text format" );
	else if( !OpenClipboard(hd) )
		ERR_PRINT_STRING( "Cannot open the Clipboard" );
	else
	{
		HGLOBAL   hglb = GetClipboardData(CF_UNICODETEXT);
		if( hglb )
		{
			LPTSTR lptstr = (LPTSTR) GlobalLock(hglb); 
			if( lptstr )
			{
				o_dst.set( lptstr );
				GOOD_PRINT_STRING( "copy \"%s\" from clipboard", o_dst.get() );
				b_ret = true;
			}
			GlobalUnlock(hglb); 
		}
		CloseClipboard();
	}
	return b_ret;
}

bool	c_clipboard::move_to( C_PCHAR str_src, INT32 len )	{	return (str_src && len > 0)  ? win_clipboard_move_to( str_src, len ) : false;	}
bool	c_clipboard::move_to( C_PCHAR str_src )				{	return str_src ? move_to( str_src, (INT32) strlen(str_src) ) : false;	}
bool	c_clipboard::move_to( o_str CONST & o_src )			{	return move_to( o_src.get(), o_src.get_len() );		}

bool	c_clipboard::move_to_with_doublequote( C_PCHAR str_src )
{
	if( str_src )
	{
		o_str	o;
		o.add_char( '\"' );
		o.add( str_src );
		o.add_char( '\"' );
		return move_to( o );
	}
	return false;
}

bool	c_clipboard::move_to_with_doublequote( o_str CONST & o_src )
{
	if( o_src.get_len() > 0 )
	{
		o_str	o;
		o.add_char( '\"' );
		o.add( o_src );
		o.add_char( '\"' );
		return move_to( o );
	}
	return false;
}

bool	c_clipboard::move_from( o_str& o_dst )	{	return win_clipboard_get(o_dst);	}
