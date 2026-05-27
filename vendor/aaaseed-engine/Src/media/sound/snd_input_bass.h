
#ifdef AAA_SND_INPUT_BASS_H
#error "SND_INPUT_BASS_H included more than once."
#endif
#define AAA_SND_INPUT_BASS_H 1


#ifndef AAA_SOUND_INPUT_H
#	include "sound_input.h"
#endif
#ifndef AAA_WRAP_BASS_H
#	include "lib_wrappers/wrap_bass.h"
#endif

class c_sound_input_bass final : public c_sound_input
{
private :
	HRECORD			_bass_record_chan;		// recording channel

protected:
	virtual	bool	enable_low();
	virtual	bool	disable_low();

public:
	c_sound_input_bass();
	virtual	~c_sound_input_bass();

	virtual	void	init();
	virtual	void	deinit();

	virtual	void	update();
};
