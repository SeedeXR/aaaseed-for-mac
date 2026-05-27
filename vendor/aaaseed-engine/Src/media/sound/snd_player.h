
#ifdef AAA_SND_PLAYER_H
#error "SND_PLAYER_H included more than once."
#endif
#define AAA_SND_PLAYER_H 1


#ifndef AAA_OBJ_H
#	include		"infrastructure/obj/obj.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class	c_sound_player : public c_obj
{
protected :
	bool		_b_enabled;

	bool		_b_playing;
	bool		_b_loop;
	INT32		_sample_rate;
	INT32		_sample_bits;
	REAL		_duration;
	REAL		_volume;
	REAL		_pan;
	REAL		_freq;
	INT32		_channel_nb;

public:
	c_sound_player();
	virtual	~c_sound_player();

	virtual	void	init();
	virtual	void	deinit();

	virtual	void	update();

	virtual AAA_ERR	load_from_file( o_str CONST & filename );

	virtual	void	set_device( INT32 i )	= 0;

			REAL	get_volume()			{ return _volume; }
			REAL	get_pan()				{ return _pan; }
			INT32	get_sample_rate()		{ return _sample_rate; }
			REAL	get_duration()			{ return _duration; }
			INT32	get_sample_bits()		{ return _sample_bits; }
			INT32	get_channel_nb()		{ return _channel_nb; }
			REAL	get_frequency()			{ return _freq; }

			bool	is_loop()				{ return _b_loop; }
	virtual	void	set_loop( bool b_loop )	{ _b_loop = b_loop; }

	virtual	void	set_volume( REAL volume ) = 0;
	virtual	void	set_pan( REAL pan ) = 0;
	virtual	void	set_position( REAL pos );
	virtual	REAL	get_position();
	virtual	void	set_frequency( REAL freq )	{}
	//virtual	void	set_loop( bool b_loop );

	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();

	virtual bool	is_playing()			{ return _b_playing; }

	virtual	INT32	get_status();

			bool	is_enable()				{ return _b_enabled; }
};



