#ifndef AAA_SND_INPUT_H
#	include "snd_input.h"
#endif
#ifndef AAA_SOUND_INPUT_H
#	include "sound_input.h"
#endif
#ifndef AAA_SOUND_NEW_H
#	include "sound_new.h"
#endif
#include "math/v.h"
#include "infrastructure/param/param_declare.h"
#include "snd_master.h"
#include <vector>
#include "file/file_csv.h"


//todo we need a version we specify the sample_nb in the src and the size of the destination buffer too
class c_buf_loop_real final : public c_obj
{
private:
	INT32				_nb_max;
	INT32				_nb;
	REAL*				_data;
//	mutable aaa::MUTEX	_lock;

public:
	c_buf_loop_real();
	~c_buf_loop_real();

			void	put(		INT16* src, INT32 stride, INT32 nb );
	FINLINE	INT32	get_nb()						{	return _nb;	}
			void	set_nb_max(	INT32 nb );
			REAL*	get_data(	INT32 nb );
};

c_buf_loop_real::c_buf_loop_real()
	:_nb_max	(0)
	,_nb		(0)
	,_data		(nullptr)
{
}

c_buf_loop_real::~c_buf_loop_real()
{
	FREE( _data );
}

void c_buf_loop_real::set_nb_max( INT32 nb )
{
	if( _nb_max < nb )
	{
		_data = (REAL*) REALLOC_SIGNATURE( _data, nb * sizeof(REAL), __FUNCTION__ );	//todo check allocation for message
		_nb_max = _data ? nb : 0;
	}
}

//todo deal with nb bigger than _nb_max
void c_buf_loop_real::put( INT16* src, INT32 stride, INT32 nb )
{
	INT32 offset = _nb + nb - _nb_max;
	REAL* pr;
	if( offset > 0 )
	{
		INT32 nb_valid = _nb - offset;
		if( nb_valid > 0 )
		{
			MEMMOVE( (void*)_data,  (void*)(_data + offset), nb_valid * sizeof(REAL) );
			pr = _data + _nb_max - nb;
			_nb = _nb_max;
		}
		else
		{
			pr = _data;
		}
	}
	else
	{
		pr = _data + _nb;
		_nb += nb;
	}
	DOUBLE f = 1. / REAL(  1 << ( 16 - 1 ) );
	for( ; nb>0; --nb )
	{
		*pr = REAL((*src) * f);
		++pr;
		src += stride;
	}
}

REAL* c_buf_loop_real::get_data( INT32 nb )
{
	return _data + _nb_max - nb;
}

FACTORY_CREATE_V1( c_snd_input, snd_input, sound Input, snd_input );

bool	gb_verbose_fft;
void	c_snd_input::set_fft_verbose( bool in )
{
	gb_verbose_fft = in;
	SWITCH_PRINT_STATE( "FFT Verbose", gb_verbose_fft );
}
void	c_snd_input::flip_fft_verbose()
{
	set_fft_verbose( !gb_verbose_fft );
}

void	c_snd_input::c_init()
{
#if	AAA_LIB_USE_FFTW3()
	if( dll_fftw3.init() )
	{
		//2025 Sep Maa don't know how to extract version from dll
		//GOOD_PRINT_STRING( fftw_version );
	}
#endif
}
void	c_snd_input::c_deinit()
{
#if	AAA_LIB_USE_FFTW3()
	dll_fftw3.deinit();
#endif
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void	c_snd_input::freeze()		{ _b_buffer_read = true;	}
void	c_snd_input::unfreeze()		{ _b_buffer_read = false;	}

/*
void	c_snd_input::reset_fft()
{
	_u8_fft_done = 0;
}
*/

namespace	n_snd_input
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 19 + c_snd_input::CALIBRATE_NB;
	CONSTEXPR INT32	GROUP_NB		= 0;

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON( Active )
		//hack
		PARAM_DEF_INT32_POS_ZERO(	device					)
		PARAM_DEF_STR_LOCKED(		name					)
		PARAM_DEF_STR_LOCKED(		type					)
		PARAM_DEF_INT32(			channel_nb,				1, 4,		1, 32					)
		PARAM_DEF_SYMBO_PSTR(		sample_rate,			0, 2,		snd::sample_rate_str	)
		PARAM_DEF_SYMBO_PSTR_ONE(	sample_bits,			snd::sample_bits_str				)
		PARAM_DEF_INT32(			capture_size_pow2,		1, 10,		1, PARAM_MAX_INT32		)
		PARAM_DEF_BOOL_ON(			enable					)
		PARAM_DEF_INT32_LOCKED(		sample_per_capture		)
		PARAM_DEF_INT32_LOCKED(		capture_index			)

		//hack
	//	PARAM_DEF_BOOL_OFF(			flip_stereo				)

		PARAM_DEF_BOOL_OFF(			amplitude_raw			)
		PARAM_DEF_BOOL_OFF(			amplitude_in_log		)

		PARAM_DEF_BOOL_ON(			calibration_auto		)
		PARAM_DEF_REAL_POS(			calibration_auto_delay, .001, 10.	)
		PARAM_DEF_BOOL_OFF(			calibration_01			)
		PARAM_DEF_BOOL_OFF(			calibration_02			)	
		PARAM_DEF_BOOL_OFF(			calibration_03			)
		PARAM_DEF_BOOL_OFF(			calibration_04			)
		PARAM_DEF_BOOL_OFF(			calibration_05			)
		PARAM_DEF_BOOL_OFF(			calibration_06			)
		PARAM_DEF_BOOL_OFF(			calibration_07			)
		PARAM_DEF_BOOL_OFF(			calibration_08			)


		//	PARAM_DEF_REAL_ZERO(	out_min					)
		//	PARAM_DEF_REAL_ONE(		out_max					)

		PARAM_DEF_REAL_ZERO(		value_inactive			)
		PARAM_DEF_BOOL_OFF(			test_do					)	
		PARAM_DEF_REAL_ONE(			test_amplitude			)
		PARAM_DEF_REAL(				test_freq,				440,20,	20,40000 )
	};
}

