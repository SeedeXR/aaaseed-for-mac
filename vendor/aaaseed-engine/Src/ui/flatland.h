
#ifdef AAA_FLATLAND_H
#error "FLATLAND_H included more than once."
#endif
#define AAA_FLATLAND_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class c_viewport;
class c_window;

class c_flatland final : public c_obj_ui
{
	FACTORY_DECLARE(c_flatland,c_obj_ui);
private:
	static	bool		b_draw_focus;
	static	bool		b_draw_focus_lua;
	static	INT32		char_zone;
	static	INT32		focus_offset[2];
	static	REAL		focus_scale[2];

	static	bool		b_draw_info;
	static	bool		b_draw_info_lua;
	static	INT32		info_offset[2];
	static	REAL		info_scale[2];

	static	c_viewport*	viewport_focus;
	static	c_viewport*	viewport_info;
	static	c_viewport*	viewport_curve;

public:
	static	c_flatland*	cur;	//todo singleton

	static	REAL		ui_line_width;
	static	bool		b_ui_line_smooth_ui;

	static	bool		b_ui_gradient;
	static	bool		b_ui_back_focus;
	static	bool		b_ui_back_curve;
	static	FP32		back_color_low[4];
	static	FP32		back_color_high[4];
	static	bool		b_ui_blend;
	static	bool		b_ui_back_line;
	static	FP32		back_line_color[4];

	static	REAL		char_size_x_asked;
	static	REAL		char_size_y_asked;
	static	REAL		mess_char_size_factor_x;
	static	REAL		mess_char_size_factor_y;
	static	INT32		mess_line_nb;

	static	bool		b_draw_curve;
	static	void		flip_draw_curve();

	static	bool		b_draw_fps;
	static	bool		b_draw_frame;
	static	bool		b_draw_timecode;
	static	bool		b_draw_mess;
	static	bool		b_draw_clock;
	static	bool		b_draw_logo;

	static	bool		b_param_keyboard_change;

public:
	virtual	void				param_init_pt_static();

	static	void				c_init();
	static	void				c_deinit();

	static	FINLINE	bool		is_draw_info()				{	return	b_draw_info;					}
	static	FINLINE	void		flip_draw_info()			{	b_draw_info = !b_draw_info;				}

	static	FINLINE	bool		is_draw_focus()				{	return	b_draw_focus;					}
	static	FINLINE	void		flip_draw_focus()			{	b_draw_focus = !b_draw_focus;			}

	static	FINLINE	bool		is_draw_info_lua()			{	return	b_draw_info_lua;				}
	static	FINLINE	void		flip_draw_info_lua()		{	b_draw_info_lua = !b_draw_info_lua;		}

	static	FINLINE	bool		is_draw_focus_lua()			{	return	b_draw_focus_lua;				}
	static	FINLINE	void		flip_draw_focus_lua()		{	b_draw_focus_lua = !b_draw_focus_lua;	}

	static	FINLINE	bool		is_param_keyboard_change()	{	return b_param_keyboard_change;			}

	static	FINLINE	INT32		get_char_zone()				{	return	char_zone;						}

	static	REAL				get_char_ratio_y();
	static	REAL				get_ratio_char_x();

	static	void				draw_logo();
	static	void				draw_focus_info();

	static	bool				is_in_focus(	INT32 CONST x,	INT32 CONST y	);
	static	bool				is_in_info(		INT32 CONST x,	INT32 CONST y	);
	static	bool				is_in(			INT32 CONST x,	INT32 CONST y	);


	static	INT32				get_line_bottom();
	static	INT32				get_line(		INT32 y	);
	static	INT32				get_line(		INT32 CONST x,	INT32 CONST y	);
	static	INT32				get_col(		INT32 CONST x	);
	static	INT32				get_col(		INT32 CONST x,	INT32 CONST y	);

	static	FINLINE	c_viewport*	get_viewport_focus()		{	return viewport_focus;	}
	static	FINLINE	c_viewport*	get_viewport_info()			{	return viewport_info;	}
	static	FINLINE	c_viewport*	get_viewport_curve()		{	return viewport_curve;	}

	static	void				render( c_window* pwin );

private:
	static	void				render_before();
	static	void				render_focus();
	static	void				render_info();
	static	void				render_curve();
};
