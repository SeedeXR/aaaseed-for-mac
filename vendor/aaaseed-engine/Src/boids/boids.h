
#ifdef AAA_BOIDS_H
#error "BOIDS_H included more than once."
#endif
#define AAA_BOIDS_H 1


#ifndef AAA_BOID_BASE_H
#	include "boid_base.h"
#endif
#ifndef	_LIST_
#	include <list>
#endif
#ifndef	_DEQUE_
#	include <deque>
#endif
#ifndef	_SET_
#	include <set>
#endif
#ifndef _ATOMIC_
#	include <atomic>
#endif
#ifndef	AAA_PLACER_H
#	include "obj_ui/bdd/bdd_point/placer.h"
#endif
#ifndef AAA_DEF_NODE_H
#	include "obj_ui/deformer/def_node.h"
#endif
#ifndef AAA_OBJ_SERVER_H
#	include "infrastructure/obj/obj_server.h"
#endif
#ifndef AAA_UI_MASTER_H
#	include "ui/ui_master.h"
#endif

//INFLUENCE
class	c_influence final : public c_obj
{
public:
	REAL	_inf;					//ui
	INT32	_i_axe;					//ui
	INT32	_i_u;
	INT32	_i_v;
	REAL	_radius_ext_ui;			//ui
	REAL	_radius_int_ui;			//ui
	REAL	_radius_center;
	REAL	_radius_size_half;
	REAL	_normal_strenght;		//ui
	REAL	_pos[3];				//ui
	INT32	_s_dim;					//ui
	bool	_b_draw_ui;				//ui
protected:
private:
public:
	c_influence();
	virtual ~c_influence();
	virtual void	update();
};

class c_box_3d final : public c_obj 
{
private:
	REAL			_size[3];
	REAL			_min[3];
	REAL			_max[3];

public:
	REAL			_origin_ui[3];	//ui
	REAL			_size_ui[4];	//ui

	FINLINE	void update() NOEXCEPT;
	FINLINE	bool is_inside( REAL CONST * CONST p ) CONST NOEXCEPT
	{
		REAL tmp = *p;
		if( tmp<_min[0] || _max[0]<tmp )
			return false;
		tmp = *(p+1);
		if( tmp<_min[1] || _max[1]<tmp )
			return false;
		tmp = *(p+2);
		return( _min[2]<=tmp && tmp<=_max[2] );
	}
};


//BOIDS
class	c_bdd_point;
class	c_bdd_boid;



class	c_boids final : public c_meca_world
{
public:
	//todo do a master for all the bdd it even all the class
	enum REPULSE_BY_OTHER_TYPE : INT32
	{
		REPULSE_OFF = 0,
		REPULSE_SELF,
		REPULSE_EXPLICIT,
		REPULSE_OTHER,
	//	REPULSE_MIN,
	//	REPULSE_MAX,
		REPULSE_TYPE_MAX_NB
	};	
	enum LOCK_METHOD : INT32
	{
		LOCK_METHOD_MASTER,
		LOCK_METHOD_NO,
		LOCK_METHOD_BY_BOID,
		LOCK_METHOD_BY_CONTACT,
		LOCK_METHOD_MAX_NB
	};
	enum INTERACTION_METHOD : INT32
	{
		INTERACTION_ADD = 0,
		INTERACTION_SWITCH,
		INTERACTION_SNOOTH,
		INTERACTION_NB
	};

	static	C_PCHAR_C		repulse_by_other_type_str[REPULSE_TYPE_MAX_NB];
	static	C_PCHAR_C		lock_method_str[LOCK_METHOD_MAX_NB];
	static	C_PCHAR_C		interaction_method[INTERACTION_METHOD::INTERACTION_NB];

	static	bool			b_master_living_box_draw_ui;
	static	bool			b_master_box_draw_force_ui;
	static	bool			b_master_draw_force_ui;
	static	bool			b_master_repulse_allow_ui;
	static	bool			b_master_flock_allow_ui;
	static	bool			b_master_steer_allow_ui;
	
	static	bool			b_master_parallel_allow_ui;
	static	bool			b_master_parallel_min_nb_force_ui;	
	static	INT32			master_parallel_min_nb_alive_ui;
	static	LOCK_METHOD		s_master_parallel_lock_method_ui;

	static	REAL			master_dist_scale_ui;
	static	REAL			master_dist_repulse_scale_ui;
	static	REAL			master_dist_flock_scale_ui;
	static	REAL			master_dist_steer_scale_ui;



