#include "web_util_awesomium.h"
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#include "err.h"
		 
#include "infrastructure/layer/layer.h"	//	hack to have string defined ?
#include <iostream>

#if	AAA_LIB_USE_AWESOMIUM()
#	include "Awesomium/WebCore.h"
#	include "Awesomium/BitmapSurface.h"
#	include <lib_use.h>
	AAA_LIB_USE32( "Awesomium" )
#endif

#if	AAA_LIB_USE_AWESOMIUM()

class c_web_view_listener : public	Awesomium::WebViewListener::View
							,public	Awesomium::WebViewListener::Load
//							,public	Awesomium::WebViewListener::Process
{
protected:
	o_str	_tooltip;
	o_str	_http_address_cur;
	o_str	_http_title;
	bool	_b_loaded;
	bool	_b_verbose;
	bool	_b_page_loaded;
	bool	_b_page_load_fail;
public:
	c_web_view_listener() :	_b_loaded(false)
							,_b_verbose(false)
							,_b_page_loaded(false)
							,_b_page_load_fail(false)
	{}
	~c_web_view_listener()		{}

	FINLINE bool		is_page_loaded()				{ return _b_page_loaded; }
	FINLINE	bool		is_page_load_fail()				{ return _b_page_load_fail; }

	FINLINE	void		set_reset_page_load()			{ _b_page_loaded = false; _b_page_load_fail = false; }

	FINLINE	C_PCHAR_C	get_tooltip()					{ return _tooltip.get(); }
	FINLINE	C_PCHAR_C	get_address_cur()				{ return _http_address_cur.get(); }
	FINLINE	C_PCHAR_C	get_title()						{ return _http_title.get(); }
	FINLINE	void		set_verbose( bool CONST b_in )	{ _b_verbose = b_in; }

	virtual void OnChangeTitle(Awesomium::WebView* caller, const Awesomium::WebString& title)
	{
		CHAR	str_title[ 1024 ];
		title.ToUTF8( str_title, title.length() );
		_http_title.set( str_title );
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe : title is now %s", str_title );
	}
	virtual void	OnChangeAddressBar(Awesomium::WebView* caller, const Awesomium::WebURL& url)
	{
		CHAR	str_title[ 1024 ];
		Awesomium::WebString	path = url.spec();	
		path.ToUTF8( str_title, path.length() );
		_http_address_cur.set( str_title );
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe : address bar is now %s", str_title );
	}
	virtual void	OnChangeTooltip(Awesomium::WebView* caller, const Awesomium::WebString& tooltip )
	{
		CHAR	str_title[ 1024 ];
		tooltip.ToUTF8( str_title, tooltip.length() );
		_tooltip.set( str_title );
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe : tooltip is now %s", str_title );
	}
	virtual void	OnChangeTargetURL(Awesomium::WebView* caller, const Awesomium::WebURL& url )
	{
		CHAR	str_title[ 1024 ];
		Awesomium::WebString	path = url.spec();
		
		path.ToUTF8( str_title, path.length() );
	//	_http_address_cur.set( str_title );
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe : target url is now %s", str_title );
	}
	virtual void	OnChangeCursor(Awesomium::WebView* caller, Awesomium::Cursor cursor )
	{
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	}
	virtual void	OnChangeFocus(Awesomium::WebView* caller, Awesomium::FocusedElementType focused_type )
	{
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	}
	virtual void	OnShowCreatedWebView(Awesomium::WebView* caller, Awesomium::WebView* new_view, const Awesomium::WebURL& opener_url, const Awesomium::WebURL& target_url, const Awesomium::Rect& initial_pos, bool is_popup )
	{
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	}
	virtual	void	OnAddConsoleMessage(Awesomium::WebView* caller, const Awesomium::WebString& message, int line_number, const Awesomium::WebString& source )
	{
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	}
	virtual void	OnBeginLoadingFrame(Awesomium::WebView* caller, int64 frame_id, bool is_main_frame, const Awesomium::WebURL& url, bool is_error_page )
	{
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	}
	virtual void	OnFailLoadingFrame(Awesomium::WebView* caller, int64 frame_id, bool is_main_frame, const Awesomium::WebURL& url, int error_code, const Awesomium::WebString& error_description)
	{
		_b_page_load_fail = true;
		if( _b_verbose )
			ERR_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	}
	virtual void	OnFinishLoadingFrame(Awesomium::WebView* caller, int64 frame_id, bool is_main_frame, const Awesomium::WebURL& url)
	{
		_b_page_loaded = true;
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	}
	void	OnDocumentReady(Awesomium::WebView* caller, const Awesomium::WebURL& url )
	{
		if( _b_verbose )
			GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	}
	//virtual void	OnUnresponsive(Awesomium::WebView* caller)
	//{
	//	if( _b_verbose )
	//		GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	//}
	//virtual void	OnResponsive( Awesomium::WebView* caller)
	//{
	//	if( _b_verbose )
	//		GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	//}
	//virtual	void	OnCrashed( Awesomium::WebView* caller, Awesomium::TerminationStatus status)
	//{
	//	if( _b_verbose )
	//		GOOD_PRINT_STRING( "bdd_web_awe in %s()", __FUNCTION__ );
	//}
};


Awesomium::WebCore*	c_web_interface_awe::web_core = nullptr;

c_web_interface_awe::c_web_interface_awe()
	:_web_view(nullptr),
	_web_listener(nullptr)
{
}
c_web_interface_awe::~c_web_interface_awe()
{
}

