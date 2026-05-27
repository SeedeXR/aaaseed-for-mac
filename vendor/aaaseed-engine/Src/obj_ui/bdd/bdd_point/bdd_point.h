
#ifdef AAA_BDD_POINT_H
#error "BDD_POINT_H included more than once."
#endif
#define AAA_BDD_POINT_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_AAA_TIME_H
#	include "time/aaa_time.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef _UNORDERED_MAP_
#	include <unordered_map>
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class c_dot;

//	created for "ce qui nous regarde" at Aichi Japan
//		was our first version at having a tracking over time using Point grey bumble bee tracking
//			
class	c_bdd_point final : public c_bdd_multiple
{
	friend	class	c_dot;
	FACTORY_DECLARE(c_bdd_point,c_bdd_multiple);
private:
	REAL				_origin[3];
	REAL				_size[3];
	INT32				_max_sample;
	INT32				_nb_to_draw;
//	REAL*				pos;

	bool				_b_net_receive_ui;
	bool				_b_net_send_ui;
	bool				_b_net_send_filtered_ui;
	bool				_b_net_send_analyse_ui;
	INT32				_net_send_analyse_skip_nb;
//	INT32				s_net_dst;

	REAL				_dot_time_to_live;
	INT32				_dot_sample_nb_max;
	bool				_b_restart_trig_ui;

	bool				_b_draw_point_ui;
	bool				_b_draw_line_ui;
	REAL				_draw_interval;
	REAL				_max_interval;
	bool				_b_draw_raw_ui;
	bool				_b_draw_filtered_ui;
	bool				_b_draw_link_ui;
	bool				_b_draw_multiple_raw_ui;

//	REAL				speed_threshold;

	o_str				_tracking_fname;
	bool				_b_play_restart_ui;
	bool				_b_play_ui;
	bool				_b_play_loop_ui;
	bool				_b_play_started;
	bool				_b_play_stopped;
	bool				_b_play_end;

	bool				_b_debug_ui;
	INT32				_dot_data_max_nb	{};

	bool				_b_verbose;
	bool				_b_verbose_time;
	bool				_b_verbose_dot;
	REAL				_time_ui;
	REAL				_timestamp_ui;

	FILE*				_f_tracking;
	REAL				_time_stamp;
//	REAL				time_cur;
	aaa::time::ST_TIME	_time_stamp_begin;
	REAL				_time_stopped;
	aaa::time::ST_TIME	_time_stop;

	bool				_b_analyse_active_ui;
	mutable aaa::MUTEX	_lock;

	typedef	std::unordered_map< INT32, c_dot* >	DOTS_MAP;
	DOTS_MAP			_dots;

	static INT32 CONST DOTS_ID_FOR_TRAXS_NB = 16;
	INT32				_dots_id_for_traxs[DOTS_ID_FOR_TRAXS_NB];
	std::vector<c_dot*>	_dots_free;
public:
	REAL				_speed_stop_threshold;
	REAL				_speed_start_threshold;
	INT32				_speed_start_count;
	REAL				_speed_min;
	REAL				_speed_max;
	INT32				_points_nb;
	INT32				_points_nb_moving;
	INT32				_points_nb_free;
	REAL				_speed_sigma_moving;
	REAL				_speed_sigma;
	INT32				_frame_index;	//todo make sure we don't send it several time using multiscreen
	REAL				_trax_default_y;

//	INT32		mocap_feed_channel;
public:
	bool				_b_filter_active;
	REAL				_inter;
	REAL				_limit_dist;
	REAL				_limit_speed;
	REAL				_limit_accel;
	REAL				_offset[2];
	REAL				_sca[2];
	REAL				_tra[2];

private:
	void	init();
	void	alloc(); 
	void	dealloc();
	void	clear_dots();
public:

	virtual	void	param_init_pt();
	
	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

			void	analyse();
	
//	virtual	void	set_point_nb( INT32 nb );
	virtual	INT32	get_point_nb() final override;

	virtual	bool	get_point( REAL* dst, INT32 CONST index ) final override;
	virtual	bool	get_point_and_speed_rnd_time_rel( REAL* CONST pos, REAL* CONST speed, REAL t_in ) final override;
	
			INT32	ctl_index_from_id( INT32 id );
			void	ctl_get_pos( REAL* dst, INT32 index, bool b_filtered );

			void	receive_osc(	CHAR* str );
			void	treat_str(		CHAR* str );

			void	new_dot(		INT32 CONST id );
			void	new_dot(		INT32 CONST id, REAL CONST x, REAL CONST y, REAL CONST z=0., REAL CONST r=0. );
			void	delete_dot(		INT32 CONST id );
			void	set_dot_pos(	INT32 CONST id, REAL CONST * CONST p, REAL r );
	FINLINE	void	set_dot_pos(	INT32 CONST id, REAL CONST x, REAL CONST y, REAL CONST z, REAL r )
					{
						REAL	vec[3];
						vec[0] = x; vec[1] = y; vec[2] = z;
						set_dot_pos( id, vec, r );
					}
			void	check_dot_time();

	virtual	bool	set_point(		INT32 index, REAL CONST * src );

	virtual	AAA_ERR	load_data_from_filename(	o_str CONST & filename, INT32 type_io );
	virtual	AAA_ERR	load_do_after(				o_str CONST & filename );
			void	read_tracking_from_file(	o_str CONST & filename );
			void	play_init();
			void	play_from_file();
			void	play_restart();
			void	play_stop();
			void	play_end();

			void	lock();
			void	unlock();
};

extern	c_bdd_point*	bdd_point_cur;


