
#ifdef AAA_BDD_BOID_H
#error "BDD_BOID_H included more than once."
#endif
#define AAA_BDD_BOID_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_BOIDS_H
#	include "boids/boids.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_NET_UTIL_OBJ_H
#	include "obj_ui/com/net_util_obj.h"
#endif

class c_def_node;

class	c_master_boid final : public c_obj_ui 
{
	FACTORY_DECLARE( c_master_boid, c_obj_ui );
private:
public:
	virtual	void	param_init_pt();
};


class	c_bdd_boid final : public c_bdd_multiple 
{
	FACTORY_DECLARE( c_bdd_boid, c_bdd_multiple );
private:
	c_boids				_boids;

	REAL				_birth_nb_trig_ui;
	REAL				_death_nb_trig_ui;
	INT32				_nb_allocated_ui;

	REAL				_time_interval_max_ui;	
	REAL				_time_interval_ui;
	REAL				_time_factor_ui;	
	bool				_b_restart_trig_ui;
	bool				_b_real_time_ui;	
	c_delta_t			_delta_t;

	REAL*				_vec_for_deform;

	INT32				_nb_alive_ui;
	bool				_b_mocap_feed_ui;
	INT32				_mocap_feed_channel_ui;
//	bool				_b_sort_out_by_id;

	//draw
	bool				_b_repulse_draw_ui;
	bool				_b_repulse_draw_net_ease;
	REAL				_repulse_draw_net_ease_in_ui;
	REAL				_repulse_draw_net_ease_out_ui;
	bool				_b_repulse_net_color_define;
	FP32				_repulse_color_ui[4];
	FP32				_repulse_net_color_ui[4];
	bool				_b_repulse_net_draw;
	bool				_b_repulse_net_draw_ui;
	bool				_b_repulse_net_img_use_ui;
	INT32				_s_repulse_net_img_bind_ui;
	REAL				_repulse_net_img_u_begin_ui;
	REAL				_repulse_net_img_u_factor_ui;
	REAL				_repulse_net_img_v_ui;
	REAL				_repulse_draw_factor_ui;

	bool				_b_flock_draw_ui;
	FP32				_flock_color_ui[4];
	REAL				_flock_draw_factor_ui;
	bool				_b_steer_draw_ui;
	FP32				_steer_color_ui[4];
	REAL				_steer_draw_factor_ui;

	bool				_b_box_size_draw_ui;
	bool				_b_repulse_circle_draw_ui;

	REAL				_scale_ui[4];
	REAL				_scale[3];

	bool				_b_draw_circle_ui;
	bool				_b_draw_point_ui;
	bool				_b_draw_line_ui;
	bool				_b_draw_speed_min_ui;
	REAL				_draw_speed_min_ui;
	bool				_b_draw_force_ui;
	bool				_b_draw_force_after_ui;

	o_str				_target_name_symbo_ui;
	c_bdd*				_bdd_target;
	REAL				_transfer_blob_radius_ui;
	REAL				_transfer_translate_ui[3];
	REAL				_transfer_scale_ui[4];

	bool				_b_deform_ui;
	c_def_node*			_def_node_ui;

	c_net_buf_obj		_net_buf;
	typedef	c_map_receiver< c_boids::BOID_CONT, c_boid >	TYPE_MAP_RECEIVER;
	TYPE_MAP_RECEIVER	_map_receiver;

//	typedef	std::vector<c_boid>	boid_cont;
//	typedef	c_map_receiver< boid_cont, c_boid >	TYPE_MAP_RECEIVER;

public:
	static c_master_boid*	master;

			void	init();
			void	alloc(); 
			void	dealloc();
public:
	virtual	void	restart();

	virtual	void	param_init_pt();
	
	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();
			void	draw_forces();

	virtual	void	draw_normal_point( REAL len );

	virtual	INT32	get_point_nb() final override;
	virtual	REAL*	get_point_pt(		INT32 CONST index ) final override;
	virtual	INT32	get_point_and_id(	REAL* dst, INT32 CONST index ) final override;
//	virtual	REAL*	get_points();
	virtual	bool	set_point(			INT32 index, REAL CONST * src ) final override;

			bool	set_box_by_id(		INT32 id, REAL* src );
			bool	set_point_by_id(	INT32 id, REAL* src );
			bool	get_point_by_id(	INT32 id, REAL* dst );

//			void	set_box_size( INT32 index, REAL* box_size );
//			void	set_pos( INT32 index, REAL* src );

	FINLINE	void	create_at( REAL* pos, REAL* speed = nullptr )	{	_boids.create_at( pos, speed );		}

	virtual	void	transfer_blobs_to( BLOBS_CONT& blobs );

	FINLINE	bool	kill_by_id( INT32 id )							{	return _boids.kill_by_id( id );		}
	FINLINE	void	set_killed( c_boid* b )							{	_boids.kill_next_update( b );		}
	FINLINE	void	kill_all()										{	_boids.kill_all();					}
};

