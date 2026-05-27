
#ifdef AAA_BOID_H
#error "BOID_H included more than once."
#endif
#define AAA_BOID_H 1


#ifndef	AAA_BOID_BASE_H
#	include "boids/boid_base.h"
#endif
#ifndef AAA_RAND_H
#	include "math/rand.h"
#endif
#ifndef AAA_BOIDS_H
#	include "boids.h"
#endif
#ifndef AAA_V_H
#	include "math/v.h"
#endif
#if BOID_USE_LOCK()
#	ifndef AAA_AAA_MUTEX_H
#		include "aaa/aaa_mutex.h"
#	endif
#endif

class c_net_buf_obj;

class	c_boid final: public c_poid
{
	friend class c_bdd_boid;
	friend class c_boids;
protected:
	static c_rand_lin		rnd;

	c_boids*				_boids;
	INT32					_ga;
	INT32					_gb;
	REAL					_dist_repulse;
//	REAL					_dist_repulse_squared;

	REAL					_box_size[3];
	REAL					_box_size_half[3];

#if BOID_USE_LOCK()
	mutable aaa::c_spinlock	_lock;

	INT32					_seen_count;
	INT32					_repulse_count;
	INT32					_flock_count;
	INT32					_steer_count;
#else
	std::atomic<INT32>		_seen_count;
	std::atomic<INT32>		_repulse_count;
	std::atomic<INT32>		_flock_count;
	std::atomic<INT32>		_steer_count;
#endif	//BOID_USE_LOCK()

	REAL					_isolation_time;

	REAL					_repulse[3];
	REAL					_flock_bary[3];
	REAL					_steer[3];

public:
/*
	FINLINE	bool	less( c_boid CONST * CONST b ) CONST NOEXCEPT
					{
						INT32 CONST	delta = _ga - b->_ga;
						return delta != 0 ? (delta < 0) : (_gb < b->_gb) ;
					}
*/

	c_boid( c_boids* boids_in = nullptr );
	~c_boid();

#if BOID_USE_LOCK()
	FINLINE void		lock()		{	_lock.lock();		}
	FINLINE void		unlock()	{	_lock.unlock();		}
#endif

	//hack we force definition because of the use of mutex or atomic
	// probably wrong when it is called
	c_boid( c_boid CONST & boid ) : c_poid( boid )
	{
		_boids			= boid._boids;
		_ga				= boid._ga;
		_gb				= boid._gb;
			
		cpy_v3( _box_size,		boid._box_size		);
		cpy_v3( _box_size_half,	boid._box_size_half );
			
#if BOID_USE_LOCK()
		_seen_count		= boid._seen_count;
		_repulse_count	= boid._repulse_count;
		_flock_count	= boid._flock_count;
		_steer_count	= boid._steer_count;
#else
		_seen_count		= boid._seen_count.load();
		_repulse_count	= boid._repulse_count.load();
		_flock_count	= boid._flock_count.load();
		_steer_count	= boid._steer_count.load();
#endif
		_isolation_time = boid._isolation_time;
			
		cpy_v3( _repulse,		boid._repulse		);
		cpy_v3( _flock_bary,	boid._flock_bary	);
		cpy_v3( _steer,			boid._steer			);
	}
private:
	FINLINE	void	compute_pos_futur( REAL CONST dt ) NOEXCEPT
					{
						if( _boids->is_speed() )
						{
							REAL s[3];
							add_v3( s, get_speed(), _boids->get_speed() );
							add_scale_v3( (REAL*)get_pos_futur(), get_pos(),	s,				dt	);
						}
						else
						{
							add_scale_v3( (REAL*)get_pos_futur(), get_pos(),	get_speed(),	dt	);
						}
						//	add_scale_v3( get_pos_futur(), pos, speed, OVER_ONE( get_speed_max()) );
						//	add_scale_v3( get_pos_futur(), pos, speed, get_time_to_stop());
					}
public:
							FINLINE	void	do_living_box() NOEXCEPT;
									void	update_movement(	c_influence CONST * CONST target, c_boids::st_box_living CONST * CONST box ) NOEXCEPT;
							FINLINE void	update_movement(	INT32 CONST i ) NOEXCEPT;
private:
	template < INT32 INDEX >FINLINE	void	do_living_box_one() NOEXCEPT;

