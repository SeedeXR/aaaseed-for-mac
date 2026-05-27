
#ifdef AAA_BDD_CURVE_EDIT_H
#error "BDD_CURVE_EDIT_H included more than once."
#endif
#define AAA_BDD_CURVE_EDIT_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_AAA_CONST_H
#	include "infrastructure/aaa_const.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

class c_control_key;

class	c_bdd_curve_edit final : public c_bdd_multiple
{
	FACTORY_DECLARE( c_bdd_curve_edit, c_bdd_multiple );
public:
	static	INT32 CONST	CURVE_EDIT_CHANNEL_NB_MAX = CHANNEL_NB_MAX;
private:
	static	c_instance_by_channel< c_bdd_curve_edit, CURVE_EDIT_CHANNEL_NB_MAX > inst_by_channel;
public:
	static	c_bdd_curve_edit*	get_from_channel( INT32 channel_id )	{	return inst_by_channel.get( channel_id );	}

private:
	INT32		_channel_id;
public:
	static	c_bdd_curve_edit*	cur;

	using cont_control = std::vector<c_control_key*>;
	using iter_control = cont_control::iterator;

private:
	INT32			_control_id_cur;
	c_control_key*	_p_control_cur;

	cont_control	_controls;					//	data allocated is here
	c_control_key*	_control_phase_data;		//	and here for phase data

	c_control_key*	_control_phase;				//	phase used

	REAL		_draw_min[2];
	REAL		_draw_max[2];
	REAL		_draw_min_ui[2];
	REAL		_draw_max_ui[2];
	///	INT32	curve_edit_;
	INT32		_s_visu_curve;
	INT32		_s_visu_axe;

	REAL		_phase_cur;
	bool		_b_phase_control_ui;
	c_delta_t	_delta_t;
	bool		_b_phase_rel_ui;
//	bool		_b_follow_phase_ui;
	bool		_b_play_ui;
	bool		_b_stop_on_phase_ui;
	bool		_b_loop_ui;
	bool		_b_forward_ui;
	bool		_b_play_trig_ui;
	bool		_b_stop_trig;
	bool		_b_prev_trig;
	bool		_b_prev_trig_ui;
	bool		_b_next_trig;
	bool		_b_next_trig_ui;
	bool		_b_prev_step_trig;
	bool		_b_prev_step_trig_ui;
	bool		_b_next_step_trig;
	bool		_b_next_step_trig_ui;
	bool		_b_restart_trig_ui;
	REAL		_center_ui[3];
	REAL		_center[3];
//	REAL		_size[3];

	bool		_b_clear_trig_ui;

	REAL		_ui_size_marker_ui[3];
	bool		_b_ui_draw_ui;
	bool		_b_ui_draw_axe_ui;
	bool		_b_ui_draw_phase_cur_ui;
	bool		_b_grid_draw_ui;
	bool		_b_ui_draw_curve_ui;
	bool		_b_ui_draw_curve_cur_only_ui;
	bool		_b_ui_draw_last_ui;
	INT32		_draw_last_nb;
	bool		_b_ui_draw_point_ui;
	bool		_b_ui_color_auto_ui;
	bool		_b_ui_draw_follow_phase_ui;
	bool		_b_ui_draw_phase_ui;

	bool		_b_crosshair_draw_ui;
	REAL		_crosshair_u;
	REAL		_crosshair_v;

	REAL		_grid_min[2];
	REAL		_grid_max[2];
	INT32		_grid_nb_u;
	INT32		_grid_nb_u_sub;
	INT32		_grid_nb_v;
	INT32		_grid_nb_v_sub;

	bool		_b_grid_lock_u_ui;
	REAL		_grid_lock_u;
	bool		_b_grid_lock_v_ui;
	REAL		_grid_lock_v;

	bool		_b_ui_intercept_ui;
	bool		_b_ui_lock_u_ui;
	bool		_b_ui_lock_v_ui;
	bool		_b_ui_phase_ui;
//	REAL		ui_pos_[3];
	
	INT32		_color_loop_max;
	INT32		_type_default;
	INT32		_control_nb;
//	INT32		control_start;
//	INT32		control_end;

//	bool		_b_begin;

//	INT32		curve_edit_datagrid_id_;

	bool		_b_need_save;

	INT32		_i_col_auto_cur;

	REAL		_text_line_width;
	REAL		_text_offset[2];
	REAL		_text_scale[2];

private:
			void			clear();
			void			dealloc();

			FINLINE	INT32	build_id( INT32 id );	// 1 based
			c_control_key*	control_new();
	FINLINE	c_control_key*	control_get_low( INT32 id_in );
			c_control_key*	control_get( INT32 id_in );
			c_control_key*	control_get_if_exist( INT32 id_in );

			c_control_key*	control_insert();
			void			control_delete();
			void			control_dealloc();
			c_control_key*	control_next();
			c_control_key*	control_prev();

			void	draw_control_cur_name();
			void	draw_grid();
			void	draw_axes();
			void	draw_control_phase();
			void	draw_1d();
			void	draw_2d();
			void	draw_3d();
			void	draw_phase_2d();
			void	draw_crosshair();
public:
	//
	//	UI_INTERCEPT
	//
	virtual	bool	can_ui_intercept()			{	return true; }
	virtual bool	set_ui_intercept( bool b )	{	return _b_ui_intercept_ui = b; }
	virtual bool	is_ui_intercept()			{	return _b_ui_intercept_ui; }

	virtual	bool	do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y );

	virtual	bool	mouse_down(					FP32& u_start, FP32& v_start );
	virtual	void	mouse_move(					FP32 u_in, FP32 v_in );
	virtual	void	mouse_up(					FP32 u_in, FP32 v_in );

			void	channel_register();

			void	init();
	virtual	void	param_init_pt();

	virtual	AAA_ERR	load_do_after(				o_str CONST & filename );
	virtual AAA_ERR	save_do_after(				o_str CONST & filename );

	c_control_key*	control_get_by_name(		C_PCHAR_C name );

			REAL	control_get_phase(			INT32 id_in, INT32 key_id );	//todo
			REAL	control_get_value(			INT32 id_in, REAL in );
			REAL	control_get_value_by_name(	C_PCHAR_C name, REAL phase );
			REAL	control_get_value(			INT32 id_in );
			REAL	control_get_value_by_name(	C_PCHAR_C name );
			REAL	control_get_value_axe(		INT32 id_in, INT32 axe );
			REAL	control_get_value_axe(		INT32 id_in, REAL in, INT32 axe );

//	void	alloc_curve();

	virtual	void	draw_single();
	virtual	void	draw_multiple();
	virtual	void	update();
};