c_sound_input*		c_snd_input::get_sound_input_valid() //lv we protect from crash
{
	c_sound_input* si = _sound_input;
	if( si && !si->is_valid() )
		si = nullptr;
	return si;
}

void	c_snd_input::param_init_pt()
{
	INT32	h = 0;

	c_sound_input*	si = get_sound_input_valid();

	param_set_pt(			h, get_pt_active()			);
	param_set_pt(			h, _device_ui				);
	param_set_pt_even_null(	h, si ? si->get_device_name()	: nullptr );
	param_set_pt_even_null(	h, si ? si->get_type_name()		: nullptr );
	param_set_pt(			h, _channel_nb_ui			);
	param_set_pt(			h, _sample_rate_ui			);
	param_set_pt(			h, _sample_bits_ui			);
	param_set_pt(			h, _capture_size_pow2_ui	);
	param_set_pt(			h, _b_enabled_ui			);
	param_set_pt(			h, _sample_per_capture		);
	param_set_pt(			h, _capture_count			);

//	param_set_pt(			h, _b_stereo_flip_ui		);

	param_set_pt(			h, _b_raw_ui				);
	param_set_pt(			h, _b_log_ui				);

	param_set_pt(			h, _b_calibrate_auto_ui		);
	param_set_pt(			h, _calibrate_auto_delay_ui	);
	param_set_pt_n(			h, _b_calibrate_ui,			CALIBRATE_NB	);

	//	param_set_pt( h, out_min_ );
	//	param_set_pt( h, out_max_ );

	param_set_pt(			h, _value_inactive_ui	);
	param_set_pt(			h, _b_test_ui			);
	param_set_pt(			h, _test_amp_ui			);
	param_set_pt(			h, _test_freq_ui		);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_snd_input )
	,_b_buffer_read			(false)
	//,_u8_fft_done			(false)
	,_sound_input			(nullptr)	// low level sound_input
	,_capture_count			(0)
	,_fft_count				(0)
	//,_b_enabled			(true)
	,_b_enabled_ui			(true)
//	,_b_stereo				(true)
	//,_b_stereo_start_with	(true)
	//,_device				(-1)
	,_channel_first			(0)
	,_channel_nb			(2)
	,_channel_nb_ui			(0)
	,_sample_rate_ui		(0)
	,_sample_rate			(snd::SAMPLE_PER_SEC_DEF)
	,_sample_bits_ui		(0)
	,_sample_bits			(16)
	,_sample_per_capture	(0)
//	,_b_stereo_flip_ui		(true)
	,_value_inactive_ui		(0)
	,_b_calibrate_auto_ui	(true)
	,_calibrate_auto_delay_ui(0)
	,_test_freq_ui			(20)
	,_test_amp_ui			(0)
	,_sound_buffer			(nullptr)
	,_buffer_size			(0)
	,_b_enabled				(false)
	,_samples				(nullptr)
	,_count_new				(0)
	,_byte_per_sample		(0)
	,_lib_used				(nullptr)
{
#if	AAA_USE_FFT_DENPO()
	alloc_fft_denpo();
#endif
	//init();
	param_init_with( n_snd_input::param, n_snd_input::PARAM_NB_MAX );
}

//todosnd use this instead of constructor
//void	c_snd_input::init()
//{
//	_b_buffer_read = false;
//	_u8_fft_done = false;
//
//	_sound_input = nullptr;	// low level sound_input
//	_capture_count = 0;
//	_fft_count = 0;
//
//	_b_enabled = true;
//	_b_enabled_ui = true;
//
//	_b_stereo = true;
//	_b_stereo_start_with = true;
//
//	_device = 0;
//	_channel_first = 0;
//	_channel_nb = 2;
//	_channel_ui = 0;
//	_sample_rate_ui = 0;
//	_sample_rate = snd::SAMPLE_PER_SEC_DEF;
//	_sample_bits_ui = 0;
//	_sample_bits = 16;
//	_sample_per_capture = 0;
//
//	_b_stereo_flip = true;
//
//	_value_inactive = 0;
//
//	_b_calibrate_auto = true;
//	_calibrate_auto_delay = 0;
//
//	_freq_test = 0;
//
//	_buffer_size = 0;
//
//	_ps_l = nullptr;
//	_ps_r = nullptr;
//
//	_b_enabled = false;
//	_samples = nullptr;
//}

void	c_snd_input::sample_ui_convert()
{
// convert interface symbolic to usable values
/*
	switch( _channel_nb_ui )
	{
	case c_sound::CHANNEL_MONO :
	case c_sound::CHANNEL_STEREO :		_channel_nb = -_channel_nb_ui;
	default :							_channel_nb = _channel_nb_ui;		break;
	}
*/
	//todo deal with the max of the device
	_channel_nb = _channel_nb_ui;

	switch( _sample_rate_ui )
	{
	case snd::SAMPLE_RATE_11025 :	_sample_rate = 11025;			break;
	case snd::SAMPLE_RATE_22050 :	_sample_rate = 22050;			break;
	case snd::SAMPLE_RATE_48000 :	_sample_rate = 48000;			break;
	case snd::SAMPLE_RATE_96000 :	_sample_rate = 96000;			break;
	case snd::SAMPLE_RATE_192000 :	_sample_rate = 192000;			break;
	case snd::SAMPLE_RATE_44100 :
	default :						_sample_rate = 44100;			break;
	}

	switch( _sample_bits_ui )
	{
	case snd::SAMPLE_BITS_8 :		_sample_bits = 8;				break;
	case snd::SAMPLE_BITS_24 :		_sample_bits = 24;				break;
	case snd::SAMPLE_BITS_32 :		_sample_bits = 32;				break;
	case snd::SAMPLE_BITS_FP :		_sample_bits = 32;				break;
	case snd::SAMPLE_BITS_16 :
	default :						_sample_bits = 16;				break;
	}
}

