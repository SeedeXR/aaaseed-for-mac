
#include "dialog_wrapper.h"
#include "Thread/aaa_thread.h"
#include "system/shared/SystemUtils.h"
#include "infrastructure/aaa_window.h"
#include "language/lua/aaalua_wrap.h"

//todo namespace
bool	gb_dlg_verbose = false;

CONSTEXPR C_PCHAR_C	DIALOG_HEADER = "# DIALOG : ";
void	DLG_PRINT_STRING( C_PCHAR fmt, ... )
{
	if( gb_dlg_verbose )
	{
		va_list args;
		va_start( args, fmt );
		HEADER_PRINT_STRING_VA( DIALOG_HEADER, fmt, args );
		va_end(args);
	}
}

std::vector<c_dialog_wrapper*>	c_dialog_wrapper::wrappers;
c_dialog_wrapper*				c_dialog_wrapper::wrapping = nullptr;
aaa::MUTEX_RECURSIVE			c_dialog_wrapper::access;

c_dialog_wrapper::c_dialog_wrapper( c_obj_ui* obj )
	:_obj(obj)
	,_b_to_destroy		{false}
	,_b_lua_callback	{false}
	, _lua_id			{0}
	,_b_title_need_set	{false}
{
}

c_dialog_wrapper::~c_dialog_wrapper()
{
}

void c_dialog_wrapper::set_param( p_param CONST param )
{
	_param = param;
	_pt_ref = param->get_pt_const_void();
}

void	c_dialog_wrapper::set_title( C_PCHAR_C title )
{
	if( title )
		_title.set( title );
	else
	{
		_title.set( _param->get_name() );
		if( _obj )
		{
			_title.add( " In " );
			_title.add( _obj->get_name_str() );
		}
	}
	_b_title_need_set = true;
}	

void	c_dialog_wrapper::set_lua_callback( bool CONST b_on, INT32 CONST lua_id )
{
	_b_lua_callback = b_on;
	_lua_id = lua_id;
}

void	c_dialog_wrapper::set_hd( HWND hd )
{
	INT32 CONSTEXPR SIZE = 1024;
	wchar_t wch[SIZE] = {};
	_hd_dlg = hd;
	::GetWindowTextW( _hd_dlg, wch, SIZE );

	if( *wch == 0 )
	{
		CHAR narrow[SIZE];
		C_PCHAR_C name = _param->get_name().get();
		if( _obj )
			snprintf( narrow, SIZE-1, "%.450s->%.450s", _obj->get_name_str(), name );
		else
			snprintf( narrow, SIZE-1, "%.450s", name );
		sysutils::utf8_to_unicode( wch, SIZE, narrow );
		::SetWindowTextW( hd, wch );
	}
}

void	c_dialog_wrapper::do_begin( C_PCHAR_C type )
{
	if( _b_lua_callback )
		g_lua_wrap_master->do_fn_b_pass_iss( nullptr, "aaa.dialog_hook", _lua_id, "open", type );
}

void	c_dialog_wrapper::do_end( INT32 CONST return_value_int, C_PCHAR_C return_value )
{
	EndDialog( _hd_dlg, return_value_int );
	if( _b_lua_callback )
		g_lua_wrap_master->do_fn_b_pass_iss( nullptr, "aaa.dialog_hook", _lua_id, "close", return_value );
	_b_to_destroy = true;
}

bool	c_dialog_wrapper::is_dialog_on_same_data( c_param CONST * param )
{
	bool	b_ret = false;
	
	std::lock_guard<aaa::MUTEX_RECURSIVE> guard(access);
	
	for( auto const & p_wrapper : wrappers )
	{
		if( p_wrapper->_param == param || p_wrapper->_pt_ref == param->get_pt_const_void()  )
		{
			b_ret = true;
			break;
		}
	}

	return b_ret;
}

void	c_dialog_wrapper::add( c_dialog_wrapper* pdw )
{
	if( wrapping )
		BOX_ERR( "class c_dialog_wrapper add()\nwrapping should be NULL." );
	wrapping = pdw;
}

