
#ifdef AAA_MECA_H
#error "MECA_H included more than once."
#endif
#define AAA_MECA_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif

//WORLD
class c_meca_world : public c_obj
{
	friend	class	c_bdd_boid;	//hack
	friend	class	c_boids;	//hack
	friend	class	c_boid;		//hack
private:
	REAL	_speed_min_squared;
	REAL	_speed_max_squared;
	REAL	_accel_max_squared;
	REAL	_viscosity_factor;

	REAL	_time_to_stop;

	REAL	_speed_min_ui;
	REAL	_speed_max_ui;
	REAL	_accel_max_ui;
	bool	_b_accel_max_target_box_ui;
	bool	_b_accel_max_interaction_ui;
	REAL	_viscosity_ui;
	REAL	_mass_over_one;
	REAL	_mass_ui;	//perhaps should be on each poid

	REAL	_vertical_ratio_limit;

	bool	_b_speed;
	bool	_b_offset;

	REAL	_accel[3];
	REAL	_speed[3];
	REAL	_offset_ui[3];


public:
	FINLINE	REAL	get_speed_min()					CONST	{	return _speed_min_ui;			}
	FINLINE	REAL	get_speed_max()					CONST	{	return _speed_max_ui;			}
	FINLINE	REAL	get_accel_max()					CONST	{	return _accel_max_ui;			}
												
	FINLINE	REAL	get_speed_min_squared()			CONST	{	return _speed_min_squared;		}
	FINLINE	REAL	get_speed_max_squared()			CONST	{	return _speed_max_squared;		}
	FINLINE	REAL	get_accel_max_squared()			CONST	{	return _accel_max_squared;		}
											
	FINLINE	REAL	get_viscosity()					CONST	{	return _viscosity_ui;			}
	FINLINE	REAL	get_viscosity_factor()			CONST	{	return _viscosity_factor;		}
														
	FINLINE	REAL	get_time_to_stop()				CONST	{	return _time_to_stop;			}
	FINLINE	REAL	get_mass()						CONST	{	return _mass_ui;				}
	FINLINE	REAL	get_mass_over_one()				CONST	{	return _mass_over_one;			}
											
	FINLINE	REAL	get_vertical_ratio_limit()		CONST	{	return _vertical_ratio_limit;	}
	FINLINE	REAL*	get_vertical_ratio_limit_pt()			{	return &_vertical_ratio_limit;	}
											
	FINLINE	REAL*	get_accel()								{	return _accel;					}
	FINLINE	REAL*	get_speed()								{	return _speed;					}
	FINLINE	REAL*	get_offset()							{	return _offset_ui;				}
														
	FINLINE	bool	is_speed()						CONST	{	return _b_speed;				}
	FINLINE	bool	is_offset()						CONST	{	return _b_offset;				}

			void	update();

	c_meca_world();
};

class c_def_node;

//OBJ
class	c_meca_obj
{
//			REAL	mass;	//	just a reminder
//	FINLINE	REAL	get_mass()			{	return mass; }
private:
	c_meca_world*	_world;

	UINT32			_id;

	REAL			_pos[3];
	REAL			_pos_futur[3];
	REAL			_pos_to_draw[3];		//	needed because of deformer

	REAL			_speed[3];

	REAL			_pos_last[3];
	REAL			_pos_to_draw_last[3];	//	needed because of deformer
	REAL			_speed_last[3];
	REAL			_speed_squared;

	// speed acummulate the effects of accel from frame to frame and don't take in account speed offset like the one from boids
	//	and we need it when dealing with curvature. this is why we have these fns
	REAL			_speed_last_used[3];
	REAL			_speed_last_used_norm;

	void					init();
public:
	c_meca_obj( c_meca_world* world_in = nullptr );
	FINLINE	void			set_world( c_meca_world* world_in )			{	_world = world_in;								}

	FINLINE	REAL			get_speed_min()					CONST		{	return _world->get_speed_min();					}
	FINLINE	REAL			get_speed_max()					CONST		{	return _world->get_speed_max();					}
	FINLINE	REAL			get_accel_max()					CONST		{	return _world->get_accel_max();					}

//	FINLINE	REAL			get_accel_factor()							{	return _world->get_accel_max();	}

	FINLINE	REAL			get_speed_min_squared()			CONST		{	return _world->get_speed_min_squared();			}
	FINLINE	REAL			get_speed_max_squared()			CONST		{	return _world->get_speed_max_squared();			}
	FINLINE	REAL			get_accel_max_squared()			CONST		{	return _world->get_accel_max_squared();			}

