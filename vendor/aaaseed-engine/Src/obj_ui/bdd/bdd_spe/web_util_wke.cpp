#include "web_util_wke.h"
#include "err.h"

#include "lib_wrappers/wrap_wke.h"

#if	AAA_LIB_USE_WKE()
#	include <lib_use.h>
	AAA_LIB_USE32( "wke" )
#endif



/*
jsValue JS_CALL js_msgBox(jsExecState es)
{
	const wchar_t* text = jsToStringW(es, jsArg(es, 0));
	const wchar_t* title = jsToStringW(es, jsArg(es, 1));

	//todo	MessageBox(hMainWnd, text, title, 0);

	return jsUndefined();
}

static int s_testCount = 0;
jsValue JS_CALL js_getTestCount(jsExecState es)
{
	return jsInt(s_testCount);
}

jsValue JS_CALL js_setTestCount(jsExecState es)
{
	s_testCount = jsToInt(es, jsArg(es, 0));

	return jsUndefined();
}
*/

c_web_interface_wke::c_web_interface_wke()
	:_web_view			(	nullptr	)
	,_x_store			(	0		)
	,_y_store			(	0		)
	,_zoom_factor		(	1.0f	)
{
}

c_web_interface_wke::~c_web_interface_wke()
{
	end();
//todo
//	wkeShutdown();
#if	AAA_LIB_USE_WKE()
	dll_wke.wkeShutdown();
#endif
}

static bool	b_init = false;
bool	c_web_interface_wke::b_dll_loaded = false;


void	c_web_interface_wke::end()
{
#if	AAA_LIB_USE_WKE()
	if( _web_view )
	{

		//_web_view->destroy();
		dll_wke.wkeDestroyWebView( _web_view );

		_web_view = nullptr;
		b_dll_loaded = dll_wke.deinit();
	}
#endif	
}

bool	c_web_interface_wke::begin( INT32 size_x, INT32 size_y, bool b_back_transparent, INT32 render_max_fps )
{
	if( _web_view )
		return true;

#if	AAA_LIB_USE_WKE()
	if( !b_dll_loaded )
	{
		//GOOD_PRINT_STRING( "dir is %s", c_file::dir_get_def() );
		b_dll_loaded = dll_wke.init();
		if ( !b_dll_loaded )
			return false;
	}

	if( !b_init )
	{
			//GOOD_PRINT_STRING( "dir is %s", c_file::dir_get_def() );

		dll_wke.wkeInit();
		GOOD_PRINT_STRING( "wke init version %d %s", dll_wke.wkeVersion(),  dll_wke.wkeVersionString() );
		b_init = true;
	}
		
	//jsBindFunction("msgBox", js_msgBox, 2);
	//jsBindGetter("testCount", js_getTestCount);
	//jsBindSetter("testCount", js_setTestCount);
	_web_view = dll_wke.wkeCreateWebView();
	if( !_web_view )
	{
		ERR_PRINT_STRING( "%s() can't create a wke WebView", __FUNCTION__ );
		return false;
	}	
	set_transparent( b_back_transparent );
	//_web_view->loadHTML(L"<p style=\"background-color: #00FF00\">Testing</p><img id=\"webkit logo\" src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul>");
#else
	ERR_PRINT_STRING( "this version of AAASeed compiled without wke" );
	return false;
#endif
	return true;
}

void	c_web_interface_wke::set_transparent( bool b_transparent )
{
#if	AAA_LIB_USE_WKE()
	_web_view->setTransparent( b_transparent );
	_b_back_transparent = b_transparent;
#endif
}

void	c_web_interface_wke::resize( INT32 size_x, INT32 size_y )
{
#if	AAA_LIB_USE_WKE()
	_web_view->resize( size_x, size_y );
#endif
}
bool	c_web_interface_wke::is_dirty()
{
	//DBG_PRINT_STRING( "title %s", _web_view->title() );
#if	AAA_LIB_USE_WKE()
	return _web_view->isDirty();
#else
	return false;
#endif
}
void	c_web_interface_wke::render( UINT8* buf, INT32 size_x, INT32 ch_nb )
{
#if	AAA_LIB_USE_WKE()
	_web_view->paint( buf, size_x*ch_nb+0 );
#endif
}
void	c_web_interface_wke::load_url( C_PCHAR_C url )
{
	_b_page_load_failed = false;
	_b_page_loaded = false;
#if	AAA_LIB_USE_WKE()
	_web_view->loadURL( url );
#endif
}

