#include "power_spectrum.h"
#ifndef AAA_WRAP_FFTW3_H
#	include "lib_wrappers/wrap_fftw3.h"
#endif

#if	AAA_USE_FFT_DENPO()

static	unsigned int	ShuffleIndex( UINT32 i, INT32 WordLength )
// Function     : Finds the shuffle index of array elements. The array length
//                must be a power of two; The power is stored in "WordLength".
//   Return value : With "i" the source array index, "ShuffleIndex"
//                  returns the destination index for shuffling.
//   Comment      : -
{
	unsigned int	NewIndex = 0;
	for( UINT8 BitNr = 0; BitNr < WordLength; ++BitNr )
	{
		NewIndex = NewIndex << 1;
		if ( ( i&1 ) != 0 )
		{
			NewIndex = NewIndex + 1;
		}
		i = i >> 1;
	}
	return NewIndex;
}

void	c_snd_input::alloc_fft_denpo()
{
//	UINT32	i; 
	UINT32	dst;
	UINT32	tmp;
	//	initialize the fft index shuffling precompute array
	for( UINT32 i = 0; i < SOUND_SAMPLE_PER_CAPTURE; ++i )
	{
		//pu16_fft_shuffle[u32_counter] = UINT16( shuffle( u32_counter,SOUND_CAPTURE_SIZE_IN_BIT ) );
		sound_buffer[i] = 0; // the sound
		reorder[i] = i;
	}
	//	Shuffle all elements
	for( UINT32 i = 0; i < SOUND_SAMPLE_PER_CAPTURE ; ++i )
	{                              //	Find index to exchange elements
		dst = ShuffleIndex( i, SOUND_CAPTURE_SIZE_IN_BIT );
		if ( dst > i )
		{	//	Exchange elements
			tmp = reorder[i];
			reorder[i] = reorder[dst]; 
			reorder[dst] = tmp;
		}
	}
}

#endif

c_spectrum::c_spectrum( INT32 sample_per_capture )
	:_b_calibrate		( false )
	,_power_spectrum	( nullptr )
	,_min				( nullptr )
	,_max				( nullptr )
	,_factor			( nullptr )
#if	AAA_LIB_USE_FFTW3()
	,_plan				( nullptr )
	,_window_value		( nullptr )
	,_in_fftw			( nullptr )
	,_out_fftw			( nullptr )
#elif AAA_USE_FFT_DENPO()
	,reorder			( nullptr )
	,pc_fft				( nullptr )
#endif
{
	alloc( sample_per_capture );
}

namespace {
	CONSTEXPR	REAL CALIBRATE_INIT_MIN = 0.;
	CONSTEXPR	REAL CALIBRATE_INIT_MAX = 0.;
	CONSTEXPR	REAL CALIBRATE_INIT_FACTOR = 0.;
}

bool	c_spectrum::alloc( INT32 CONST sample_per_capture )
{
	dealloc();
	_sample_nb = sample_per_capture;
	_spectrum_nb = sample_per_capture / 2 + 1;

	_power_spectrum	= ALLOC_ARRAY_AND_INIT<REAL>( _spectrum_nb, 0 );
	_min			= ALLOC_ARRAY_AND_INIT<REAL>( _spectrum_nb, CALIBRATE_INIT_MIN );
	_max			= ALLOC_ARRAY_AND_INIT<REAL>( _spectrum_nb, CALIBRATE_INIT_MAX );
	_factor			= ALLOC_ARRAY_AND_INIT<REAL>( _spectrum_nb, CALIBRATE_INIT_FACTOR );
#if	AAA_LIB_USE_FFTW3()
	//	we should implement first Nuttall window, continuous first derivative
	_window_value	= ALLOC_ARRAY_AND_INIT<DOUBLE>( _sample_nb, 0 );
	_in_fftw		= ALLOC_ARRAY_AND_INIT<DOUBLE>( _sample_nb, 0 );
	_out_fftw		= ALLOC_ARRAY_AND_INIT<DOUBLE>( _sample_nb, 0 );
	_plan = dll_fftw3.fftw_plan_r2r_1d( _sample_nb / 2, _in_fftw, _out_fftw, FFTW_R2HC, FFTW_PATIENT );
	return true;	//todo add check
#elif AAA_USE_FFT_DENPO()
	reorder = new UINT32[sample_nb];
	pc_fft = C_COMPLEX[sample_nb];
	return true;	//todo add check
#else
	return false;
#endif
}

void	c_spectrum::dealloc()
{
	_sample_nb = 0;
	_spectrum_nb = 0;

	SAFE_DELETE_ARRAY( _power_spectrum );
	SAFE_DELETE_ARRAY( _min );
	SAFE_DELETE_ARRAY( _max );
	SAFE_DELETE_ARRAY( _factor );
#if	AAA_LIB_USE_FFTW3()
	if( _plan )
	{
		dll_fftw3.fftw_destroy_plan( _plan );
		_plan = nullptr;
	}
	SAFE_DELETE_ARRAY( _window_value );
	SAFE_DELETE_ARRAY( _in_fftw );
	SAFE_DELETE_ARRAY( _out_fftw );
#elif AAA_USE_FFT_DENPO()
	SAFE_DELETE_ARRAY( reorder_ );
	SAFE_DELETE_ARRAY( pc_fft_ );
#endif
}