c_snd_input::~c_snd_input()
{
	//	TerminateThread( h_notify_thread, 0 );
	disable();
	for( auto & p : _spectrums )
		SAFE_DELETE_ARRAY( p );
	_spectrums.clear();

	for( auto & p : _bufs_big )
		SAFE_DELETE( p );
	_bufs_big.clear();

	SAFE_DELETE_ARRAY( _sound_buffer	);
	SAFE_DELETE_ARRAY( _samples			);
}

namespace {
	CONST CHAR  data_ext[]  = "data_xx";
//	CONST CHAR  left_ext[]  = "data_left";
//	CONST CHAR  right_ext[]	= "data_right";
};

AAA_ERR	c_snd_input::save_do_after( o_str CONST & filename_in )
{
	AAA_ERR retcode = AAA_OK;

#if	AAA_LIB_USE_FFTW3() || AAA_USE_FFT_DENPO()
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( data_ext );

		INT32	nb_real = _sample_per_capture / 2 + 1;
		INT32 i = 0;
		for( auto & ps : _spectrums )
		{
			++i;
			filename.set_digits( -2, 2, i );

			INT32	nb_write = 0;
			FILE* file = file_csv::open( filename, true );		
				nb_write =  file_csv::write_real_grid(	file, ps->get_min(), 1, nb_real );
				nb_write += file_csv::write_real_grid(	file, ps->get_max(), 1, nb_real );
			file_csv::close( file );

			if( nb_write != (nb_real * 2) ) 
				retcode = ERR_ANY;
		}
	o_str::pop_name();
#endif
	return retcode;
}

AAA_ERR	c_snd_input::load_do_after( o_str CONST & filename_in )
{
	AAA_ERR retcode = AAA_OK;

#if	AAA_LIB_USE_FFTW3() || AAA_USE_FFT_DENPO()
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( data_ext );

		//TODO move to c_ps_stuff
		INT32	nb_real = _sample_per_capture / 2 + 1;
		INT32 i = 0;
		for( auto & ps : _spectrums )
		{
			if( i <= CALIBRATE_NB )
				ps->calibrate_set( _b_calibrate_ui[i] );

			++i;
			filename.set_digits( -2, 2, i );

			INT32	nb_read = 0;			
			FILE* file = file_csv::open( filename, false );
				nb_read =  file_csv::read_float_grid(	file, ps->get_min(), 1, nb_real );
				nb_read += file_csv::read_float_grid(	file, ps->get_max(), 1, nb_real );
			file_csv::close( file );

			if( nb_read == (nb_real * 2) ) 
				ps->calibrate_generate_factor( filename.get() );
			else
				retcode = ERR_ANY;
		}
	o_str::pop_name();
#endif
	return retcode;
}

void	c_snd_input::update()
{
	if( !is_active() )
		return;

	c_sound_input*	si = get_sound_input_valid();

	if( _b_enabled_ui && _device_ui != (si ? si->get_device() : -1 ) )
		disable();	//	stop it to start it again

	if( _b_enabled_ui != _b_enabled )
	{
		if( _b_enabled_ui )
			enable();
		else
			disable();
	}
	if( !_b_enabled )
		return;

	_count_new = 0;
	si = get_sound_input_valid();

	if( _lib_used && si )
	{
		si->update();
		//UINT8*	pt = si->get_sound_buffer();
		//if( pt )
		//	MEMCPY( _sound_buffer, pt, _buffer_size );
		c_buffer_stream& stream = si->get_capture_stream();
		//	right way to do but the rest don't deal with this
		//while( stream->get( _sound_buffer ) )

		INT32	nb_ch = get_channel_nb();
		while( true )
		{
			INT32 len = stream.get( _sound_buffer, _buffer_size );
			if( len > 0 )
			{
				INT32 nb_sample = (len>>1) / nb_ch;
				for( INT32 ch=0; ch<nb_ch; ++ch )
					_bufs_big[ch]->put( ((INT16*)_sound_buffer)+ch, nb_ch, nb_sample );
				++_count_new;
			}
			else
				break;
		}//	empty the buffer till we hold the last
		_capture_count = stream.get_count_out();
		if( _count_new )	//todo follow and check this
			do_fft();
	}

	INT32 i = 0;
	for( auto & ps : _spectrums )
	{
		ps->calibrate_set( _b_calibrate_ui[i] );
		if( _b_calibrate_ui[i] )
		{
			DOUBLE val;
			if( _b_calibrate_auto_ui && _calibrate_auto_delay_ui != 0. )
			{
				//				calibrate_auto_step = REAL( SOUND_SAMPLE_PER_CAPTURE )/( REAL( SOUND_SAMPLE_PER_SEC )*calibrate_auto_delay );
				//deal with time	
				_delta_t.update();
				val = _delta_t.get_dt() / _calibrate_auto_delay_ui;
			}
			else
				val = DOUBLE(0) ;
			ps->set_calibrate_auto_step( REAL(val) );
			++i;
		}
	}
}

void	c_snd_input::disable()
{
	if( _sound_input && _sound_input->is_valid() )
	{
		//_sound_input->disable();	//done when input delete below
//todo feb 2-18 Maa removed the delete
		if( _lib_used )
		{
			_lib_used->delete_input( _sound_input );
			_lib_used = nullptr;
		}
		_sound_input = nullptr;
	}
	_b_enabled = false;
}