	FINLINE	REAL			get_time_to_stop()				CONST		{	return _world->get_time_to_stop();				}
	FINLINE	REAL			get_vertical_ratio_limit()		CONST		{	return _world->get_vertical_ratio_limit();		}
	FINLINE	REAL*			get_vertical_ratio_limit_pt()	CONST		{	return _world->get_vertical_ratio_limit_pt();	}

	FINLINE	CONST	REAL*	RESTRICT	get_pos()				CONST	{	return _pos;									}
	FINLINE			REAL*	RESTRICT	get_pos()						{	return _pos;									}
	FINLINE	CONST	REAL*	RESTRICT	get_pos_futur()			CONST	{	return _pos_futur;								}
	FINLINE	CONST	REAL*	RESTRICT	get_pos_to_draw()		CONST	{	return _pos_to_draw;							}
	FINLINE			REAL*	RESTRICT	get_pos_to_draw()				{	return _pos_to_draw;							}

	FINLINE	CONST	REAL*	RESTRICT	get_speed()				CONST	{	return _speed;									}
	FINLINE			REAL*	RESTRICT	get_speed()						{	return _speed;									}

	FINLINE	CONST	REAL*	RESTRICT	get_pos_last()			CONST	{	return _pos_last;								}
	FINLINE	CONST	REAL*	RESTRICT	get_pos_to_draw_last()	CONST	{	return _pos_to_draw_last;						}
	FINLINE			REAL*	RESTRICT	get_pos_to_draw_last()			{	return _pos_to_draw_last;						}
	FINLINE	CONST	REAL*	RESTRICT	get_speed_last()		CONST	{	return _speed_last;								}
	FINLINE	void			cpy_to_last()								{
																			cpy_v3( _pos_last,			_pos );
																			cpy_v3( _pos_to_draw_last,	_pos_to_draw );
																			cpy_v3( _speed_last,		_speed );
																		}

	FINLINE	REAL* CONST 	get_speed_last_used_pt()					{	return _speed_last_used;						}
	FINLINE	REAL			get_speed_last_used_norm()		CONST		{	return _speed_last_used_norm;					}
	FINLINE	void			set_speed_last_used_norm( REAL CONST n )	{	_speed_last_used_norm = n;						}


	FINLINE	REAL			get_speed_squared()				CONST		{	return _speed_squared;							}
	FINLINE	void			set_speed_squared( REAL s2 )				{	_speed_squared = s2;							}

	FINLINE	c_meca_world*	get_world()						CONST		{	return _world;									}

	FINLINE	void			set_pos_to_draw( REAL CONST * CONST src )	{	cpy_v3( _pos_to_draw, src );					}
	FINLINE	void			cpy_pos_to_pos_to_draw()					{	set_pos_to_draw( _pos );						}
	FINLINE	void			cpy_pos_to_draw( REAL* dst )				{	cpy_v3( dst, _pos_to_draw );					}

	FINLINE	void			set_id( UINT32 CONST id )					{	_id = id;										}
	FINLINE	UINT32			get_id()						CONST		{	return _id;										}
	//hack FOR CSI Then SAMSUNG
	FINLINE	UINT32			get_id_local_only()				CONST		{	return _id & 0xffffff;	} 

public:
	//hack	fast and dirty need to be redone / cleaned
	static	void	do_deform(	REAL* pos_tmp_buf,	INT32 CONST nb, c_meca_obj * CONST * CONST hd_beg,	c_def_node* CONST def, REAL CONST * CONST scale,	bool CONST b_parallel );
	static	void	get_pos(	REAL* dst,			INT32       nb, c_meca_obj * CONST *       hd,		bool CONST b_futur,									bool CONST b_parallel );

	//	u, v, w are centered in the box here
	template< INT32 DIM >
	FINLINE	void place_at_uvw( REAL CONST * CONST origin_pos, REAL CONST * CONST origin_size, REAL CONST * CONST vec, INT32 CONST i_a=0, INT32 CONST i_b=0 )
	{
		cpy_v3( _pos, origin_pos );
		if( origin_size )
		{
			switch( DIM )
			{
			case 1:
				_pos[i_a] += vec[0] * origin_size[i_a];
				break;
			case 2:
				_pos[i_a] += vec[0] * origin_size[i_a];
				_pos[i_b] += vec[1] * origin_size[i_b];
				break;
			case 3:
				_pos[0] += vec[0] * origin_size[0];
				_pos[1] += vec[1] * origin_size[1];
				_pos[2] += vec[2] * origin_size[2];
				break;
			}
		}
		cpy_v3( _pos_last,		_pos );
		cpy_v3( _pos_to_draw,	_pos );

		clear_v3( _speed );
	}
};