c_spectrum::~c_spectrum()
{
	dealloc();
}

FINLINE	void	c_spectrum::calibrate_check_index( INT32 index, REAL in )
{
	bool b_do_factor = false;
	if( _calibrate_auto_step != 0 )
	{
		//		REAL	tmp = ( max[index] - min[index] ) * calibrate_auto_step;
		//todo use double everywhere here
		REAL	tmp = REAL( (_max[index]-_min[index] ) * _calibrate_auto_step * .5 );
		_max[index] -= tmp;
		_min[index] += tmp;
		b_do_factor = true;
	}
	if( _max[index] < in )
	{
		_max[index] = in;
		b_do_factor = true;
	}
	if( _min[index] > in )
	{
		_min[index] = in;
		b_do_factor = true;
	}
	if( b_do_factor )
	{
		_factor[index] = OVER_ONE_AS_REAL( _max[index] - _min[index] );
	}
}

FINLINE DOUBLE compute_magnitude( bool b_log, DOUBLE a, DOUBLE b )
{
	DOUBLE val = sqrt( a*a + b*b );
	//	convert magnitude to a log scale (dB)
	if( b_log )
		val = MAX0( 20 * log10( val ) );
	return val;
}
FINLINE DOUBLE compute_magnitude( bool b_log, DOUBLE a )
{
	DOUBLE val = ABS( a );
	//	convert magnitude to a log scale (dB)
	if( b_log )
		val = MAX0( 20 * log10( val ) );
	return val;
}

#if	AAA_LIB_USE_FFTW3()
void	c_spectrum::fttw( bool b_raw, bool b_log )
{
	DOUBLE		tmp; 
	DOUBLE		tmp2; 
	REAL*		pmin;
	REAL*		pfac;
	REAL*		ps;
	INT32		nb;

	//fill the img part with 0
	dll_fftw3.fftw_execute( _plan );

	ps = _power_spectrum;
	pmin = _min;
	pfac = _factor;

	tmp = _out_fftw[0];	// DC component
	tmp = compute_magnitude( b_log, tmp );
	if( !b_raw )
	{
		if( _b_calibrate )
			calibrate_check_index( 0, REAL(tmp) );
		tmp = ( tmp - *pmin++ ) * *pfac++;
	}
	*ps++ = REAL(tmp);

	nb = _sample_nb / 2;	//hack
#if 1
	DOUBLE*	src = _out_fftw;
	for( INT32 k = 1; k < ( nb + 1 ) / 2; ++k )
	{
		tmp  = *++src;
		tmp2 = *++src;
		tmp = compute_magnitude( b_log, tmp, tmp2 );
		if( !b_raw )
		{
			if( _b_calibrate )
				calibrate_check_index( k, REAL(tmp) );
			tmp = ( tmp - *pmin++ ) * *pfac++;
		}
		*ps++ = REAL(tmp);
	}
#else	// previous version 2024 Nov
	for( INT32 k = 1; k < ( nb + 1 ) / 2; ++k )  // ( k < N/2 rounded up ) */
	{
		tmp = _out_fftw[k];
		tmp2 = _out_fftw[nb - k];
		tmp = compute_magnitude( b_log, tmp, tmp2 );
		if( !b_raw )
		{
			if( _b_calibrate )
				calibrate_check_index( k, REAL(tmp) );
			tmp = ( tmp - *pmin++ ) * *pfac++;
		}
		*ps++ = REAL(tmp);
	}
#endif

	if( nb % 2 == 0 )	// N is even
	{
		tmp = _out_fftw[nb];
		tmp = compute_magnitude( b_log, tmp );	// Nyquist freq
		if( !b_raw )
		{
			if( _b_calibrate )
				calibrate_check_index( nb / 2, REAL(tmp) );
			tmp = ( tmp - *pmin++ ) * *pfac++;
		}
		*ps++ = REAL(tmp);
	}
}
#endif

void	c_spectrum::calibrate_set( bool b_in )
{
	if( _b_calibrate != b_in )
	{
		if( b_in )
		{
			REAL* p_min	= _min	  - 1;
			REAL* p_max	= _max	  - 1;
			REAL* p_fac	= _factor - 1;
			for( INT32 i = _sample_nb / 2 + 1; i > 0; --i )
			{
				*++p_min = CALIBRATE_INIT_MIN;
				*++p_max = CALIBRATE_INIT_MAX;
				*++p_fac = CALIBRATE_INIT_FACTOR;
			}
		}
		_b_calibrate = b_in;
		SWITCH_PRINT_STATE( "FFT Calibration", _b_calibrate );
	}
}

void	c_spectrum::calibrate_generate_factor( C_PCHAR_C filename )
{
	INT32 err_nb = 0;

	REAL* p_min	= _min	  - 1;
	REAL* p_max	= _max	  - 1;
	REAL* p_fac	= _factor - 1;
	for( INT32 i = _sample_nb / 2; i >= 0; --i )
	{
		REAL v = *++p_max - *++p_min;
		if( v != 0. )
			*++p_fac = REAL( 1. / v );
		else
		{
			++err_nb;
			*++p_fac = 0.;
		}
	}
	if( err_nb )
	{
//		sprintf( err_str, "%d  data invalid in %s.", err_nb, filename );
		ERR_PRINT_STRING( "%s() Calibration %d  data invalid in %s.", __FUNCTION__, err_nb, filename );
	}
}
