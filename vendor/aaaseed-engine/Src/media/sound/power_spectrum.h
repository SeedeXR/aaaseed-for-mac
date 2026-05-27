
#ifdef AAA_POWER_SPECTRUM_H
#error "POWER_SPECTRUM_H included more than once."
#endif
#define AAA_POWER_SPECTRUM_H 1


//How to use:
//
//	for accessing to the frequencies
// 1)enable() return true or false
//
// 2)reset_fft() ask for fft (asynchr.)
//
// ..Put some code here, in order to leave time to the routine for
//		computing the fft
// 3)get_freq_band
//
//		INT8 get_freq_band(FLOAT f_start,	//the index of the first scanned frequency
//						FLOAT f_end,		//the index of the last scanned frequency
//						UINT32 u32_nband,	//number of different frequency bands 
//						FLOAT* pf_bands,	//an array of [u32_nbands] floats to received de values
//						UINT8 u8_wait);
//
//
//		The fact that index are expressed in float have no means, 
//		their converted into integer, it's just for my own usage convenience
//
//	4) go to 2
//
//	5)disable (optional, it is included in the destructor)
//
//
//
// for accessing directly to the sample
//	 freeze() stop the sound digitalization
//		
//		directly read the values
//   the values are in 	pu8_sound_buffer
//		even if sample are in 16 bit, there is SOUND_SAMPLE_PER_CAPTURE*samples
//
//  unfreeze() restart it
//		


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_WRAP_FFTW3_H
#	include "lib_wrappers/wrap_fftw3.h"
#endif

#define	AAA_USE_FFT_DENPO()	0

#define FFT_NOT_ON_THIS_CHANNEL	-2
#define FFT_NOT_ENABLED			-1
#define FFT_NOT_READY			0
#define FFT_OK					1

//todo no calibrage in

#if	AAA_LIB_USE_FFTW3()
#	ifndef FFTW3_H
#		include <fftw3.h>
#	endif
#elif AAA_USE_FFT_DENPO()
class C_COMPLEX
{
public:
	FP32	f_real;
	FP32	f_img;
};
#endif

class	c_spectrum
{
//	friend class c_snd_input;
private:
	bool		_b_calibrate;
	REAL		_calibrate_auto_step;

	REAL*		_power_spectrum;
	REAL*		_min;
	REAL*		_max;
	REAL*		_factor;

	INT32		_sample_nb;
	INT32		_spectrum_nb;

#if	AAA_LIB_USE_FFTW3()
	fftw_plan	_plan;
	DOUBLE*		_window_value;
	DOUBLE*		_in_fftw;
	DOUBLE*		_out_fftw;
#elif AAA_USE_FFT_DENPO()
	UINT32*		reorder;
	C_COMPLEX*	pc_fft;
	//the buffers
	//	FLOAT		pf_img[SOUND_SAMPLE_PER_CAPTURE]; //real part of the spectra
	//	FLOAT		pf_real[SOUND_SAMPLE_PER_CAPTURE]; //imaginary part of the spectra
	//	UINT16		pu16_fft_shuffle[SOUND_SAMPLE_PER_CAPTURE];
#endif

	void			dealloc();
	bool			alloc( INT32 CONST ample_per_capture );
	FINLINE	void	calibrate_check_index( INT32 index, REAL in );

public:
	c_spectrum( INT32 sample_per_capture );
	virtual ~c_spectrum();

#if	AAA_LIB_USE_FFTW3() || AAA_USE_FFT_DENPO()
	REAL*			get_min()				{	return _min;			}
	REAL*			get_max()				{	return _max;			}
	REAL*			get_power_spectrum()	{	return _power_spectrum;	}
	DOUBLE*			get_in()				{	return _in_fftw;		}
#endif

	void			calibrate_set( bool CONST b_in );
	void			calibrate_generate_factor( C_PCHAR_C filename );

	void			set_calibrate_auto_step( REAL val )		{	_calibrate_auto_step = val;	}
#if	AAA_LIB_USE_FFTW3()
	void			fttw( bool CONST b_raw, bool CONST b_log );
#endif
};
