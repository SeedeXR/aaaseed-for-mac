
#ifdef AAA_SNDINPUT_H
#error "SNDINPUT_H included more than once."
#endif
#define AAA_SNDINPUT_H 1


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
#	include "obj_ui.h"
#endif
//#ifdef	WIN32
//#	include <dsound.h>
//#endif
#ifndef AAA_DELTA_T_H
#	include "delta_t.h"
#endif
//#ifndef	__SOUND_NEW_H__
//#	include "Sound/sound_new.h"
//#endif
//#ifndef BASSASIO_H
//#	include "bassasio.h"
//#endif
//#ifndef BASS_H
//#	include "bass.h"
//#endif

#define	USE_FFT_DENPO	FALSE
//todo no calibrage in
#define	USE_FFTW		TRUE
#if	USE_FFTW
#ifndef FFTW3_H
#include <fftw3.h>
#endif
#endif

#define FFT_NOTENABLED -1
#define FFT_NOTREADY	0
#define FFT_OK			1

void fft_verbose_set( BOOL in );
void fft_verbose_flip();

#if	USE_FFT_DENPO
class C_COMPLEX
{
public:
	FLOAT f_real;
	FLOAT f_img;
};
#endif

//extern	c_sound*	sound;

class c_ps_stuff	//	ps is for power spectrum
{
friend class c_snd_input;
private:
	BOOL	b_calibrate_;
	REAL	calibrate_auto_step_;

	REAL*	power_spectrum_;
	REAL*	min_;			
	REAL*	max_;
	REAL*	factor_;

	INT32	sample_nb_;
	INT32	spectrum_nb_;
#if	USE_FFTW
	fftw_plan	plan_;
	DOUBLE*		in_fftw_;
	DOUBLE*		out_fftw_;
#endif
#if	USE_FFT_DENPO
	UINT32*		reorder;
	C_COMPLEX*	pc_fft;
	//the buffers
	//	FLOAT		pf_img[SOUND_SAMPLE_PER_CAPTURE]; //real part of the spectra
	//	FLOAT		pf_real[SOUND_SAMPLE_PER_CAPTURE]; //imaginary part of the spectra

	//	UINT16		pu16_fft_shuffle[SOUND_SAMPLE_PER_CAPTURE];
#endif

	c_ps_stuff();
	~c_ps_stuff();

	void			dealloc();
	void			alloc( INT32 sample_per_capture);
	void			fttw();
	void			calibrate_set( BOOL in );
	FINLINE	void	calibrate_check_index( INT32 index, REAL in );
	void			calibrate_generate_factor( const CHAR* filename );
};

class c_snd_input final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_snd_input,c_obj_active_ui);
public:
	static	const	INT32 SOUND_BIT_PER_SAMPLE = 16;
	static	const	INT32 SOUND_BYTE_PER_SAMPLE = ( SOUND_BIT_PER_SAMPLE >> 3 );

	static	const	INT32 SOUND_SAMPLE_PER_SEC = 44100;
	static	const	INT32 SOUND_BYTE_PER_SECOND_MONO = ( SOUND_SAMPLE_PER_SEC * SOUND_BYTE_PER_SAMPLE );

	static	const	INT32 SOUND_BUFFER_NB = 4;	//for directsound
	static	const	INT32 SOUND_CAPTURE_SIZE_IN_BIT	= 10;
	static	const	INT32 SOUND_SAMPLE_PER_CAPTURE = ( 1 << SOUND_CAPTURE_SIZE_IN_BIT );
	static	const	INT32 SOUND_BYTE_PER_CAPTURE = ( SOUND_SAMPLE_PER_CAPTURE * SOUND_BYTE_PER_SAMPLE );

private:
//	volatile BOOL	b_sound_capture_thread_on_;	
	volatile BOOL	b_buffer_read_;
	volatile UINT8	u8_fft_done_;

	INT32			ftt_capture_count_;
	INT32			fft_count_;


	BOOL			b_enabled_;
	BOOL			b_enabled_ui_;

	BOOL			b_stereo_start_with_;
	INT32			device_;
	INT32			channel_nb_;
	INT32			channel_nb_ui_;
	//	BOOL			b_right_;
	BOOL			b_stereo_flip_;

	REAL			value_inactive_;

	BOOL			b_calibrate_ui_[2];
	BOOL			b_calibrate_auto_;
	REAL			calibrate_auto_delay_;

	//	REAL			out_min_;
	//	REAL			out_max_;

	REAL			freq_test_;
	c_delta_t		delta_t_;

	INT32			id_;
	UINT8*			sound_buffer_; //[SOUND_BYTE_PER_CAPTURE];	// the sound
	INT32			buffer_size_;

	c_ps_stuff		ps_l_;
	c_ps_stuff		ps_r_;

