
#ifdef AAA_BDD_PART_H
#error "BDD_PART_H included more than once."
#endif
#define AAA_BDD_PART_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_PLACER_H
#	include "placer.h"
#endif
#ifndef AAA_POID_H
#	include "boids/poid.h"
#endif
#ifndef AAA_NET_UTIL_OBJ_H
#	include "obj_ui/com/net_util_obj.h"
#endif
#ifndef AAA_PARTICLE_H
#	include "obj_ui/bdd/bdd_point/particle.h"
#endif
#ifndef AAA_OBJ_SERVER_H
#	include "infrastructure/obj/obj_server.h"
#endif
#ifndef	AAA_NIB_H
#	include "obj_ui/bdd/bdd_old/nib.h"
#endif
#define PART_SORT_CONT	std::vector

class	c_img_2d;
class	c_seedcam;

//	i :	interface
//	e : emission
//	r : rendering
//	m : movement
class c_bdd_opencl_part;

class c_bdd_particle final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_particle,c_bdd_multiple);
	friend	c_bdd_opencl_part;
public:
	static	c_bdd_particle*		cur;
private:
	c_obj_array_server<c_particle>	_poids;			//	data is there
	c_poids_looper<c_particle>		_looper;

	INT32							_i_axe_u;
	INT32							_i_axe_v;
	INT32							_emission_axe;					//i
	INT32							_emission_axe_last;				//i

	REAL*							_field_in;
	REAL*							_field_out;

	REAL							_dt;
//	REAL							_over_dt;
	c_delta_t						_delta_t;

	//	Emission
	INT32							_nb_allocated_ui;				//i
	INT32							_nb_allocated;

	INT32							_nb_min;						//i

	INT32							_nb_max_ui;						//i
	INT32							_nb_max;
	INT32							_nb_part_alive;					//i

	REAL							_nb_by_sec;						//i
	REAL							_nb_to_create_trig;				//i
	bool							_b_flow_continuous;				//i
	bool							_b_regular;
	INT32							_nb_active;
	REAL							_nb_to_create_left_over;

