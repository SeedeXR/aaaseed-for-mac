
#ifdef AAA_PARTICLE_H
#error "PARTICLE_H included more than once."
#endif
#define AAA_PARTICLE_H 1


#ifndef AAA_POID_H
#	include "boids/poid.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif

class c_net_buf_obj;

class	c_particle final : public c_poid
{
	REAL	_dt;
	REAL	_birth;
	REAL	_death;
	REAL	_life_over_one;

	REAL	_color_map_v;
	FP32	_color[4];

public:
	//	FINLINE	void		set_birth( REAL birth )					{	_birth = birth;	}
	FINLINE	REAL			get_birth()						CONST	{	return _birth;	}
	//	FINLINE	void		set_death( REAL death )					{	_death = death;	}
	FINLINE	REAL			get_death()						CONST	{	return _death;	}
	FINLINE	void			set_dt( REAL dt )						{	_dt = dt;		}
	FINLINE	REAL			get_dt()						CONST	{	return _dt;		}
	
	FINLINE	void			set_life( REAL birth, REAL life )
	{
		_birth = birth;
		_death = _birth + life;
		_life_over_one = OVER_ONE_AS_REAL(life);	// just to control the fade out
	}
	FINLINE	REAL			get_age_rel_at( REAL time )		CONST
	{
		REAL	tmp = ( time - _birth ) * _life_over_one;
		//todoqq	find a better solution
		CLAMP_REF_01( tmp );
		return tmp;
	}

	FINLINE	REAL			get_color_map_v()				CONST	{	return _color_map_v;	}
	FINLINE	FP32*		get_color_pt()							{	return _color;			}
	FINLINE	void			set_color_map_v( REAL v )				{	_color_map_v = v;		}
	FINLINE	void			set_color_alpha( FP32 a )			{	_color[3] = a;			}

			UINT8*			write_to_mem( UINT8* pt )				CONST;
			void			write_to_mem( c_net_buf_obj* net_buf )	CONST;
			CONST UINT8*	read_from_mem( UINT8 CONST * pt );
			CONST UINT8*	read_from_mem( UINT8 CONST * pt, REAL t );
};