bool	c_snd_input::enable()
{
	bool	retcode = false;
	if( !_sound_input )	
	{
		_sound_input = snd::g_master->get_device_input();
		if( !_sound_input )
			return	retcode;
	}
	if( !_sound_input->is_valid() )
		return	retcode;
	_lib_used = snd::g_master->get_lib();

	sample_ui_convert();

	if( !_sound_input->enable( 0, _device_ui, _channel_first, _channel_nb, _sample_rate, _sample_bits, snd::SAMPLE_PER_CAPTURE_DEF ) )	//hack was	1<<_capture_size_pow2_ui ) )
		return	retcode;
	//todo use channel_first


	//_device = _sound_input->get_device();
	_buffer_size = _sound_input->get_buffer_size();
	if( _buffer_size > 0 )
	{	//todo	we should alloc // dealloc
		_sound_buffer	=	ALLOC_ARRAY_AND_INIT<UINT8>	( _buffer_size, 0 );
		_samples		=	ALLOC_ARRAY_AND_INIT<DOUBLE>( _buffer_size, 0 );
	}
	_byte_per_sample	= ( _sample_bits >> 3 );
	_sample_per_capture	= _sound_input->get_sample_per_capture();
	for( INT32 i=0; i < _channel_nb; ++i )
	{
		auto ps = new c_spectrum( _sample_per_capture );
		_spectrums.push_back(ps);

		auto p = new c_buf_loop_real;
		p->set_nb_max( 44100*4 );
		_bufs_big.push_back(p);
	}

	if( _sound_buffer )
	{
		_b_enabled = true;
		retcode = true;
		//c_snd_input::reset_fft();
	}

	return retcode;
}


void	c_snd_input::do_fft()
{
	if( _fft_count == _capture_count )
		return;
	_fft_count = _capture_count;

#if AAA_LIB_USE_FFTW3()
		fftw();
#elif AAA_USE_FFT_DENPO()
		fft_denpo();
#endif
	//hack all that should be cleanup
//	reset_fft();
	if( gb_verbose_fft )
		VERBOSE_PRINT_STRING( "%d", _fft_count );			
	/*	if( !u8_fft_done )
	{
	if( b_wait )
	while( !u8_fft_done );
	else
	return FFT_NOTREADY;
	}
	*/
}

//CONSTEXPR REAL MEL_MIN = 0;          // Minimum Mel scale value (for 0 Hz)
//CONSTEXPR REAL MEL_MAX = 2595.0;     // Maximum Mel scale value for 20 kHz
//INT32 freq_nyquist = 20000;			// Nyquist frequency should be changed to our sampling rate)

// Convert linear frequency to Mel scale
//double freq_to_mel(double freq)
//{
//	return MEL_MAX * log10(1.0 + freq / 700.0);
//}
// Convert Mel scale to linear frequency
//double mel_to_freq(double mel)
//{
//	return 700.0 * (pow(10.0, mel / MEL_MAX) - 1.0);
//}
// Normalize Mel value to the range [0, 1]
//double mel_normalized(double mel)
//{
//	return (mel - MEL_MIN) / (MEL_MAX - MEL_MIN);
//}

INT32 c_snd_input::get_spectrum_band( REAL* p_bands, UINT32 CONST band_nb, INT32 CONST channel_in, REAL begin, REAL end, bool CONST b_linear )
{
#if	AAA_LIB_USE_FFTW3() || AAA_USE_FFT_DENPO()
	if( !_b_enabled )
	{
		clear_vr( p_bands, band_nb );
		return FFT_NOT_ENABLED;
	}

	//	do_fft();
	//normalize and swap
	CLAMP_REF_01( begin	);
	CLAMP_REF_01( end	);
	ORDER( begin,end );

	if( begin == end )
		clear_vr( p_bands, band_nb );
	else
	{
		REAL* power_spectrum = nullptr;
		//todo add a selector 
		c_spectrum*	p_spectrum = _spectrums[0];

		if( p_spectrum )
			power_spectrum = p_spectrum->get_power_spectrum();
		
		
		//todo find the power_spectrum[0] bug	
		//if ( band_nb == 1 && begin == 0. && end == 1. )
		//	p_bands[0] = power_spectrum[0];
		//else
		if( !power_spectrum )
		{
			clear_vr( p_bands, band_nb );
			return FFT_NOT_ON_THIS_CHANNEL;
		}

		INT32 float_par_capture = _sample_per_capture / sizeof(REAL);
		REAL delta = ( end - begin ) / REAL( band_nb );
//		err_print( "sample_per_capture %d", _sample_per_capture );
		for( UINT32 i = 0; i < band_nb; ++i )
		{
//			ERR_PRINT_STRING( "Going there with index %d and delta %g", i, delta );
			REAL low  = begin ;
			REAL high = begin + delta;

			//todo deal really with it
			// if( !b_linear )
			//{	//hack 2024 Nov progressed
			//    low  = mel_normalized( freq_to_mel(low*freq_nyquist) );
			//	low  = CLAMP_01(low);
			//	high = mel_normalized( freq_to_mel(high*freq_nyquist) );
			//	high = CLAMP_01(high);
			//}

			low	 *=	float_par_capture;
			high *=	float_par_capture;

			REAL f_sum = integrate_1d_real( power_spectrum + 1, low, high );
//			ERR_PRINT_STRING( "low %g high %g f_sum %g", low, high, f_sum );

			p_bands[i] = f_sum / ( high - low );
			
			begin += delta;
			//	f_sum = SQRT( f_sum );
		}
	}

	/*
	UINT32 count;
	UINT32 next_stop;
	UINT32 cur;

	start *= SOUND_SAMPLE_PER_CAPTURE*.5;
	end *= SOUND_SAMPLE_PER_CAPTURE*.5;
	cur = UINT32( freq_cur );
	for ( i = 0; i < band_nb; ++i )
	{
	freq_cur += freq_delta;
	next_stop = UINT32( freq_cur );
	count = 0;
	f_sum = 0.;
	do
	{
	f_sum += power_spectrum[cur];
	++cur;
	++count;
	}
	while( cur < next_stop );
	f_sum = SQRT( f_sum );
	p_bands[i] = f_sum/REAL( count );a
	cur = next_stop;
	}
	*/
	return band_nb;
#else //AAA_LIB_USE_FFTW3() || AAA_USE_FFT_DENPO()
	clear_vr( p_bands, band_nb );
	return FFT_NOT_ENABLED;
#endif
}

