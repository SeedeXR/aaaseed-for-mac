
#ifdef AAA_DISPLAY_INFO_H
#error "DISPLAY_INFO_H included more than once."
#endif
#define AAA_DISPLAY_INFO_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_RECT_H
#	include "aaa/aaa_rect.h"
#endif


class	c_display_info final : public c_obj_ui
{
	FACTORY_DECLARE( c_display_info, c_obj_ui );

public:
	struct screen_info;

	enum SORT_TYPE : INT32
	{
		SORT_NO = 0,
		SORT_X,
		SORT_Y,
		SORT_MAX_NB
	};

private:
	aaa::rect::lbrt_sxy			_window_lbrt;
	INT32						_screen_nb;
	bool						_b_display_info_trig;
	SORT_TYPE					_s_sort;
	bool						_b_just_updated;

	void						param_init_pt_rect(	INT32& h, aaa::rect::lbrt_sxy& rect );
	void						param_init_pt_info(	INT32& h, struct screen_info& info );

public:
	static c_display_info*		master;

	void						trig_update_info();
	void						update_info();
	virtual	void				param_init_pt();

	void						update();

	void						clear_info(			screen_info* CONST info );
	void						init_info(			screen_info* CONST info, INT32 id_os, INT32 left, INT32 right, INT32 bottom, INT32 top );	//todo private and friend of fn
	screen_info*				get_info(			INT32 index );
	aaa::rect::lbrt_sxy&		get_info_rect(		INT32 index );
	INT32						get_screen_nb()		{	return _screen_nb;		}

	aaa::rect::lbrt_sxy CONST &	get_window_rect()	{	return _window_lbrt;	}
	void						set_window_rect(	INT32 x,	INT32 y,	INT32 sx,	INT32 sy );
	void						set_window_xy(		INT32 x,	INT32 y );
	void						set_window_sxy(		INT32 sx,	INT32 sy );
};


