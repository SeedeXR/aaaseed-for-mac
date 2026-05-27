// File: wrap_dll.cpp - generic dll wrapper (platform dependend)
//
// Created: sr@20100421
//

//#include    <windowsx.h>

#include "aaa_def.h"
#include "err.h"
#include "wrap_dll.h"		// my own header

#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif


bool	gb_wrap_dll_verbose	= true;
#include <lib_use.h>
AAA_LIB_USE_MESSAGE( "DelayImp")		// For Delayed Loading Dll Functions


//=============================================================================
// Infrastructure.
//
//-----------------------------------------------------------------------------
//
void	wrap_unresolved_call ( LPCSTR lib_name )
{
	ERR_PRINT_STRING( "%s: *** unresolved call ***", lib_name );
}

// ---------------------------------------------------------------------------
// TRICK: This is a tricky dummy function - just let linker think, that
//  wrappers addresses are REFERENCED, so linker with /LTGC + /OPT:REF will
//  not remove them. Well, one day we will do it better, I hope. // sr@20100423
//	return value not used but we don't want optimization to skip stuff
ptrdiff_t	wrap_dll_func( LPVOID func_name, LPVOID func_addr )
{
	static ptrdiff_t trick = reinterpret_cast<uintptr_t>(func_addr) - reinterpret_cast<uintptr_t>(func_name);
	return( trick );
}

// ---------------------------------------------------------------------------
//
static void	_print_error_message ( LPCSTR pfx, LPCSTR name, DWORD err )
{
	ERR_PRINT_STRING( "%s%s failed : %s", pfx, name, aaa::system::get_err_message(err).c_str() ); 
}//_print_error_message




// ---------------------------------------------------------------------------
UINT32	wrap_dll_init_linked ( LPCSTR p_name )
{
	GOOD_PRINT_STRING( "Use STATIC LINKED lib : %s", p_name );
	return( NO_ERROR ); 
}

// ---------------------------------------------------------------------------
UINT32	wrap_dll_deinit_linked ( LPCSTR p_name )
{
	GOOD_PRINT_STRING( "Unused STATIC LINKED lib : %s", p_name ); 
	return( NO_ERROR ); 
}
// ---------------------------------------------------------------------------


#if	WRAP_MAA_WAY

namespace {

// ---------------------------------------------------------------------------
void dump_wrapper ( c_wrap_dll_item& p_beg )
{
//	DBG_PRINT_STRING( "dummy __CNT is 0x%08X ------------------- ", __CNT );

	DBG_PRINT_STRING( "wrappers start at 0x%08X", p_beg );
	c_wrap_dll_item* p = &p_beg;
	DBG_PRINT_STRING( "0x%08X 0x%08X : %s", &p->name, p->fn, p->name );
	p = p->next;
	while( p && p->name != (LPSTR)(INVALID_HINSTANCE) )
	{
		DBG_PRINT_STRING( "\t0x%08X 0x%08X : %s", &p->name, p->fn, p->name );
		if( !p->fn )
		{
			ERR_PRINT_STRING( "fn pointer is NULL it should not !" );
			break;
		}
		p = p->next;
	}
}// dump_wrapper

// ---------------------------------------------------------------------------
int		wrap_link_dll_low( HINSTANCE hInst, LPSTR dllname, c_wrap_dll_item* p, INT32& nb )
{
	FARPROC	f_addr;

	int		n_err = 0;
	//int		n_wraps = 0;

	while( p && p->name != (LPSTR)(INVALID_HINSTANCE) )
	{
		++nb;
		if( !p->fn )
		{
			ERR_PRINT_STRING( "fn pointer is NULL it should not ! skipping the rest" );
			break;
		}
		f_addr = ::GetProcAddress( hInst, p->name );
		if( !f_addr )
		{
			ERR_PRINT_STRING( "DLL %s:%s link error %s", dllname, p->name, aaa::system::get_err_message().c_str() );
			++n_err;
		}
		else
		{
			p->fn = f_addr;
		}
		p = p->next;
	}
	return( n_err );
}	// wrap_link_dll_low

}// end of namespace anonymous

// ---------------------------------------------------------------------------
UINT32	wrap_dll_init ( c_wrap_dll_item& p_beg )
{
	if( gb_wrap_dll_verbose )
		dump_wrapper( p_beg );

	DWORD err = ERROR_SUCCESS;

	c_wrap_dll_item* p = &p_beg;
	HINSTANCE*	p_hlib = (HINSTANCE*) &(p->fn);
	LPSTR		dll_name = p->name;

	if( *p_hlib != INVALID_HINSTANCE )
	{
		ERR_PRINT_STRING( "WRAP %s. Double load is ignored=%x", dll_name, *p_hlib );
		return( ERROR_SUCCESS );		// well, is not success in fact, but an error
	}

	wchar_t * wtxt = sysutils::utf8_to_unicode( dll_name );
	HINSTANCE h_inst = ::LoadLibraryW( wtxt );
	sysutils::free_str_tmp( wtxt );
	if( !h_inst )
	{
		err = aaa::system::get_err_last();
		_print_error_message( "WRAP ", dll_name, err );
	}
	else
	{
		int	nb = 0;
		DWORD nret = wrap_link_dll_low( h_inst, dll_name, p->next, nb );
		if( nret )
			ERR_PRINT_STRING( "WRAPPED '%s'. Unresolved links = %d/%d", dll_name, nret, nb );
		else
			GOOD_PRINT_STRING( "WRAPPED '%s'. All %d links resolved", dll_name, nb );
		*p_hlib = h_inst;
	}
	return( err );
}// wrap_dll_init

