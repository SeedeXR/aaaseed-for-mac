
#ifdef AAA_SND_INPUT_BASS_ASIO_H
#error "SND_INPUT_BASS_ASIO_H included more than once."
#endif
#define AAA_SND_INPUT_BASS_ASIO_H 1


#ifndef AAA_SOUND_INPUT_H
#	include "sound_input.h"
#endif

class c_sound_bass_asio;

class c_sound_input_bass_asio final : public c_sound_input
{
//	FACTORY_DECLARE( c_sound_input_bass_asio, c_sound_input);
private :
//
//	INT32	_asio_buf_lenght;

			void	update_low();

	virtual	bool	enable_low();
	virtual	bool	disable_low();
protected:

public:
	c_sound_bass_asio*	_sound;

	c_sound_input_bass_asio();
	virtual	~c_sound_input_bass_asio();

	virtual	void	init();
	virtual	void	deinit();

	virtual	void	update();

//	virtual	bool	enable( INT32 id, INT32 device, INT32 channel_first, INT32 channel_nb, INT32 sample_rate, INT32 sample_bits, INT32 sample_per_capture );
	
//			INT32	get_asio_buf_length()		{ return _asio_buf_len; };

};




