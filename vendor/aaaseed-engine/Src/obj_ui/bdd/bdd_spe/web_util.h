
#ifdef AAA_WEB_UTIL_H
#error "WEB_UTIL_H included more than once."
#endif
#define AAA_WEB_UTIL_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class c_web_interface
{
protected:
	bool	_b_init;
	bool	_b_back_transparent;
	bool	_b_verbose;
	bool	_b_page_loaded;
	bool	_b_page_load_failed;
public:
	c_web_interface()
		:_b_init( false )
		,_b_back_transparent( false )
		,_b_verbose( false )
		,_b_page_loaded(false)
		,_b_page_load_failed(false)
	{

	}
	virtual bool	begin( INT32 size_x, INT32 size_y, bool b_back_transparent, INT32 render_max_fp )=0;
	virtual void	end() {}
	virtual	void	load_url( C_PCHAR_C url ) = 0;
	virtual	void	focus() {}
	virtual	void	resize( INT32 size_x, INT32 size_y ) = 0;
	virtual	bool	is_dirty() = 0;
	virtual void	render( UINT8* buf, INT32 step, INT32 ch_nb ) = 0;
	virtual	void	update() {}
	virtual	void	pause()		{}
	virtual	void	resume()	{}

	virtual	void	zoom_in()		{}
	virtual	void	zoom_out()		{}
	virtual	void	zoom_reset()	{}

	virtual	void	select_all()	{}

	virtual void	inject_mouse_move(	INT32 x, INT32 y ) = 0;
	virtual void	inject_mouse_down(	INT32 but ) = 0;
	virtual void	inject_mouse_up(	INT32 but ) = 0;
	virtual void	inject_mouse_wheel(	INT32 scroll ) = 0;

	virtual C_PCHAR_C	get_address_cur()	{	return "";	}
	virtual C_PCHAR_C	get_title()			{	return "";	}
	virtual C_PCHAR_C	get_tooltip()		{	return "";	}

	virtual	void	set_transparent( bool CONST b_transparent ) = 0;
	bool			is_transparent()	{ return _b_back_transparent; }
	void			set_verbose( bool CONST b_in )	{ _b_verbose = b_in; }

	bool			is_page_load_failed()	{ return _b_page_load_failed; }
	bool			is_page_loaded()		{ return _b_page_loaded; }

	virtual	void	goto_history_offset( INT32 offset ) {}
};