	static	master::NO_ALLOW_FORCE	s_master_net_draw_ui;
	static	master::NO_ALLOW_FORCE	s_master_repulse_draw_ui;
	static	master::NO_ALLOW_FORCE	s_master_flock_draw_ui;
	static	master::NO_ALLOW_FORCE	s_master_steer_draw_ui;


	struct st_box_living
	{
		c_poid::LIVING_BOX_TYPE			s_type[3];
		DOUBLE							influence_repulse;
		REAL							border_size_over_one;
		REAL							min[3];
		REAL							max[3];
		REAL							size[3];
		REAL							size_half[3];
		REAL							radius;
		REAL							radius_squared;

		bool							b_active_ui	{false};	//ui
		c_poid::LIVING_BOX_TYPE			s_type_all_ui;			//ui
		c_poid::LIVING_BOX_TYPE			s_type_ui[3];			//ui		
		REAL							influence_ui;			//ui
		REAL							exponent_ui;			//ui
		REAL							border_size_ui;			//ui
		REAL							pos_ui[3];				//ui
		REAL							size_ui[4];				//ui
		bool							b_sphere_ui;			//ui
		bool							b_draw_ui;				//ui				


	};

private:
	typedef c_boid*				pboid;
	typedef	std::list<c_boid>	BOID_CONT;
	typedef	std::list<c_boid*>	BOID_CONT_PT;

	friend class c_bdd_boid;
	friend class c_boid;

	INT32							_id_ui;
	INT32							_universe_id_ui;

	//	cache to opt fns call
	INT32							_s_dim_ui;								//ui
	REAL							_cluster_by_unit_ui;					//ui
	c_influence						_inf_target;
	c_influence						_inf_death;

	INT32							_cluster_interact_nb;
	REAL							_interact_dist;
	REAL							_interact_dist_squared;
	REAL							_interact_radius_to_substract;
	REAL							_interact_dist_edge;
	REAL							_interact_internal_radius_ui;

	REAL							_diameter_ui;
// ALLOCATED / ALIVE
	INT32							_nb_allocated;
	INT32							_nb_alive_min_ui;						//ui
	INT32							_nb_alive_max_ui;						//ui
// PARALLEL
	bool							_b_parallel;
	bool							_b_parallel_ask_ui;
	INT32							_parallel_min_nb_alive;
	LOCK_METHOD						_s_lock_method;
	LOCK_METHOD						_s_lock_method_ui;
// CURVATURE
	bool							_b_curvature_constraint_ui;
	REAL							_curvature_turn_by_sec_ui;
	DOUBLE							_curvature_sin_threshold;
	DOUBLE							_curvature_cos_threshold;
// REPULSE base
	bool							_b_repulse;
	REAL							_repulse_factor;
	REAL							_repulse_dist;
	REAL							_repulse_dist_squared;
	REAL							_repulse_dist_over_one;
// FLOCK base
	REAL							_flock_dist_squared;
	REAL							_flock_dist_squared_interior;
// STEER base
	REAL							_steer_dist_squared;
// TEST
	INT32							_test_sucess_ui;
	INT32							_test_failed_ui;
	REAL							_test_score_ui;
// TIME
	REAL							_dt;
	DOUBLE							_over_dt;
	DOUBLE							_over_dt_squared;
//	DOUBLE							_over_dt_pow;

	bool							_b_need_contact;
// VISIBILITY
	INT32							_nb_seen_max_ui;
	bool							_b_vis;
	bool							_b_vis_repul;
	bool							_b_vis_flock;
	bool							_b_vis_steer;

// BIRTH
	REAL							_birth_rate_ui;							//ui
	REAL							_birth_nb_ui;							//ui
	REAL							_birth_origin_ui[3];					//ui
	REAL							_birth_origin_size_ui[4];				//ui
	REAL							_birth_origin_size[3];
	c_poid::ID_GENERATOR			_s_id_generator_ui;
//DEATH
	REAL							_death_rate_ui;							//ui
	REAL							_death_nb_ui;							//ui
	REAL							_death_target_dist;						//ui

	bool							_b_death_box_ui;						//ui
	c_box_3d						_death_box;

	bool							_b_death_by_choking_ui;					//ui
	INT32							_death_choking_nb_ui;					//ui
	bool							_b_death_when_isolated_ui;				//ui
	REAL							_isolation_delay_ui;					//ui

	std::deque<class c_poid_to_create>	_to_create;		//	used to store info to create boid, see create_at()
	std::set<c_boid*>					_to_kill;		//	used to store boid passed to kill_next_update()


	c_obj_array_server<c_boid>*		_poids;			//	data is kept here there, and accessed using different pointers or functions