//static	FINLINE	bool	is_stereo_right( INT32 ch )	{	return (ch & 1) == 0 ;	}
static	FINLINE	INT32	get_local_index( INT32 ch )	{	return ch-1;		}



INT32	c_snd_input::get_spectrum_raw_band( REAL* pf_bands, INT32 CONST channel_in )
{
	INT32	band_nb = get_spectrum_band_nb();
#if	AAA_LIB_USE_FFTW3() || AAA_USE_FFT_DENPO()
	if( !_b_enabled )
	{
		clear_vr( pf_bands, band_nb );
		return FFT_NOT_ENABLED;
	}
	//	do_fft();

	REAL*		power_spectrum = nullptr;
	//c_spectrum*	p_spectrum = is_stereo_right(channel_in) ? _ps_r : _ps_l ;
	//todo add a selector 
	c_spectrum*	p_spectrum = _spectrums[0];

	if( p_spectrum )
		power_spectrum = p_spectrum->get_power_spectrum();

	if( !power_spectrum )
	{
		clear_vr( pf_bands, band_nb );
		return FFT_NOT_ON_THIS_CHANNEL;
	}

	for ( INT32 i = 0; i < band_nb; ++i )
	{
		pf_bands[i] = power_spectrum[i];
		if( power_spectrum[i] > 1.000001 )
		{
#if AAA_DEBUG()
			DBG_PRINT_STRING( "c_snd_input power_spectrum[%d] over 1 : %f", i, ( DOUBLE )power_spectrum[i] );
#endif
		}
	}
	return band_nb;
#else
	clear_vr( pf_bands, band_nb );
	return FFT_NOT_ENABLED;
#endif
}

template<class T>
void	transfert_from_int( T* dst, INT32 dst_nb, CONST void* src, INT32 src_nb, INT32 src_inc, INT32 bits_nb, T factor )
{
	if( !dst )
	{
		debug_break( "%s() null destination", __FUNCTION__ );
		return;
	}

	INT32	offset;
	UINT32	nb = MIN( dst_nb, src_nb );

	//	if( dst_nb == src_nb )
	{
		//UINT32	i;

		offset = 1 << ( bits_nb - 1 );
		factor /= T( offset );
		--dst;
		switch( bits_nb )
		{
		case 8:
			{
				UINT8* p8 = ( UINT8* )src;
				for( ; nb > 0 ; --nb )
				{
					*++dst = T( *p8 ) * factor;
					p8 += src_inc;
				}
			}
			break;
		case 16:
			{
				INT16* p16 = ( INT16* )src;
				for( ; nb > 0 ; --nb )
				{
					*++dst = T( *p16 ) * factor;
					p16 += src_inc;
				}
			}
			break;
		}
	}
	for( nb = dst_nb-src_nb; nb > 0 ; --nb )
		*++dst = T( 0 );
	/*	else if( IMOD( src_nb, dst_nb ) == 0 )
	{
	T	tmp;
	INT32	i;
	INT32	j;
	INT32	loop_nb;

	loop_nb = src_nb/dst_nb;
	offset = ( 1<<( bits_nb-1 ) )*loop_nb;
	factor /= offset;
	switch( bits_nb )
	{
	case 8:
	{
	UINT8* p8 = ( UINT8* )src;
	for ( i = 0; i < dst_nb ; ++i )
	{
	tmp = 0;
	for ( j = 0; j < loop_nb ; ++j )
	{
	tmp += T( *p8 );
	p8 += src_inc;
	}
	*dst++ = tmp * factor;
	}
	}
	break;	
	case 16:
	{
	INT16* p16 = ( INT16* )src;
	for ( i = 0; i < dst_nb ; ++i )
	{
	tmp = 0;
	for ( j = 0; j < loop_nb ; ++j )
	{
	tmp += T( *p16 );
	p16 += src_inc;
	}
	*dst++ = tmp * factor;
	}
	}
	break;
	}
	}
	else
	{
	transfert_from_int( dst, nb, src, nb, src_inc, bits_nb, factor );
	//	DBG_PRINT_STRING( "transfert_int_to_real() case not implemented yet" );
	}
	*/
}

//static	DOUBLE	samples[c_snd_input::sample_per_capture_];

REAL	c_snd_input::get_area( INT32 CONST channel_in, REAL th )
{
	if( _samples )
	{
		//todo why 2
		transfert_from_int( _samples, _sample_per_capture, _sound_buffer + get_local_index(channel_in) * 2, _sample_per_capture/_channel_nb, _channel_nb, _sample_bits, 128. );
		DOUBLE	max,d;
		get_min_max_of_array( d, max, _samples, _sample_per_capture );
		max = MAX( ABS( d ), ABS( max ) );

		if( th == 0. )
		{
			DOUBLE sum = 0;
			DOUBLE* pd = _samples - 1;
			INT32 nb = _sample_per_capture + 1;	// bacause preincrement
			while( --nb > 0 )
				sum += ABS( *++pd );
			return REAL(sum / DOUBLE( _sample_per_capture ));
		}
		else
		{	//kept (2024 Nov) but unused and probably wrong
			if( max != 0. && th != max )
			{
				th = REAL( th / max);
				DOUBLE sum = 0;
				DOUBLE* pd = _samples - 1;
				INT32 nb = _sample_per_capture + 1;	// bacause preincrement
				while( --nb > 0 )
				{
					d = ABS( *++pd );
					d /= max;
					sum += MAX0( d - th );
				}
				return REAL( sum /( DOUBLE( _sample_per_capture * ( 1 - th ) ) ) );
			}
		}
	}
	return REAL(0);
}