	FINLINE	void	add_accel_box_repulse_one(	REAL& accel,		REAL CONST min,		REAL CONST max,		REAL CONST pos_futur	) CONST NOEXCEPT;
	FINLINE	void	add_accel_box_repulse_xyz(	REAL* accel,		REAL CONST* min,	REAL CONST* max,	REAL CONST* pos_futur	) CONST NOEXCEPT;

//	FINLINE	bool	do_box_wrap(		INT32 CONST ind,	REAL CONST min,		REAL CONST max ) NOEXCEPT;
										
//							FINLINE	void	do_box_bounce(			INT32 CONST ind,	REAL CONST min,		REAL CONST max ) NOEXCEPT;
//							FINLINE void	do_box_bounce()					NOEXCEPT;
//	template < INT32 DIM >	FINLINE	void	do_box_bounce_side_wrap_axe()	NOEXCEPT;//
//	template < INT32 DIM >	FINLINE	void	do_box_wrap()					NOEXCEPT;
//	template < INT32 DIM >	FINLINE	void	do_box_wrap_random()			NOEXCEPT;
//	template < INT32 DIM >	FINLINE	void	do_box_die()					NOEXCEPT;


public:		
	FINLINE	void	set_boids( c_boids* CONST boids_in ) NOEXCEPT	{ _boids = boids_in; set_world((c_meca_world*) boids_in); }

	//FINLINE	REAL CONST*	get_repulse()	CONST	{ return _repulse; }
	//FINLINE	REAL CONST*	get_flocking()	CONST	{ return _flock_bary; }

	CONSTEXPR void	clear_interaction_minimum() NOEXCEPT
					{
						_repulse_count = 0;
						_flock_count = 0;
						_steer_count = 0;
						_seen_count = 0;
					}
	CONSTEXPR void	clear_interaction() NOEXCEPT
					{
						clear_interaction_minimum();
						clear_v3( _repulse );
						clear_v3( _flock_bary );
						clear_v3( _steer );
					}

	FINLINE	void	inc_repulse()	NOEXCEPT	{	++_repulse_count;	}
	FINLINE	void	inc_steer()		NOEXCEPT	{	++_steer_count;		}
	FINLINE	void	inc_flock()		NOEXCEPT	{	++_flock_count;		}
	FINLINE	void	inc_seen()		NOEXCEPT	{	++_seen_count;		}

	FINLINE	void	add_repulse(	REAL CONST * CONST repulse )	NOEXCEPT	{	inc_repulse();	add_v3( _repulse, repulse );	}
	FINLINE	void	sub_repulse(	REAL CONST * CONST repulse )	NOEXCEPT	{	inc_repulse();	sub_v3( _repulse, repulse );	}

	FINLINE	void	add_steer(		REAL CONST * CONST target )		NOEXCEPT	{	inc_steer();	add_v3( _steer, target );		}

	FINLINE	void	add_flock(		REAL CONST * CONST target )		NOEXCEPT	{	inc_flock();	add_v3( _flock_bary, target );	}

	CONSTEXPR REAL CONST *	get_box_size() CONST NOEXCEPT
			{
				return _box_size;
			}
	FINLINE void	set_box_size( REAL CONST * CONST in ) NOEXCEPT
			{
				cpy_v3( _box_size, in );
				scale_v3( _box_size_half, _box_size, FP32(.5) );
			}

			void	write_to_mem( c_net_buf_obj* net_buf );
	UINT8 CONST *	read_from_mem( UINT8 CONST * pt );
	static	CONSTEXPR INT32	get_mem_net_size() NOEXCEPT	{	return 28;	}	//todo 28 ?
};



