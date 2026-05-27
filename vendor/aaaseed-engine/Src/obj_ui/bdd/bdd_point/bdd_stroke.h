
#ifdef AAA_BDD_STROKE_H
#error "BDD_STROKE_H included more than once."
#endif
#define AAA_BDD_STROKE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_STROKE_H
#	include "draw/stroke.h"
#endif
#ifndef AAA_REGISTRY_GENERIC_H
#	include "infrastructure/factory/registry_generic.h"
#endif

class	c_bdd_stroke final
	: public c_bdd_multiple
{
//	typedef std::vector<c_stroke_tablet*> n_strokes;

	FACTORY_DECLARE( c_bdd_stroke, c_bdd_multiple );
public:
	enum DRAW_METHOD : INT32
	{
		DRAW_ALL = 0,
		DRAW_BY_STROKE,
		DRAW_BY_TIME,
		DRAW_METHODE_MAX_NB
	};

//	static	CONST	INT32	MOCAP_CHANNEL_NB_MAX = CHANNEL_NB_MAX;
private:
//	static	c_instance_by_channel< c_bdd_stroke, MOCAP_CHANNEL_NB_MAX > inst_by_channel;
public:
//	static	c_bdd_stroke*	get_from_channel( INT32 channel_id )	{	return inst_by_channel.get( channel_id );	}
public:
//	static	c_bdd_stroke*	def;
//	static	c_bdd_stroke*	cur;
//	static	c_bdd_stroke*	ui;

private:
//	INT32										_channel_id;		//	use to find mocap object by Channel
	bool										_b_contact;
	bool										_b_draw_deplacement;
	bool										_b_draw_crosshair;
	bool										_b_draw_rect;
//	bool										_b_dataset_empty;

	bool										_b_stroking;
	INT32										_stroke_nb;
	INT32										_point_nb;
	DOUBLE										_duration;

	FP32										_prim_size_min;
	FP32										_prim_size_factor;

	FP32										_prim_size_min_ui;
	FP32										_prim_size_max_ui;
	FP32										_prim_size_factor_ui;

//	REAL										_time_factor;
//	REAL										_time_factor_ui;
//	REAL										_frame_by_sec;
//	REAL										_frame_length;
//	REAL										_time_len;
//	REAL										_phase;
//	REAL										_phase_last;

	bool										_b_erase_last_trig_ui;
	bool										_b_erase_drawn_last_trig_ui;
	bool										_b_erase_move_all_trig;
	bool										_b_erase_all_trig;
	bool										_b_erase_before_trig;
	bool										_b_erase_drawn_trig;
	bool										_b_erase_after_trig;
	bool
												_b_restart_trig_ui;
	bool										_b_record;
	bool										_b_record_ui;

//	bool										_b_play;
//	bool										_b_play_ui;
//	bool										_b_loop_ui;

	REAL										_origin_ui[3];
	REAL										_origin_store[3];
	bool										_b_center_strokes_trig;

	c_strokes_tablet*							_strokes_cur;

	FP32										_color_move_ui[5];

	bool										_b_verbose_ui;
	bool										_b_ui_intercept_ui;

	INT32										_dataset_id_ui;
	INT32										_dataset_id;
	bool										_b_dataset_save_trig;
	o_str										_dataset_dir;
	c_map_server< INT32, c_strokes_tablet >		_datasets;

	o_str										_filename_store;

	bool										_b_move;	
	REAL										_pos_store[2];

	DRAW_METHOD									_s_draw_method;
	REAL										_draw_phase_begin;
	REAL										_draw_phase_end;
	INT32										_draw_stroke_begin;
	INT32										_draw_stroke_end;

//	INT32										_net_channel;
//	bool										_b_net_send_ui;

//	mutable aaa::MUTEX							_data_lock;
			void	init();
			void	update_info();

public:
	static	INT32 CONST	DATASET_ID_MAX	=	256;

//			bool	alloc(				INT32 nb_frame_in,	INT32 node_nb_in ); 
	virtual	void	dealloc();
			void	erase_strokes_all();
			void	erase_stroke_last();
			void	erase_stroke_drawn_last();
			void	erase_stroke_move_all();

	virtual	void	param_init_pt();
	virtual	void	param_init();
	virtual	void	update();
	virtual	void	restart();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

private:
//			bool	get_point_on_seg(	REAL* CONST pt_in, REAL CONST when, INT32 CONST seg_id, REAL CONST where );
//			INT32	get_segment_rnd();
//	FINLINE	void	get_draw_node_start_stop( INT32& start, INT32& stop );
//			void	draw_path_multiple();
			void	set_dataset(			INT32 dataset_id );

			AAA_ERR	save_dataset(			o_str CONST & filename_in, INT32 dataset_id );
			void	save_dataset_current();
			AAA_ERR	load_dataset(			o_str CONST & filename_in, INT32 dataset_id, INT32 zero_nb );
		
			void	make_filename(			o_str& filename_dst, o_str CONST & filename_in, INT32 dataset_id, INT32 zero_nb );
			o_str& 	do_before_load_save(	o_str CONST & filename_in, INT32 dataset_id, INT32 zero_nb );
			void	do_after_load_save();

public:
	//
	//	UI_INTERCEPT
	//
	virtual	bool	is_mouse_use();

	virtual	bool	can_ui_intercept()			{	return true; }
	virtual bool	set_ui_intercept( bool b )	{	return _b_ui_intercept_ui = b; }
	virtual bool	is_ui_intercept()			{	return _b_ui_intercept_ui; }

	virtual	bool	do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y );

	virtual	bool	mouse_down(					FP32& u_start, FP32& v_start );
	virtual	void	mouse_move(					FP32 u_in, FP32 v_in );
	virtual	void	mouse_up(					FP32 u_in, FP32 v_in );

//	virtual	bool	get_point_rnd(						REAL* CONST dst,			        REAL CONST when ) final override;
//	virtual	bool	get_point_and_speed_rnd_time_abs(	REAL* CONST dst, REAL* CONST speed,	REAL CONST when ) final override;
//	virtual	bool	get_point_and_speed_rnd_time_rel(	REAL* CONST dst, REAL* CONST speed,	REAL CONST when ) final override;
//			void	build_segment_weight();

//	virtual	AAA_ERR	save_do_after(	o_str CONST & filename_in );
	virtual AAA_ERR	load_do_after(	o_str CONST & filename_in );

	FINLINE	bool	is_valid_stroke_id(		INT32 id )
					{
#if	AAA_DEBUG()
						//todoqq
						//		if( node_id < 1 || node_id > _node_nb || _seq->len_rel == 0)
						//		debug_break( "%s() invalid node id", __FUNCTION__ );
#endif
						return INSIDE_MIN_MAX( id, 1, _stroke_nb );
					};

//	FINLINE	REAL*	get_data(			INT32 frame );
//	FINLINE	REAL*	get_data_index(		INT32 index );
//	FINLINE	REAL*	get_data(			INT32 frame,	INT32 node_id	)	{	return get_data_node(	get_data(frame),	node_id );	}
//	FINLINE	REAL*	get_data_cur(						INT32 node_id	)	{	return get_data_node(	_data_cur,			node_id );	}

//			void	lock();
//			void	unlock();
};