INT32	c_snd_input::get_wave( REAL* dst, UINT32 nb, INT32 CONST channel_in )
{
	//	if( b_stereo_flip )
	//		channel_in = ( channel_in & 0xfffe ) | ( 1-( channel_in & 1 );
	INT32 channel_index = IMOD( channel_in-1, _channel_nb );
	if( _b_test_ui )
	{
		REAL	factor = _test_freq_ui / REAL( _sample_rate );
		for( UINT32 i = 0; i < nb ; ++i )
			*dst++ = SIN_TURN( i * factor ) * _test_amp_ui;
	}
	else
	{
		if( _sound_buffer )
		{
			auto buf = _bufs_big[channel_index];
			MEMCPY( dst, (INT8*)buf->get_data(nb), nb*sizeof(REAL), __FUNCTION__ );
			//todonow		if( b_stereo_start_with && b_right )
			//transfert_from_int( dst, nb, _sound_buffer + get_local_index(channel_in) * 2, _sample_per_capture/_channel_nb, _channel_nb, _sample_bits, REAL( 1. ) );
			//		else
			//			transfert_from_int( dst, nb, sound_buffer, SOUND_SAMPLE_PER_CAPTURE, channel_nb, SOUND_BIT_PER_SAMPLE, 1. );
		}
		else
		{
			clear_vr( dst, nb );
			nb = 0;
		}
	}
	return nb;
}

INT32	c_snd_input::general_get_wave( REAL* dst, UINT32 nb, INT32 CONST channel_in )
{
	c_snd_input* c_snd_input = snd::g_master->get_by_channel( channel_in );
	if( c_snd_input )
		return	c_snd_input->get_wave( dst, nb, channel_in );
	else
		return 0;
}

REAL	c_snd_input::general_get_area( INT32 CONST channel_in, REAL th )
{
	c_snd_input* c_snd_input = snd::g_master->get_by_channel( channel_in );
	if( c_snd_input )
		return	c_snd_input->get_area( channel_in, th );
	else
		return 0;
}

#if	AAA_LIB_USE_FFTW3()
void	c_snd_input::fill_fftw_array( UINT8* pu8_array_in, DOUBLE* pc_array_out, UINT32 u32_size )
{
	if( _b_test_ui )
	{
		REAL factor = _test_freq_ui / REAL( _sample_rate );
		for( UINT32 src = 0; src < u32_size ; ++src )
		{
			*pc_array_out++ = SIN_TURN( src * factor ) * _test_amp_ui;
		}
	}
	else
	{
		transfert_from_int( pc_array_out, u32_size, pu8_array_in, u32_size, _channel_nb, _sample_bits, 1. );
	}
}

void	c_snd_input::fftw()
{
	INT32	nb = _buffer_size / _byte_per_sample;	//todo check, is it ok ? it was / 2;

	if( is_active() )
	{
		INT32 i = 0;
		for( auto & ps : _spectrums )
		{
			fill_fftw_array( _sound_buffer + _byte_per_sample * i, ps->get_in(), nb );
			ps->fttw( _b_raw_ui, _b_log_ui );
			++i;
		}
	}
	else
	{
		for( auto & ps : _spectrums )
		{
			REAL* pr = ps->get_power_spectrum();
			//	k < N/2 rounded up 
			for( INT32 k = 0; k < ( nb + 1 ) / 2; ++k )
				*pr++ = _value_inactive_ui;
			// N is even
			if( nb % 2 == 0 )
				*pr++ = _value_inactive_ui;
		}
	}
}
#elif AAA_USE_FFT_DENPO()

static REAL CosArray[28] =
{ /* cos{-2pi/N} for N = 2, 4, 8, ... 16384 */
	-1.00000000000000f,  0.00000000000000f,  0.70710678118655f,
	0.92387953251129f,  0.98078528040323f,  0.99518472667220f,
	0.99879545620517f,  0.99969881869620f,  0.99992470183914f,
	0.99998117528260f,  0.99999529380958f,  0.99999882345170f,
	0.99999970586288f,  0.99999992646572f,
	/* cos{2pi/N} for N = 2, 4, 8, ... 16384 */
	-1.00000000000000f,  0.00000000000000f,  0.70710678118655f,
	0.92387953251129f,  0.98078528040323f,  0.99518472667220f,
	0.99879545620517f,  0.99969881869620f,  0.99992470183914f,
	0.99998117528260f,  0.99999529380958f,  0.99999882345170f,
	0.99999970586288f,  0.99999992646572f
};

static REAL SinArray[28] =
{ /* sin{-2pi/N} for N = 2, 4, 8, ... 16384 */
	0.00000000000000f, -1.00000000000000f, -0.70710678118655f,
	-0.38268343236509f, -0.19509032201613f, -0.09801714032956f,
	-0.04906767432742f, -0.02454122852291f, -0.01227153828572f,
	-0.00613588464915f, -0.00306795676297f, -0.00153398018628f,
	-0.00076699031874f, -0.00038349518757f,
	/* sin{2pi/N} for N = 2, 4, 8, ... 16384 */
	0.00000000000000f,  1.00000000000000f,  0.70710678118655f,
	0.38268343236509f,  0.19509032201613f,  0.09801714032956f,
	0.04906767432742f,  0.02454122852291f,  0.01227153828572f,
	0.00613588464915f,  0.00306795676297f,  0.00153398018628f,
	0.00076699031874f,  0.00038349518757f
};

