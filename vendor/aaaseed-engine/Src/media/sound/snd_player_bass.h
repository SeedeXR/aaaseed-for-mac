
#ifdef AAA_SOUND_PLAYER_BASS_H
#error "SOUND_PLAYER_BASS_H included more than once."
#endif
#define AAA_SOUND_PLAYER_BASS_H 1


#ifndef AAA_SND_PLAYER_H
#	include "snd_player.h"
#endif
#ifndef AAA_WRAP_BASS_H
#	include "lib_wrappers/wrap_bass.h"
#endif

class	c_sound_player_bass final : public c_sound_player
{
private :
	HSTREAM			_bass_stream;			// playing channel
	UINT32			_byte_per_sec;

//	REAL			freq_;

	void	get_pan_low();
	void	get_volume_low();

public:
	c_sound_player_bass();
	~c_sound_player_bass();

	virtual	void	init();
	virtual	void	deinit();

	virtual	void	update();

	virtual	void	set_device( INT32 i )	{};

	virtual AAA_ERR	load_from_file( o_str CONST & filename );

	virtual	void	set_volume(		REAL volume );
	virtual	void	set_pan(		REAL pan );
	virtual	void	set_position(	REAL pos );
	virtual	REAL	get_position();

	virtual	void	set_loop( bool b_loop );

	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();

	virtual bool	is_playing()		{ return _b_playing; }

	virtual	INT32	get_status();

};