void	c_dialog_wrapper::cleanup()
{
//	if( !wrappers.empty() )	// TODO	understand why we need this test
	{
		std::lock_guard<aaa::MUTEX_RECURSIVE> guard(access);

		std::vector<c_dialog_wrapper*>::iterator it = wrappers.begin();
		while( it != wrappers.end() )
		{
			if( (*it)->is_to_destroy() )
			{
				c_dialog_wrapper* pt = *it;
//				*it = nullptr;	// we should not need it but we got strange stuff going on
				wrappers.erase(it);
				delete pt;
				it = wrappers.begin(); //we restart incase there is several to cleanup
			}
			else
				++it;
		}
	}
	SAFE_DELETE( wrapping );
}

void	c_dialog_wrapper::update_all()
{
	if( !wrappers.empty() )
	{
		if( access.try_lock() )
		{
			for( auto const & p_wrapper : wrappers )
			{
				if( p_wrapper )
					p_wrapper->update();
				else
				{
					ERR_PRINT_STRING( "Bug in c_dialog_wrapper::update_all()" );
					//debug_break( "Bug in c_dialog_wrapper::update_all()" );
				}
			}
			access.unlock();
		}
	}
}

void	c_dialog_wrapper::attach( HWND hd )
{
	if( wrapping )
	{
		std::lock_guard<aaa::MUTEX_RECURSIVE> guard(access);
		wrapping->set_hd(hd );
		wrappers.push_back(wrapping);
		wrapping = nullptr;
	}
	else
	{
		BOX_ERR( "class	c_dialog_wrapper attach()\nCan't attach to NULL.");
	}
}

//no lock because called only by do_callback below
c_dialog_wrapper*	c_dialog_wrapper::find_from_hd( HWND hd )
{
	for( auto const & p_wrapper : wrappers )
	{
		if( p_wrapper->is_hd( hd ) )
			return p_wrapper;
	}

	return nullptr;
}

bool	c_dialog_wrapper::do_callback( HWND hd_dlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	c_dialog_wrapper*	pdw;
	switch( message ) 
	{
	case WM_INITDIALOG:
		c_dialog_wrapper::attach( hd_dlg );
		break;
	}
	{
		std::lock_guard<aaa::MUTEX_RECURSIVE> guard(access);
		pdw = find_from_hd( hd_dlg );
		if( pdw )
		{
			if( pdw->_b_title_need_set )
			{
				SetWindowTextA( pdw->get_hd(), pdw->_title.get() );
				pdw->_b_title_need_set = false;
			}
			return pdw->do_callback( message, wParam, lParam );
		}
	}
	return false;
}

INT_PTR	CALLBACK	dialog_proc( HWND hd_dlg, UINT message, WPARAM wParam, LPARAM lParam )
{ 
	return c_dialog_wrapper::do_callback( hd_dlg, message, wParam, lParam ) ? TRUE : FALSE;
//	return FALSE;
} 

HWND get_parent()
{
#if 1
	return GetDesktopWindow();	//function (got modeless dialog to function)
#else
	return get_window_main_handle(); //don't function ???
#endif
}

void	
#ifdef	WIN32
__cdecl
#endif
do_dialog( void* res_id )
{
	DialogBox( nullptr, MAKEINTRESOURCE( reinterpret_cast<intptr_t>(res_id) ), get_parent(), (DLGPROC)dialog_proc );
	//erase the dialog wrapper
	c_dialog_wrapper::cleanup();
	c_thread::end();
}

HWND hd_dialog = 0;

void	
#ifdef	WIN32
__cdecl
#endif
do_dialog_modeless( void* res_id )
{
	hd_dialog = CreateDialog( nullptr, MAKEINTRESOURCE( reinterpret_cast<intptr_t>(res_id) ), get_parent(), (DLGPROC)dialog_proc );
	ShowWindow( hd_dialog, SW_SHOW ); 
	//erase the dialog wrapper
//	c_dialog_wrapper::cleanup();
//	c_thread::end();
}