//	// Bass
//	HRECORD			bass_record_chan_;		// recording channel

//	// Bass Asio
//	INT32			asio_buf_lenght_;

	//// DirectX stuff
	//LPDIRECTSOUNDCAPTUREBUFFER	lpDSBuffer_;
	//HANDLE						h_NotifyEvent_[SOUND_BUFFER_NB+1];
	//HANDLE						h_notify_thread_;
	//LPDIRECTSOUNDCAPTURE		p_DSCapture_;
	//LPDIRECTSOUNDNOTIFY			lpDsNotify_;  // pointer to the interface
	//WAVEFORMATEX				wfx_;
	//DWORD						dwThreadId_;

	void	print_avalaible_mode( DWORD mask);

	//void	update_bass();
	//void	disable_directsound();
	//void	disable_bass();
	//void	disable_bass_asio();
	//BOOL	enable_directsound();
	//BOOL	enable_bass();
	//BOOL	enable_bass_asio();

public:

//	friend	DWORD CALLBACK	HandleNotifications( LPVOID lpvoid );
//	//	friend	BOOL CALLBACK	Bass_RecordingCallback( HRECORD handle, void *buffer, DWORD length, DWORD user );
//	friend	void CALLBACK	bass_asio_proc( BOOL input, DWORD channel, void *buffer, DWORD lenght, DWORD user );
//
//	INT32	get_asio_buf_lenght()			{ return asio_buf_lenght_; };

//	virtual	const char*	const	get_class_name()	{ return "snd_input"; };
//	virtual	const char*	const	get_fname_ext()		{ return "snd_input"; };

	void	param_init_pt();
	void	param_init();

	AAA_ERR	save_do_after( char* const filename );
	AAA_ERR	load_do_after( char* const filename );

	void	disable();
	BOOL	enable();
	void	init();
	void	update();

	void	alloc_fft();
	void	dealloc_fft();
	void	fft();

#if	USE_FFTW
	void	alloc_fftw();
	void	dealloc_fftw();
	void	fftw();
	void	fill_fftw_array( UINT8*	pu8_array_in, DOUBLE* pc_array_out, UINT32 u32_size );
#endif

#if	USE_FFT_DENPO
	void	alloc_fft_denpo();
	void	dealloc_fft_denpo();
	void	fft_denpo();
	UINT32	shuffle( UINT32 u32_index_in, UINT32 u32_length );
	void	reorder_fft_array( UINT8* pu8_array_in, C_COMPLEX* pc_array_out, UINT32 u32_size );
#endif
	//	void	calibrate_generate_factor( CHAR* filename);
	//	void	calibrate_set( BOOL& b_cal, BOOL in, REAL* min, REAL* max);
	//	FINLINE	void	calibrate_check_index( REAL* ps_min, REAL* ps_max, REAL* ps_factor, INT32 index, REAL in);

	FINLINE INT32	spectrum_band_nb_get() { return SOUND_SAMPLE_PER_CAPTURE/2 + 1; };

	void	freeze();
	void	unfreeze();
	void	reset_fft();
	void	do_fft();
	void	release_notification_events();

	INT32	get_spectrum_band( REAL start, REAL end, UINT32 nband, REAL* pf_bands, BOOL b_right, BOOL b_linear, BOOL u8_wait );
	INT32	get_spectrum_raw_band( REAL* pf_bands, BOOL b_right );
	INT32	get_wave( REAL* dst, UINT32 nb, INT32 channel_in );
	REAL	get_area( INT32 channel_in, REAL th );

public:
	static	const	INT32 SND_INPUT_NB	= 2;
	//todoqq adapt load and save to change SND_INPUT_NB value (load and save do 2 for now)
	static	c_snd_input*	snd_inputs[SND_INPUT_NB];
	FINLINE	static	c_snd_input*	get( INT32 channel_in)	{ return snd_inputs[(channel_in-1)/2]; }
	FINLINE	static	INT32			general_get_wave( REAL* dst, UINT32 nb, INT32 channel_in )
		{
		c_snd_input* snd_input = get( channel_in );
		if( snd_input )
			return	snd_input->get_wave( dst, nb, ( channel_in - 1) & 1 );
		else
			return 0;
		}

	FINLINE	static	REAL	general_get_area( INT32 channel_in, REAL th )
		{
		c_snd_input* snd_input = get( channel_in );
		if( snd_input )
			return	snd_input->get_area( ( channel_in - 1) & 1 , th);
		else
			return 0;
		}
};




