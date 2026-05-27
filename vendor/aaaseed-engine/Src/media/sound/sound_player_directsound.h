
#ifdef AAA_SOUND_PLAYER_DS_H
#error "SOUND_PLAYER_DS_H included more than once."
#endif
#define AAA_SOUND_PLAYER_DS_H 1


#ifndef AAA_DELTA_T_H
#	include "delta_t.h"
#endif
#ifndef AAA_SOUND_PLAYER_H
#	include "sound_player.h"
#endif
#ifdef	WIN32
#	ifndef __DSOUND_INCLUDED__
#		include <dsound.h>
#	endif
#endif


class c_sound_player_ds final : public c_sound_player
{
private :
	LPDIRECTSOUND			p_ds_;
	LPDIRECTSOUNDBUFFER		buffer_;
	UINT32					byte_per_sec_;

			void	get_pan_low();
			void	get_volume_low();

public:


//	virtual	const char*	const	get_class_name()	{ return "sound_player_ds"; };
//	virtual	const char*	const	get_fname_ext()		{ return "sound_player_ds"; };

	c_sound_player_ds();
	virtual	~c_sound_player_ds();

	virtual void	init();
	virtual	void	deinit();

	virtual	void	update();


			void	set_device( LPDIRECTSOUND p_ds )	{ p_ds_ = p_ds; };

	virtual AAA_ERR	load_from_file( const CHAR* filename );

	virtual	void	set_volume( REAL volume );
	virtual	void	set_pan( REAL pan );
	virtual	void	set_position( REAL pos );
	virtual	REAL	get_position();


	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();

	virtual BOOL	is_playing()		{ return b_playing_; };

	virtual	INT32	get_status();

};




