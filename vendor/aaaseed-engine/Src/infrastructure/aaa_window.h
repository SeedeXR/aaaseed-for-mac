
#ifdef AAA_AAA_WINDOW_H
#error "AAA_WINDOW_H included more than once."
#endif
#define AAA_AAA_WINDOW_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif
#ifndef AAA_AAA_DEF_H
#	include "aaa_def.h"
#endif

class c_window
{
private:
	INT32	_x;
	INT32	_y;
	INT32	_sx;
	INT32	_sy;
#if !AAA_NEW_DESIGN()
	INT32	_win_id;
#endif
	bool	_b_visible;
public:
	c_window();

	FINLINE	INT32	get_x()		CONST	{ return _x; }
	FINLINE	INT32*	get_x_pt()			{ return &_x; }
	FINLINE	INT32	get_y()		CONST	{ return _y; }
	FINLINE	INT32*	get_y_pt()			{ return &_y; }
	FINLINE	INT32	get_sx()	CONST	{ return _sx; }
	FINLINE	INT32*	get_sx_pt()			{ return &_sx; }
	FINLINE	INT32	get_sy()	CONST	{ return _sy; }
	FINLINE	INT32*	get_sy_pt()			{ return &_sy; }

#if !AAA_NEW_DESIGN()
	FINLINE	INT32	get_win_id()	CONST				{ return _win_id; }
	FINLINE	void	set_win_id(		INT32 CONST in	)	{ _win_id = in; }
#endif

	FINLINE	bool	is_visible()	CONST				{ return _b_visible; }

			void	store_xy(		INT32 x,	INT32 y		);
			void	store_sxy(		INT32 sx,	INT32 sy	);


	FINLINE	void	set_visible(	bool CONST in	)	{ _b_visible = in; }

			void	set_cur();
			void	post_redisplay();

			void	save_placement();
			void	load_placement();
public:
//	static	c_window*	get_by_index( UINT32 index );
//	static 	void		get_win_id_by_index( INT32 index );
//	static 	void		set_cur_by_index( INT32 index );
	static void		set_window_xy_sxy(		INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy );
	static void		set_window_fullscreen(	INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy );
	static bool		is_fullscreen();

	static void		push_window();
	static void		pop_window();
	static void		set_window_topmost();
	static void		set_window_notopmost();
//todo	static void		minimize_window();
//tostatic do	static void		restore_window();
	static void		set_monitor_off();
	static void		set_monitor_standby();
	static void		set_monitor_on();

	static void		push_console();
	static void		pop_console();
	static void		minimize_console();
	static void		restore_console();
public:
	static INT32	border_x;
	static INT32	border_y;

	static bool		b_force;
	static bool		b_force_trig;
	static INT32	force_x;
	static INT32	force_y;
	static INT32	force_size_x;
	static INT32	force_size_y;

	static void		do_force();
};

extern	c_window*	win_main;
extern	c_window*	p_win_param;

//todo	clean this
FINLINE	INT32	get_render_window_sx()	{ return win_main->get_sx(); }
FINLINE	INT32	get_render_window_sy()	{ return win_main->get_sy(); }

extern	bool	set_window_main_title_prefix(	C_PCHAR_C prefix );
extern	bool	set_window_main_title(			C_PCHAR_C title );

extern	HWND	get_window_main_handle();