void	c_web_interface_wke::inject_mouse_move( INT32 x, INT32 y )
{
	unsigned int flags = 0;

/*
	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;
*/
	//flags = wParam;

	//handled =
	_x_store = x;
	_y_store = y;
#if	AAA_LIB_USE_WKE()
	_web_view->mouseEvent( WKE_MSG_MOUSEMOVE, x, y, 0 );
#endif
}

void	c_web_interface_wke::inject_mouse_down( INT32 but )
{
#if	AAA_LIB_USE_WKE()
	unsigned int val;
	switch( but )
	{
	case 0:	val = WKE_MSG_LBUTTONDOWN;	break;
	case 1:	val = WKE_MSG_MBUTTONDOWN;	break;
	case 2:	val = WKE_MSG_RBUTTONDOWN;	break;
	}
	bool handled = _web_view->mouseEvent( val, _x_store, _y_store, 0 );
	//DBG_PRINT_STRING( handled ? "True" : "False" );
#endif
}
void	c_web_interface_wke::inject_mouse_up( INT32 but )
{
#if	AAA_LIB_USE_WKE()
	unsigned int val;
	switch( but )
	{
	case 0:	val = WKE_MSG_LBUTTONUP;	break;
	case 1:	val = WKE_MSG_MBUTTONUP;	break;
	case 2:	val = WKE_MSG_RBUTTONUP;	break;
	}
	_web_view->mouseEvent( val, _x_store, _y_store, 0 );
#endif
}
void	c_web_interface_wke::inject_mouse_wheel( INT32 scroll )
{
#if	AAA_LIB_USE_WKE()
	_web_view->mouseWheel( _x_store, _y_store, scroll, 0 );
#endif
}

void	c_web_interface_wke::goto_history_offset( INT32 offset )
{
#if	AAA_LIB_USE_WKE()
	if( offset >= 0 )
	{
		for( INT32 i=offset; i>0; --i )
			_web_view->goForward();
	}
	else
	{
		for( INT32 i=-offset; i>0; --i )
			_web_view->goBack();
	}
#endif
}

C_PCHAR_C	c_web_interface_wke::get_title()
{
#if	AAA_LIB_USE_WKE()
	return (C_PCHAR_C)_web_view->title();
#else
	return "No wke in this AAAseed version";
#endif
}

void	c_web_interface_wke::update()
{
#if	AAA_LIB_USE_WKE()
	//if( _web_view->is_loaded() )
	//{
	//	GOOD_PRINT_STRING( "Doc is loaded" );
	//}
	if( _web_view->isLoadFailed() )
	{
		_b_page_load_failed = true;
		_b_page_loaded = false;
		//	GOOD_PRINT_STRING( "Doc is isLoadFailed" );
	}
	if( _web_view->isLoadComplete() )
	{
		_b_page_loaded = true;
	//	GOOD_PRINT_STRING( "Doc is isLoadComplete" );
	}
	//if( _web_view->isDocumentReady() )
	//	GOOD_PRINT_STRING( "Doc is isDocumentReady" );
#endif
}

void	c_web_interface_wke::zoom_in()
{
#if	AAA_LIB_USE_WKE()
	_zoom_factor *= .8f;
	_web_view->setZoomFactor( _zoom_factor );
#endif
}
void	c_web_interface_wke::zoom_out()
{
#if	AAA_LIB_USE_WKE()
	_zoom_factor *= 1.25f;
	_web_view->setZoomFactor( _zoom_factor );
#endif
}
void	c_web_interface_wke::zoom_reset()
{
#if	AAA_LIB_USE_WKE()
	_zoom_factor = 1.0f;
	_web_view->setZoomFactor( _zoom_factor );
#endif
}
void	c_web_interface_wke::select_all()
{
#if	AAA_LIB_USE_WKE()
	_web_view->selectAll();
#endif
}
void	c_web_interface_wke::pause()
{
#if	AAA_LIB_USE_WKE()
	if( _web_view )
		_web_view->sleep();
#endif
}
void	c_web_interface_wke::resume()
{
#if	AAA_LIB_USE_WKE()
	if( _web_view )
		_web_view->awaken();
#endif
}