void	c_snd_input::reorder_fft_array( UINT8* pu8_array_in,
									C_COMPLEX* pc_array_out,
									UINT32 u32_size )
{	//	Shuffle all elements
	UINT32	src;
	C_COMPLEX*	p_complex = pc_array_out;
	REAL	factor;
	INT32	offset;
	INT32	i;
	UINT32*	p_order = reorder;

	if( freq_test != 0. )
	{
		factor = freq_test / SOUND_SAMPLE_PER_SEC;
		for( src = 0; src < u32_size ; ++src )
		{
			p_complex->f_real = SIN_TURN( *p_order++ * factor );
			p_complex->f_img = 0.0f;
			++p_complex;
		}
	}
	else
	{
		offset = 1<<( SOUND_BIT_PER_SAMPLE-1 );
		factor = 1./offset;
		switch( SOUND_BIT_PER_SAMPLE )
		{
		case 8:
			//set the buffers
			for ( src = 0; src < u32_size ; ++src )
			{
				i = pu8_array_in[*p_order++];
				p_complex->f_real = REAL( i-offset ) * factor;
				p_complex->f_img = 0.0f;
				++p_complex;
			}
			break;

		case 16:
			INT16* pi16_array_in = ( INT16* )pu8_array_in;
			//set the buffers
			for ( src = 0; src < u32_size ; ++src )
			{
				i = pi16_array_in[*p_order++];
				p_complex->f_real = REAL( i-offset ) * factor;
				p_complex->f_img = 0.0f;
				++p_complex;
			}
			break;
		}
	}
}

/*
UINT32	c_snd_input::shuffle( UINT32 u32_index_in,UINT32 u32_length )
{
__asm
{
mov	ebx,u32_index_in
xor eax,eax
mov	ecx,u32_length
shffl:	rcr	bx,1
rcl ax,1
loop shffl
}
}
*/

/*
void	C_WHAAA::reorder_fft_array( UINT8* pu8_array_in,UINT8* pu8_array_out,UINT32 u32_size )
{
UINT16* pu16_temp;
pu16_temp = pu16_fft_shuffle;

_asm
{
push esi
push edi


xor eax,eax
mov	ebx,pu16_array
mov	ecx,u32_size
xor edx,edx
mov esi,pu8_array_in
mov edi,pu8_array_out

reordr: mov ax,[ebx]
mov dl,[esi+eax]
add ebx,2
ror edx,8

mov ax,[ebx]
mov dl,[esi+eax]
add ebx,2
ror edx,8

mov ax,[ebx]
mov dl,[esi+eax]
add ebx,2
ror edx,8

mov ax,[ebx]
mov dl,[esi+eax]
add ebx,2
ror edx,8

mov	[edi],edx
add edi,4
sub ecx,4
jnz reordr

pop edi
pop esi
}
}
*/

void	c_snd_input::fft_denpo()
{
	UINT32 u32_angle;
	UINT32 u32_section;
	UINT32 u32_offset;
	UINT32 u32_pos1;
	UINT32 u32_pos2;
	UINT32 u32_offset_count;
	FP32 f_cos;
	FP32 f_sin;
	FP32 f_real;
	FP32 f_temp;
	FP32 f_img;
	FP32 f_temp_real;
	FP32 f_temp_img;


	//fill the real part with the incoming signal
	reorder_fft_array( 	sound_buffer,
		pc_fft,
		SOUND_SAMPLE_PER_CAPTURE );
	//fill the img part with 0

	u32_angle = 0;
	u32_section = 1;

	while ( u32_section < SOUND_SAMPLE_PER_CAPTURE )
		{
		u32_offset = u32_section << 1;
		f_cos = CosArray[u32_angle];
		f_sin = SinArray[u32_angle];
		f_real = 1.0f;
		f_img = 0.0f;
		for ( u32_offset_count = 0; u32_offset_count < u32_section; ++u32_offset_count )
			{
			u32_pos1 = u32_offset_count;
			do
				{
				u32_pos2 = u32_pos1 + u32_section;
				// Perform 2-Point DFT 
				f_temp_real =	1.0 * f_real * pc_fft[u32_pos2].f_real - 
					1.0 * f_img * pc_fft[u32_pos2].f_img;
				f_temp_img =	1.0 * f_real * pc_fft[u32_pos2].f_img + 
					1.0 * f_img * pc_fft[u32_pos2].f_real;

				pc_fft[u32_pos2].f_real = pc_fft[u32_pos1].f_real - f_temp_real;   // For pf_real-part 
				pc_fft[u32_pos2].f_img = pc_fft[u32_pos1].f_img - f_temp_img;      //For pf_img-part 
				pc_fft[u32_pos1].f_real = pc_fft[u32_pos1].f_real + f_temp_real;
				pc_fft[u32_pos1].f_img = pc_fft[u32_pos1].f_img + f_temp_img;

				u32_pos1 = u32_pos1 + u32_offset;
			} while ( u32_pos1 < SOUND_SAMPLE_PER_CAPTURE );

			//                 k
			//  Calculate new Q = cos( ak ) + j*sin( ak ) = Qr + j*Qi 
			//          -2*pi
			//  with: a = -----
			//            N
			f_temp = f_real;
			f_real = f_real * f_cos - f_img * f_sin;
			f_img = f_img * f_cos + f_temp * f_sin;
			}
		u32_section <<= 1;
		++u32_angle;
		}
	//	pu16_freq_out[0] = UINT16( 	pf_real[0] * pf_real[0] + pf_img[0] * pf_img[0] );
	FP32*	pf;
	pf = power_spectrum;
	for( u32_pos1 = 0; u32_pos1 < ( SOUND_SAMPLE_PER_CAPTURE/2+1 ); ++u32_pos1 )
		{
		f_real = pc_fft[u32_pos1].f_real;
		f_img = pc_fft[u32_pos1].f_img;
		*pf++ = f_real * f_real + f_img * f_img;
		}
}

