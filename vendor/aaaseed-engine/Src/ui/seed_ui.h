
#ifdef AAA_SEED_UI_H
#error "SEED_UI_H included more than once."
#endif
#define AAA_SEED_UI_H 1


#ifndef	AAA_PARAM_H
#	include "infrastructure/param/param.h"
#endif
#ifndef AAA_MOUSE_H
#	include "mouse.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class c_bdd;
class c_event_mouse;
class c_param;

class ui
{
private:
	static	bool	b_edit;
	static	bool	b_camera_edit;

public:
	enum MOVE_TYPE : INT32
		{
		MOVE_NONE = 0,
		MOVE_ROT,
		MOVE_SCA,
		MOVE_TRA,
		MOVE_MAP,
		MOVE_PARAM,
		MOVE_DOLLY,
		SCROLL_PARAM,
		MOVE_BDD_DO,	//careful mark the last one in the interface (c_pref for now)
		MOVE_ORTHO,
		MOVE_MAP_ORI,
		MOVE_PARAM_TO_CONFIRM,
		SCROLL_PARAM_TO_CONFIRM,
		MOVE_NB
		};

	static	C_PCHAR_C	str_move_type[ MOVE_NB ];
	static	MOVE_TYPE	move_type;
	static	MOVE_TYPE	move_type_left;
	static	o_str		move_type_left_str;
	static	REAL		scaling_factor_ui[ MOVE_NB ];
	static	bool		b_slider_turn_ui;
	static	bool		b_mouse_two_button;
	static	REAL		click_double_interval;

	static	c_bdd*		bdd_eating_mouse;
//mouse position in pixel in the window
	static	INT32		mouse_pos_pixel[2];
	static	INT32		mouse_pos_pixel_start[2];
//same info but in a param viewport (flatland) in canonical coor [0.,1.]
	static	REAL		mouse_focus[2];
	static	REAL		mouse_focus_start[2];

	static	bool		b_mouse_button_state_ui_out[ mouse::BUTTON_NB ];

	static	bool		mouse_motion_generate_click;

	static	REAL		value_x;
	static	REAL		value_y;
	static	REAL		camera_inter_ui;
	static	REAL		param_value_out;

	static	bool		b_camera_symbolics_editing;

	static	FINLINE	bool		is_edit()				{ return b_edit; }
	static	FINLINE	bool*		get_edit_pt()			{ return &b_edit; }
	static			void		set_edit( bool b );
	static			void		flip_edit();

	static			void		do_mouse_down_in_video( INT32 CONST x, INT32 CONST y, mouse::BUTTON CONST button );

	static	FINLINE	bool		is_camera_edit()		{ return b_camera_edit; }
	static	FINLINE	bool*		get_camera_edit_pt()	{ return &b_camera_edit; }
	static			void		set_camera_edit( bool b );
	static			void		flip_camera_edit();

	static	void				switch_move_left();
	static	void				switch_cam_none();
	static	void				switch_tra();
	static	void				switch_sca();
	static	void				switch_rot();
	static	void				switch_map();
	static	void				switch_map_ori();
	static	void				start_param_move();
	static	void				start_param_scroll();
	static	void				switch_dolly();
	static	void				switch_ortho();
	static	void				switch_dist( bool b_pers );
	static	void				start_bdd_do();

	static	FINLINE	MOVE_TYPE	get_move_type()		{ return move_type; }
	static	FINLINE	bool		is_bdd_do()			{ return get_move_type() == MOVE_BDD_DO;	}
	static	FINLINE	bool		is_move_param()		{ return get_move_type() == MOVE_PARAM;		}

	static	void				reset_xy_value();
	static	void				set_xy_value();
	static	bool				cancel_xy();
	static	void				stop_xy();

	static	void				begin_camera_symbolics_editing();
private:
	static	bool				end_camera_symbolics_editing( bool b_cancel );

	static	void				set_move_type_low( MOVE_TYPE move_type_in );

	static	void				set_move_type( MOVE_TYPE move_in );
	static	void				start_xy();
	static	void				do_xy();
	static	void				update_mouse();

	static	void				enable_mouse_tracking();
	static	void				disable_mouse_tracking();

public:
	static	void				menu_update();
	static	void				clear_mouse_frame_data();
	static	bool				param_do_action( c_param * CONST par, aaa::param::ACTION action );
	static	void				mouse_do( c_event_mouse* ev );
	static	void				do_it();

	static	void				motion_do( c_event_mouse* ev );

	static	void				set_but_state( mouse::BUTTON button, mouse::STATE state );

	static bool					is_double( REAL CONST time, REAL CONST time_last );
//	static	void				init_manymouse();
};