//FINLINE	void	c_boid::do_box_bounce( INT32 CONST ind, REAL CONST min, REAL CONST max ) NOEXCEPT
//{
//	REAL tmp = get_pos()[ind] - min;
//	if( tmp < 0 )
//	{
//		get_pos()[ind] = min-tmp;
//		get_speed()[ind] = -get_speed()[ind];
//	}
//	else
//	{
//		tmp = get_pos()[ind] - max;
//		if( tmp > 0 )
//		{
//			get_pos()[ind] = max-tmp;
//			get_speed()[ind] = -get_speed()[ind];
//		}
//	}
//}

//FINLINE void	c_boid::do_box_bounce() NOEXCEPT
//{
//	//todo merge with particle box
////todo sphere part
////	if( _boids->_b_living_box_sphere_ui )
////	{
//		//todo optimized done quickly
//		//	and refined
//		//tmp = norm_squared_v3r( get_pos() );
//		//if( tmp > _boids->_living_box_radius_squared )
////		{
//			//todo implement it
//			//	tmp = SQRT(tmp);
//			//	REAL	norm_over_one = OVER_ONE(tmp);	//we keep for normalization
//
//			//	tmp = compute_repulse( tmp-_boids->_living_box_radius, _boids->_living_box_border_size );
//			//	add_scale_v3( accel, get_pos_futur(), -tmp*box_inf*norm_over_one );
////		}
////	}
////	else
//	//todo optimize using dimension
//	auto& box = _boids->_box_living;
//	REAL*	min = box.min;
//	REAL*	max = box.max;
//	if( _boids->_b_visibility_with_box_ui )
//	{
//		do_box_bounce( 0, min[0] + box.size_half[0], max[0] - box.size_half[0] );
//		do_box_bounce( 1, min[1] + box.size_half[1], max[1] - box.size_half[1] );
//		do_box_bounce( 2, min[2] + box.size_half[2], max[2] - box.size_half[2] );
//	}
//	else
//	{
//		do_box_bounce( 0, min[0], max[0] );
//		do_box_bounce( 1, min[1], max[1] );
//		do_box_bounce( 2, min[2], max[2] );
//	}
//}

//FINLINE	void	c_boid::do_box_wrap_x() NOEXCEPT
//{
//	auto& box = _boids->_box_living;
//	REAL f = get_pos()[0];
//	if( f < box.min[0] )
//		get_pos_to_draw()[0] = get_pos()[0] = f + box.size_half[0] * 2.;
//	else if( box.max[0] < f )
//		get_pos_to_draw()[0] = get_pos()[0] = f - box.size_half[0] * 2.;
//}
//FINLINE	void	c_boid::do_box_wrap_y() NOEXCEPT
//{
//	auto& box = _boids->_box_living;
//	REAL f = get_pos()[1];
//	if( f < box.min[1] )
//		get_pos_to_draw()[1] = get_pos()[1] = f + box.size_half[1] * 2.;
//	else if( box.max[0] < f )
//		get_pos_to_draw()[1] = get_pos()[1] = f - box.size_half[1] * 2.;
//}
//FINLINE	void	c_boid::do_box_wrap_z() NOEXCEPT
//{
//	auto& box = _boids->_box_living;
//	REAL f = get_pos()[2];
//	if( f < box.min[2] )
//		get_pos_to_draw()[2] = get_pos()[2] = f + box.size_half[2] * 2.;
//	else if( box.max[0] < f )
//		get_pos_to_draw()[2] = get_pos()[2] = f - box.size_half[2] * 2.;
//}

//FINLINE	bool	c_boid::do_box_wrap( INT32 CONST ind, REAL CONST min, REAL CONST max ) NOEXCEPT
//{
//	REAL* pos = get_pos();
//	REAL tmp = min - pos[ind];
//	if( 0 < tmp )
//	{
//		//avoid drawing of lines from one side to the other		
//		//hack pos_to_draw is updated in bdd_draw when drawn only
//		get_pos_to_draw()[ind] = pos[ind] = max - tmp;
//		//tmp = max-min;
//		//get_pos_to_draw_last()[ind] += max - min;
//		clear_draw();
//		return true;
//	}
//	else
//	{
//		tmp = pos[ind] - max;
//		if( 0 < tmp )
//		{
//			//hack pos_to_draw is updated in bdd_draw when drawn only
//			get_pos_to_draw()[ind] = pos[ind] = min + tmp;
//			//get_pos_to_draw_last()[ind] -= max - min;
//			clear_draw();
//			return true;
//		}
//	}
//	return false;
//}

