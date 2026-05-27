
#ifdef AAA_BDD_SND_SYNTH_SPECTRUM_H
#error "BDD_SND_SYNTH_SPECTRUM_H included more than once."
#endif
#define AAA_BDD_SND_SYNTH_SPECTRUM_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_WRAP_BASS_H
#	include "lib_wrappers/wrap_bass.h"
#endif
#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

//CONST	INT32	BDD_SOUND_AVERAGER_MAX_NB = 256;

CONST	INT32	FFT_SYNTH_BAND_MAX = 1024;


class	c_bdd_snd_synth_spectrum final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_snd_synth_spectrum,c_bdd_multiple);
private:
	bool				_b_use_img_ui;
	aaa::COMPO			_s_component;
	REAL				level_min_;
	REAL				level_max_;
	INT32				band_nb_;
	REAL				volume_;
	REAL				freq_start_;
	REAL				freq_stop_;
	REAL				freq_bias_;
	REAL				freq_power_;
	REAL				freq_u_;
	REAL				min_;
	REAL				max_;

	bool				_b_start;
	bool				_b_start_ui;
	bool				_b_trig_start_ui;
	bool				_b_trig_stop_ui;
	bool				_b_trig_init_ui;

	REAL				shape_gain_;
	REAL				shape_bias_;

	REAL				vol_power_;

	HSTREAM				_bass_stream;
	REAL				vol_last_[FFT_SYNTH_BAND_MAX];
	REAL				phase_[FFT_SYNTH_BAND_MAX];
	REAL				band_[FFT_SYNTH_BAND_MAX];
	REAL				dph_[FFT_SYNTH_BAND_MAX];
	REAL				vol_[FFT_SYNTH_BAND_MAX];

	mutable aaa::MUTEX	_lock;

public:
			void	init();
			void	alloc(); 
			void	dealloc();

	virtual	void	update();
	virtual	void	draw_single();

	virtual	void	param_init_pt();

			INT32	write_stream( void *buffer_in, INT32 length);
			void	img_read_band( c_img_2d* img_src);
			void	prepare_band();

			void	synth_start();
			void	synth_stop();
			void	synth_init();

			void	lock();
			void	unlock();
};
