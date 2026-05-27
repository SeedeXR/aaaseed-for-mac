
#ifdef AAA_SNDINPUT__BASS_H
#error "SNDINPUT__BASS_H included more than once."
#endif
#define AAA_SNDINPUT__BASS_H 1



#ifndef AAA_DELTA_T_H
#	include "delta_t.h"
#endif
#ifndef AAA_SND_INPUT_H
#	include "snd_input.h"
#endif
#ifndef BASS_H
#	include "bass.h"
#endif


class c_snd_input_bass final : public c_snd_input
{
	FACTORY_DECLARE(c_snd_input_bass,c_snd_input);
private :
	// Bass
	HRECORD			bass_record_chan;		// recording channel
	WAVEFORMATEX				wfx;

			void	update_low();
public:

	void	param_init_pt();
	void	param_init();

	AAA_ERR	save_do_after( char* const filename );
	AAA_ERR	load_do_after( char* const filename );

	virtual	void	disable();
	virtual	BOOL	enable();
	virtual	void	init();
	virtual	void	update();
	virtual	INT32	get_capture_count()		{ return capture_count_; };

};