	c_boid**						_hd_calc;		//	used in the central part of the update : simpler struct -> faster process
													//		when used we can change _living and _unused without troubles
													//todo follow the code path where is is used outside of the update loop
private:
	c_boid**						_hd_draw;		//	filled in the last phase to obtain a faster and parallelized draw 
	INT32							_nb_draw;

	st_box_living					_box_living;
	c_influence*					_target;	

public:
	BOID_CONT_PT					_living;		//	valid except when in central part of update() the active boid
	BOID_CONT_PT					_unused;		//	valid except when in central part of update() the pool for creation

	INT32							_nb_alive;		//todo refine

	std::atomic<INT32>				_contacts_index;
	c_poid_contact*					_contacts;
	INT32							_contacts_nb_allocated;
	INT32							_contacts_nb;

	INT32							_contact_do_lua_ref;
	bool							_b_contact_do_lua_ui;
	bool							_b_is_repulse_do_lua_ui;

	c_placer						_placer;

	bool							_b_target_ui;							//ui
	bool							_b_target;
	REAL							_center_ui[3];							//ui


	REAL							_visibility_dist;

	bool							_b_visibility_ui;						//ui
	bool							_b_visibility_with_box_ui;				//ui
	REAL							_visibility_angle_ui;					//ui
	REAL							_visibility_cos;
// REPULSE
	bool							_b_repulse_ui;							//ui
	REAL							_repulse_dist_self;
	REAL							_repulse_influence_ui;					//ui
	REAL							_repulse_dist_ui;						//ui

	REAL							_repulse_pow_ui;						//ui
	bool							_b_repulse_visibility_ui;				//ui
	REAL							_repulse_special_ui;					//ui
	REAL							_repulse_old_ui;						//ui
//	bool							_b_repulse_no_limit_ui;					//ui
	REAL							_repulse_force_separation_dist_squared;
	REAL							_repulse_force_separation_dist_ui;		//ui
	REAL							_repulse_force_separation_factor;
	REAL							_repulse_force_separation_factor_ui;	//ui

	bool							_b_repulse_field;
	bool							_b_repulse_field_max_ui;				//ui
	bool							_b_repulse_field_ui;					//ui
	c_def_node::FIELD_TYPE			_s_repulse_field_ui;					//ui
	INT32							_repulse_field_src_axe_ui;				//ui
	REAL							_repulse_field_min;
	REAL							_repulse_field_factor;
	REAL							_repulse_field_power_ui;				//ui
	REAL							_repulse_field_zero_ui;					//ui
	REAL							_repulse_field_one_ui;					//ui
	c_def_node*						_def_node_repulse;
	bool							_b_repulse_field_inverse_ui;			//ui

	INT32							_s_repulse_by_other_type_ui;			//ui
	REAL							_repulse_by_other_influence_ui;			//ui
	REAL							_repulse_by_other_distance_ui;			//ui
// FLOCK
	bool							_b_flock_ui;							//ui
	bool							_b_flock;
	REAL							_flock_influence_ui;					//ui
	REAL							_flock_dist_ui;							//ui
	REAL							_flock_dist;
	REAL							_flock_dist_interior_ui;				//ui
	bool							_b_flock_visibility_ui;					//ui
// STEER
	bool							_b_steer_ui;							//ui
	bool							_b_steer;
	REAL							_steer_influence_ui;					//ui
	REAL							_steer_dist_ui;							//ui
	REAL							_steer_dist;
	bool							_b_steer_visibility_ui;					//ui

	REAL							_speed_noise_factor;
	REAL							_speed_noise_factor_ui;					//ui

	INT32							_id_generator;

	c_bdd_point*					_bdd_point_out;
	bool							_b_feed_bdd_point_ui;
	
	INT32							_i_u, _i_v, _i_axe;
	bool							_b_speed_noise;

	bool							_b_move_ui;
	INTERACTION_METHOD				_s_interaction_method;

	REAL							_field_influence;
	bool							_b_field_speed_ui;						//im
	bool							_b_field_predicted_position_ui;			//ui
	REAL							_field_influence_ui;					//ui
//	REAL							_field_danger_influence_ui;				//ui
	REAL							_field_threshold_ui;					//ui
	REAL*							_field_in;
	REAL*							_field_out;
	REAL*							_field_repulse_out;
	
	c_bdd_boid*						_bdd_boid;

	INT32							_s_check_duplicate_ui;
	bool							_b_verbose_multiple_kill_ui;