//template < INT32 DIM >
//FINLINE	void	c_boid::do_box_wrap() NOEXCEPT
//{
//	//todo merge with particle box
////todo sphere part
////	if( _boids->_b_living_box_sphere_ui )
////	{
////	}
////	else
//	{
//		//todo optimize using dimension
//		REAL*	min = _boids->_box_living.min;
//		REAL*	max = _boids->_box_living.max;
//		switch( DIM )
//		{
//		case 1:		{
//						INT32 iu = _boids->_i_axe;
//						do_box_wrap( iu, min[iu], max[iu] );
//					}
//					break;
//		case 2:		{
//						INT32 iu = _boids->_i_u;
//						INT32 iv = _boids->_i_v;
//						do_box_wrap( iu, min[iu], max[iu] );
//						do_box_wrap( iv, min[iv], max[iv] );
//					}
//					break;
//		default:
//		case 3:		do_box_wrap( 0, min[0], max[0] );
//					do_box_wrap( 1, min[1], max[1] );
//					do_box_wrap( 2, min[2], max[2] );
//					break;
//		}
//	}
//}

//todo done only for z axe here
//template < INT32 DIM >
//FINLINE	void	c_boid::do_box_wrap_random() NOEXCEPT
//{
//	//todo merge with particle box
////todo sphere part
////	if( _boids->_b_living_box_sphere_ui )
////	{
////	}
////	else
//	{
//		//todo optimize using dimension
//		REAL*	min = _boids->_box_living.min;
//		REAL*	max = _boids->_box_living.max;
//		switch( DIM )
//		{
//		case 1:		{
//						INT32 iu = _boids->_i_axe;
//						do_box_wrap( iu, min[iu], max[iu] );
//					}
//					break;
//		case 2:		{
//						INT32 iu = _boids->_i_u;
//						INT32 iv = _boids->_i_v;
//						if( do_box_wrap( iu, min[iu], max[iu] ) )
//						{
//							get_pos_to_draw()[iv] = get_pos()[iv] = min[iv] + (max[iv] - min[iv]) * rnd.get_ufloat();
//						}
//						if( do_box_wrap( iv, min[iv], max[iv] ) )
//						{
//							get_pos_to_draw()[iu] = get_pos()[iu] = min[iu] + (max[iu] - min[iu]) * rnd.get_ufloat();
//						}
//					}
//					break;
//		default:
//		case 3:		if( do_box_wrap( 0, min[0], max[0] ) )
//					{
//						get_pos_to_draw()[1] = get_pos()[1] = min[1] + (max[1] - min[1]) * rnd.get_ufloat();
//						get_pos_to_draw()[2] = get_pos()[2] = min[2] + (max[2] - min[2]) * rnd.get_ufloat();
//					}
//					if( do_box_wrap( 1, min[1], max[1] ) )
//					{
//						get_pos_to_draw()[0] = get_pos()[0] = min[0] + (max[0] - min[0]) * rnd.get_ufloat();
//						get_pos_to_draw()[2] = get_pos()[2] = min[2] + (max[2] - min[2]) * rnd.get_ufloat();
//					}
//					if( do_box_wrap( 2, min[2], max[2] ) )
//					{
//						get_pos_to_draw()[0] = get_pos()[0] = min[0] + (max[0] - min[0]) * rnd.get_ufloat();
//						get_pos_to_draw()[1] = get_pos()[1] = min[1] + (max[1] - min[1]) * rnd.get_ufloat();
//					}
//					break;
//		}	
//	}
//}


