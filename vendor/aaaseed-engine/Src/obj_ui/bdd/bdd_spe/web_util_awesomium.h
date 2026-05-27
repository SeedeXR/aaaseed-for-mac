
#ifdef AAA_WEB_UTIL_AWESOMIUM_H
#error "WEB_UTIL_AWESOMIUM_H included more than once."
#endif
#define AAA_WEB_UTIL_AWESOMIUM_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#if AAA_WIN64()
#	define AAA_LIB_USE_AWESOMIUM() 0
#else
#	define AAA_LIB_USE_AWESOMIUM() 1

#endif
#ifndef AAA_WEB_UTIL_H
#	include "web_util.h"
#endif

namespace Awesomium
{
	class WebView;
	class WebCore;
}
class c_web_view_listener;

class c_web_interface_awe final : public c_web_interface
{
private:
	Awesomium::WebView*			_web_view;
	c_web_view_listener*		_web_listener;

	static	Awesomium::WebCore*	web_core;
public:
	c_web_interface_awe();
	~c_web_interface_awe();

	virtual bool	begin( INT32 size_x, INT32 size_y, bool b_back_transparent, INT32 render_max_fp );
	virtual void	end();
	virtual	void	load_url( C_PCHAR_C url );
	virtual	void	focus();
	virtual	void	resize( INT32 size_x, INT32 size_y );
	virtual	bool	is_dirty();
	virtual void	render( UINT8* buf, INT32 size_x, INT32 ch_nb );
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

	virtual C_PCHAR_C	get_address_cur();
	virtual C_PCHAR_C	get_title();
	virtual C_PCHAR_C	get_tooltip();

	virtual	void	set_transparent( bool b_transparent );

	virtual	void	goto_history_offset( INT32 offset );
};
