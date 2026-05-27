
//#include <iostream>
//#include <stdexcept>
//#include <cmath>
//#include <ctime>


#include "infrastructure/obj/obj.h"
#include "err.h"


typedef DOUBLE TIME_STAMP ; // in seconds

static const TIME_STAMP time_undefined = -1.0 ;


class c_filter_low_pass : public c_obj
{
	DOUBLE	_raw;
	DOUBLE	_alpha;
	DOUBLE	_last;
	bool	_b_init = false;

	void set_alpha( DOUBLE alpha )
	{
		if( alpha<=0.0 || alpha>1.0 ) 
		{
			ERR_PRINT_STRING( "%s() alpha (%d) should be in (0.0., 1.0]", __FUNCTION__, alpha );
			if( alpha <= 0. )
			{
				alpha = .0001;
				ERR_PRINT_STRING( "alpha forced to .0001" );
			}
			else
			{
				alpha = 1.;
				ERR_PRINT_STRING( "alpha forced to 1." );
			}
		}
		else
			_alpha = alpha ;
	}

public:
	c_filter_low_pass( DOUBLE alpha, DOUBLE value_init=0.0 )
	{
		_raw = _last = value_init;
		set_alpha( alpha );
	}

	DOUBLE filter( DOUBLE value )
	{
		DOUBLE result;
		if( _b_init )
			result = _alpha*value + (1.0-_alpha)*_last ;
		else
		{
			result = value;
			_b_init = true;
		}
		_raw = value;
		_last = result;
		return result;
	}


	DOUBLE filter_with_alpha( DOUBLE value, DOUBLE alpha )
	{
		set_alpha(alpha);
		return filter(value);
	}


	bool	is_value_raw_last(void)		{	return _b_init;	}
	DOUBLE	get_value_raw_last(void)	{	return _raw;	}
};

// -----------------------------------------------------------------

class c_filter_one_euro : public c_obj
{
	DOUBLE				_freq		;
	DOUBLE				_cutoff_min	;
	DOUBLE				_beta		;
	DOUBLE				_dcutoff	;
	c_filter_low_pass*	_x			;
	c_filter_low_pass*	_dx			;
	TIME_STAMP			_time_last	;

	DOUBLE alpha( DOUBLE cutoff )
	{
		DOUBLE te = 1.0 / _freq ;
		DOUBLE tau = 1.0 / (2*M_PI*cutoff) ;
		return 1.0 / (1.0 + tau/te) ;
	}

	void set_frequency( DOUBLE f )
	{
		if( f<=0 )
		{
			ERR_PRINT_STRING( "%s() freq (%d) should be > 0.0", __FUNCTION__, f );
			f = 90.; 
		}
		_freq = f;
	}

	void set_cutoff_min( DOUBLE mc )
	{
		if( mc<=0 )
		{
			ERR_PRINT_STRING( "%s() min cutoff (%d) should be > 0.0", __FUNCTION__, mc );
			mc = 1.; 
		}
		_cutoff_min = mc ;
	}

	void set_beta( DOUBLE b )
	{
		_beta = b ;
	}

	void set_cutoff_derivate( DOUBLE dc )
	{
		if( dc<=0 )
		{
			ERR_PRINT_STRING( "%s() derivative cutoff (%d) should be > 0.0", __FUNCTION__, dc );
			dc = 1.; 
		}
		_dcutoff = dc ;
	}

public:

	c_filter_one_euro( DOUBLE freq, DOUBLE cutoff_min=1.0, DOUBLE beta=0.0, DOUBLE cutoff_derivative=1.0 )
	{
		set_frequency(freq);
		set_cutoff_min(cutoff_min);
		set_beta(beta);
		set_cutoff_derivate(cutoff_derivative) ;
		_x = new c_filter_low_pass( alpha(cutoff_min) );
		_dx = new c_filter_low_pass( alpha(cutoff_derivative) );
		_time_last = time_undefined;
	}

	DOUBLE filter( DOUBLE value, TIME_STAMP timestamp=time_undefined )
	{
		// update the sampling frequency based on timestamps
		if( _time_last!=time_undefined && timestamp!=time_undefined )
			_freq = 1.0 / (timestamp-_time_last) ;
		_time_last = timestamp ;
		// estimate the current variation per second 
		DOUBLE dvalue = _x->is_value_raw_last() ? (value - _x->get_value_raw_last())*_freq : 0.0 ; // FIXME: 0.0 or value?
		DOUBLE edvalue = _dx->filter_with_alpha( dvalue, alpha(_dcutoff) ) ;
		// use it to update the cutoff frequency
		DOUBLE cutoff = _cutoff_min + _beta*fabs(edvalue) ;
		// filter the given value
		return _x->filter_with_alpha( value, alpha(cutoff) ) ;
	}

	~c_filter_one_euro( void )
	{
		FREE_AND_NULL( _x );
		FREE_AND_NULL( _dx );
	}

};

/*
int
main(int argc, char **argv) {
  randSeed() ;

  double duration = 10.0 ; // seconds

  double frequency = 120 ; // Hz
  double mincutoff = 1.0 ; // FIXME
  double beta = 1.0 ;      // FIXME
  double dcutoff = 1.0 ;   // this one should be ok

  std::cout << "#SRC OneEuroFilter.cc" << std::endl
		<< "#CFG {'beta': " << beta << ", 'freq': " << frequency << ", 'dcutoff': " << dcutoff << ", 'mincutoff': " << mincutoff << "}" << std::endl
		<< "#LOG timestamp, signal, noisy, filtered" << std::endl ;

  OneEuroFilter f(frequency, mincutoff, beta, dcutoff) ;
  for (TimeStamp timestamp=0.0; timestamp<duration; timestamp+=1.0/frequency) {
	double signal = sin(timestamp) ;
	double noisy = signal + (unifRand()-0.5)/5.0 ;
	double filtered = f.filter(noisy, timestamp) ;
	std::cout << timestamp << ", "
		  << signal << ", "
		  << noisy << ", "
		  << filtered
		  << std::endl ;
  }
  
  return 0 ;
}
*/