//template < INT32 DIM >
//FINLINE	void	c_boid::do_box_bounce_side_wrap_axe() NOEXCEPT
//{
//	//todo merge with particle box
////todo sphere part
////	if( _boids->_b_living_box_sphere_ui )
////	{
////	}
////	else
//	{
//		//todo optimize using dimension
//		REAL*	min = _boids->_box_living.min;
//		REAL*	max = _boids->_box_living.max;
//		INT32 i;
//		switch( DIM )
//		{
//		case 1:		i = _boids->_i_u;
//					do_box_wrap( i, min[i], max[i] );
//					break;
//		case 2:		i = _boids->_i_u;
//					if( _boids->_b_visibility_with_box_ui )		do_box_bounce( i, min[i] + _box_size_half[i], max[i] - _box_size_half[i] );
//					else										do_box_bounce( i, min[i], max[i] );
//					i = _boids->_i_v;
//					do_box_wrap( i, min[i], max[i] );
//					break;
//		default: 
//		case 3:		i = _boids->_i_u;
//					if( _boids->_b_visibility_with_box_ui )		do_box_bounce( i, min[i] + _box_size_half[i], max[i] - _box_size_half[i] );
//					else										do_box_bounce( i, min[i], max[i] );
//					i = _boids->_i_v;
//					if( _boids->_b_visibility_with_box_ui )		do_box_bounce( i, min[i] + _box_size_half[i], max[i] - _box_size_half[i] );
//					else										do_box_bounce( i, min[i], max[i] );
//					i = _boids->_i_axe;
//					do_box_wrap( i, min[i], max[i] );
//					break;
//		}
//	}
//}

//template < INT32 DIM >
//FINLINE void	c_boid::do_box_die() NOEXCEPT
//{
//	//todo merge with particle box
////todo sphere part
////	if( _boids->_b_living_box_sphere_ui )
////	{
////	}
////	else
//	{
//		//todo optimize using dimension
//		REAL*	min = _boids->_box_living.min;
//		REAL*	max = _boids->_box_living.max;
//
//		switch( _boids->_s_dim_ui )
//		{
//		case 1:
//			{
//				INT32 i = _boids->_i_axe;
//				if(		get_pos()[i] < min[i] || max[i] < get_pos()[i]		)
//				{
//					this->mark_to_kill();
//					//_boids->kill_one_and_remove( this );
//				}
//			}
//			break;
//		case 2:
//			{
//				INT32 iu = _boids->_i_u;
//				INT32 iv = _boids->_i_v;
//				if(		get_pos()[iu] < min[iu] || max[iu] < get_pos()[iu]
//					||	get_pos()[iv] < min[iv] || max[iv] < get_pos()[iv]
//					)
//				{
//					this->mark_to_kill();
//					//_boids->kill_one_and_remove( this );
//				}
//			}
//			break;
//		case 3:
//			if(		get_pos()[0] < min[0] || max[0] < get_pos()[0]
//				||	get_pos()[1] < min[1] || max[1] < get_pos()[1]
//				||	get_pos()[2] < min[2] || max[2] < get_pos()[2]
//				)
//			{
//				this->mark_to_kill();
//				//_boids->kill_one_and_remove( this );
//			}
//			break;
//		}
//	}
//}

//maa this don't work
#if 0
static REAL CONST REPULSE_MAX = 10000.;	//todo	make it bettee, but for now we avoid infinite acelleration
static FINLINE	REAL compute_repulse( REAL val, REAL CONST size )
{
	val = size - val; 
	if( val >= 0. )
		val = REPULSE_MAX;
	else
	{
		val = size * OVER_ONE(val);	// this is d/x
		if( val > REPULSE_MAX )
			val = REPULSE_MAX;
	}
	//	this is a repulsion in (d^2/x^2-1)*d which scale correctly with d (distance of influence)
	//	but we divide by x = norm(vec) because we will multiply vec
	//	(d^2/x^2-1)*d/x = (d/x)^3 - d/x
	//	we should aim at ((d/x)^n-1)*d/x

	return val*val*val - val;
}
#else
namespace
{
	CONSTEXPR	DOUBLE compute_box_accel( REAL CONST val, c_boids::st_box_living CONST * CONST box )
	{
		DOUBLE val_out = val * box->border_size_over_one * 4.;
		return val_out * val_out;
	}
}
#endif