	static bool		cluster_less( c_boid CONST * CONST a, c_boid CONST * CONST b ) NOEXCEPT;
/*	struct cluster_less	{ 
	public: 
		bool CONST operator() ( c_boid CONST * CONST a, c_boid CONST * CONST b ) CONST NOEXCEPT	{	return a->less( b );	}
		//	bool CONST operator() ( INT32 CONST a, INT32 CONST b ) CONST	{	return a < b ;	}
	};	*/

//	sorting vector is the fastest for now
	std::vector< c_boid* >			_sorter;

private:
			void	build_array_sorted( REAL dt, bool b_parallel );
public:
	c_boids();
	~c_boids();

			bool	alloc( UINT32 nb );
			void	dealloc();
	FINLINE	INT32	get_nb_allocated() CONST	{ return _nb_allocated; }

			void	restart();
			void	update( REAL CONST dt, bool CONST b_need_contact );

private:
	FINLINE	INT32	make_cluster( REAL CONST pos );

	template< INT32 DIM >
		FINLINE	bool	compute_dist_squared_and_vector_boxed_if_inferior(	REAL CONST * CONST RESTRICT pos_i, REAL CONST * CONST RESTRICT pos_j,
																			REAL CONST * CONST RESTRICT box_i, REAL CONST * CONST RESTRICT box_j,
																			REAL* CONST RESTRICT vec, REAL& dist_squared	) CONST NOEXCEPT ;
	template< INT32 DIM >
		FINLINE REAL	get_cos( REAL CONST * CONST a, REAL CONST a_norm_over_one, REAL CONST * CONST b ) CONST;

	template< INT32 DIM, INTERACTION_METHOD METHOD, bool B_SELF, INT32 S_LOCK >	FINLINE void	update_interaction_one(		c_boid* CONST RESTRICT bi,	c_boid* CONST RESTRICT bj ) NOEXCEPT;

	template< INT32 DIM, INTERACTION_METHOD METHOD, INT32 S_LOCK >						void	update_interaction_self() NOEXCEPT ;
	template< INT32 DIM, INTERACTION_METHOD METHOD >									void	update_interaction_self() NOEXCEPT ;
//	template< INT32 DIM >																void	update_interaction_self() NOEXCEPT ;
																						void	update_interaction_self() NOEXCEPT ;

	template< INT32 DIM, INTERACTION_METHOD METHOD, INT32 S_LOCK >						void	update_interaction_other(	c_boids* CONST targ ) NOEXCEPT ;
	template< INT32 DIM, INTERACTION_METHOD METHOD >									void	update_interaction_other(	c_boids* CONST targ ) NOEXCEPT ;
//	template< INT32 DIM >																void	update_interaction_other(	c_boids* CONST targ ) NOEXCEPT ;
																						void	update_interaction_other(	c_boids* CONST targ ) NOEXCEPT ;

	FINLINE																				void	update_interaction_helper() NOEXCEPT;
																						void	update_interaction() NOEXCEPT;
	
																						void	update_movement( INT32 nb ) NOEXCEPT;


	FINLINE	void	kill_one_low(			c_boid* CONST b );
			void	kill_one_and_remove(	c_boid* CONST b );
			void	kill_all();
			void	kill_next_update(		c_boid* CONST b );	//todo refine needed for kill done thru lua

			bool	kill_by_id( INT32 id );	//this can happen during interaction so..

			void	clean_dup( CHAR CONST * CONST str );

	FINLINE	c_boid* create_one();

	FINLINE	bool	is_speed_noise()			CONST		{	return	_b_speed_noise;						}
	FINLINE	REAL	get_speed_noise_factor()	CONST		{	return	_speed_noise_factor;				}

	FINLINE	INT32	get_boid_to_draw( c_boid**& hd_boid )	{	hd_boid = _hd_draw;		return _nb_draw;	}

//todo use hd_calc or hd_draw
			c_boid*	get_boid_data_direct(	INT32 CONST index ) CONST;
	FINLINE	c_boid*	get_boid_from_id(		INT32 CONST id )	CONST	{	return	get_boid_data_direct(id-1);			}	//todo this is weird and sign of a hack
	FINLINE	c_boid*	get_boid_from_index(	INT32 CONST index ) CONST
	{	//todo optimize and protect from picking a wrong one
		if( 0<=index && index<_nb_alive )
		{
			//danger
#if 0
			BOID_CONT_PT::iterator	it = _living.begin();
			advance( it, index );
			return *it;
#else
//quick hack for celestins
//todo both aspect have to be kept 1/check 2/speed of access of previous code is unaceptable because n*n
			if( _hd_calc[0] )
				return _hd_calc[index];
#endif
		}
		return nullptr;
	}

	FINLINE	INT32	get_id()	CONST						{	return _id_ui; }
			void	create_at( REAL CONST * CONST pos, REAL CONST * CONST speed = nullptr );

};