// ---------------------------------------------------------------------------
UINT32	wrap_dll_deinit( c_wrap_dll_item& p_beg, LPVOID undef_call )
{
	UINT32		err     = NO_ERROR;

	c_wrap_dll_item* p = &p_beg;
	HINSTANCE*	p_hlib = (HINSTANCE*) &(p->fn);
	LPSTR		dll_name = p->name;

	if( *p_hlib == INVALID_HINSTANCE )
	{
		DBG_PRINT_STRING( "UNWRAP %s. Mo valid instance to unload", dll_name );
	}
	else
	{
		if( !::FreeLibrary( *p_hlib ) )
		{
			err = aaa::system::get_err_last();
			_print_error_message( "UNWRAP ", dll_name, err );
			return( err );
		}
		*p_hlib = INVALID_HINSTANCE;
	}

	int			nb = 0;
	p = p->next;
	while( p && p->name != (LPSTR)(INVALID_HINSTANCE) )
	{
		p->fn = (FARPROC) undef_call;
		p = p->next;
		++nb;
	}
	GOOD_PRINT_STRING( "UNWRAPPED '%s'. links=%d", dll_name, nb );

	return( err );
}// wrap_dll_deinit
#else

namespace {}

// ---------------------------------------------------------------------------
int	wrap_link_dll_low( HINSTANCE hInst, LPSTR dllname, LPSTR* wraps, LPINT p_nwraps )
{
	FARPROC	f_addr;
	LPSTR	p_name;
	int		n_err = 0;
	int		n_wraps = 0;

	while( *wraps != (LPSTR)(INVALID_HINSTANCE) )
	{
		p_name = *wraps++;
		f_addr = ::GetProcAddress( hInst, p_name );
		if( !f_addr )
		{
			ERR_PRINT_STRING( "DLL %s:%s link error(%d)", dllname, p_name, aaa::system::get_err_last() );
			++n_err;
		}
		else
		{
			*wraps = (LPSTR)f_addr;
		}
		++wraps;
		++n_wraps;
	}
	*p_nwraps = n_wraps;
	return( n_err );
}	// wrap_link_dll_low

}// end of namespace anonymous

// ---------------------------------------------------------------------------
UINT32	wrap_dll_init ( LPVOID* wraps, LPVOID* phandle, LPVOID* p_last )
{
	if( gb_wrap_dll_verbose )
		dump_wrapper( wraps );

	LPSTR*		p_wrap = (LPSTR*) wraps;
	LPSTR		p_name = *p_wrap++;
	HINSTANCE*	p_hlib = (HINSTANCE*) p_wrap++;

	if( *p_hlib != INVALID_HINSTANCE )
	{
		ERR_PRINT_STRING( "WRAP %s. Double load is ignored=%x", p_name, *p_hlib );
		return( ERROR_SUCCESS );		// well, is not success in fact, but an error
	}

	DWORD	err = ERROR_SUCCESS;
	HINSTANCE	h_inst = ::LoadLibrary( p_name );
	if( !h_inst )
	{
		err = aaa::system::get_err_last();
		_print_error_message( "WRAP ", p_name, err );
	}
	else
	{
		int			n_wraps = 0;
		DWORD nret = wrap_link_dll_low( h_inst, p_name, p_wrap, &n_wraps );
		if( nret )
			ERR_PRINT_STRING( "WRAPPED '%s'. Unresolved links = %d/%d", p_name, nret, n_wraps );
		else
			GOOD_PRINT_STRING( "WRAPPED '%s'. All %d links resolved", p_name, n_wraps );
		*p_hlib = h_inst;
	}

	return( err );
}// wrap_dll_init

// ---------------------------------------------------------------------------
UINT32	wrap_dll_deinit ( LPVOID* wraps, LPVOID undef_call )
{
	LPSTR		p_name;
	HINSTANCE*	p_hlib;
	LPSTR*		p_wrap = (LPSTR*) wraps;
	int			n_wraps = 0;
	UINT32		err     = NO_ERROR;

	p_name = *p_wrap++;
	p_hlib = (HINSTANCE*) p_wrap++;
	//
	if( !::FreeLibrary( *p_hlib ) )
	{
		err = aaa::system::get_err_last();
		_print_error_message( "UNWRAP ", p_name, err );
		return( err );
	}

	// re-init the data structure

	*p_hlib = INVALID_HINSTANCE;
	while( *p_wrap!=(LPSTR)(INVALID_HINSTANCE) )
	{
		++p_wrap;		// skip the function name
		*p_wrap = (LPSTR)undef_call;
		++n_wraps;
		++p_wrap;		// back to the function name
	}
	GOOD_PRINT_STRING( "UNWRAPPED '%s'. links=%d", p_name, n_wraps );

	return( err );
}// wrap_dll_deinit

#endif
//EOF:  wrap_dll.cpp - generic dll wrapper (platform dependend)