/*
void c_snd_input::fft()
{
UINT32 u32_angle;
UINT32 u32_section;
UINT32 u32_offset;
UINT32 u32_pos1;
UINT32 u32_pos2;
UINT32 u32_offset_count;
FLOAT f_cos;
FLOAT f_sin;
FLOAT f_real;
FLOAT f_temp;
FLOAT f_img;
FLOAT f_temp_real;
FLOAT f_temp_img;
C_COMPLEX	*p1;
C_COMPLEX	*p2;

//fill the real part with the incoming signal
reorder_fft_array( 	sound_buffer,
pc_fft,
SOUND_SAMPLE_PER_CAPTURE,
SOUND_CAPTURE_SIZE_IN_BIT );
//fill the img part with 0

u32_angle = 0;
u32_section = 1;

while ( u32_section < SOUND_SAMPLE_PER_CAPTURE )
{
u32_offset = u32_section << 1;
f_cos = CosArray[u32_angle];
f_sin = SinArray[u32_angle];
f_real = 1.0f;
f_img = 0.0f;
for ( u32_offset_count = 0; u32_offset_count < u32_section; ++u32_offset_count )
{
u32_pos1 = u32_offset_count;
do
{
u32_pos2 = u32_pos1 + u32_section;
p1 = &pc_fft[u32_pos1];
p2 = &pc_fft[u32_pos2];
// Perform 2-Point DFT 
f_temp_real =	f_real * p2->f_real - f_img * p2->f_img;
f_temp_img =	f_real * p2->f_img + f_img * p2->f_real;

p2->f_real	= p1->f_real - f_temp_real;             // For pf_real-part 
p2->f_img	= p1->f_img - f_temp_img;             //For pf_img-part 
p1->f_real	+= f_temp_real;
p1->f_img	+= f_temp_img;

u32_pos1 += u32_offset;
}
while ( u32_pos1 < SOUND_SAMPLE_PER_CAPTURE );

//                 k
//  Calculate new Q = cos( ak ) + j*sin( ak ) = Qr + j*Qi 
//          -2*pi
//  with: a = -----
//            N
f_temp = f_real;
f_real = f_real * f_cos - f_img * f_sin;
f_img = f_img * f_cos + f_temp * f_sin;
}
u32_section <<= 1;
++u32_angle;
}
//	pu16_freq_out[0] = UINT16( 	pf_real[0] * pf_real[0] + pf_img[0] * pf_img[0] );
FLOAT*	pf;
pf = power_spectrum;
for( u32_pos1 = 0; u32_pos1 < ( SOUND_SAMPLE_PER_CAPTURE/2+1 ); ++u32_pos1 )
{
f_real = pc_fft[u32_pos1].f_real;
f_img = pc_fft[u32_pos1].f_img;
*pf++ = f_real * f_real + f_img * f_img;
}
u8_fft_done = 1;
DBG_PRINT_STRING( "fft %d", ftt_capture_count );

}
*/
#endif	//AAA_USE_FFT_DENPO()

/*
// ***** BEATDETECTOR.H *****
#ifndef BeatDetectorH
#define BeatDetectorH

class TBeatDetector
{
private:
  float KBeatFilter;        // Filter coefficient
  float Filter1Out, Filter2Out;
  float BeatRelease;        // Release time coefficient
  float PeakEnv;            // Peak enveloppe follower
  bool BeatTrigger;         // Schmitt trigger output
  bool PrevBeatPulse;       // Rising edge memory
public:
  bool BeatPulse;           // Beat detector output

  TBeatDetector();
  ~TBeatDetector();
  virtual void setSampleRate(float SampleRate);
  virtual void AudioProcess (float input);
};
#endif

/*
// ***** BEATDETECTOR.CPP *****
#include "BeatDetector.h"
#include "math.h"

#define FREQ_LP_BEAT 150.0f    // Low Pass filter frequency
#define T_FILTER 1.0f/(2.0f*M_PI*FREQ_LP_BEAT)  // Low Pass filter time constant
#define BEAT_RTIME 0.02f  // Release time of enveloppe detector in second

TBeatDetector::TBeatDetector()
// Beat detector constructor
{
  Filter1Out=0.0;
  Filter2Out=0.0;
  PeakEnv=0.0;
  BeatTrigger=false;
  PrevBeatPulse=false;
  setSampleRate(snd::SAMPLE_PER_SEC_DEF);
}

TBeatDetector::~TBeatDetector()
{
  // Nothing specific to do...
}

void TBeatDetector::setSampleRate (float sampleRate)
// Compute all sample frequency related coeffs
{
  KBeatFilter=1.0/(sampleRate*T_FILTER);
  BeatRelease=(float)exp(-1.0f/(sampleRate*BEAT_RTIME));
}

void TBeatDetector::AudioProcess (float input)
// Process incoming signal
{
  float EnvIn;

  // Step 1 : 2nd order low pass filter (made of two 1st order RC filter)
  Filter1Out=Filter1Out+(KBeatFilter*(input-Filter1Out));
  Filter2Out=Filter2Out+(KBeatFilter*(Filter1Out-Filter2Out));

  // Step 2 : peak detector
  EnvIn=fabs(Filter2Out);
  if (EnvIn>PeakEnv) PeakEnv=EnvIn;  // Attack time = 0
  else
  {
	PeakEnv*=BeatRelease;
	PeakEnv+=(1.0f-BeatRelease)*EnvIn;
  }

  // Step 3 : Schmitt trigger
  if (!BeatTrigger)
  {
	if (PeakEnv>0.3) BeatTrigger=true;
  }
  else
  {
	if (PeakEnv<0.15) BeatTrigger=false;
  }

  // Step 4 : rising edge detector
  BeatPulse=false;
  if ((BeatTrigger)&&(!PrevBeatPulse))
	BeatPulse=true;
  PrevBeatPulse=BeatTrigger;
}
*/
