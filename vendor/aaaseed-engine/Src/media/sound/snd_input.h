
#ifdef AAA_SND_INPUT_H
#error "SND_INPUT_H included more than once."
#endif
#define AAA_SND_INPUT_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef _QUEUE_
#	include <queue>
#endif
//because of compile flag
#ifndef AAA_POWER_SPECTRUM_H
#	include "power_spectrum.h"
#endif

//todo no calibrage in

#define FFT_NOT_ON_THIS_CHANNEL	-2
#define FFT_NOT_ENABLED			-1
#define FFT_NOT_READY			0
#define FFT_OK					1

//class c_spectrum;
class c_sound_input;
class c_buf_loop_real;

class c_snd_input final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_snd_input, c_obj_active_ui );

	friend class c_sound;
	c_sound* _lib_used;
public:
	static	INT32 CONST CALIBRATE_NB = 8;

	static	void		c_init();
	static	void		c_deinit();

	static	INT32 CONST	SOUND_CHANNEL_NB_MAX	=	8;	//todo deal with this

	static	INT32		general_get_wave(	REAL* dst, UINT32 nb, INT32 CONST channel_in );
	static	REAL		general_get_area(	INT32 CONST channel_in, REAL th );

private:
	INT32				_sample_per_capture;


	volatile	bool	_b_buffer_read;
//	volatile	UINT8	_u8_fft_done;

	INT32				_capture_count;
	INT32				_fft_count;
	INT32				_count_new;

	bool				_b_enabled;
	bool				_b_enabled_ui;

//	bool				_b_stereo;
//	bool				_b_stereo_start_with;

	INT32				_device_ui;
//	INT32				_device;
//	o_str				_device_name;
	INT32				_channel_first;
	INT32				_channel_nb;
	INT32				_channel_nb_ui;
	INT32				_sample_rate_ui;
	INT32				_sample_rate;
	INT32				_sample_bits_ui;
	INT32				_sample_bits;
	INT32				_capture_size_pow2_ui;

//	bool				_b_stereo_flip_ui;

	REAL				_value_inactive_ui;

	bool				_b_calibrate_ui[CALIBRATE_NB];
	bool				_b_calibrate_auto_ui;
	REAL				_calibrate_auto_delay_ui;

	bool				_b_raw_ui;
	bool				_b_log_ui;
	//REAL				out_min_;
	//REAL				out_max_;

	REAL				_test_amp_ui;
	REAL				_test_freq_ui;
	bool				_b_test_ui;
	c_delta_t			_delta_t;

	UINT8*				_sound_buffer;
	INT32				_buffer_size;
	INT32				_byte_per_sample;

	DOUBLE*				_samples;

	std::vector< c_buf_loop_real* >	_bufs_big;	
	std::vector<c_spectrum*>		_spectrums;
	c_sound_input*					_sound_input;	// low level sound_input

	void				sample_ui_convert();

	c_sound_input*		get_sound_input_valid(); //lv we protect from crash

public:
	//todo move these two fns to snd master
	static	void	set_fft_verbose( bool CONST in );
	static	void	flip_fft_verbose();

	virtual	void	param_init_pt();

	virtual	AAA_ERR	save_do_after( o_str CONST & filename );
	virtual	AAA_ERR	load_do_after( o_str CONST & filename );

			void	disable();
			bool	enable();
		//	void	init();
			void	update();

			void	alloc_fft();
			void	dealloc_fft();
			void	fft();

	FINLINE	INT32	get_sample_per_capture()	{ return _sample_per_capture; };

#if	AAA_LIB_USE_FFTW3()
			void	alloc_fftw();
			void	dealloc_fftw();
			void	fftw();
			void	fill_fftw_array( UINT8*	pu8_array_in, DOUBLE* pc_array_out, UINT32 u32_size );
#elif AAA_USE_FFT_DENPO()
			void	alloc_fft_denpo();
			void	dealloc_fft_denpo();
			void	fft_denpo();
			UINT32	shuffle( UINT32 u32_index_in, UINT32 u32_length );
			void	reorder_fft_array( UINT8* pu8_array_in, C_COMPLEX* pc_array_out, UINT32 u32_size );
#endif

	FINLINE INT32	get_spectrum_band_nb() { return _sample_per_capture/ 2 + 1; };

			void	freeze();
			void	unfreeze();
//			void	reset_fft();
			void	do_fft();

			INT32	get_spectrum_band(		REAL* pf_bands, UINT32 CONST nband, INT32 CONST channel_in, REAL begin, REAL end, bool CONST b_linear );
			INT32	get_spectrum_raw_band(	REAL* pf_bands, INT32 CONST channel_in );
			INT32	get_wave(				REAL* dst,		UINT32 nb, INT32 CONST channel_in );
			REAL	get_area(				INT32 CONST channel_in, REAL th );

			INT32	get_count_new()		CONST	{	return _count_new;	}
			INT32	get_channel_nb()	CONST	{	return _channel_nb;	}
};