FINLINE	void c_boid::add_accel_box_repulse_one( REAL& accel, REAL CONST min, REAL CONST max, REAL CONST pos_futur ) CONST NOEXCEPT
{
	REAL tmp = min - pos_futur;
	if( tmp > 0 )
	{
		c_boids::st_box_living CONST * CONST box = &_boids->_box_living;
		accel += REAL(box->influence_repulse * compute_box_accel( tmp, box ));
	}
	else
	{
		tmp = pos_futur - max;
		if( tmp > 0 )
		{
			c_boids::st_box_living CONST * CONST box = &_boids->_box_living;
			accel -= REAL(box->influence_repulse * compute_box_accel( tmp, box ));
		}
	}
}

FINLINE	void c_boid::add_accel_box_repulse_xyz( REAL* accel, REAL CONST * min, REAL CONST * max, REAL CONST * pos_futur ) CONST NOEXCEPT
{
	auto CONST& box = _boids->_box_living;
	if( box.s_type[0] == c_poid::LIVING_BOX_REPULSE )
		add_accel_box_repulse_one( accel[0], min[0], max[0], pos_futur[0] );
	if( box.s_type[1] == c_poid::LIVING_BOX_REPULSE )
		add_accel_box_repulse_one( accel[1], min[1], max[1], pos_futur[1] );
	if( box.s_type[2] == c_poid::LIVING_BOX_REPULSE )
		add_accel_box_repulse_one( accel[2], min[2], max[2], pos_futur[2] );
}

template < INT32 INDEX >
FINLINE	void	c_boid::do_living_box_one() NOEXCEPT
{
	auto CONST& box = _boids->_box_living;
	if( INSIDE_MIN_MAX( box.s_type[INDEX], LIVING_BOX_DIE, LIVING_BOX_BOUNCE ) )
	{
		REAL	min = box.min[INDEX];
		REAL	max = box.max[INDEX];
		REAL	pos = get_pos()[INDEX];
		if(	pos < min || max < pos )
		{
			switch( box.s_type[INDEX] )
			{
			case LIVING_BOX_DIE:
				this->mark_to_kill();
				break;
			case LIVING_BOX_WRAP_RANDOM:
				switch( INDEX )
				{
				case 0:		get_pos_to_draw()[1] = get_pos()[1] = box.min[1] + ( box.max[1] - box.min[1] ) * rnd.get_fp32_01();
							get_pos_to_draw()[2] = get_pos()[2] = box.min[2] + ( box.max[2] - box.min[2] ) * rnd.get_fp32_01();
							break;
				case 1:		get_pos_to_draw()[2] = get_pos()[2] = box.min[2] + ( box.max[2] - box.min[2] ) * rnd.get_fp32_01();
							get_pos_to_draw()[0] = get_pos()[0] = box.min[0] + ( box.max[0] - box.min[0] ) * rnd.get_fp32_01();
							break;
				case 2:		get_pos_to_draw()[0] = get_pos()[0] = box.min[0] + ( box.max[0] - box.min[0] ) * rnd.get_fp32_01();
							get_pos_to_draw()[1] = get_pos()[1] = box.min[1] + ( box.max[1] - box.min[1] ) * rnd.get_fp32_01();
							break;
				}
				// yes no break here
			case LIVING_BOX_WRAP:
				get_pos_to_draw()[INDEX] = get_pos()[INDEX] = FWRAP( pos, min, max );
				clear_draw();
				break;
			case LIVING_BOX_BOUNCE:
				if( _boids->_b_visibility_with_box_ui )
				{
					min += box.size_half[INDEX];
					max -= box.size_half[INDEX];
				}
				{
					REAL tmp = pos - min;
					if( tmp < 0 )
					{
						get_pos()[INDEX] = min - tmp;
						get_speed()[INDEX] = -get_speed()[INDEX];
					}
					else
					{
						tmp = pos - max;
						if( tmp > 0 )
						{
							get_pos()[INDEX] = max - tmp;
							get_speed()[INDEX] = -get_speed()[INDEX];
						}
					}
				}
				break;
			//done in update_movement
			//case LIVING_BOX_BOUNCE:
			default:
				break;
			}
		}
	}
}

