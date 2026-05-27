
#ifdef AAA_SOUND_PLAYER_DS_H
#error "SOUND_PLAYER_DS_H included more than once."
#endif
#define AAA_SOUND_PLAYER_DS_H 1


#ifndef AAA_SND_PLAYER_H
#	include "snd_player.h"
#endif
#ifdef	WIN32
#	ifndef __DSOUND_INCLUDED__
#		include <dsound.h>
#	endif
#endif

class c_sound_player_ds final : public c_sound_player
{
private :
	LPDIRECTSOUND			_p_ds;
	LPDIRECTSOUNDBUFFER		_buffer;
	UINT32					_byte_per_sec;
	REAL					_freq;

			void	get_pan_low();
			void	get_volume_low();

public:
	c_sound_player_ds();
	virtual	~c_sound_player_ds();

	virtual	void	init();
	virtual	void	deinit();

	virtual	void	update();


//	virtual	void	set_device( LPDIRECTSOUND p_ds )	{ _p_ds = p_ds; };
	virtual	void	set_device( INT32 i )	{};

	virtual AAA_ERR	load_from_file( o_str CONST & filename );

	virtual	void	set_volume(		REAL volume );
	virtual	void	set_pan(		REAL pan );
	virtual	void	set_position(	REAL pos );
	virtual	REAL	get_position();
	virtual	void	set_frequency(	REAL freq );

	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();

	virtual bool	is_playing()		{ return _b_playing; }

	virtual	INT32	get_status();

};




