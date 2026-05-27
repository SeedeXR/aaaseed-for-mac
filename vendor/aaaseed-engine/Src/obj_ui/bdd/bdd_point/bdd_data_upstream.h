
#ifdef AAA_BDD_DATA_UPSTREAM_H
#error "BDD_DATA_UPSTREAM_H included more than once."
#endif
#define AAA_BDD_DATA_UPSTREAM_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_OURTIME_H
#	include "time/ourtime.h"
#endif
#ifndef _MAP_
#	include <map>
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

struct	st_door
{
	REAL	time_;
	REAL	pos_[3];
	REAL	size_[3];
	REAL	rot_;
	REAL	depth_;
	INT32	id_;
	REAL	left_;
	REAL	right_;
	REAL	y_[4];
};

//struct	st_idoor
//{
//	st_door	door_;
//};

//typedef	vector<st_door>	door_vector;
class	c_door_state
{
public:
	INT32			owner_id_;
	INT32			new_owner_id_;
	BOOL			b_active_;
	REAL			presence_;
	BOOL			b_touched_;
public:
	c_door_state();
	~c_door_state()	{};
	void		init();
};

class	c_door_stream
{
public:
	deque<st_door>	data_;
	REAL			time_begin_;
	REAL			time_end_;
	c_door_state	state_;
public:
	c_door_stream();
	~c_door_stream();

	void		init();
	void		deinit();

	st_door*	push( REAL time_in );
	INT32		get_nb();
	st_door*	get_door( INT32 index );

	void		update();
};

class	c_seq_upstream
{
public:
	map<INT32, c_door_stream*>	door_streams_;	//hack public?
	vector<c_door_stream*>		door_stream_free_;
	o_str						name_;
	REAL						duration_;
	REAL						vanish_time_;
	REAL						appear_time_;
	REAL						video_offset_;
	INT32						erased_nb_;
	BOOL						b_used_;	// not used for the moment
	BOOL						b_format_old_;
public:
	void			clear();
	void			reset();
	void			dealloc();
	void			create( INT32 nb);

	c_door_stream*	get_door_stream_new( INT32 obj_id );
	c_door_stream*	get_door_stream_always( INT32 obj_id );
	c_door_state*	get_door_state( INT32 obj_id );
};

CONST	INT32	SEQ_UPSTREAM_MAX_NB = 32;	//todo make it dynamic ?

class	c_bdd_data_upstream final : public c_bdd_multiple
{
	FACTORY_DECLARE( c_bdd_data_upstream, c_bdd_multiple );
private:
	REAL	origin_[3];
	REAL	size_[3];
	INT32	max_sample_;
	INT32	nb_to_draw_;
	REAL*	pos_;

	BOOL	b_door_do_;

/*
//	BOOL	b_analyse_active;
	bool	b_net_send_analyse;
	bool	b_net_receive;
	bool	b_net_send;
	bool	b_net_send_filtered;
	INT32	net_send_analyse_skip_nb;
//	INT32	s_net_dst;
*/
	REAL	dot_ttl_;
	bool	_b_restart_trig_ui;

	INT32	upstream_datagrid_id_;

	BOOL	b_draw_traj_point_;
	BOOL	b_draw_traj_line_;
	BOOL	b_draw_door_;
	BOOL	b_draw_box_;
	BOOL	b_draw_back_to_front_;
	REAL	draw_interval_;
	REAL	max_interval_;

//	REAL	speed_threshold;
	BOOL	b_upstream_;
	REAL	upstream_fps_;
	REAL	upstream_width_;
	REAL	upstream_height_;
	INT32	upstream_x_max_;
	INT32	upstream_y_max_;

	o_str	tracking_fname_;
	BOOL	b_play_restart_;
	BOOL	b_play_;
	BOOL	b_play_loop_;
	BOOL	b_play_started_;
	BOOL	b_play_stopped_;
	BOOL	b_play_end_;

	//BOOL	b_verbose;
	//BOOL	b_verbose_time;
	//BOOL	b_verbose_dot;
	REAL	time_ui_;
	REAL	time_cur_;
	//REAL	timestamp_ui;

	c_delta_t	delta_t_;
	REAL	dt_;
	REAL	time_stamp_;
	ST_TIME		time_stamp_begin_;
	REAL		time_stopped_;
//	ST_TIME		time_stop;
	REAL	time_last_;
	REAL	time_factor_;
	REAL	contact_dist_;
	BOOL	b_contact_only_first_;
	BOOL	b_contact_reset_every_frame_;

//SEQ
	BOOL	b_seq_reset_on_restart_;

	c_seq_upstream	_seqs[SEQ_UPSTREAM_MAX_NB];
	c_seq_upstream*	seq_cur_;
	INT32			seq_cur_index_;
	INT32			seq_nb_;
	INT32			seq_ui_index_;
	INT32			erased_nb_;

	REAL	vanish_time_factor_;
	REAL	appear_time_factor_;

	map<REAL,st_door>	doors_cur_;

public:
	INT32	speed_start_count_;
	REAL	speed_min_;
	REAL	speed_max_;
	INT32	points_nb_;
	REAL	speed_sigma_;
	INT32	frame_index_;	//todo make sure we don't send it several time in mutiscreen

//	INT32		mocap_feed_channel;
public:

public:
	void	init();
	void	alloc(); 
	void	dealloc();
public:

	virtual	void	param_init_pt();
	
	virtual	void	update();
	virtual	void	draw_single();
			void	draw_door();
			void	draw_box();
			void	draw_traj();
	virtual	void	draw_multiple();

			void	draw_single_curve();
			void	draw_door_curve();
			void	draw_box_curve();
			void	draw_traj_curve();
			void	draw_multiple_curve();

//			void	analyse();
	virtual	INT32	get_point_nb();
	virtual	void	get_point( REAL* dst, INT32 index );
	virtual	void	get_point_and_speed_rnd_time_rel( REAL* pos, REAL* speed, REAL t_in );

//			void	receive_osc( CHAR* str);
//			void	treat_str( CHAR* str);

			void			build_door_cur( REAL time );

//			void	dot_new( INT32 id, REAL x, REAL y );
			void	dot_delete( INT32 id );
//			void	dot_pos( INT32 id, REAL* p, REAL r );
/*	FINLINE	void	dot_pos( INT32 id, REAL x, REAL y, REAL z, REAL r )
					{
					REAL	vec[3];
					vec[0] = x; vec[1] = y; vec[2] = z;
					dot_pos( id, vec, r);
					}
			void	dot_check_time();
*/

			void	read_data( FILE* file);
			void	load_files();
	virtual	void	load_data_from_filename( CONST CHAR* CONST filename, INT32 type_io );
	virtual	AAA_ERR	load_do_after( CONST CHAR* CONST filename );
			void	read_data_from_file( CONST CHAR* CONST filename );
			void	read_data_low();
			void	play();

			void	seqs_clear();
			void	seq_set( INT32 index_in );

			void	contact_check();
			// not used for the moment
			BOOL	seq_is_used( INT32 index)	{ return _seqs[index].b_used_; }
};
extern	c_bdd_data_upstream*	bdd_data_upstream_cur;

#endif	//  __BDD_DATA_UPSTREAM_H__