//todo add template < INT32 DIM > and use it
FINLINE	void	c_boid::do_living_box() NOEXCEPT
{
	do_living_box_one<0>();
	do_living_box_one<1>();
	do_living_box_one<2>();

//	switch( _boids->_s_dim_ui )
//	{
//	case 1:		do_living_box<1>();	break;
//	case 2:		do_living_box<2>();	break;
//	default:	do_living_box<3>();	break;
//	}

//	switch( _boids->_box_living.s_type[0] )
//	{
//	//case LIVING_BOX_NO :					break;
//	case LIVING_BOX_DIE :					do_box_die<DIM>();					break;
//	case LIVING_BOX_WRAP :					do_box_wrap<DIM>();					break;
//	case LIVING_BOX_WRAP_RANDOM :			do_box_wrap_random<DIM>();			break;
//	case LIVING_BOX_BOUNCE_SIDE_WRAP_AXE:	do_box_bounce_side_wrap_axe<DIM>();	break;
//	case LIVING_BOX_BOUNCE :				do_box_bounce();					break;
//	case LIVING_BOX_WRAP :					do_box_wrap();						break;
//	//case LIVING_BOX_REPULSE :				break;
//	}
}


FINLINE void	c_boid::update_movement( INT32 CONST i ) NOEXCEPT
{
	if( is_to_kill() )
	{
		//kill_one_and_remove( b );
		//if( _s_check_duplicate_ui > 1 )
		// 		clean_dup( "after kill_one in loop" );
		return;
	}

	if( is_want_to_die() )
	{
		REAL	dist2 = _boids->_death_target_dist;
		dist2 *= dist2;
		if( get_dist_squared_if_in_dist_squared_v3r( _boids->_inf_death._pos, get_pos(), dist2 ) < dist2 )
		{
			mark_to_kill();
			//kill_one_and_remove( b );
			return;
		}
	}

	if( !is_fix() )
	{
		set_draw();
		auto inf = _boids->_field_influence;
		if( inf != 0. )	//	submit to speed field
		//todo adapt to dimension
		{
			REAL*	p_field	= _boids->_field_out + i*3;
			if( is_not_null_v3( p_field ) )
				add_scale_v3( get_speed(), p_field, inf );
		}

		bool b_want_to_die = is_want_to_die();
		switch( _boids->_s_dim_ui )
		{
		case 1:
			{
				auto i_u = _boids->_i_u;
				auto i_v = _boids->_i_v;
				get_speed()[i_u] = get_speed()[i_v] = 0;
				auto const& box = _boids->_box_living;
				if( b_want_to_die )
					update_movement( &_boids->_inf_death, nullptr );	// no box constraint on dying boid
				else
					update_movement( _boids->_target, &box );
				//get_pos()[i_u] = _boids->_inf_target._pos[i_u];
				//get_pos()[i_v] = _boids->_inf_target._pos[i_v];
				get_pos()[i_u] = box.pos_ui[i_u];
				get_pos()[i_v] = box.pos_ui[i_v];
			}
			break;
		case 2:
			{
				auto i_axe = _boids->_i_axe;
				get_speed()[i_axe] = 0;
				auto const& box = _boids->_box_living;
				if( is_want_to_die() )
					update_movement( &_boids->_inf_death, nullptr );	// no box constraint on dying boid
				else
					update_movement( _boids->_target, &box );
				//get_pos()[i_axe] = _boids->_inf_target._pos[i_axe];
				get_pos()[i_axe] = box.pos_ui[i_axe];
			}
			break;
		default:
			if( is_want_to_die() )
				update_movement( &_boids->_inf_death, nullptr );	// no box constraint on dying boid
			else
				update_movement( _boids->_target, &_boids->_box_living );
			break;
		}
		if( !b_want_to_die )
		{
			do_living_box();
		}
	//hack	tried
	//	if( field_influence != 0. )	//	submit to speed field
	//	{
	//		add_scale_v3( b->get_pos(), &_field_out[i*3], field_influence );
	//	}
	}	// end is_fix()
}