//	INT32							index_start;
//	INT32							index_stop;

	REAL							_life_time;						//i
	REAL 							_life_time_jitter;				//i
	REAL 							_life_time_jitter_gain;			//i
	REAL 							_life_time_jitter_bias;			//i
	c_poid::ID_GENERATOR			_s_id_generator;				//i

	//	Box
	INT32							_s_living_box_type;				//i
	REAL							_living_box_bounce_factor;		//i
	REAL							_living_box_min[3];
	REAL							_living_box_max[3];
	REAL							_living_box_size[3];
	REAL							_living_box_size_ui[3];			//i
	REAL							_living_box_center[3];			//i
	REAL							_living_box_size_factor;		//i

	c_bdd*							_emission_bdd;
	o_str							_emission_bdd_name_sym;			//i
	REAL							_emission_origin[3];			//i
	REAL							_emission_origin_last[3];
	REAL							_emission_origin_speed[3];
	REAL							_emission_origin_speed_last[3];
	bool							_b_emission_use_prev_ui;		//i

	c_placer						_placer;
	REAL							_emission_box_size_ui[3];		//i
	REAL							_emission_box_size[3];
	REAL							_emission_box_size_factor;		//i

	bool							_b_emission_image_use_ui;		//i 
	REAL							_emission_image_u_factor;		//i
	REAL							_emission_image_v_factor;		//i
	REAL							_emission_image_u_offset;		//i
	REAL							_emission_image_v_offset;		//i
	bool							_b_emission_image_clamped_ui;	//i

	REAL							_emission_speed_ui[3];			//ie
	REAL							_emission_speed[3];
	REAL							_emission_speed_factor;			//ie
	REAL							_emission_object_speed_factor;	//ie
	bool							_b_emission_object_speed_interval;		//ie
	REAL							_emission_object_speed_interval_min;	//ie
	REAL							_emission_object_speed_interval_max;	//ie

	//	Jitter
	REAL							_jitter_cone_angle;				//ij
	REAL							jitter_speed_abs;				//ij
	REAL							jitter_jitter;					//ij
	REAL							jitter_jitter_gain;				//ij
	REAL							jitter_jitter_bias;				//ij
	REAL							jitter_jitter_last;
	REAL							jitter_jitter_gain_last;
	REAL							jitter_jitter_bias_last;
	INT32							_jitter_vec_nb;					//ij
	INT32							_jitter_vec_nb_last;
	INT32							_jitter_seed;					//ij
	INT32							_jitter_seed_last;
	REAL							_jitter_speed[3];				//m
	REAL							_jitter_factor[3];				//ie
	REAL							_jitter_factor_last[3];

	//	Physic
	bool							_b_accel;
	REAL							_accel[3];						//im
	bool							_b_speed;
	REAL							_speed[3];						//im
	bool							_b_offset;
	REAL							_offset[3];						//im
	REAL							_speed_max;						//im

	//	Turbulence
	REAL							turb_accel;						//it
	REAL							turb_speed;						//it
	bool							b_turb_fractal_sum;				//it
	REAL							turb_freq;						//it
	INT32							turb_harm_nb;					//it
	REAL							turb_gain;						//it
	REAL							turb_bias;						//it
	REAL							turb_offset_ui[3];				//it
	REAL							turb_offset[3];
	REAL							turb_factor[3];					//it
	REAL							turb_accel_factor[3];
	REAL							turb_speed_factor[3];
	bool							_b_turb_factor_axe[3];
	bool							_b_turb_factor;

	//	Rendering
	INT32							_s_render_type;					//ir
	REAL							_render_size_min;				//ir
	bool							_b_render_life_color;			//ir
	REAL							_render_life_color_ease_in;
	REAL							_render_life_color_ease_out;
	bool							_b_render_life_color_map_ui;	//ir
	bool							_b_render_life_color_map;
	REAL							color_map_v	;
	REAL							color_map_dv ;
	REAL							color_map_alpha_gain;
	REAL							color_map_alpha_bias;
	REAL							color_map_alpha_gain_factor;
	REAL							color_map_alpha_bias_factor;
	bool							b_render_map_interp;			//ir

	bool							_b_clipping_ui;					//i
	bool							_b_render_line_smooth;			//ir
	REAL							_render_aperture;				//ir
	REAL							_render_size[3];
	bool							_b_render_size_life;			//ir
	REAL							_render_size_life_bias;			//ir
	REAL							_render_size_life_bias_factor;
	REAL							_render_size_death_ui[4];		//ir
	REAL							_render_size_death[3];
	REAL							_render_size_half;	// to keep the particle size


	bool							_b_scale;
	REAL							_scale_ui[4];					//ir
	REAL							_scale[3];

	bool							_b_multiple_use_list;

	REAL*							_vec_jitter;
//	REAL							f_axe_u;
//	REAL							f_axe_v;

	INT32							_id_generator;

	bool							_b_restart_trig_ui;

	INT32							_s_bind_emission_map;
	c_img_2d*						_img_emission_map;

	INT32							_s_bind_life_color_map;
	c_img_2d*						_img_life_color_map;

//visibility
	bool							_b_visibility;
	bool							_b_visibility_ui;
	INT32							s_bind_visibility;
	c_img_2d*						img_visibility;
	REAL							img_visibility_u_ori;
	REAL							img_visibility_v_ori;
	REAL							img_visibility_u_factor;
	REAL							img_visibility_v_factor;
	bool							_b_visibility_clamped_ui;

	INT32							_s_render_sort;				//ir
	REAL							_render_sort_min;			//ir
	REAL							_render_sort_max;			//ir
	bool							_b_sort_ease_color;			//ir
	REAL							_sort_color_begin;			//ir
	REAL							_sort_color_end;			//ir
	REAL							_sort_color_ease_in;		//ir
	REAL							_sort_color_ease_out;		//ir
	REAL							_sort_color_nor_factor;

	REAL							rotate_freq;				//i
	REAL							rotate_space_freq;			//i
	INT32							rotate_space_axe;			//i
	REAL							rotate_id_factor;			//i
	REAL							rotate_direction[3];		//i

	bool							b_internal_def;
	bool							b_field;
	bool							b_field_speed_ui;			//im
	bool							b_field_speed;
	bool							b_field_size;
	bool							b_field_acc_ui;				//im
	bool							b_field_acc;
	REAL							_field_size[3];
	REAL							_field_size_ui[4];
	REAL							viscosity;
	bool							b_move;
	bool							_b_deformer;

	INT32							s_special;

	c_net_buf_obj					_net_buf;

	bool							_b_feed_bdd_point;
	bool							_b_feed_bdd_point_ui;		//i

	bool							_b_part_close;
	INT32							_part_close_nb;
	REAL							_part_close_dist;

	std::vector<REAL>				_add_pos;

	PART_SORT_CONT<c_particle*>		_part_sort;
	PART_SORT_CONT<c_particle*>		_part_close;

