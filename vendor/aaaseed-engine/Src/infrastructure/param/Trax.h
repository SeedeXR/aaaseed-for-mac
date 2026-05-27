
#ifdef AAA_TRAX_H
#error "TRAX_H included more than once."
#endif
#define AAA_TRAX_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif


class	c_flux_filter;
class	c_averager;
struct	ST_TRAX_EVENT;

class	c_trax final : public c_obj_active_ui
{
	friend	class c_lua_wrap;
	FACTORY_DECLARE( c_trax, c_obj_active_ui );

public:
	static CONSTEXPR INT32 IN_ACTIVE_BOOL_NB = 8;

private:
	INT32			_fn_type;
	INT32			_fn_type_ui;
	INT32			_output_type;
	REAL			_threshold;

	DOUBLE			_freq_ui;
	DOUBLE			_phase_ui;
	DOUBLE			_phase;
	DOUBLE			_phase_cur;
	DOUBLE			_phase_last;
	REAL			_ease_before;
	
	c_delta_t		_delta_t;

	REAL			_ease_after;
	bool			_b_trig_last;
	bool			_b_trig_last_3d[3];
	bool			_b_restart_trig_ui;
	bool			_b_record;

	bool			_b_in_active[IN_ACTIVE_BOOL_NB];

	INT32			_channel_id;
	INT32			_control_index_ui;		//todo before 2025 February was starting at 1 but changed to 0 : not sure of all the side effects
	INT32			_channel_id_bis;
	INT32			_control_index_bis_ui;	//todo before 2025 February was starting at 1 but changed to 0 : not sure of all the side effects

	REAL			_min;
	REAL			_max;
	REAL			_offset;
	REAL			_gain;
	REAL			_bias;
	REAL			_round;

	INT32			_trax_index;

	INT32			_s_limit;
	REAL			_limit_min;
	REAL			_limit_max;

	REAL			_value_fn_out;
	REAL			_value_out[3];
	REAL			_value_last_low[3];
	INT32			_s_out_sel[3];
	REAL			_filter_factor ;
	C_PCHAR			_str_out;

	o_str			_format;
	o_str			_comment;
	o_str			_var_name;

	INT32			_s_net_out;
	INT32			_net_channel_id;
//	INT32			_net_control_id;

	INT32			_s_max_out;
	INT32			_max_channel_id;
	INT32			_max_control_id;

	INT32			_s_draw;
	FP32			_color[4];
	REAL			_line_size;
	bool			_b_draw_fixed;
	FP32			_draw_factor;

	INT32			_dim;
	c_averager*		_averager;
	c_flux_filter*	_flux;
	ST_TRAX_EVENT*	_p_event;
	ST_TRAX_EVENT*	_p_event_cur;
	ST_TRAX_EVENT*	_p_event_limit;
	INT32			_event_index_max;

//	bool			b_midi_out;
	c_connex*		_con_last;		//	no allocation just store to detect change

//	INT32			s_filter_type;
	DOUBLE			_change_val_old[3];
	DOUBLE			_change_val_new[3];
	DOUBLE			_change_val_last[3];
//	REAL			change_start_time;

	CHAR			_letter[2];

			void		dealloc();
//			void		alloc(INT32 param_nb);
			void		alloc_events(); 
			void		dealloc_events();

	FINLINE	void		compute_phase_time();
	FINLINE	void		compute_phase();
	FINLINE	void		get_double_in(	DOUBLE& val );

	FINLINE	void		flux_update();
	FINLINE	void		compute_fn_1d(	DOUBLE& val );
	FINLINE	void		compute_fn_3d(	DOUBLE* val );
	FINLINE	void		update_low_1d();
	FINLINE	void		update_low_3d();
			void		update_low();

	FINLINE		c_param*	get_plug_in_then_out_first();

	FINLINE	c_param*	get_plug_in(	INT32& i );
	FINLINE	c_param*	get_plug_in_first();

	FINLINE	c_param*	get_plug_out(	INT32& i );
	FINLINE	c_param*	get_plug_out_first();
protected:
			void		update_param_header();
public:
//constructor linked	
			void		trax_init( INT32 index, C_PCHAR_C str_id );

			void		build_name( C_PCHAR_C name_in );
	virtual	void		param_init_pt();
	virtual void		prepare_for_ui();
	virtual	void		param_init();
	virtual	o_str*		get_comment()	{ return &_comment; }

	virtual	void		cell_draw_obj( REAL CONST size );

	FINLINE	void		update()
						{
							if( is_active() )
								update_low();
						}

			void		unplug_in_all()	{ c_obj_ui::unplug_in_all(); }

	virtual AAA_ERR		save_do_after( o_str CONST & filename );
	virtual AAA_ERR		load_do_after( o_str CONST & filename );

			void		plug_out(	c_obj_ui* obj, c_param* param );
			void		plug_in(	c_obj_ui* obj, c_param* param );

	virtual void		draw();
			void		start_record();
			void		stop_record();
			void		start_loop( bool CONST b_record_in );
			void		stop_loop();
			void		make_slave();

			void		set_fn_type(	INT32 i	)	{ _fn_type_ui = i; }
			void		set_fn_midi(	INT32 channel_id_in , INT32 control_index_in );
			void		set_index(		INT32 index	);
			INT32		get_index();
//			INT32		get_trax_number() { return trax_number; }

	FINLINE	INT32		get_net_channel_id();
	FINLINE	INT32		get_net_control_id();

			AAA_ERR		save_data_to_file(		o_str CONST & filename );
			AAA_ERR		load_data_from_file(	o_str CONST & filename );

	static	INT32		get_index_from_param_pt(	c_param* p_in );
	static	C_PCHAR_C	get_type_from_param_pt(		c_param* p_in );
	static	INT32		menu_build( INT32 base_id_in, PT_MENU_FN menu_fn );

//	void	err_print( C_PCHAR_C str );
//	void	err_print( C_PCHAR_C stra, C_PCHAR_C strb );
//	AAA_ERR	read_data_3d();
//	AAA_ERR	read_data_3d_from_file( CONST char *CONST filename);
};

