
#ifdef AAA_WEB_UTIL_WKE_H
#error "WEB_UTIL_WKE_H included more than once."
#endif
#define AAA_WEB_UTIL_WKE_H 1


#ifndef AAA_WEB_UTIL_H
#	include "web_util.h"
#endif

//#if WINVER >= 0x601
//#	pragma message( "WINVER is " AAA_STRING(WINVER) ) 
#if AAA_WIN64()
#	define AAA_LIB_USE_WKE() 0
#else
#	define AAA_LIB_USE_WKE() 1
#endif
//#endif

namespace wke
{
	class IWebView;
}

class c_web_interface_wke final : public c_web_interface
{
public:
	static	bool	b_dll_loaded;
private:
	wke::IWebView*	_web_view;
	INT32			_x_store;
	INT32			_y_store;
	REAL			_zoom_factor;
	//o_str			_http_title;
public:
	c_web_interface_wke();
	~c_web_interface_wke();

	virtual bool	begin( INT32 size_x, INT32 size_y, bool b_back_transparent, INT32 render_max_fp );
	virtual void	end();
	virtual	void	load_url( C_PCHAR_C url );
	//	virtual	void	focus();
	virtual	void	resize( INT32 size_x, INT32 size_y );
	virtual	bool	is_dirty();
	virtual void	render( UINT8* buf, INT32 step, INT32 ch_nb );
	virtual	void	update();
	virtual	void	pause();
	virtual	void	resume();

	virtual	void	zoom_in();
	virtual	void	zoom_out();
	virtual	void	zoom_reset();

	virtual	void	select_all();

	virtual void	inject_mouse_move(	INT32 x, INT32 y );
	virtual void	inject_mouse_down(	INT32 but );
	virtual void	inject_mouse_up(	INT32 but );
	virtual	void	inject_mouse_wheel( INT32 scroll );

	//	virtual CHAR*	get_address_cur();
	virtual C_PCHAR_C	get_title();

	//	virtual CHAR*	get_tooltip();

	virtual	void	set_transparent( bool b_transparent );

	virtual	void	goto_history_offset( INT32 offset );
};