private:
	FINLINE	void		do_color(				c_particle* pa	);
	FINLINE	void		draw_single_line_one(	c_particle* pa, c_seedcam* cam, bool const b_color_change,							REAL t, REAL t_last, bool const b_color_inter );
	FINLINE	void		draw_single_point_one(	c_particle* pa, c_seedcam* cam, bool const b_color_change		);
	FINLINE	void		draw_single_quad_one(	c_particle* pa, c_seedcam* cam, bool const b_color_change,	REAL*& p_field_size,	REAL t, REAL t_last, bool const b_rot_and_scale );
	FINLINE	void		draw_multiple_one(		c_particle* pa, c_seedcam* cam, bool const b_color_change,	REAL*& p_field_size,	REAL t, REAL t_last, bool const b_rot_and_scale );
	REAL*				_field_tmp; 

public:
	template< bool B_TURB, INT32 S_LIVING_BOX >	
	FINLINE	void		move_one(				c_particle* pa );	
public:
			void		init();
			void		alloc(); 
			void		dealloc();

	virtual	void		update();
	virtual	void		draw_single();
	virtual	void		draw_multiple();

#if	APP_SPECIAL_NIB()
			void		nib_move();
			void		nib_draw();
#endif
	virtual	void		restart();

			void		alloc_part(				INT32 CONST nb_in ); 
			void		dealloc_part();

			void		init_vec_jitter(		INT32 CONST nb_in );
			void		alloc_vec_jitter(		INT32 CONST nb_in );
			void		dealloc_vec_jitter();
	
			void		kill_one(				c_particle* CONST p_part );
	FINLINE	c_particle*	create_one(				INT32 CONST index, REAL CONST birth_time,						bool CONST b_ressucitate );
			bool		place_and_create_one(	INT32 CONST index, REAL CONST birth_time, REAL CONST time_cur,	bool CONST b_ressucitate );
			void		place_one_on_side(		c_particle* CONST p_part, INT32 CONST axe );
			bool		kill_by_id(				INT32 CONST id );

			void		copy_pos_to(			REAL* dst );
			void		copy_pos_to_draw_from(	REAL CONST * src );
			void		copy_pos_to_pos_to_draw();
private:
	FINLINE	c_particle*	get_particle(			INT32 CONST index );
public:
	virtual	INT32		get_point_nb() final override;
	virtual	bool		get_point(							REAL* CONST dst, INT32 CONST index ) final override;
	virtual	INT32		get_point_and_id(					REAL* CONST dst, INT32 CONST index ) final override;
	virtual	bool		get_point_and_speed_rnd_time_rel(	REAL* CONST dst, REAL* CONST speed, REAL t_in ) final override;

			void		add_particle_at(		REAL* pos = nullptr );

	virtual bool		can_implicit() final override { return true; }

			void		create_and_kill(		INT32 nb_to_create_this_frame );
			void		move();


	virtual	void		param_init_pt();
			AAA_ERR		load_do_before(			o_str CONST & filename );

			void		net_send_create(		INT32 CONST index, REAL CONST t );
			void		net_send_kill(			INT32 CONST index );

//	void net_receive_one();
			void		do_sort();
			void		do_close();

//	FINLINE	PART_SORT_CONT<c_particle*>&	get_part_close()		CONST	{ return _part_close; }
//	FINLINE	REAL							get_part_close_dist()	CONST	{ return _part_close_dist; }
};


