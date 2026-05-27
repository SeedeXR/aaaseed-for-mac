#include "boid.h"
#include "obj_ui/com/net_util_obj.h"
//unused for now (2025 November)
//#include "nanoflann.hpp"


c_rand_lin			c_boid::rnd;

c_boid::c_boid( c_boids* boids_in ) : c_poid( (c_meca_world*) boids_in )
{
	clear_v3( _box_size );
	clear_v3( _box_size_half );
	_boids = boids_in;
	if( boids_in )
		debug_break();
	//	boids->add_boid( this );
}

c_boid::~c_boid()
{
	//	boids->remove_boid( this );
}


namespace {
	c_rand_lin	noise_pos;
};

//todo add visibility (how the animal can see the others)
void c_boid::update_movement( c_influence CONST * CONST target, c_boids::st_box_living CONST * CONST box ) NOEXCEPT
{
	cpy_to_last();

	REAL* p_speed = get_speed();
#if	AAA_DEBUG()
	if( _isnan(p_speed[0]) )
		clear_v3(p_speed);
#endif

//
//	ACCEL
//
	{
		REAL	accel_force[3];
		REAL	accel_box_target[3];
		cpy_v3( accel_box_target, _boids->get_accel() );

	// We compute accel for target
		//todo we use influence twice
		if( target )
		{
			REAL inf = target->_inf;
			REAL vec[3];
			//	sub_v3( vec, target->target, get_pos_futur() );
			sub_v3( vec, target->_pos, get_pos() );

			switch( target->_s_dim )
			{
			case 1:	vec[target->_i_axe] = 0;							break;
			case 2:	vec[target->_i_u] = 0;		vec[target->_i_v] = 0;	break;
			}

			REAL factor = 0.;
			REAL n;
			if( inf > 0 )
			{
				REAL re = target->_radius_ext_ui;
				if( re > 0. )
				{
					n = norm_v3r(vec);
					if( n > re )
						factor = n - re ;
					else if( target->_radius_int_ui > n )
						factor = n - target->_radius_int_ui;
					else
					{
						factor = 0.;
						//factor = .0001 * ( n - target->_radius_center ) / target->_radius_size_half;
					}
				}
			}
			else
			{
				n = norm_v3r(vec);
				factor = 1.;
			}

			if( target->_normal_strenght != 0. )
			{
				REAL nor[3];
				switch( target->_i_axe )
				{
				case 0:	cross_x_v3r( nor, vec );	break;
				case 1:	cross_y_v3r( nor, vec );	break;
				case 2:	cross_z_v3r( nor, vec );	break;
				}
				add_scale_v3( accel_box_target, nor, target->_normal_strenght / n );
			}

			if( factor != 0. )
			{
#if 0
				//scale_v3( vec, get_speed_max());
				//todo this is bad

				//limit_v3r(	vec, target->_inf );
				scale_v3( vec, _boids->_over_dt * factor * inf / n);	//goto speed	
				sub_v3( vec, p_speed);
				scale_v3( vec, _boids->_over_dt );	//goto accel

				//		dist = norm_v3r( vec);
				//		if( dist > get_accel_max() )
				//			inf *= get_accel_max()/dist;

				add_scale_v3( accel_box_target, vec, inf );
#else
				add_scale_v3( accel_box_target, vec, factor * _boids->_over_dt * inf / n );
#endif
			}
		}	//if( target )

	// We compute accel for box	
		if( box )
		{
			if( box->influence_repulse!=0. )
			{
				//todo box need a center
				//todo merge with particle box
				if( box->b_sphere_ui )
				{
					//todo optimized done quickly
					//todo box at center should be used to opt
					//	and refined
					//	tmp = norm_squared_v3r( get_pos_futur() );
					REAL vec[3];
					sub_v3( vec, get_pos_futur(), box->pos_ui );
					REAL tmp = norm_squared_v3r(vec);
					if( tmp > box->radius_squared )
					{
						tmp = SQRT(tmp);
						tmp = REAL( -OVER_ONE_AS_REAL(tmp) * box->influence_repulse * compute_box_accel(tmp - box->radius, box) );
						add_scale_v3( accel_box_target, vec, tmp );
					}
				}
				else
				{
					//todoopt unroll to optimize 
					if( _boids->_b_visibility_with_box_ui )
					{
						REAL min[3];
						REAL max[3];
						add_v3( min, _boids->_box_living.min, _box_size_half );
						sub_v3( max, _boids->_box_living.max, _box_size_half );
						add_accel_box_repulse_xyz( accel_box_target, min, max, get_pos_futur() );
					}
					else
					{
						add_accel_box_repulse_xyz( accel_box_target, _boids->_box_living.min, _boids->_box_living.max, get_pos_futur() );
					}
				}
			}
		}

		//clear_v3( accel_force );
		if( _repulse_count )
		{
			//if( !_boids->_b_repulse_no_limit_ui )
			cpy_v3( accel_force, _repulse );
		}
		else
		{
			if( _boids->_b_death_when_isolated_ui )
			{
				_isolation_time = _isolation_time + _boids->_dt;
				if( _isolation_time >= _boids->_isolation_delay_ui )
				{
					this->mark_to_kill();
					//_boids->kill_one_and_remove( this );
					return;
				}
			}
			clear_v3( accel_force );
		}

		if( _flock_count )	//&& _boids->_b_flock )
		{
			REAL	vec[3];
			scale_v3( _flock_bary, OVER_ONE(_flock_count) );		//	we need to keep the bary vector to draw it
			//				sub_v3( vec, flock_bary, get_pos_futur() );
			sub_v3( vec, _flock_bary, get_pos() );
			REAL tmp = _boids->_flock_influence_ui;
			add_scale_v3( accel_force, vec, tmp * _boids->_over_dt_squared );
		}
		if( _steer_count )	//&& _boids->_b_steer )
		{
			//	we need to keep the steer vector to draw it
			//	over one
			scale_v3( _steer, OVER_ONE(_steer_count) * _boids->_steer_influence_ui );
			add_scale_v3( accel_force, _steer, _boids->_over_dt );
		}

		if( _boids->is_speed_noise() )
		{
			REAL	f = _boids->get_speed_noise_factor();
			switch( _boids->_s_dim_ui )
			{
			case 3:
				accel_force[0] += noise_pos.get_fp32() * f;
				accel_force[1] += noise_pos.get_fp32() * f;
				accel_force[2] += noise_pos.get_fp32() * f;
				break;
			case 2:
				accel_force[_boids->_i_u] += noise_pos.get_fp32() * f;
				accel_force[_boids->_i_v] += noise_pos.get_fp32() * f;
				break;
			case 1:
				accel_force[_boids->_i_axe] += noise_pos.get_fp32() * f;
				break;
			}
		}

		//	limit accel
		if( _boids->_b_accel_max_target_box_ui )
		{
			REAL n2 = norm_squared_v3r( accel_box_target );
			if( n2 > get_accel_max_squared() )
				scale_v3( accel_box_target, get_accel_max() / SQRT(n2) );
		}

		if( _boids->_b_accel_max_interaction_ui )
		{
			REAL n2 = norm_squared_v3r( accel_force );
			if( n2 > get_accel_max_squared() )
				scale_v3( accel_force, get_accel_max() / SQRT(n2) );
		}

		//	if( _repulse_count &&_boids->_b_repulse_no_limit_ui )
		//	if( _repulse_count )
		//	add_v3( accel, _repulse );
		add_v3( accel_force, accel_box_target );

			//limit curvature
			//{
			//	REAL speed_norm[3];
			//	REAL accel_norm[3];
			//	REAL snorm = norm_v3r( p_speed );
			//	REAL anorm = norm_v3r( accel_force );
			//	REAL c = dot_v3r( speed_norm, accel_norm ) / (snorm*anorm);
			//	if( c <= 0 )
			//	{
			//	}
			//}

//
//	SPEED
//
		//todo there is still work to do around mass
		//factor *= _boids->_dt * _boids->get_mass_over_one();
		add_scale_v3( p_speed, accel_force, _boids->_dt * _boids->get_mass_over_one() );
	}

#if	AAA_DEBUG()
	if( _isnan( p_speed[0] ) )
		clear_v3( p_speed );
#endif
	{
		//hack viscosity need to used dt at some point
		REAL factor = get_world()->get_viscosity_factor();
		//	clamp speed
		REAL d2 = norm_squared_v3r( p_speed );
		if( d2 <= get_speed_min_squared() )
		{
			//careful this have strange side effects in particular we need to add escape
			//	from a "one line problem"  no half turn on line
			//	in particular this cancel the box constraint for particle _living on axis
			if( d2 == 0. )
			{
				switch( _boids->_s_dim_ui )
				{
				case 3:
					p_speed[0]				= noise_pos.get_fp32();
					p_speed[1]				= noise_pos.get_fp32();
					p_speed[2]				= noise_pos.get_fp32();
					break;
				case 2:
					p_speed[_boids->_i_u]	= noise_pos.get_fp32();
					p_speed[_boids->_i_v]	= noise_pos.get_fp32();
					break;
				case 1:
					p_speed[_boids->_i_axe]	= noise_pos.get_fp32();
					break;
				}
#if	AAA_DEBUG()
				if( _isnan( p_speed[0] ) )
					clear_v3( p_speed );
#endif
				d2 = norm_squared_v3r( p_speed );
			}
			factor *= get_speed_min() / SQRT(d2);
		}
		else if( d2 > get_speed_max_squared() )
			factor *= get_speed_max() / SQRT(d2);
		//loc_speed -= viscosity * loc_speed * loc_speed;
		scale_v3( p_speed, factor );	// we need to keep before using dt
	}
#if	AAA_DEBUG()
	if( _isnan( p_speed[0] ) )
		clear_v3( p_speed );
#endif

	{
		REAL factor = _boids->get_vertical_ratio_limit();
		if( factor > 0. )
		{
			REAL d2_h = sum_squared_v2r( p_speed[0], p_speed[2] ) * factor * factor;
			REAL d2_v = p_speed[1] * p_speed[1];
			if( d2_v > d2_h )	//todo can preprocess this
			{
				if( d2_h == 0. )
				{
					p_speed[0] = p_speed[1] * factor;
				}
				else
				{	// done for fish in aquarium
					// it does strange things in the generic case
					//todo we should has a selector for bebavior like: None, fishvo, fish ....
					REAL ratio_v = sqrtf( d2_v / d2_h );
					p_speed[0] *= ratio_v;
					p_speed[2] *= ratio_v;
				}
#if	AAA_DEBUG()
				if( _isnan( p_speed[0] ) )
					clear_v3( p_speed );
#endif
	//was
	//			if( p_speed[1] < 0 )	p_speed[1] = -factor * sqrtf( len_h );
	//			else					p_speed[1] =  factor * sqrtf( len_h );
			}
		}
	}

	{
		REAL s[3];
		REAL* ps;

		//cpy_v3( speed_last, slu );
		//(
		//set_speed_last_used_norm(

		//	what come now don't accumulate in the poid because the poid speed is saved just before
		if( _boids->is_speed() )
		{
			add_v3( s, p_speed, _boids->get_speed() );
			ps = s;
		}
		else
			ps = p_speed;

		REAL n_speed = norm_v3r( ps );
		if( _boids->_b_curvature_constraint_ui )
		{			
			if( n_speed!=0. )
			{
				REAL n_speed_last = get_speed_last_used_norm();
				if( n_speed_last!=0. )
				{
					DOUBLE c = dot_v3r( ps, get_speed_last_used_pt() ) / (n_speed * n_speed_last);
					if( c < _boids->_curvature_cos_threshold )
					{	// angle is to big
						REAL vec[3];
						sub_v3( vec, ps, get_speed_last_used_pt() );
						REAL vec_norm = norm_v3r( vec );
						DOUBLE size = n_speed_last * _boids->_curvature_sin_threshold;
						interpolate_v3( ps, get_speed_last_used_pt(), ps, size/vec_norm );	
						n_speed = norm_v3r( ps );
					}
				}
			}
		}

		cpy_v3( get_speed_last_used_pt(), ps );
		set_speed_last_used_norm( n_speed );

//
//	POSITION
//
		add_scale_v3( get_pos(), ps, _boids->_dt );
		set_speed_squared( norm_squared_v3r(ps) );
	}

	if( _boids->is_offset() )
		add_v3( get_pos(), _boids->get_offset() );

	if( _boids->_b_death_box_ui && _boids->_death_box.is_inside( get_pos() ) )
	{
		this->mark_to_kill();
		//_boids->kill_one_and_remove( this );
	}
	//REAL	speed_tmp[3];
	//	cross_v3r( vec, speed, speed_tmp );
	//	if( norm_v3r( vec) < 0.1 )
	//		cpy_v3( speed_old, speed_tmp );
}

void c_boid::write_to_mem( c_net_buf_obj* CONST net_buf )
{
	net_buf->write_value(		get_id()	);
	net_buf->write_value_v3(	get_speed()	);
	net_buf->write_value_v3(	get_pos()	);
}

UINT8 CONST * c_boid::read_from_mem( UINT8 CONST * pt )
{
	UINT32 id;
	pt = c_net::read_value( pt, &id );
	set_id( id );
	pt = c_net::read_value_v3( pt, get_speed()	);
	pt = c_net::read_value_v3( pt, get_pos()	);
	return pt;
}
