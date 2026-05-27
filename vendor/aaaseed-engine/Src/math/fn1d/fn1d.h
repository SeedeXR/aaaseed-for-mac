
#ifdef AAA_FN1D_H
#error "FN1D_H included more than once."
#endif
#define AAA_FN1D_H 1


#ifndef AAA_AAA_DEF_H
#	include "aaa_def.h"
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif



//////
////	FNX
//
class	c_fn1d : public c_obj
{
protected:
			bool	_b_to_update;
public:
	c_fn1d()			{ _b_to_update = true; }
	virtual	~c_fn1d()	{};
	virtual	void	compute();
			void	update();
			bool	is_to_update() CONST { return _b_to_update; }

	virtual	REAL	get( REAL f_in ) ;
			REAL	get_from_2_cont( REAL a, REAL b, REAL fb );
			REAL	get_from_2( REAL a, REAL b, REAL fb );
};

//////
////	FNX PERIODIC
//
class	c_fn1d_periodic : public c_fn1d
{
protected:
	REAL	_frequency;
	REAL	_min;
	REAL	_max;
	INT32	_visualize_step;
public:
	c_fn1d_periodic()
	{
		_visualize_step = 1024;
		_min = _max = _frequency = 0.;
	}
	virtual		~c_fn1d_periodic() {};

		void	set_min_max( REAL min_in, REAL max_in );
		REAL	get_min()	{ return _min; }
		REAL	get_max()	{ return _max; }
		void	set_center_ratio( REAL center, REAL ratio )
		{
			ratio = ( ratio - REAL(1.) ) / ( ratio + REAL(1.) );
			set_min_max( REAL( center * ( 1. - ratio ) ), REAL( center * ( 1. + ratio ) ) );
		}
		REAL	get_center()		{ return ( _max + _min ) / REAL(2.); }
		REAL	get_deviation()		{ return ( _max - _min ) / REAL(2.); }

		void	set_frequency( REAL frequency_in = 1.0 )
		{
			if ( frequency_in != _frequency )
			{
				_frequency = frequency_in;
				_b_to_update = true;
			}
		}
		void	set_period( REAL period )	{ set_frequency( REAL(1.) / period ); }
		REAL	get_frequency()				{ return _frequency; }

		void	visualize_in_rect(REAL x, REAL y, REAL w, REAL h );
		void	visualize();

};

//////
////	FNX PERIODIC DONE WITH AN ARRAY OF SAMPLES 
//
class	c_fn1d_periodic_table : public c_fn1d_periodic
{
protected:
	REAL	*_samples;
	REAL	_f_sample_nb;
	INT32	_sample_nb;
public:
	c_fn1d_periodic_table()
	{
		_samples = nullptr;
		_sample_nb  = 0;
		_f_sample_nb  = 0;
	}
	virtual ~c_fn1d_periodic_table();

		void	samples_alloc();
		void	set_sample_nb( INT32 nb_in )
		{
			if( _sample_nb != nb_in )
			{
				_sample_nb = nb_in;
				_f_sample_nb = REAL( nb_in );
				_visualize_step = nb_in;
				_b_to_update = true;
			}
		}
		INT32	get_sample_nb()	CONST	{ return _sample_nb; }	
	
		REAL	get( REAL f_in);
		void	find_min_max();
		void	normalize();
};

//////
////	FN 1D PERIODIC DONE WITH AN ARRAY OF SAMPLES
//			AND USING A RANDOM GENERATOR
class	c_fn1d_periodic_table_rand : public c_fn1d_periodic_table
{
protected:
	UINT32	_seed;
	REAL	_gain;
	REAL	_bias;
public:
	c_fn1d_periodic_table_rand()
	{
		_seed = 0;
		_gain = _bias = 0.;
		set_gain();
		set_bias();
		set_seed();
	}
	virtual	~c_fn1d_periodic_table_rand() {};

		void	set_seed( UINT32 seed_in = 8 )
		{
			if ( seed_in != _seed )
			{
				_seed = seed_in;
				_b_to_update = true;
			}
		}
		void	set_gain( REAL gain_in = .5 )
		{
			if ( gain_in != _gain )
			{
				_gain = gain_in;
				_b_to_update = true;
			}
		}
		void	set_bias( REAL bias_in = .5 )
		{
			if ( bias_in != _bias )
			{
				_bias = bias_in;
				_b_to_update = true;
			}
		}
};