void	c_web_interface_awe::set_transparent( bool b_transparent )
{
	if( _web_view )
		_web_view->SetTransparent( b_transparent );
	_b_back_transparent = b_transparent;
}
void	c_web_interface_awe::update()
{
	if( _web_view )
	{
//		if( _web_view->IsLoading() )
		Awesomium::Error err = _web_view->last_error();
		if( err != Awesomium::kError_None )
		{
			ERR_PRINT_STRING( "Error in web_view %d", err );
		}
		web_core->Update();
		if( _web_view->IsCrashed() )
			ERR_PRINT_STRING( "Webview process is crashed" );
		if( _web_listener )
		{
			_web_listener->set_verbose( _b_verbose );
			_b_page_loaded = _web_listener->is_page_loaded();
			_b_page_load_failed = _web_listener->is_page_load_fail();
		}
	}
}
void	c_web_interface_awe::load_url( C_PCHAR_C url )
{
	Awesomium::WebURL web_url( Awesomium::WebString::CreateFromUTF8( url, strlen( url ) ) );
	_web_listener->set_reset_page_load();
	_web_view->LoadURL( web_url );
}
void	c_web_interface_awe::focus()
{
	_web_view->Focus();
}
void	c_web_interface_awe::resize( INT32 size_x, INT32 size_y )
{
	_web_view->Resize( size_x, size_y );
}
bool	c_web_interface_awe::is_dirty()
{
	// Get the WebView's rendering Surface. The default Surface is of type 'BitmapSurface', we must cast it before we can use it.
	Awesomium::BitmapSurface* surface = (Awesomium::BitmapSurface*)_web_view->surface();
	if( surface )
		return surface->is_dirty();
	return false;
}
void	c_web_interface_awe::render( UINT8* buf, INT32 size_x, INT32 ch_nb )
{
	INT32	step = size_x * ch_nb;
	// todo check if the change between render and is_dirty for surface
	Awesomium::BitmapSurface* surface = (Awesomium::BitmapSurface*)_web_view->surface();
	if( surface )
	{
		surface->CopyTo( buf, step, ch_nb, false, false );
	}
}
void	c_web_interface_awe::inject_mouse_move( INT32 x, INT32 y )
{
	_web_view->InjectMouseMove( x, y );
}
void	c_web_interface_awe::inject_mouse_down( INT32 but )
{ 
	Awesomium::MouseButton val;
	switch( but )
	{
	case 0:	val = Awesomium::kMouseButton_Right;	break;
	case 1:	val = Awesomium::kMouseButton_Middle;	break;
	case 2:	val = Awesomium::kMouseButton_Left;		break;
	}
	_web_view->InjectMouseDown( val );
}
void	c_web_interface_awe::inject_mouse_up( INT32 but )
{
	Awesomium::MouseButton val;
	switch( but )
	{
	case 0:	val = Awesomium::kMouseButton_Right;	break;
	case 1:	val = Awesomium::kMouseButton_Middle;	break;
	case 2:	val = Awesomium::kMouseButton_Left;		break;
	}
	_web_view->InjectMouseUp( val );
}
void	c_web_interface_awe::inject_mouse_wheel( INT32 scroll )
{
	// todo horizontal scroll is new
	_web_view->InjectMouseWheel( scroll, 0 );
//	web_core->Update();
}
C_PCHAR_C	c_web_interface_awe::get_address_cur()	{	return _web_listener->get_address_cur();	}
C_PCHAR_C	c_web_interface_awe::get_title()		{	return _web_listener->get_title();			}
C_PCHAR_C	c_web_interface_awe::get_tooltip()		{	return _web_listener->get_tooltip();		}

void	c_web_interface_awe::goto_history_offset( INT32 offset )
{
	_web_view->GoToHistoryOffset( offset );
}

void	c_web_interface_awe::end()
{
	if( _web_view )
	{
		_web_view->Destroy();
		//delete _web_view;
		_web_view = nullptr;
		SAFE_DELETE( _web_listener );
	}
}

bool	c_web_interface_awe::begin( INT32 size_x, INT32 size_y, bool b_back_transparent, INT32 render_max_fps )
{
	//todo delete web_core at some point
	if( !web_core )
	{
		Awesomium::WebConfig conf;
		conf.log_level = Awesomium::kLogLevel_Verbose;

		web_core = Awesomium::WebCore::Initialize( conf );
		if( !web_core )
		{
			ERR_PRINT_STRING( "%s() can't create a Awesomium::WebCore", __FUNCTION__ );
			return false;
		}
	}
	_web_view = web_core->CreateWebView( size_x, size_y );
	_web_listener = new c_web_view_listener;
	_web_view->set_view_listener( _web_listener );
	_web_view->set_load_listener( _web_listener );
//	_web_view->set_process_listener( _web_listener );
	_b_init = true;
	return true;
}

void	c_web_interface_awe::zoom_in()
{
	_web_view->ZoomIn();
}
void	c_web_interface_awe::zoom_out()
{
	_web_view->ZoomOut();
}
void	c_web_interface_awe::zoom_reset()
{
	_web_view->ResetZoom();
}
void	c_web_interface_awe::select_all()
{
	_web_view->SelectAll();
}
void	c_web_interface_awe::pause()
{
	if( _web_view )
		_web_view->PauseRendering();
}
void	c_web_interface_awe::resume()
{
	if( _web_view )
		_web_view->ResumeRendering();
}
#endif